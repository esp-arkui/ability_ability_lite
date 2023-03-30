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

#include "ability_thread.h"

#include "ability_errors.h"
#include "ability_manager_inner.h"
#include "slite_ability_state.h"
#include "los_task.h"
#include "ability_inner_message.h"
#include "adapter.h"

extern "C" void LP_TaskBegin();
extern "C" void LP_TaskEnd();

namespace OHOS {
namespace AbilitySlite {

AbilityThread::AbilityThread() = default;

AbilityThread::~AbilityThread()
{
    delete ability_;
    ability_ = nullptr;
    if (messageQueueId_ != nullptr) {
        osMessageQueueDelete(messageQueueId_);
    }
    messageQueueId_ = nullptr;
}

void AbilityThread::AppTaskHandler(UINT32 uwArg)
{
    auto messageQueueId = reinterpret_cast<osMessageQueueId_t>(uwArg);
    if (messageQueueId == nullptr) {
        return;
    }

    for (;;) {
        AbilityInnerMsg innerMsg;
        uint8_t prio = 0;
        osStatus_t ret = osMessageQueueGet(messageQueueId, &innerMsg, &prio, osWaitForever);
        if (ret != osOK) {
            return;
        }
        AbilityThread *abilityThread = innerMsg.abilityThread;
        if (abilityThread == nullptr) {
            continue;
        }
        LP_TaskBegin();
        switch (innerMsg.msgId) {
            case AbilityMsgId::CREATE:
                abilityThread->HandleCreate(innerMsg.want);
                ClearWant(innerMsg.want);
                AdapterFree(innerMsg.want);
                innerMsg.want = nullptr;
                break;
            case AbilityMsgId::FOREGROUND:
                abilityThread->HandleForeground(innerMsg.want);
                ClearWant(innerMsg.want);
                AdapterFree(innerMsg.want);
                innerMsg.want = nullptr;
                break;
            case AbilityMsgId::BACKGROUND:
                abilityThread->HandleBackground();
                break;
            case AbilityMsgId::DESTROY:
                abilityThread->HandleDestroy();
                LP_TaskEnd();
                return; // here exit the loop, and abort all messages afterwards
            default:
                break;
        }
        LP_TaskEnd();
    }
}

int32_t AbilityThread::HandleCreate(const Want *want)
{
    if (ability_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (want == nullptr) {
        return PARAM_NULL_ERROR;
    }
    ability_->OnCreate(*want);
    return ERR_OK;
}

int32_t AbilityThread::HandleForeground(const Want *want)
{
    if (ability_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (want == nullptr) {
        return PARAM_NULL_ERROR;
    }
    ability_->OnForeground(*want);
    return ERR_OK;
}

int32_t AbilityThread::HandleBackground()
{
    if (ability_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    ability_->OnBackground();
    return ERR_OK;
}

int32_t AbilityThread::HandleDestroy()
{
    if (ability_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    ability_->OnDestroy();
    return ERR_OK;
}
} // namespace AbilitySlite
} // namespace OHOS