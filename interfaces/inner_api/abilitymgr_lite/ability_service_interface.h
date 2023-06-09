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

#ifndef OHOS_AMS_INTERFACE_H
#define OHOS_AMS_INTERFACE_H

#include "feature.h"
#ifndef __LITEOS_M__
#include "iproxy_server.h"
#else
#include "iunknown.h"
#endif
#include "want.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

const char AMS_SERVICE[] = "abilityms";
const char AMS_FEATURE[] = "AmsFeature";
const char AMS_SLITE_FEATURE[] = "AmsSliteFeature";
const char AMS_INNER_FEATURE[] = "AmsInnerFeature";

#ifdef __LITEOS_M__
constexpr uint32_t TRANSACTION_MSG_TOKEN_MASK = 0xFFFF;
constexpr uint32_t TRANSACTION_MSG_STATE_MASK = 0xFFFF;
constexpr uint32_t TRANSACTION_MSG_STATE_OFFSET = 16;
#endif

enum AmsCommand {
    START_ABILITY = 0,
    TERMINATE_ABILITY,
    ATTACH_BUNDLE,
    CONNECT_ABILITY,
    CONNECT_ABILITY_DONE,
    DISCONNECT_ABILITY,
    DISCONNECT_ABILITY_DONE,
    ABILITY_TRANSACTION_DONE,
    TERMINATE_SERVICE,
    START_ABILITY_WITH_CB,
    INNER_BEGIN,
    TERMINATE_APP = INNER_BEGIN,
    DUMP_ABILITY,
    TERMINATE_APP_BY_BUNDLENAME,
    ADD_ABILITY_RECORD_OBSERVER,
    REMOVE_ABILITY_RECORD_OBSERVER,
    TERMINATE_MISSION,
    TERMINATE_ALL,
    COMMAND_END,
};

#ifndef __LITEOS_M__
/**
 * Expose to start or terminate ability.
 */
struct AmsInterface {
    INHERIT_SERVER_IPROXY;
    int32 (*StartAbility)(const Want *want);
    int32 (*TerminateAbility)(uint64_t token);
    int32 (*ConnectAbility)(const Want *want, SvcIdentity *svc, uint64_t token);
    int32 (*DisconnectAbility)(SvcIdentity *svc, uint64_t token);
    int32 (*StopAbility)(const Want *want);
};

struct AmsInnerInterface {
    INHERIT_SERVER_IPROXY;
    int32 (*StartKeepAliveApps)();
    int32 (*TerminateApp)(const char *bundleName);
};
#else
struct AmsSliteInterface {
    INHERIT_IUNKNOWN;
    int32_t (*StartAbility)(const Want *want);
    int32_t (*TerminateAbility)(uint64_t token);
    int32_t (*SchedulerLifecycleDone)(uint64_t token, int state);
    int32_t (*ForceStopBundle)(uint64_t token);
    ElementName *(*GetTopAbility)();
    void *(*GetMissionInfos)(uint32_t maxNum);
};
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif // OHOS_AMS_INTERFACE_H
