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

#include "bundle_manager.h"
#include "ability_errors.h"
#include "securec.h"
#include "utils.h"

uint8_t QueryAbilityInfo(const Want *want, AbilityInfo *abilityInfo)
{
    if (want == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (want->element == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (want->element->bundleName == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (strcmp(want->element->bundleName, APP1_BUNDLE_NAME) == 0 ||
        strcmp(want->element->bundleName, APP2_BUNDLE_NAME) == 0 ||
        strcmp(want->element->bundleName, APP3_BUNDLE_NAME) == 0) {
        abilityInfo->bundleName = OHOS::Utils::Strdup(want->element->bundleName);
        abilityInfo->srcPath = OHOS::Utils::Strdup(APP_BUNDLE_PATH);
        return ERR_OK;
    }
    return PARAM_CHECK_ERROR;
}
