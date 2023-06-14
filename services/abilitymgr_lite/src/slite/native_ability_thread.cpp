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

#include "native_ability_thread.h"

#include <cstring>
#include "aafwk_event_error_code.h"
#include "ability_errors.h"
#include "ability_inner_message.h"
#include "ability_record_manager.h"
#include "adapter.h"
#include "ability_thread.h"
#include "abilityms_log.h"
#include "los_task.h"
#include "slite_ability_loader.h"

namespace OHOS {
namespace AbilitySlite {
static char g_NativeAppTask[] = "NativeAppTask";
constexpr int32_t APP_TASK_PRI = 25;
constexpr int32_t QUEUE_LENGTH = 32;

osMessageQueueId_t NativeAbilityThread::nativeQueueId_ = nullptr;
UINT32 NativeAbilityThread::nativeTaskId_ = UINT32_MAX;
SliteAbility *NativeAbilityThread::nativeAbility_ = nullptr;

NativeAbilityThread::NativeAbilityThread() = default;

NativeAbilityThread::~NativeAbilityThread()
{
    if (ability_ == nullptr) {
        return;
    }
#ifdef _MINI_MULTI_TASKS_
    delete ability_;
    ability_ = nullptr;
#else
    if ((ability_->bundleName_ == nullptr) || (strcmp(ability_->bundleName_, LAUNCHER_BUNDLE_NAME) != 0)) {
        delete ability_;
        ability_ = nullptr;
    }
#endif
}

int32_t NativeAbilityThread::InitAbilityThread(const AbilityRecord *abilityRecord)
{
    if (abilityRecord == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "NativeAbilityThread init fail, abilityRecord is null");
        return PARAM_NULL_ERROR;
    }
    if (abilityRecord->appName == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "NativeAbilityThread init fail, appName is null");
        return PARAM_NULL_ERROR;
    }
    if (state_ != AbilityThreadState::ABILITY_THREAD_UNINITIALIZED) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "NativeAbilityThread init fail, the AbilityThread is already inited");
        return PARAM_CHECK_ERROR;
    }

    if (nativeQueueId_ == nullptr) {
        nativeQueueId_ = osMessageQueueNew(QUEUE_LENGTH, sizeof(SliteAbilityInnerMsg), nullptr);
    }
    if (nativeQueueId_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "NativeAbilityThread init fail: messageQueueId is null");
        return MEMORY_MALLOC_ERROR;
    }

    HILOG_INFO(HILOG_MODULE_AAFWK, "CreateAppTask.");
    if (nativeTaskId_ == UINT32_MAX) {
        TSK_INIT_PARAM_S stTskInitParam = { 0 };
        LOS_TaskLock();
        stTskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC) (NativeAbilityThread::NativeAppTaskHandler);
        stTskInitParam.uwStackSize = NATIVE_TASK_STACK_SIZE;
        stTskInitParam.usTaskPrio = OS_TASK_PRIORITY_LOWEST - APP_TASK_PRI;
        stTskInitParam.pcName = g_NativeAppTask;
        stTskInitParam.uwResved = 0;
        stTskInitParam.uwArg = reinterpret_cast<UINT32>((uintptr_t) nativeQueueId_);
        uint32_t ret = LOS_TaskCreate(&nativeTaskId_, &stTskInitParam);
        if (ret != LOS_OK) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "NativeAbilityThread init fail: LOS_TaskCreate ret %{public}d", ret);
            osMessageQueueDelete(nativeQueueId_);
            LOS_TaskUnlock();
            return CREATE_APPTASK_ERROR;
        }
    }

    state_ = AbilityThreadState::ABILITY_THREAD_INITIALIZED;
#ifdef _MINI_MULTI_TASKS_
    ability_ = SliteAbilityLoader::GetInstance().CreateAbility(SliteAbilityType::NATIVE_ABILITY,
        abilityRecord->appName);
#else
    if (strcmp(abilityRecord->appName, LAUNCHER_BUNDLE_NAME) == 0) {
        if (nativeAbility_ == nullptr) {
            nativeAbility_ = SliteAbilityLoader::GetInstance().CreateAbility(SliteAbilityType::NATIVE_ABILITY,
                abilityRecord->appName);
        }
        ability_ = nativeAbility_;
    } else {
        ability_ = SliteAbilityLoader::GetInstance().CreateAbility(SliteAbilityType::NATIVE_ABILITY,
            abilityRecord->appName);
    }
#endif
    if (ability_ == nullptr) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "NativeAbility create fail");
        LOS_TaskUnlock();
        return MEMORY_MALLOC_ERROR;
    }
    ability_->SetToken(abilityRecord->token);
    LOS_TaskUnlock();
    HILOG_INFO(HILOG_MODULE_AAFWK, "NativeAbilityThread init done");
    return ERR_OK;
}

int32_t NativeAbilityThread::ReleaseAbilityThread()
{
    if (state_ != AbilityThreadState::ABILITY_THREAD_INITIALIZED) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "NativeAbilityThread release fail, the AbilityThread is not inited");
        return PARAM_CHECK_ERROR;
    }
    return ERR_OK;
}

osMessageQueueId_t NativeAbilityThread::GetMessageQueueId() const
{
    return nativeQueueId_;
}

UINT32 NativeAbilityThread::GetAppTaskId() const
{
    return nativeTaskId_;
}

void NativeAbilityThread::Reset()
{
    if (nativeQueueId_ != nullptr) {
        osMessageQueueDelete(nativeQueueId_);
    }
    nativeQueueId_ = nullptr;
    nativeTaskId_ = 0;
    nativeAbility_ = nullptr;
}

void NativeAbilityThread::NativeAppTaskHandler(UINT32 uwArg)
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
                break; // this task will be kept alive
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
