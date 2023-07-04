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

#ifndef OHOS_ABILITY_SLITE_ABILITY_LOCK_GUARD_H
#define OHOS_ABILITY_SLITE_ABILITY_LOCK_GUARD_H

#include "cmsis_os2.h"

namespace OHOS {
namespace AbilitySlite {
class AbilityLockGuard {
public:
    explicit AbilityLockGuard(osMutexId_t& mutex): mutex_(mutex)
    {
        osMutexAcquire(mutex_, osWaitForever);
    }

    ~AbilityLockGuard()
    {
        osMutexRelease(mutex_);
    }

    AbilityLockGuard(const AbilityLockGuard&) = delete;
    AbilityLockGuard& operator=(const AbilityLockGuard&) = delete;
private:
    osMutexId_t& mutex_;
};
} // AbilitySlite
} // namespace OHOS
#endif // OHOS_ABILITY_SLITE_ABILITY_LOCK_GUARD_H
