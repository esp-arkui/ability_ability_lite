/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "abilityms_client.h"

#include "ability_errors.h"
#include "ability_service_interface.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "samgr_lite.h"
#include "want_utils.h"

namespace OHOS {
const unsigned int ERROR_SLEEP_TIMES = 300000;
const unsigned int RETRY_TIMES = 20;

static int32_t Callback(void *owner, int code, IpcIo *reply)
{
    return 0;
}

bool AbilityMsClient::Initialize() const
{
    if (amsProxy_ != nullptr) {
        return true;
    }
    int retry = RETRY_TIMES;
    IUnknown *iUnknown = nullptr;
    while (retry--) {
        iUnknown = SAMGR_GetInstance()->GetFeatureApi(AMS_SERVICE, AMS_FEATURE);
        if (iUnknown == nullptr) {
            usleep(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }
        (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&amsProxy_);
        if (amsProxy_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_APP, "ams proxy is null");
            usleep(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }
        return true;
    }
    if (iUnknown == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "iUnknown is null");
    }
    return false;
}

int AbilityMsClient::SchedulerLifecycleDone(uint64_t token, int state) const
{
    if (amsProxy_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    IpcIo req;
    char data[MAX_IO_SIZE];
    IpcIoInit(&req, data, MAX_IO_SIZE, 0);
    WriteUint64(&req, token);
    WriteInt32(&req, state);
    return amsProxy_->Invoke(amsProxy_, ABILITY_TRANSACTION_DONE, &req, nullptr, Callback);
}

int AbilityMsClient::ScheduleAms(const Want *want, uint64_t token, const SvcIdentity *sid, int commandType) const
{
    if (amsProxy_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    IpcIo req;
    char data[MAX_IO_SIZE];
    IpcIoInit(&req, data, MAX_IO_SIZE, 3);
    if (token != 0) {
        WriteUint64(&req, token);
    }
    if (sid != nullptr) {
        bool ret = WriteRemoteObject(&req, sid);
        if (!ret) {
            return SERIALIZE_ERROR;
        }
#ifdef __LINUX__
        if (commandType == ATTACH_BUNDLE) {
            pid_t pid = getpid();
            WriteUint64(&req, pid);
        }
#endif
    }
    if (want != nullptr && !SerializeWant(&req, want)) {
        return SERIALIZE_ERROR;
    }
    return amsProxy_->Invoke(amsProxy_, commandType, &req, nullptr, Callback);
}
} //  namespace OHOS
