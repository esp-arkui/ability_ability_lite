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

#ifndef OHOS_ABILITY_SLITE_MISSION_INFO_H
#define OHOS_ABILITY_SLITE_MISSION_INFO_H

#include <stdint.h>

namespace OHOS {
namespace AbilitySlite {
class MissionInfo {
public:
    MissionInfo() = default;
    ~MissionInfo();

    MissionInfo(const MissionInfo &missionInfo);
    MissionInfo &operator=(const MissionInfo &missionInfo);

    const char *GetAppName() const
    {
        return appName_;
    }

    void SetAppName(const char *name);
private:
    char *appName_ = nullptr;
};

struct MissionInfoList {
    MissionInfoList() = default;
    ~MissionInfoList()
    {
        delete[] missionInfos;
    }
    MissionInfoList(const MissionInfoList &) = delete;
    MissionInfoList &operator=(const MissionInfoList &) = delete;

    MissionInfo *missionInfos = nullptr;
    uint32_t length = 0;
};
} // namespace AbilitySlite
} // namespace OHOS
#endif // OHOS_ABILITY_SLITE_MISSION_INFO_H
