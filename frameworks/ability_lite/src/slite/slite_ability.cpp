/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "slite_ability.h"
#include "slite_ability_state.h"
#include "abilityms_slite_client.h"
#include "utils.h"

namespace OHOS {
namespace AbilitySlite {
SliteAbility::SliteAbility(const char *bundleName)
{
    bundleName_= OHOS::Utils::Strdup(bundleName);
}

SliteAbility::~SliteAbility()
{
    AdapterFree(bundleName_);
}

void SliteAbility::OnCreate(const Want &want)
{
    abilityState_ = SLITE_STATE_INITIAL;
    (void)AbilityMsClient::GetInstance().SchedulerLifecycleDone(token_, SLITE_STATE_INITIAL);
}

void SliteAbility::OnRestoreData(AbilitySavedData *data)
{
    if (data == nullptr) {
        return;
    }
    data->Reset();
}

void SliteAbility::OnForeground(const Want &want)
{
    abilityState_ = SLITE_STATE_FOREGROUND;
    (void)AbilityMsClient::GetInstance().SchedulerLifecycleDone(token_, SLITE_STATE_FOREGROUND);
}

void SliteAbility::OnBackground()
{
    abilityState_ = SLITE_STATE_BACKGROUND;
    (void)AbilityMsClient::GetInstance().SchedulerLifecycleDone(token_, SLITE_STATE_BACKGROUND);
}

void SliteAbility::OnSaveData(AbilitySavedData *data)
{
}

void SliteAbility::OnDestroy()
{
    abilityState_ = SLITE_STATE_UNINITIALIZED;
    (void)AbilityMsClient::GetInstance().SchedulerLifecycleDone(token_, SLITE_STATE_UNINITIALIZED);
}

void SliteAbility::HandleExtraMessage(const SliteAbilityInnerMsg &innerMsg)
{
}

int32_t SliteAbility::GetState() const
{
    return abilityState_;
}
} // namespace AbilitySlite
} // namespace OHOS
