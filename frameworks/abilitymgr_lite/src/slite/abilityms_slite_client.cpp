/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "abilityms_slite_client.h"

#include "ability_errors.h"
#include "abilityms_log.h"
#include "adapter.h"
#include "cmsis_os2.h"
#include "los_task.h"
#include "samgr_lite.h"
#include "securec.h"
#include "want.h"
#include "utils.h"

namespace OHOS {
namespace AbilitySlite {
bool AbilityMsClient::Initialize() const
{
    if (amsProxy_ != nullptr) {
        return true;
    }
    int retry = RETRY_TIMES;
    while (retry--) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(AMS_SERVICE, AMS_SLITE_FEATURE);
        if (iUnknown == nullptr) {
            HILOG_ERROR(HILOG_MODULE_APP, "iUnknown is null");
            osDelay(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }

        (void)iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&amsProxy_);
        if (amsProxy_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_APP, "ams proxy is null");
            osDelay(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }
        return true;
    }
    return false;
}

int32_t AbilityMsClient::StartAbility(const Want *want) const
{
    if (want == nullptr || want->element == nullptr || !Initialize()) {
        return PARAM_CHECK_ERROR;
    }

    if (identity_ == nullptr) {
        return PARAM_CHECK_ERROR;
    }

    // The data and info will be freed in the service
    auto *data = static_cast<StartAbilityData *>(AdapterMalloc(sizeof(StartAbilityData)));
    if (data == nullptr) {
        return MEMORY_MALLOC_ERROR;
    }
    Want *info = static_cast<Want *>(AdapterMalloc(sizeof(Want)));
    if (info == nullptr) {
        return MEMORY_MALLOC_ERROR;
    }
    info->element = nullptr;
    info->data = nullptr;
    info->dataLength = 0;
    info->appPath = nullptr;
    SetWantElement(info, *(want->element));
    if (want->data != nullptr) {
        SetWantData(info, want->data, want->dataLength);
        HILOG_INFO(HILOG_MODULE_APP, "start ability with input data");
    } else {
        const char* defaultData = "data";
        SetWantData(info, defaultData, 5);
        HILOG_INFO(HILOG_MODULE_APP, "start ability with default data");
    }
    data->want = info;
    data->curTask = LOS_CurTaskIDGet();
    Request request = {
        .msgId = START_ABILITY,
        .len = sizeof(StartAbilityData),
        .data = data,
        .msgValue = 0,
    };
    return SAMGR_SendRequest(identity_, &request, nullptr);
}

int32_t AbilityMsClient::TerminateAbility(uint64_t token) const
{
    if (identity_ == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    Request request = {
        .msgId = TERMINATE_ABILITY,
        .len = 0,
        .data = nullptr,
        .msgValue = static_cast<uint32_t>(token & TRANSACTION_MSG_TOKEN_MASK),
    };
    return SAMGR_SendRequest(identity_, &request, nullptr);
}

int32_t AbilityMsClient::SchedulerLifecycleDone(uint64_t token, int32_t state) const
{
    if (identity_ == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    Request request = {
        .msgId = ABILITY_TRANSACTION_DONE,
        .len = 0,
        .data = nullptr,
        .msgValue = static_cast<uint32_t>((token & TRANSACTION_MSG_TOKEN_MASK) |
                                          (state << TRANSACTION_MSG_STATE_OFFSET)),
    };
    return SAMGR_SendRequest(identity_, &request, nullptr);
}

int32_t AbilityMsClient::ForceStopBundle(uint64_t token) const
{
    if (identity_ == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    Request request = {
        .msgId = TERMINATE_APP,
        .len = 0,
        .data = nullptr,
        .msgValue = static_cast<uint32_t>(token & TRANSACTION_MSG_TOKEN_MASK),
    };
    return SAMGR_SendRequest(identity_, &request, nullptr);
}

ElementName *AbilityMsClient::GetTopAbility() const
{
    if (!Initialize()) {
        return nullptr;
    }
    return amsProxy_->GetTopAbility();
}

int32_t AbilityMsClient::ForceStop(char *bundleName) const
{
    if (identity_ == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    char *name = Utils::Strdup(bundleName);
    Request request = {
        .msgId = TERMINATE_APP_BY_BUNDLENAME,
        .len = (int16)strlen(name),
        .data = reinterpret_cast<void *>(name),
    };

    return SAMGR_SendRequest(identity_, &request, nullptr);
}

int32_t AbilityMsClient::AddAbilityRecordObserver(AbilityRecordObserver *observer)
{
    if (identity_ == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    Request request = {
        .msgId = ADD_ABILITY_RECORD_OBSERVER,
        .msgValue = reinterpret_cast<uint32>(observer),
    };

    return SAMGR_SendRequest(identity_, &request, nullptr);
}

int32_t AbilityMsClient::RemoveAbilityRecordObserver(AbilityRecordObserver *observer)
{
    if (identity_ == nullptr) {
        return PARAM_CHECK_ERROR;
    }
    Request request = {
        .msgId = REMOVE_ABILITY_RECORD_OBSERVER,
        .msgValue = reinterpret_cast<uint32>(observer),
    };

    return SAMGR_SendRequest(identity_, &request, nullptr);
}

void AbilityMsClient::SetServiceIdentity(const Identity *identity)
{
    identity_ = identity;
}
} // namespace AbilitySlite
} // namespace OHOS
