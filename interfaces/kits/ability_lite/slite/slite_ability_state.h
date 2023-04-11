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

#ifndef OHOS_SLITE_ABILITY_STATE_H
#define OHOS_SLITE_ABILITY_STATE_H

namespace OHOS {
namespace AbilitySlite {
/**
 * @brief Enumerates all lifecycle states that an ability will go through over the course of its lifetime.
 *
 * @since 1.0
 * @version 1.0
 */
enum SliteAbilityState {
    SLITE_STATE_UNKNOWN,
    /**
     * Uninitialized state: An ability is in this state when it has not been initialized.
     */
    SLITE_STATE_UNINITIALIZED,

    /**
     * Initial state: An ability is in this state after it is initialized.
     */
    SLITE_STATE_INITIAL,

    /**
     * Foreground state: An ability is in this state when it is switched to the foreground.
     */
    SLITE_STATE_FOREGROUND,

    /**
     * Background state: An ability is in this state after it returns to the background.
     */
    SLITE_STATE_BACKGROUND,
};
} // namespace AbilitySlite
} // namespace OHOS
#endif // OHOS_SLITE_ABILITY_STATE_H
