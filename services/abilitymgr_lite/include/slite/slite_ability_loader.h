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

#ifndef ABILITYLITE_SLITE_ABILITY_LOADER_H
#define ABILITYLITE_SLITE_ABILITY_LOADER_H

#include "slite_ability.h"

namespace OHOS {
namespace AbilitySlite {

enum class SliteAbilityType {
    JS_ABILITY,
    NATIVE_ABILITY,
};

using SliteAbilityCreatorFunc = SliteAbility*(*)(const char* bundleName);

class SliteAbilityLoader final {
public:
    static SliteAbilityLoader &GetInstance()
    {
        static SliteAbilityLoader instance;
        return instance;
    }

    SliteAbilityLoader() = default;

    ~SliteAbilityLoader() = default;

    void SetAbilityCreatorFunc(SliteAbilityType type, SliteAbilityCreatorFunc creator);

    SliteAbility *CreateAbility(SliteAbilityType type, const char *bundleName);

    void UnsetAbilityCreatorFunc(SliteAbilityType type);

private:
    SliteAbilityCreatorFunc jsAbilityCreatorFunc_ = nullptr;
    SliteAbilityCreatorFunc nativeAbilityCreatorFunc_ = nullptr;
};
} // namespace AbilitySlite
} // namespace OHOS

#endif //ABILITYLITE_SLITE_ABILITY_LOADER_H
