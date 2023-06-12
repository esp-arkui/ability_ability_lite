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

#include "ability_manager_client.h"
#include "abilityms_slite_client.h"

namespace OHOS {
namespace AbilitySlite {
AbilityManagerClient &AbilityManagerClient::GetInstance()
{
    static AbilityManagerClient instance;
    return instance;
}

void AbilityManagerClient::AddAbilityRecordObserver(AbilityRecordObserver *observer)
{
    AbilityMsClient::GetInstance().AddAbilityRecordObserver(observer);
}

void AbilityManagerClient::RemoveAbilityRecordObserver(AbilityRecordObserver *observer)
{
    AbilityMsClient::GetInstance().RemoveAbilityRecordObserver(observer);
}

int32_t AbilityManagerClient::TerminateAll(const char* excludedBundleName)
{
    return AbilityMsClient::GetInstance().TerminateAll(excludedBundleName);
}

MissionInfoList *AbilityManagerClient::GetMissionInfos(uint32_t maxNum) const
{
    return AbilityMsClient::GetInstance().GetMissionInfos(maxNum);
}
}
}
