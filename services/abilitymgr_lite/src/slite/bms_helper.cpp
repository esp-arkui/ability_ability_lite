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

#include "bms_helper.h"
#include "utils.h"

namespace OHOS::AbilitySlite {
BMSHelper::~BMSHelper()
{
    Erase();
}

void BMSHelper::RegisterBundleNames(List<char *> &names)
{
    for (auto node = names.Begin(); node != names.End(); node = node->next_) {
        char *bundleName = node->value_;
        if (bundleName != nullptr) {
            char *name = Utils::Strdup(bundleName);
            bundleNames.PushBack(name);
        }
    }
}

void BMSHelper::Erase()
{
    while (bundleNames.Front() != nullptr) {
        char *name = bundleNames.Front();
        AdapterFree(name);
        bundleNames.PopFront();
    }
}

bool BMSHelper::IsNativeApp(const char *bundleName)
{
    if (bundleName == nullptr) {
        return false;
    }
    auto next = bundleNames.Begin();
    for (auto node = next; node != bundleNames.End(); node = next) {
        char *bundleName_ = node->value_;
        next = node->next_;
        if (strcmp(bundleName, bundleName_) == 0) {
            return true;
        }
    }
    return false;
}
}