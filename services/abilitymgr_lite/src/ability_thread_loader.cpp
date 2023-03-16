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

#include "ability_thread_loader.h"

namespace OHOS {
void AbilityThreadLoader::SetCreatorFunc(AbilityThreadCreatorType type, AbilityThreadCreator creator)
{
    if (creator == nullptr) {
        return;
    }
    switch (type) {
        case AbilityThreadCreatorType::JS_CREATOR:
            jsAbilityThreadCreator_ = creator;
            break;
        case AbilityThreadCreatorType::NATIVE_CREATOR:
            nativeAbilityThreadCreator_ = creator;
            break;
        default:
            break;
    }
}

SliteAbilityThread *AbilityThreadLoader::CreateAbilityThread(AbilityThreadCreatorType type) const
{
    if (AbilityThreadCreatorType::JS_CREATOR == type && jsAbilityThreadCreator_ != nullptr) {
        return jsAbilityThreadCreator_();
    }
    if (AbilityThreadCreatorType::NATIVE_CREATOR == type && nativeAbilityThreadCreator_ != nullptr){
        return nativeAbilityThreadCreator_();
    }
    return nullptr;
}

void AbilityThreadLoader::unRegister(AbilityThreadCreatorType type)
{
    switch (type) {
        case AbilityThreadCreatorType::JS_CREATOR:
            jsAbilityThreadCreator_ = { nullptr };
            break;
        case AbilityThreadCreatorType::NATIVE_CREATOR:
            nativeAbilityThreadCreator_ = { nullptr };
            break;
        default:
            break;
    }
}
}