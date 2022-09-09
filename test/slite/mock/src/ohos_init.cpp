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

#include "ohos_init.h"
#include "samgr_lite.h"

OhosInitMgr &OhosInitMgr::GetInstance()
{
    static OhosInitMgr ohosInitMgr;
    return ohosInitMgr;
}

void OhosInitMgr::Register(InitCall initFunc)
{
    std::lock_guard<std::mutex> lockGuard(initFuncListLock_);
    if (!isInit) {
        initFuncList_.push_back(initFunc);
    }
}

void OhosInitMgr::RegisterFeature(InitCall initFunc)
{
    std::lock_guard<std::mutex> lockGuard(initFuncListLock_);
    if (!isInit) {
        initFeatureFuncList_.push_back(initFunc);
    }
}

void OhosInitMgr::Init()
{
    std::lock_guard<std::mutex> lockGuard(initFuncListLock_);
    if (!isInit) {
        for (auto &func : initFuncList_) {
            func();
        }
        for (auto &func : initFeatureFuncList_) {
            func();
        }
        isInit = true;
    }
}

void OhosInitMgr::Uninit()
{
    std::lock_guard<std::mutex> lockGuard(initFuncListLock_);
    if (isInit) {
        SAMGR_TerminateAllService();
        isInit = false;
    }
}