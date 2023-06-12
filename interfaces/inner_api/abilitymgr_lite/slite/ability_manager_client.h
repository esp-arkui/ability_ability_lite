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

#ifndef OHOS_ABILITY_SLITE_ABILITY_MANAGER_CLIENT_H
#define OHOS_ABILITY_SLITE_ABILITY_MANAGER_CLIENT_H

#include <stdint.h>

#include "ability_record_observer.h"
#include "mission_info.h"

namespace OHOS {
namespace AbilitySlite {
class AbilityManagerClient {
public:
    static AbilityManagerClient &GetInstance();

    void AddAbilityRecordObserver(AbilityRecordObserver *observer);
    void RemoveAbilityRecordObserver(AbilityRecordObserver *observer);

    int32_t TerminateAll(const char* excludedBundleName);

    MissionInfoList *GetMissionInfos(uint32_t maxNum = 0) const;

private:
    AbilityManagerClient() = default;
    ~AbilityManagerClient() = default;
};
} // AbilitySlite
} // namespace OHOS
#endif // OHOS_ABILITY_SLITE_ABILITY_MANAGER_CLIENT_H
