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

#include "slite_ability_loader.h"

namespace OHOS::AbilitySlite {
void SliteAbilityLoader::SetAbilityCreatorFunc(SliteAbilityType type, SliteAbilityCreatorFunc creator)
{
    if (creator == nullptr) {
        return;
    }
    switch (type) {
        case SliteAbilityType::JS_ABILITY:
            jsAbilityCreatorFunc_ = creator;
            break;
        case SliteAbilityType::NATIVE_ABILITY:
            nativeAbilityCreatorFunc_ = creator;
            break;
        default:
            break;
    }
}

SliteAbility *SliteAbilityLoader::CreateAbility(SliteAbilityType type, const char *bundleName)
{
    switch (type) {
        case SliteAbilityType::JS_ABILITY:
            if (jsAbilityCreatorFunc_ != nullptr) {
                return jsAbilityCreatorFunc_(bundleName);
            }
            break;
        case SliteAbilityType::NATIVE_ABILITY:
            if (nativeAbilityCreatorFunc_ != nullptr) {
                return nativeAbilityCreatorFunc_(bundleName);
            }
            break;
        default:
            break;
    }
    return nullptr;
}

void SliteAbilityLoader::UnsetAbilityCreatorFunc(SliteAbilityType type)
{
    switch (type) {
        case SliteAbilityType::JS_ABILITY:
            jsAbilityCreatorFunc_ = nullptr;
            break;
        case SliteAbilityType::NATIVE_ABILITY:
            nativeAbilityCreatorFunc_ = nullptr;
            break;
        default:
            break;
    }
}
}