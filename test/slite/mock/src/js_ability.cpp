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

#include "js_ability.h"

#include <cstring>
#include "js_async_work.h"

namespace OHOS {
namespace ACELite {


JSAbility::~JSAbility()
{
}

void JSAbility::Launch(const char * const abilityPath, const char * const bundleName, uint16_t token,
                       const char *pageInfo)
{
}

void JSAbility::Show()
{
}

void JSAbility::Hide()
{
}

void JSAbility::TransferToDestroy()
{
}

void JSAbility::BackPressed()
{
}

const char *JSAbility::GetPackageName()
{
    return "";
}

// this public interface will be deprecated, only fatal scenario can trigger force destroy
void JSAbility::ForceDestroy()
{
}

bool JSAbility::IsRecycled()
{
    return (jsAbilityImpl_ == nullptr);
}

void JSAbility::LazyLoadHandleRenderTick(void *data)
{
}

void JSAbility::HandleRenderTick()
{
}
} // namespace ACELite
} // namespace OHOS
