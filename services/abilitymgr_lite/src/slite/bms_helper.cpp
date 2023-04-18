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
#include "aafwk_event_error_code.h"
#include "ability_errors.h"
#include "abilityms_log.h"
#include "utils.h"

namespace OHOS::AbilitySlite {
BMSHelper::~BMSHelper()
{
    Erase();
}

void BMSHelper::RegisterBundleNames(const List<char *> &names)
{
    for (auto node = names.Begin(); node != names.End(); node = node->next_) {
        char *bundleName = node->value_;
        if (bundleName != nullptr) {
            char *name = Utils::Strdup(bundleName);
            bundleNames_.PushBack(name);
        }
    }
}

void BMSHelper::Erase()
{
    while (bundleNames_.Front() != nullptr) {
        char *name = bundleNames_.Front();
        AdapterFree(name);
        bundleNames_.PopFront();
    }
}

bool BMSHelper::IsNativeApp(const char *bundleName)
{
    if (bundleName == nullptr) {
        return false;
    }
    auto next = bundleNames_.Begin();
    for (auto node = next; node != bundleNames_.End(); node = next) {
        char *bundleName_ = node->value_;
        next = node->next_;
        if (strcmp(bundleName, bundleName_) == 0) {
            return true;
        }
    }
    return false;
}

uint8_t BMSHelper::QueryAbilitySvcInfo(const Want *want, AbilitySvcInfo *svcInfo)
{
    if (want == nullptr || want->element == nullptr || want->element->bundleName == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (IsNativeApp(want->element->bundleName)) {
        svcInfo->bundleName = OHOS::Utils::Strdup(want->element->bundleName);
        svcInfo->path = nullptr;
        svcInfo->isNativeApp = true;
        return ERR_OK;
    }

    AbilityInfo abilityInfo = { nullptr, nullptr };
    QueryAbilityInfo(want, &abilityInfo);
    if (!IsValidAbility(&abilityInfo)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Ability Service returned bundleInfo is not valid");
        ClearAbilityInfo(&abilityInfo);
        return PARAM_CHECK_ERROR;
    }
    svcInfo->bundleName = OHOS::Utils::Strdup(abilityInfo.bundleName);
    svcInfo->path = OHOS::Utils::Strdup(abilityInfo.srcPath);
    svcInfo->isNativeApp = false;
    ClearAbilityInfo(&abilityInfo);
    return ERR_OK;
}

bool BMSHelper::IsValidAbility(const AbilityInfo *abilityInfo)
{
    if (abilityInfo == nullptr) {
        return false;
    }
    if (abilityInfo->bundleName == nullptr || abilityInfo->srcPath == nullptr) {
        return false;
    }
    if (strlen(abilityInfo->bundleName) == 0 || strlen(abilityInfo->srcPath) == 0) {
        return false;
    }
    return true;
}
}