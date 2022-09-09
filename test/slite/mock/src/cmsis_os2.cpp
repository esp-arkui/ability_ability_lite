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

#include "cmsis_os2.h"
#include <queue>
#include <mutex>
#include <map>
#include <memory>
#include "adapter.h"
#include "securec.h"

namespace {
class Queue {
public:
    explicit Queue(uint32_t msgSize)
    {
        msgSize_ = msgSize;
    }

    ~Queue()
    {
        std::lock_guard<std::mutex> lockGuard(lock_);
        // no freeing the buffer to expose the problem.
        while (!queue_.empty()) {
            void *ptr = queue_.front();
            queue_.pop();
            // AdapterFree(ptr);
        }
    }

    osStatus_t Put(const void *msgPtr)
    {
        if (msgPtr == nullptr) {
            return osErrorParameter;
        }
        void *ptr = AdapterMalloc(msgSize_);
        if (ptr == nullptr) {
            return osErrorNoMemory;
        }
        errno_t err = memcpy_s(ptr, msgSize_, msgPtr, msgSize_);
        if (err != EOK) {
            return osErrorNoMemory;
        }
        std::lock_guard<std::mutex> lockGuard(lock_);
        queue_.push(ptr);
        return osOK;
    }

    osStatus_t Get(void *msgPtr)
    {
        if (msgPtr == nullptr) {
            return osErrorParameter;
        }
        void *ptr = nullptr;
        {
            std::lock_guard<std::mutex> lockGuard(lock_);
            if (queue_.empty()) {
                return osErrorParameter;
            }
            ptr = queue_.front();
            queue_.pop();
        }
        if (ptr == nullptr) {
            return osErrorNoMemory;
        }
        errno_t err = memcpy_s(msgPtr, msgSize_, ptr, msgSize_);
        if (err != EOK) {
            return osErrorNoMemory;
        }
        AdapterFree(ptr);
        return osOK;
    }

    bool Empty()
    {
        std::lock_guard<std::mutex> lockGuard(lock_);
        return queue_.empty();
    }

private:
    uint32_t msgSize_ { 0 };
    std::queue<void *> queue_;
    std::mutex lock_;
};

class QueueManager {
public:
    static QueueManager &GetInstance()
    {
        static QueueManager queueManager;
        return queueManager;
    }

    osMessageQueueId_t NewMessageQueue(uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr)
    {
        std::shared_ptr<Queue> queuePtr = std::make_shared<Queue>(msg_size);
        osMessageQueueId_t queueId = queuePtr.get();
        std::lock_guard<std::mutex> lockGuard(queueMapLock_);
        queueMap_.insert({ queueId, queuePtr });
        return queueId;
    }

    osStatus_t PutMessageQueue(osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout)
    {
        std::lock_guard<std::mutex> lockGuard(queueMapLock_);
        auto it = queueMap_.find(mq_id);
        if (it == queueMap_.end()) {
            return osErrorParameter;
        }
        return it->second->Put(msg_ptr);
    }

    osStatus_t GetMessageQueue(osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout)
    {
        if (timeout == osWaitForever) {
            while (true) {
                queueMapLock_.lock();
                auto it = queueMap_.find(mq_id);
                if (it == queueMap_.end()) {
                    queueMapLock_.unlock();
                    return osErrorParameter;
                }
                if (it->second->Empty()) {
                    queueMapLock_.unlock();
                    usleep(50000); // sleep 50ms
                    continue;
                }
                osStatus_t status = it->second->Get(msg_ptr);
                queueMapLock_.unlock();
                return status;
            }
        }
        return osErrorParameter;
    }

    osStatus_t osMessageQueueDelete(osMessageQueueId_t mq_id)
    {
        std::lock_guard<std::mutex> lockGuard(queueMapLock_);
        auto it = queueMap_.find(mq_id);
        if (it == queueMap_.end()) {
            return osErrorParameter;
        }
        queueMap_.erase(it);
        return osOK;
    }

private:
    QueueManager() = default;

    ~QueueManager() = default;

    std::map<osMessageQueueId_t, std::shared_ptr<Queue>> queueMap_;
    std::mutex queueMapLock_;
};
}

osMessageQueueId_t osMessageQueueNew(uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr)
{
    return QueueManager::GetInstance().NewMessageQueue(msg_count, msg_size, attr);
}

osStatus_t osMessageQueuePut(osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout)
{
    return QueueManager::GetInstance().PutMessageQueue(mq_id, msg_ptr, msg_prio, timeout);
}

osStatus_t osMessageQueueGet(osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout)
{
    return QueueManager::GetInstance().GetMessageQueue(mq_id, msg_ptr, msg_prio, timeout);
}

osStatus_t osMessageQueueDelete(osMessageQueueId_t mq_id)
{
    return QueueManager::GetInstance().osMessageQueueDelete(mq_id);
}
