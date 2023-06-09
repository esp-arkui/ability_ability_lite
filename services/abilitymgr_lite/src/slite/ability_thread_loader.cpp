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

namespace OHOS::AbilitySlite {
void AbilityThreadLoader::SetCreatorFunc(AbilityThreadCreatorType type, AbilityThreadCreatorFunc func)
{
    if (func == nullptr) {
        return;
    }
    switch (type) {
        case AbilityThreadCreatorType::JS_CREATOR:
            if (jsAbilityThreadFunc_ == nullptr) {
                jsAbilityThreadFunc_ = func;
            }
            break;
        case AbilityThreadCreatorType::NATIVE_CREATOR:
            if (nativeAbilityThreadFunc_ == nullptr) {
                nativeAbilityThreadFunc_ = func;
            }
            break;
        default:
            break;
    }
}

AbilityThread *AbilityThreadLoader::CreateAbilityThread(AbilityThreadCreatorType type) const
{
    if (AbilityThreadCreatorType::JS_CREATOR == type && jsAbilityThreadFunc_ != nullptr) {
        return jsAbilityThreadFunc_();
    }
    if (AbilityThreadCreatorType::NATIVE_CREATOR == type && nativeAbilityThreadFunc_ != nullptr) {
        return nativeAbilityThreadFunc_();
    }
    return nullptr;
}

void AbilityThreadLoader::UnsetCreatorFunc(AbilityThreadCreatorType type)
{
    switch (type) {
        case AbilityThreadCreatorType::JS_CREATOR:
            jsAbilityThreadFunc_ = nullptr;
            break;
        case AbilityThreadCreatorType::NATIVE_CREATOR:
            nativeAbilityThreadFunc_ = nullptr;
            break;
        default:
            break;
    }
}
}