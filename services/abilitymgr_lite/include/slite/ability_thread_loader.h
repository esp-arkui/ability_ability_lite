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

#ifndef ABILITYLITE_ABILITY_THREAD_LOADER_H
#define ABILITYLITE_ABILITY_THREAD_LOADER_H

#include "ability_thread.h"

namespace OHOS {
namespace AbilitySlite {
enum class AbilityThreadCreatorType {
    JS_CREATOR,
    NATIVE_CREATOR,
};

/**
* @brief Method to create AbilityThread.
*
* This method can create an AbilityThread and return a pointer to the AbilityThread.
*
*/
using AbilityThreadCreatorFunc = AbilityThread*(*)();

class AbilityThreadLoader final {
public:
    static AbilityThreadLoader &GetInstance()
    {
        static AbilityThreadLoader instance;
        return instance;
    }

    AbilityThreadLoader() = default;

    ~AbilityThreadLoader() = default;

    void SetCreatorFunc(AbilityThreadCreatorType type, AbilityThreadCreatorFunc creator);

    AbilityThread *CreateAbilityThread(AbilityThreadCreatorType type) const;

    void UnsetCreatorFunc(AbilityThreadCreatorType type);

private:
    AbilityThreadCreatorFunc jsAbilityThreadFunc_ = nullptr;
    AbilityThreadCreatorFunc nativeAbilityThreadFunc_ = nullptr;
};
} // namespace AbilitySlite
} // namespace OHOS
#endif //ABILITYLITE_ABILITY_THREAD_LOADER_H