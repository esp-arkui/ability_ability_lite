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

using namespace OHOS::ACELite;

namespace OHOS::AbilitySlite {
namespace {
constexpr uint16_t BUNDLE_LIST_CAPACITY = 256;
const char* DEFAULT_STARTUP_BUNDLE_NAME = "com.ohos.launcher";
}
BMSHelper::~BMSHelper()
{
    Erase();
}

int32_t BMSHelper::RegisterBundleNames(const List<char *> &names)
{
    if (names.Size() + bundleNames_.Size() > BUNDLE_LIST_CAPACITY) {
        return PARAM_CHECK_ERROR;
    }
    for (auto node = names.Begin(); node != names.End(); node = node->next_) {
        char *bundleName = node->value_;
        if (bundleName != nullptr) {
            char *name = Utils::Strdup(bundleName);
            bundleNames_.PushBack(name);
        }
    }
    return ERR_OK;
}

int32_t BMSHelper::RegisterStartupBundleName(const char *bundleName)
{
    if (bundleName == nullptr) {
        return PARAM_NULL_ERROR;
    }
    AdapterFree(startupBundleName_);
    startupBundleName_ = Utils::Strdup(bundleName);
    return ERR_OK;
}

const char *BMSHelper::GetStartupBundleName()
{
    if (startupBundleName_ == nullptr) {
        return DEFAULT_STARTUP_BUNDLE_NAME;
    }
    return startupBundleName_;
}

int32_t BMSHelper::RegisterTemporaryBundleNames(const List<char *> &names)
{
    if (names.Size() + temporaryBundleNames_.Size() > BUNDLE_LIST_CAPACITY) {
        return PARAM_CHECK_ERROR;
    }
    for (auto node = names.Begin(); node != names.End(); node = node->next_) {
        char *bundleName = node->value_;
        if (bundleName != nullptr) {
            char *name = Utils::Strdup(bundleName);
            temporaryBundleNames_.PushBack(name);
        }
    }
    return ERR_OK;
}

bool BMSHelper::IsTemporaryBundleName(const char *bundleName)
{
    if (bundleName == nullptr) {
        return false;
    }
    for (auto node = temporaryBundleNames_.Begin(); node != temporaryBundleNames_.End(); node = node->next_) {
        char *temporaryBundleName = node->value_;
        if (temporaryBundleName != nullptr && strcmp(bundleName, temporaryBundleName) == 0) {
            return true;
        }
    }
    return false;
}

void BMSHelper::Erase()
{
    while (bundleNames_.Front() != nullptr) {
        char *name = bundleNames_.Front();
        AdapterFree(name);
        bundleNames_.PopFront();
    }
    AdapterFree(startupBundleName_);
    while (temporaryBundleNames_.Front() != nullptr) {
        char *name = temporaryBundleNames_.Front();
        AdapterFree(name);
        temporaryBundleNames_.PopFront();
    }
}

bool BMSHelper::IsNativeApp(const char *bundleName)
{
    if (bundleName == nullptr) {
        return false;
    }
    for (auto node = bundleNames_.Begin(); node != bundleNames_.End(); node = node->next_) {
        char *bundleName_ = node->value_;
        if (strcmp(bundleName, bundleName_) == 0) {
            return true;
        }
    }
    return false;
}

uint8_t BMSHelper::QueryAbilitySvcInfo(const Want *want, AbilitySvcInfo *svcInfo)
{
#ifdef _MINI_BMS_
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
    memset_s(&abilityInfo, sizeof(AbilityInfo), 0, sizeof(AbilityInfo));
    QueryAbilityInfo(want, &abilityInfo);
    if (!IsValidAbility(&abilityInfo)) {
        APP_ERRCODE_EXTRA(EXCE_ACE_APP_START, EXCE_ACE_APP_START_UNKNOWN_BUNDLE_INFO);
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Invalid AbilityInfo");
        ClearAbilityInfo(&abilityInfo);
        return PARAM_CHECK_ERROR;
    }
    svcInfo->bundleName = OHOS::Utils::Strdup(abilityInfo.bundleName);
    svcInfo->path = OHOS::Utils::Strdup(abilityInfo.srcPath);
    svcInfo->isNativeApp = false;
    ClearAbilityInfo(&abilityInfo);
    return ERR_OK;
#else
    svcInfo->bundleName = Utils::Strdup(want->element->bundleName);
    // Here users assign want->data with js app path.
    svcInfo->path = Utils::Strdup((const char *)want->data);
    return ERR_OK;
#endif
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

void BMSHelper::ClearAbilitySvcInfo(AbilitySvcInfo *abilitySvcInfo)
{
    if (abilitySvcInfo == nullptr) {
        return;
    }
    AdapterFree(abilitySvcInfo->bundleName);
    AdapterFree(abilitySvcInfo->path);
}
}
