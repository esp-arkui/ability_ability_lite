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

#include <memory>
#include <mutex>
#include <map>
#include <thread>
#include "los_task.h"

class Task;

std::mutex g_taskRunLock;
std::mutex g_taskMapLock;
std::map<uint32_t, std::shared_ptr<Task>> g_taskMap {};
static __thread UINT32 curTaskId = 0;

class Task : public std::enable_shared_from_this<Task> {
public:
    explicit Task(uint32_t taskId)
    {
        taskId_ = taskId;
    }

    ~Task() = default;

    UINT32 Run(TSK_INIT_PARAM_S *taskInitParam)
    {
        taskInitParam_ = *taskInitParam;
        thread_ = std::make_shared<std::thread>(&Task::RunThread, this, &taskInitParam_);
        if (thread_ == nullptr) {
            return LOS_ERRNO_TSK_NO_MEMORY;
        }
        thread_->detach();
        return LOS_OK;
    }

private:
    uint32_t taskId_ { 0 };
    std::shared_ptr<std::thread> thread_ { nullptr };
    TSK_INIT_PARAM_S taskInitParam_ {};

    void RunThread(TSK_INIT_PARAM_S *taskInitParam) const
    {
        {
            std::lock_guard<std::mutex> lockGuard(g_taskRunLock);
            curTaskId = taskId_;
        }
        taskInitParam->pfnTaskEntry(taskInitParam->uwArg, 0, 0, 0);
    }
};

extern UINT32 LOS_CurTaskIDGet(VOID)
{
    return curTaskId;
}

extern UINT32 LOS_TaskCreate(UINT32 *taskID, TSK_INIT_PARAM_S *taskInitParam)
{
    static uint32_t taskIdCount = 0;
    auto taskPtr = std::make_shared<Task>(taskIdCount);
    *taskID = taskIdCount;


    UINT32 errorCode = taskPtr->Run(taskInitParam);
    if (errorCode == LOS_OK) {
        std::lock_guard<std::mutex> lockGuard(g_taskMapLock);
        g_taskMap[taskIdCount] = taskPtr;
    }

    taskIdCount++;
    return errorCode;
}

extern UINT32 LOS_TaskDelete(UINT32 taskID)
{
    std::lock_guard<std::mutex> lockGuard(g_taskMapLock);
    auto it = g_taskMap.find(taskID);
    if (it != g_taskMap.end()) {
        g_taskMap.erase(it);
    }
    return LOS_OK;
}

extern VOID LOS_TaskLock(VOID)
{
    g_taskRunLock.lock();
}

extern VOID LOS_TaskUnlock(VOID)
{
    g_taskRunLock.unlock();
}

void LP_TaskBegin()
{
}

void LP_TaskEnd()
{
}
