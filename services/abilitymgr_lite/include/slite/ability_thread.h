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

#ifndef OHOS_ABILITY_THREAD_H
#define OHOS_ABILITY_THREAD_H

#include <stdint.h>
#include "ability_record.h"
#include "slite_ability.h"

namespace OHOS {
class AbilityThread {
public:
    AbilityThread();

    virtual ~AbilityThread();

    virtual int32_t InitAbilityThread(AbilityRecord *abilityRecord) = 0;

    virtual int32_t ReleaseAbilityThread() = 0;

    int32_t HandleCreate();

    int32_t HandleDestroy();

    int32_t HandleForeground();

    int32_t HandleBackground();
private:
    SliteAbility *ability_ { nullptr };
};
} // namespace OHOS

#endif //OHOS_ABILITY_THREAD_H
