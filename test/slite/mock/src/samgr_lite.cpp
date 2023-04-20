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

#include "samgr_lite.h"

#include <mutex>
#include <map>
#include <thread>
#include <memory>
#include <atomic>
#include <functional>

#include "adapter.h"
#include "cmsis_os2.h"
#include "securec.h"

constexpr int32_t QUEUE_LENGTH = 32;

BOOL RegisterServiceDummy(Service *service);

BOOL RegisterFeatureDummy(const char *serviceName, Feature *feature);

IUnknown *GetFeatureApiDummy(const char *serviceName, const char *feature);

BOOL RegisterFeatureApiDummy(const char *service, const char *feature, IUnknown *publicApi);

namespace {
class FeatureInterface : public IUnknown {
public:
    explicit FeatureInterface(Feature *feature) : IUnknown()
    {
        feature_ = feature;
        this->IUnknown::QueryInterface = FeatureInterface::QueryInterface;
    }

    static int32_t QueryInterface(IUnknown *iUnknown, int version, void **target)
    {
        auto *featureInterface = (FeatureInterface *)(iUnknown);
        *target = featureInterface->feature_;
        return 0;
    }

private:
    Feature *feature_ { nullptr };
};


class ServiceThread {
public:
    explicit ServiceThread(Service *service, Identity identity)
    {
        service_ = service;
        identity_ = identity;
    }

    ~ServiceThread()
    {
        TerminateThread();
    }

    void RunThread()
    {
        thread_ = std::make_shared<std::thread>(&ServiceThread::ThreadMain, this);
    }

    void ThreadMain()
    {
        if (service_ == nullptr) {
            return;
        }
        while (!isTerminate) {
            Request request;
            uint8_t prio = 0;
            osStatus_t ret = osMessageQueueGet(identity_.queueId, &request, &prio, osWaitForever);
            if (ret != EOK) {
                return;
            }
            service_->MessageHandle(service_, &request);
            if (request.data != nullptr) {
                AdapterFree(request.data);
            }
        }
    }

    void TerminateThread()
    {
        if (!isTerminate) {
            isTerminate = true;
            osMessageQueueDelete(identity_.queueId);
            thread_->join();
        }
    }

    BOOL RegisterFeature(Feature *feature)
    {
        feature->OnInitialize(feature, service_, identity_);
        std::string featureNameStr = feature->GetName(feature);
        std::lock_guard<std::mutex> lockGuard(featureInterfaceMapLock_);
        std::shared_ptr<FeatureInterface> featureInterface = std::make_shared<FeatureInterface>(feature);
        featureInterfaceMap_.insert({ featureNameStr, featureInterface });
        return true;
    }

    IUnknown *GetFeatureApi(const char *feature)
    {
        std::string featureNameStr = feature;
        std::lock_guard<std::mutex> lockGuard(featureInterfaceMapLock_);
        auto it = featureInterfaceMap_.find(featureNameStr);
        if (it == featureInterfaceMap_.end()) {
            return nullptr;
        }
        return it->second.get();
    }

private:
    Service *service_ { nullptr };
    std::map<std::string, std::shared_ptr<FeatureInterface>> featureInterfaceMap_ {};
    std::mutex featureInterfaceMapLock_ {};
    std::atomic<bool> isTerminate = false;
    Identity identity_ {};
    std::shared_ptr<std::thread> thread_ { nullptr };
};

class SamgrLiteImpl : public SamgrLite {
public:
    static SamgrLiteImpl &GetInstance()
    {
        static SamgrLiteImpl samgrLite;
        return samgrLite;
    }

    BOOL RegisterServiceImpl(Service *service)
    {
        if (service == nullptr) {
            return false;
        }
        osMessageQueueId_t queueId = osMessageQueueNew(QUEUE_LENGTH, sizeof(Request), nullptr);
        if (queueId == nullptr) {
            return false;
        }
        static int16_t id = 0;
        Identity identity {
            .serviceId = id,
            .featureId = id,
            .queueId = queueId,
        };
        BOOL result = service->Initialize(service, identity);
        if (result) {
            id++;
            std::shared_ptr<ServiceThread> threadPtr = std::make_shared<ServiceThread>(service, identity);
            std::string serviceNameStr = service->GetName(service);
            std::lock_guard<std::mutex> lockGuard(serviceThreadMapLock_);
            serviceThreadMap_.insert({ serviceNameStr, threadPtr });
            threadPtr->RunThread();
        }
        return result;
    }

    BOOL RegisterFeatureImpl(const char *serviceName, Feature *feature)
    {
        if (serviceName == nullptr) {
            return false;
        }
        if (feature == nullptr) {
            return false;
        }
        std::string serviceNameStr = serviceName;
        std::lock_guard<std::mutex> lockGuard(serviceThreadMapLock_);
        auto it = serviceThreadMap_.find(serviceNameStr);
        if (it == serviceThreadMap_.end()) {
            return false;
        }
        return it->second->RegisterFeature(feature);
    }

    IUnknown *GetFeatureApiImpl(const char *serviceName, const char *feature)
    {
        if (serviceName == nullptr) {
            return nullptr;
        }
        if (feature == nullptr) {
            return nullptr;
        }
        std::string serviceNameStr = serviceName;
        std::lock_guard<std::mutex> lockGuard(serviceThreadMapLock_);
        auto it = serviceThreadMap_.find(serviceNameStr);
        if (it == serviceThreadMap_.end()) {
            return nullptr;
        }
        return it->second->GetFeatureApi(feature);
    }

    BOOL TerminateAllService()
    {
        std::lock_guard<std::mutex> lockGuard(serviceThreadMapLock_);
        serviceThreadMap_.clear();
        return true;
    }

private:
    std::mutex serviceThreadMapLock_;
    std::map<std::string, std::shared_ptr<ServiceThread>> serviceThreadMap_;

    SamgrLiteImpl() : SamgrLite()
    {
        this->SamgrLite::RegisterService = RegisterServiceDummy;
        this->SamgrLite::RegisterFeature = RegisterFeatureDummy;
        this->SamgrLite::GetFeatureApi = GetFeatureApiDummy;
        this->SamgrLite::RegisterFeatureApi = RegisterFeatureApiDummy;
    }
};
}

SamgrLite *SAMGR_GetInstance(void)
{
    return &SamgrLiteImpl::GetInstance();
}

BOOL RegisterServiceDummy(Service *service)
{
    return SamgrLiteImpl::GetInstance().RegisterServiceImpl(service);
}

BOOL RegisterFeatureDummy(const char *serviceName, Feature *feature)
{
    return SamgrLiteImpl::GetInstance().RegisterFeatureImpl(serviceName, feature);
}

IUnknown *GetFeatureApiDummy(const char *serviceName, const char *feature)
{
    return SamgrLiteImpl::GetInstance().GetFeatureApiImpl(serviceName, feature);
}

BOOL RegisterFeatureApiDummy(const char *service, const char *feature, IUnknown *publicApi)
{
    return true;
}

BOOL SAMGR_TerminateAllService()
{
    return SamgrLiteImpl::GetInstance().TerminateAllService();
}
