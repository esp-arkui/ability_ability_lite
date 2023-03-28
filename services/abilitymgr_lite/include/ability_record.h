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

#ifndef OHOS_ABILITY_RECORD_H
#define OHOS_ABILITY_RECORD_H

#ifdef __LITEOS_M__
#include "cmsis_os.h"
#endif
#include "ability_info.h"

namespace OHOS {
class JsAppHost;

constexpr int FAIL_CALLBACK_ERRORCODE = 200;

/* States-feedback from launcher to AMS */
typedef enum {
    SCHEDULE_INITED,
    SCHEDULE_INACTIVE,
    SCHEDULE_ACTIVE,
    SCHEDULE_BACKGROUND,
    SCHEDULE_STOP
} AbilityState;

typedef enum {
    STATE_JS_RUNNING,
    STATE_NATIVE_RUNNING,
    STATE_JS_JUMP_NATIVE,
    STATE_NATIVE_JUMP_JS,
    STATE_JS_JUMP_JS,
} AppState;

struct AbilityData {
    AbilityData();

    ~AbilityData();

    void *wantData = nullptr;
    uint16_t wantDataSize = 0;
};

struct AbilityRecord {
public:
    AbilityRecord();

    ~AbilityRecord();

    void SetAppName(const char *name);

    void SetAppPath(const char *path);

    void SetWantData(const void *wantData, uint16_t wantDataSize);

    char *appName = nullptr;
    char *appPath = nullptr;
    AbilityData *abilityData = nullptr;
    JsAppHost *jsAppHost = nullptr;
    osMessageQueueId_t jsAppQueueId = nullptr;
    uint32_t taskId = 0;
    uint16_t token = 0;
    uint8_t state = SCHEDULE_STOP;
    bool isTerminated = false;
};
} // namespace OHOS
#endif // OHOS_ABILITY_RECORD_H
