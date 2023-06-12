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

#ifndef OHOS_ABILITY_SLITE_ABILITY_RECORD_STATE_DATA_H
#define OHOS_ABILITY_SLITE_ABILITY_RECORD_STATE_DATA_H

#include "ability_record_state.h"

namespace OHOS {
namespace AbilitySlite {
class AbilityRecordStateData {
public:
    AbilityRecordStateData() = default;
    AbilityRecordStateData(const char *appName, AbilityRecordState state);
    AbilityRecordStateData(const AbilityRecordStateData &stateData);
    AbilityRecordStateData &operator=(const AbilityRecordStateData &stateData);
    ~AbilityRecordStateData();

    void SetAppName(const char *name);

    const char *GetAppName() const
    {
        return appName_;
    }

    void SetState(AbilityRecordState state)
    {
        state_ = state;
    }

    AbilityRecordState GetState() const
    {
        return state_;
    }

private:
    char *appName_ = nullptr;
    AbilityRecordState state_ = SCHEDULE_STOP;
};
} // AbilitySlite
} // namespace OHOS
#endif // OHOS_ABILITY_SLITE_ABILITY_RECORD_STATE_DATA_H
