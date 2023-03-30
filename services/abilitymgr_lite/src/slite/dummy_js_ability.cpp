/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "dummy_js_ability.h"

namespace OHOS {
namespace AbilitySlite {
DummyJsAbility::~DummyJsAbility()
{
    delete jsAbility_;
    jsAbility_ = nullptr;
}

void DummyJsAbility::OnActive(const Want &want)
{
    if (jsAbility_ == nullptr) {
        jsAbility_ = new ACELite::JSAbility();
        if (jsAbility_ == nullptr) {
            return;
        }
        // jsAbility_->Launch;
    }
    jsAbility_->Show();
    isBackground_ = true;
    SliteAbility::OnActive(want);
}

void DummyJsAbility::OnBackground()
{
    if (jsAbility_ != nullptr) {
        isBackground_ = true;
        jsAbility_->Hide();
    }
    SliteAbility::OnBackground();
}

void DummyJsAbility::OnInactive()
{
    if (jsAbility_ != nullptr) {
        if (!isBackground_) {
            jsAbility_->HandleRenderTick();
        }
        jsAbility_->TransferToDestroy();
    }
    SliteAbility::OnInactive();
}
} // namespace AbilitySlite
} // namespace OHOS