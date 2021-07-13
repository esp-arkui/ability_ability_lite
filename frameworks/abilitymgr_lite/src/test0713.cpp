
#include "ability_service_manager.h"
#include <log.h>
#include "ability_kit_command.h"
#include "ability_service_interface.h"
#include "abilityms_client.h"
#include "ohos_errno.h"
#include "abilityms_client.h"
#include "ohos_errno.h"
#include "want.h"

namespace OHOS {
AbilityServiceManager::~AbilityServiceManager()
{
    for (const auto &storeArgs : storeList_) {
        if (storeArgs == nullptr) {
            continue;
        }
        if (storeArgs->sid != nullptr) {
            UnregisterIpcCallback(*(storeArgs->sid));
            delete storeArgs->sid;
        }
        delete storeArgs;
    }
    storeList_.clear();
}

int AbilityServiceManager::ConnectAbility(const Want &want, const IAbilityConnection &conn, uint64_t token, void *storeArg)
{
    if (conn.OnAbilityDisconnectDone == nullptr || conn.OnAbilityConnectDone == nullptr) {
        HILOG_INFO(HILOG_MODULE_APP, "IAbilityConnection callback func is null");
        return LITEIPC_EINVAL;
    }

    StoreArgs *storeArgs = AddStoreArgs(conn, storeArg);
    if (storeArgs == nullptr) {
        return LITEIPC_EINVAL;
    }
    AbilityMsClient::GetInstance().Initialize();
    int32_t result = RegisterIpcCallback(ConnectAbilityCallBack, 0, IPC_WAIT_FOREVER, storeArgs->sid, storeArgs);
    if (result != LITEIPC_OK) {
        RemoveStoreArgs(nullptr, storeArgs);
        delete storeArgs->sid;
        delete storeArgs;
        return result;
    }
    result = AbilityMsClient::GetInstance().ScheduleAms(&want, token, storeArgs->sid, CONNECT_ABILITY);
    if (result != EC_SUCCESS) {
        UnregisterIpcCallback(*(storeArgs->sid));
        RemoveStoreArgs(nullptr, storeArgs);
        delete storeArgs->sid;
        delete storeArgs;
    }
    return result;
}

bool AbilityMsClient::Initialize() const
{
    if (amsProxy_ != nullptr) {
        return true;
    }
    int retry = RETRY_TIMES;
    while (retry--) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(AMS_SERVICE, AMS_FEATURE);
        if (iUnknown == nullptr) {
            HILOG_ERROR(HILOG_MODULE_APP, "iUnknown is null");
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

    return false;
}

int AbilityMsClient::SchedulerLifecycleDone(uint64_t token, int state) const
{
    if (amsProxy_ == nullptr) {
        return PARAM_NULL_ERROR;
    }IpcIo req;
    char data[IPC_IO_DATA_MAX];IpcIoInit(&req, data, IPC_IO_DATA_MAX, 0);IpcIoPushUint64(&req, token);
    IpcIoPushInt32(&req, state);
    return amsProxy_->Invoke(amsProxy_, ABILITY_TRANSACTION_DONE, &req, nullptr, nullptr);
}

int AbilityMsClient::ScheduleAms(const Want *want, uint64_t token, const SvcIdentity *sid, int commandType) const
{
    if(amsProxy_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    IpcIo req;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&req, data, IPC_IO_DATA_MAX, 3);
    if (token != 0) {
        IpcIoPushUint64(&req, token);
    }
    if(sid != nullptr) {
        IpcIoPushSvc(&req, sid);
#ifdef __LINUX__
        if (commandType == ATTACH_BUNDLE) {
            pid_t pid = getpid();
            IpcIoPushUint64(&req, pid);
        }
#endif
    }
    if(want != nullptr && !SerializeWant(&req, want)) {
        return SERIALIZE_ERROR;
    }return amsProxy_->Invoke(amsProxy_, commandType, &req, nullptr, nullptr);
}
int AbilityServiceManager::DisconnectAbility(const IAbilityConnection &conn, uint64_t token)
{
    StoreArgs *storeArgs = RemoveStoreArgs(&conn, nullptr);
    if (storeArgs == nullptr) {
        HILOG_INFO(HILOG_MODULE_APP, "no need to disconnect");
        return LITEIPC_EINVAL;
    }
    int result = AbilityMsClient::GetInstance().ScheduleAms(nullptr, token, storeArgs->sid, DISCONNECT_ABILITY);

    ClearStore(storeArgs);
    return result;
}

StoreArgs *AbilityServiceManager::AddStoreArgs(const IAbilityConnection &conn, void *storeArg)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (GetStoreArgs(conn) != nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "func has already used");
        return nullptr;
    }
    StoreArgs *storeArgs = new StoreArgs();
    storeArgs->conn = &conn;
    storeArgs->storeArg = storeArg;
    storeArgs->sid = new SvcIdentity();
    storeList_.emplace_back(storeArgs);

    return storeArgs;
}

StoreArgs *AbilityServiceManager::GetStoreArgs(const IAbilityConnection &conn) const
{
    for (const auto storeArgs : storeList_) {
        if (storeArgs->conn == &conn) {
            return storeArgs;
        }
    }
    return nullptr;
}

StoreArgs *AbilityServiceManager::RemoveStoreArgs(const IAbilityConnection *conn, StoreArgs *storeArgs)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (storeArgs == nullptr) {
        storeArgs = GetStoreArgs(*conn);
    }
    storeList_.remove(storeArgs);

    return storeArgs;
}

int32_t AbilityServiceManager::ConnectAbilityCallBack(const IpcContext* context, void *ipcMsg, IpcIo *data, void *arg)
{
    // param check
    StoreArgs *storeArgs = static_cast<StoreArgs *>(arg);
    if (storeArgs == nullptr || ipcMsg == nullptr || storeArgs->conn == nullptr ||
        storeArgs->conn->OnAbilityConnectDone == nullptr || storeArgs->conn->OnAbilityDisconnectDone == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "storeArgs or callback func or ipcMsg is null");
        AbilityServiceManager::GetInstance().RemoveStoreArgs(nullptr, storeArgs);
        ClearStore(storeArgs);
        FreeBuffer(nullptr, ipcMsg);
        return LITEIPC_EINVAL;
    }

    // parse funcId
    uint64_t funcId = 0;
    GetCode(ipcMsg, &funcId);
    int resultCode = (funcId != SCHEDULER_ABILITY_CONNECT_FAIL) ? 0 : -1;
    // parse service sid
    SvcIdentity *serviceSid = (funcId == SCHEDULER_ABILITY_CONNECT) ? IpcIoPopSvc(data) : nullptr;
    if ((funcId == SCHEDULER_ABILITY_CONNECT) && (serviceSid == nullptr)) {
        resultCode = -1;
    }
    // parse element
    ElementName elementName = { nullptr };
    if (!DeserializeElement(&elementName, data)) {
        resultCode = -1;
    }
    if (funcId == SCHEDULER_ABILITY_DISCONNECT)storeArgs->conn->OnAbilityDisconnectDone(&elementName, resultCode, storeArgs->storeArg);
else storeArgs->conn->OnAbilityConnectDone(&elementName, serviceSid, resultCode,  storeArgs->storeArg);
    if (funcId != SCHEDULER_ABILITY_CONNECT || resultCode == -1) {
        AbilityServiceManager::GetInstance().RemoveStoreArgs(nullptr, storeArgs);
        ClearStore(storeArgs);
    }
#ifdef __LINUX__
    AdapterFree(serviceSid);
    serviceSid = nullptr;
#endif
    ClearElement(&elementName);
    FreeBuffer(nullptr, ipcMsg);
    return LITEIPC_OK;
}

void AbilityServiceManager::ClearStore(StoreArgs *storeArgs)
{
    if ((storeArgs == nullptr) || (storeArgs->sid == nullptr)) {
        HILOG_INFO(HILOG_MODULE_APP, "no need to clear storeArgs");
        return;
		}
    UnregisterIpcCallback(*(storeArgs->sid));
    delete storeArgs->sid;
    delete storeArgs;
}
int32_t AbilityKit::DetectPhoneApp(const char *callingPkgName, const char *calledPkgName, uint32_t successCallbackFunc,
    uint32_t failCallbackFunc, uint32_t context)
{
    return AbilityService::GetInstance().DetectPhoneApp(callingPkgName, calledPkgName, successCallbackFunc,
        failCallbackFunc, context);
}

int32_t AbilityKit::DetectResourceRelease()
{
    return AbilityService::GetInstance().DetectResourceRelease();
}

int32_t AbilityKit::SendMsgResourceRelease()
{
    return AbilityService::GetInstance().SendMsgResourceRelease();
}
} // namespace OHOS

#ifdef APP_PLATFORM_WATCHGT
extern "C" {
int32_t RegisterReceiver(const char *bundleName, SuccessCallback success, FailCallback fail)
{
    return OHOS::AbilityKit::RegisterReceiver(bundleName, success, fail, 0, 0, 0);
}

int32_t UnregisterReceiver(const char *bundleName)
{
    return OHOS::AbilityKit::UnregisterReceiver(bundleName);
}

int32_t SendMsgToPeerApp(bool isLocalMsg, const char *callingPkgName, const char *calledPkgName,
    const uint8_t *msgBody, uint32_t length)
{
    return OHOS::AbilityKit::SendMsgToPeerApp(isLocalMsg, callingPkgName, calledPkgName, msgBody, length, 0, 0, 0);
}
} // namespace OHOS