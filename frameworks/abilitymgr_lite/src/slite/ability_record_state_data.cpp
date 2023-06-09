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

#include "ability_record_state_data.h"

#include "adapter.h"
#include "utils.h"

namespace OHOS {
namespace AbilitySlite {
AbilityRecordStateData::AbilityRecordStateData(const char *appName, AbilityRecordState state)
{
    SetAppName(appName);
    state_ = state;
}

AbilityRecordStateData::AbilityRecordStateData(const AbilityRecordStateData &stateData)
    : AbilityRecordStateData(stateData.GetAppName(), stateData.GetState())
{
}

AbilityRecordStateData &AbilityRecordStateData::operator=(const AbilityRecordStateData &stateData)
{
    if (this != &stateData) {
        SetAppName(stateData.GetAppName());
        state_ = stateData.GetState();
    }
    return *this;
}

AbilityRecordStateData::~AbilityRecordStateData()
{
    AdapterFree(appName_);
}

void AbilityRecordStateData::SetAppName(const char *name)
{
    AdapterFree(appName_);
    appName_ = Utils::Strdup(name);
}
} // namespace AbilitySlite
} // namespace OHOS
