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

#include "ability_thread.h"

#include "abilityms_log.h"
#include "ability_errors.h"
#include "ability_inner_message.h"

namespace OHOS {
namespace AbilitySlite {

AbilityThread::AbilityThread() = default;

AbilityThread::~AbilityThread() = default;

int32_t AbilityThread::HandleCreate(const Want *want)
{
    if (ability_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (want == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (want->data != nullptr) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "start ability with data %{public}u", want->dataLength);
    } else {
        HILOG_INFO(HILOG_MODULE_AAFWK, "start ability with no data");
    }
    ability_->OnCreate(*want);
    return ERR_OK;
}

int32_t AbilityThread::HandleForeground(const Want *want)
{
    if (ability_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (want == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (want->data != nullptr) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "foreground ability with data %{public}u", want->dataLength);
    } else {
        HILOG_INFO(HILOG_MODULE_AAFWK, "foreground ability with no data");
    }
    ability_->OnForeground(*want);
    return ERR_OK;
}

int32_t AbilityThread::HandleBackground()
{
    if (ability_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    ability_->OnBackground();
    return ERR_OK;
}

int32_t AbilityThread::HandleDestroy()
{
    if (ability_ == nullptr) {
        return PARAM_NULL_ERROR;
    }
    ability_->OnDestroy();
    return ERR_OK;
}
} // namespace AbilitySlite
} // namespace OHOS
