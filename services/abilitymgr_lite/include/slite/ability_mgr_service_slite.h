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

#ifndef OHOS_ABILITY_SLITE_ABILITY_MGR_SERVICE_SLITE_H
#define OHOS_ABILITY_SLITE_ABILITY_MGR_SERVICE_SLITE_H

#include "ability_record_manager.h"
#include "feature.h"
#include "nocopyable.h"

namespace OHOS {
namespace AbilitySlite {
class AbilityMgrServiceSlite : public Service, public Feature {
public:
    static AbilityMgrServiceSlite *GetInstance();

    const Identity *GetIdentity();

    static int32_t StartAbility(const Want *want);

    static int32_t TerminateAbility(uint64_t token);

    static int32_t SchedulerLifecycleDone(uint64_t token, int state);

    static int32_t ForceStopBundle(uint64_t token);

    static ElementName *GetTopAbility();

    static void *GetMissionInfos(uint32_t maxNum);

private:
    AbilityMgrServiceSlite();

    ~AbilityMgrServiceSlite();

    static const char *GetFeatureName(Feature *feature);

    static void OnFeatureInitialize(Feature *feature, Service *parent, Identity identity);

    static void OnFeatureStop(Feature *feature, Identity identity);

    static BOOL OnFeatureMessage(Feature *feature, Request *request);

    static const char *GetServiceName(Service *service);

    static BOOL ServiceInitialize(Service *service, Identity identity);

    static TaskConfig GetServiceTaskConfig(Service *service);

    static BOOL ServiceMessageHandle(Service *service, Request *request);

    static void InitAbilityThreadLoad();

    static void InitAbilityLoad();

private:
    Identity serviceIdentity_ {};
    Identity featureIdentity_ {};
    DISALLOW_COPY_AND_MOVE(AbilityMgrServiceSlite);
};
} // namespace AbilitySlite
} // namespace OHOS
#endif // OHOS_ABILITY_SLITE_ABILITY_MGR_SERVICE_SLITE_H
