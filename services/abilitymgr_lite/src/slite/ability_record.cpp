/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "ability_record.h"

#include "adapter.h"
#include "utils.h"

namespace OHOS {
namespace AbilitySlite {
AbilityData::AbilityData() = default;

AbilityData::~AbilityData()
{
    AdapterFree(wantData);
}

AbilityRecord::AbilityRecord() = default;

AbilityRecord::~AbilityRecord()
{
    AdapterFree(appName);
    AdapterFree(appPath);
    delete abilityData;
    abilityData = nullptr;
    delete abilitySavedData;
    abilitySavedData = nullptr;
    delete abilityThread;
    abilityThread = nullptr;
}

void AbilityRecord::SetAppName(const char *name)
{
    AdapterFree(appName);
    appName = Utils::Strdup(name);
}

void AbilityRecord::SetAppPath(const char *path)
{
    AdapterFree(appPath);
    appPath = Utils::Strdup(path);
}

void AbilityRecord::SetWantData(const void *wantData, uint16_t wantDataSize)
{
    if (abilityData == nullptr) {
        abilityData = new AbilityData;
    }
    AdapterFree(abilityData->wantData);
    abilityData->wantData = Utils::Memdup(wantData, wantDataSize);
    if (abilityData->wantData == nullptr) {
        abilityData->wantDataSize = 0;
        return;
    }
    abilityData->wantDataSize = wantDataSize;
}
} // namespace AbilitySlite
} // namespace OHOS
