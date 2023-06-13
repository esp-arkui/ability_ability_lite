/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "js_ability_thread.h"

#include "abilityms_log.h"
#include "ability_errors.h"
#include "ability_inner_message.h"
#include "adapter.h"
#include "js_ability.h"
#include "js_async_work.h"
#include "los_task.h"
#include "slite_ability_loader.h"

namespace OHOS {
namespace AbilitySlite {
constexpr uint16_t APP_TASK_PRI = 25;
constexpr uint32_t QUEUE_LENGTH = 32;
static char g_jsAppTask[] = "AppTask";

JsAbilityThread::JsAbilityThread() = default;

JsAbilityThread::~JsAbilityThread()
{
    delete ability_;
    ability_ = nullptr;
    if (messageQueueId_ != nullptr) {
        osMessageQueueDelete(messageQueueId_);
        messageQueueId_ = nullptr;
    }
}

int32_t JsAbilityThread::InitAbilityThread(const AbilityRecord *abilityRecord)
{
    if (abilityRecord == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "JsAbilityThread init fail, abilityRecord is null");
        return PARAM_NULL_ERROR;
    }
    if (abilityRecord->appName == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "JsAbilityThread init fail, appName is null");
        return PARAM_NULL_ERROR;
    }
    if (state_ != AbilityThreadState::ABILITY_THREAD_UNINITIALIZED) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "JsAbilityThread init fail, the AbilityThread is already inited");
        return PARAM_CHECK_ERROR;
    }

    messageQueueId_ = osMessageQueueNew(QUEUE_LENGTH, sizeof(SliteAbilityInnerMsg), nullptr);
    if (messageQueueId_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "JsAbilityThread init fail: messageQueueId is null");
        return MEMORY_MALLOC_ERROR;
    }

    TSK_INIT_PARAM_S stTskInitParam = { nullptr };
    LOS_TaskLock();
    stTskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC) (JsAbilityThread::AppTaskHandler);
    stTskInitParam.uwStackSize = TASK_STACK_SIZE;
    stTskInitParam.usTaskPrio = OS_TASK_PRIORITY_LOWEST - APP_TASK_PRI;
    stTskInitParam.pcName = g_jsAppTask;
    stTskInitParam.uwResved = 0;
    stTskInitParam.uwArg = reinterpret_cast<uintptr_t>(messageQueueId_);
    uint32_t ret = LOS_TaskCreate(&appTaskId_, &stTskInitParam);
    if (ret != LOS_OK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "JsAbilityThread init fail: LOS_TaskCreate ret %{public}d", ret);
        osMessageQueueDelete(messageQueueId_);
        LOS_TaskUnlock();
        return CREATE_APPTASK_ERROR;
    }
    state_ = AbilityThreadState::ABILITY_THREAD_INITIALIZED;
    ability_ = SliteAbilityLoader::GetInstance().CreateAbility(SliteAbilityType::JS_ABILITY, abilityRecord->appName);
    if (ability_ == nullptr) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "JsAbility create fail");
        return MEMORY_MALLOC_ERROR;
    }
    ability_->SetToken(abilityRecord->token);
    ACELite::JsAsyncWork::SetAppQueueHandler(messageQueueId_);
    LOS_TaskUnlock();
    HILOG_INFO(HILOG_MODULE_AAFWK, "JsAbilityThread init done");
    return ERR_OK;
}

int32_t JsAbilityThread::ReleaseAbilityThread()
{
    ACELite::JsAsyncWork::SetAppQueueHandler(nullptr);
    if (state_ != AbilityThreadState::ABILITY_THREAD_INITIALIZED) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "JsAbilityThread release fail, the AbilityThread is not inited");
        return PARAM_CHECK_ERROR;
    }
    state_ = AbilityThreadState::ABILITY_THREAD_RELEASED;
    LOS_TaskDelete(appTaskId_);
    appTaskId_ = 0;
    osMessageQueueDelete(messageQueueId_);
    messageQueueId_ = nullptr;
    return ERR_OK;
}

osMessageQueueId_t JsAbilityThread::GetMessageQueueId() const
{
    return messageQueueId_;
}

UINT32 JsAbilityThread::GetAppTaskId() const
{
    return appTaskId_;
}

void JsAbilityThread::AppTaskHandler(UINT32 uwArg)
{
    auto messageQueueId = reinterpret_cast<osMessageQueueId_t>(uwArg);
    if (messageQueueId == nullptr) {
        return;
    }
    AbilityThread *defaultAbilityThread = nullptr;

    for (;;) {
        SliteAbilityInnerMsg innerMsg;
        uint8_t prio = 0;
        osStatus_t ret = osMessageQueueGet(messageQueueId, &innerMsg, &prio, osWaitForever);
        if (ret != osOK) {
            return;
        }
        AbilityThread *abilityThread = innerMsg.abilityThread;
        if (abilityThread == nullptr) {
            if (defaultAbilityThread == nullptr) {
                continue;
            }
            abilityThread = defaultAbilityThread;
        }
        LP_TaskBegin();
        switch (innerMsg.msgId) {
            case SliteAbilityMsgId::CREATE:
                defaultAbilityThread = abilityThread;
                abilityThread->HandleCreate(innerMsg.want);
                abilityThread->HandleRestore(innerMsg.abilitySavedData);
                ClearWant(innerMsg.want);
                AdapterFree(innerMsg.want);
                innerMsg.want = nullptr;
                break;
            case SliteAbilityMsgId::FOREGROUND:
                abilityThread->HandleForeground(innerMsg.want);
                ClearWant(innerMsg.want);
                AdapterFree(innerMsg.want);
                innerMsg.want = nullptr;
                break;
            case SliteAbilityMsgId::BACKGROUND:
                abilityThread->HandleBackground();
                break;
            case SliteAbilityMsgId::DESTROY:
                abilityThread->HandleSave(innerMsg.abilitySavedData);
                abilityThread->HandleDestroy();
                LP_TaskEnd();
                return; // here exit the loop, and abort all messages afterwards
            default:
                if (abilityThread->ability_ != nullptr) {
                    abilityThread->ability_->HandleExtraMessage(innerMsg);
                }
                break;
        }
        LP_TaskEnd();
    }
}
} // namespace AbilitySlite
} // namespace OHOS
