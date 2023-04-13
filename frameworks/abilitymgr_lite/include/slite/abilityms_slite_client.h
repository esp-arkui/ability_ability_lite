/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITYMS_SLITE_CLIENT_H
#define OHOS_ABILITYMS_SLITE_CLIENT_H

#include "ability_service_interface.h"
#include "nocopyable.h"
#include "want.h"

namespace OHOS {
namespace AbilitySlite {
struct StartAbilityData {
    Want *want = nullptr;
    uint32_t curTask = 0;
};

class AbilityMsClient {
public:
    static AbilityMsClient &GetInstance()
    {
        static AbilityMsClient instance;
        return instance;
    }

    ~AbilityMsClient() = default;

    bool Initialize() const;

    int32_t StartAbility(const Want *want) const;

    int32_t TerminateAbility(uint64_t token) const;

    int32_t SchedulerLifecycleDone(uint64_t token, int state) const;

    int32_t ForceStopBundle(uint64_t token) const;

    int32_t ForceStop(char *bundleName) const;

    ElementName *GetTopAbility() const;

    void SetServiceIdentity(const Identity *identity);

private:
    AbilityMsClient() = default;

    AmsSliteInterface *amsProxy_ { nullptr };
    const Identity *identity_ { nullptr };

    DISALLOW_COPY_AND_MOVE(AbilityMsClient);
};
} // namespace AbilitySlite
} // namespace OHOS

#endif // OHOS_ABILITYMS_SLITE_CLIENT_H
