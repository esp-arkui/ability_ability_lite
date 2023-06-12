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

#ifndef OHOS_ABILITY_SLITE_ABILITY_RECORD_OBSERVER_MANAGER_H
#define OHOS_ABILITY_SLITE_ABILITY_RECORD_OBSERVER_MANAGER_H

#include "ability_record_observer.h"
#include "nocopyable.h"
#include "stdint.h"
#include "utils_list.h"

namespace OHOS {
namespace AbilitySlite {
class AbilityRecordObserverManager : public NoCopyable {
public:
    static AbilityRecordObserverManager &GetInstance();

    void AddObserver(AbilityRecordObserver *observer);

    void RemoveObserver(AbilityRecordObserver *observer);

    void NotifyAbilityRecordStateChanged(const AbilityRecordStateData &data);

    void NotifyAbilityRecordCleanup(char *appName);

private:
    AbilityRecordObserverManager() = default;
    ~AbilityRecordObserverManager() = default;

    List<AbilityRecordObserver *> observers_;
};

} // namespace AbilitySlite
} // namespace OHOS
#endif  // OHOS_ABILITY_SLITE_ABILITY_RECORD_OBSERVER_MANAGER_H
