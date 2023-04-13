/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "js_async_work.h"

namespace OHOS {
namespace AbilitySlite {
void DummyJsAbility::OnCreate(const Want &want)
{
    jsAbility_.Launch(want.appPath, want.element->bundleName, token_);
    SliteAbility::OnCreate(want);
}

void DummyJsAbility::OnForeground(const Want &want)
{
    jsAbility_.Show();
    isBackground_ = false;
    SliteAbility::OnForeground(want);
}

void DummyJsAbility::OnBackground()
{
    isBackground_ = true;
    jsAbility_.Hide();
    SliteAbility::OnBackground();
}

void DummyJsAbility::OnDestroy()
{
    ACELite::JsAsyncWork::SetAppQueueHandler(nullptr);
    // the TE owner will be JS application after JS application start up except for it's lying in background,
    // call render once to make sure the last TE message is processed properly
    if (!isBackground_) {
        jsAbility_.HandleRenderTick();
    }
    jsAbility_.TransferToDestroy();
    SliteAbility::OnDestroy();
}

void DummyJsAbility::HandleExtraMessage(const SliteAbilityInnerMsg &innerMsg)
{
    switch (innerMsg.msgId) {
        case SliteAbilityMsgId::BACKPRESSED:
            jsAbility_.BackPressed();
            break;
        case SliteAbilityMsgId::ASYNCWORK: {
            auto* work = reinterpret_cast<ACELite::AsyncWork *>(innerMsg.data);
            ACELite::JsAsyncWork::ExecuteAsyncWork(work);
            break;
        }
        case SliteAbilityMsgId::TE_EVENT:
            jsAbility_.HandleRenderTick();
            break;
        default:
            break;
    }
}
} // namespace AbilitySlite
} // namespace OHOS
