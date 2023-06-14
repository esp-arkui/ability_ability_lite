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

#include "ability_record_observer_manager.h"

namespace OHOS {
namespace AbilitySlite {
AbilityRecordObserverManager &AbilityRecordObserverManager::GetInstance()
{
    static AbilityRecordObserverManager instance;
    return instance;
}

void AbilityRecordObserverManager::AddObserver(AbilityRecordObserver *observer)
{
    for (auto it = observers_.Begin(); it != observers_.End(); it = it->next_) {
        if (it->value_ == observer) {
            return;
        }
    }
    observers_.PushBack(observer);
}

void AbilityRecordObserverManager::RemoveObserver(AbilityRecordObserver *observer)
{
    for (auto it = observers_.Begin(); it != observers_.End(); it = it->next_) {
        if (it->value_ == observer) {
            observers_.Remove(it);
            return;
        }
    }
}

void AbilityRecordObserverManager::NotifyAbilityRecordStateChanged(const AbilityRecordStateData &data)
{
    for (auto it = observers_.Begin(); it != observers_.End(); it = it->next_) {
        if (it->value_ != nullptr) {
            it->value_->OnAbilityRecordStateChanged(data);
        }
    }
}

void AbilityRecordObserverManager::NotifyAbilityRecordCleanup(char *appName)
{
    for (auto it = observers_.Begin(); it != observers_.End(); it = it->next_) {
        if (it->value_ != nullptr) {
            it->value_->OnAbilityRecordCleanup(appName);
        }
    }
}
} // namespace AbilitySlite
} // namespace OHOS
