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

#ifndef OHOS_ABILITY_SLITE_ABILITY_INNER_MESSAGE_H
#define OHOS_ABILITY_SLITE_ABILITY_INNER_MESSAGE_H

#include <cstdint>
#include "ability_saved_data.h"
#include "want.h"

namespace OHOS {
namespace AbilitySlite {
enum SliteAbilityMsgId {
    UNKNOWN,
    CREATE,
    FOREGROUND,
    BACKGROUND,
    DESTROY,
    BACKPRESSED,
    ASYNCWORK,
    TE_EVENT,
    MSG_ID_MAX,
};

class AbilityThread;

struct SliteAbilityInnerMsg {
    AbilityThread *abilityThread = nullptr;
    Want *want = nullptr;
    void *data = nullptr;
    AbilitySavedData *abilitySavedData = nullptr;
    uint16_t dataLength = 0;
    uint16_t token = 0;
    SliteAbilityMsgId msgId = UNKNOWN;
};
} // namespace AbilitySlite
} // namespace OHOS
#endif // OHOS_ABILITY_SLITE_ABILITY_INNER_MESSAGE_H
