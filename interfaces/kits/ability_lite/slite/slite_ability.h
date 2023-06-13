/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_SLITE_ABILITY_H
#define OHOS_SLITE_ABILITY_H

#include "ability_inner_message.h"
#include "ability_saved_data.h"
#include "lite_context.h"
#include "want.h"

namespace OHOS {
namespace AbilitySlite {
/**
 * @brief Declares ability-related functions, including ability lifecycle callbacks and functions for connecting to or
 *        disconnecting from Particle Abilities.
 *
 * As the fundamental unit of OpenHarmony applications, abilities are classified into Feature Abilities and Particle
 * Abilities. Feature Abilities support the Page template, and Particle Abilities support the Service template.
 * An ability using the Page template is called Page ability for short and that using the Service template
 * is called Service ability.
 *
 * @since 1.0
 * @version 1.0
 */
class SliteAbility : public LiteContext {
public:
    SliteAbility(const char *bundleName = nullptr);

    virtual ~SliteAbility();

    /**
     * @brief Called when this ability is created. You must override this function if you want to perform some
     *        initialization operations during ability startup.
     *
     * This function can be called only once in the entire lifecycle of an ability.
     * @param want Indicates the {@link Want} structure containing startup information about the ability.
     */
    virtual void OnCreate(const Want &want);

    /**
     * @brief Called when the user data need to be restored.
     *
     * You can override this function to implement your own processing logic.
     */
    virtual void OnRestoreData(AbilitySavedData *data);

    /**
     * @brief Called when this ability enters the <b>SLITE_STATE_FOREGROUND</b> state.
     *
     * The ability in the <b>SLITE_STATE_FOREGROUND</b> state is visible and has focus.
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing activation information about the ability.
     */
    virtual void OnForeground(const Want &want);

    /**
     * @brief Called when this ability enters the <b>SLITE_STATE_BACKGROUND</b> state.
     *
     *
     * The ability in the <b>SLITE_STATE_BACKGROUND</b> state is invisible.
     * You can override this function to implement your own processing logic.
     */
    virtual void OnBackground();

    /**
     * @brief Called when the user data need to be saved.
     *
     * You can override this function to implement your own processing logic.
     */
    virtual void OnSaveData(AbilitySavedData *data);

    /**
     * @brief Called when this ability enters the <b>SLITE_STATE_UNINITIALIZED</b> state.
     *
     * The ability in the <b>SLITE_STATE_UNINITIALIZED</b> is being destroyed.
     * You can override this function to implement your own processing logic.
     */
    virtual void OnDestroy();

    virtual void HandleExtraMessage(const SliteAbilityInnerMsg &innerMsg);

    int32_t GetState() const;

    char *bundleName_ = nullptr;

private:
    int32_t abilityState_ = 0;
};
} // namespace AbilitySlite
using AbilitySlite::SliteAbility;
} // namespace OHOS
#endif // OHOS_SLITE_ABILITY_H
