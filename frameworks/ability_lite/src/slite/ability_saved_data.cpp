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

#include "ability_saved_data.h"
#include "adapter.h"
#include "utils.h"

namespace OHOS {
namespace AbilitySlite {

AbilitySavedData::AbilitySavedData() = default;

AbilitySavedData::~AbilitySavedData()
{
    AdapterFree(savedData);
    AdapterFree(userSavedData);
}

SavedResultCode AbilitySavedData::SetSavedData(const void *buffer, uint16_t bufferSize)
{
    if (buffer == nullptr || bufferSize == 0) {
        return SavedResultCode::SAVED_RESULT_INVALID_PARAM;
    }
    if (bufferSize > SAVED_DATA_LIMIT) {
        return SavedResultCode::SAVED_RESULT_EXCEED_UPPER_LIMIT;
    }
    void *dumpBuffer = Utils::Memdup(buffer, bufferSize);
    if (dumpBuffer == nullptr) {
        return SavedResultCode::SAVED_RESULT_ALLOC_ERROR;
    }
    AdapterFree(savedData);
    savedData = dumpBuffer;
    savedDataSize = bufferSize;
    return SavedResultCode::SAVED_RESULT_OK;
}

SavedResultCode AbilitySavedData::SetUserSavedData(const void *buffer, uint16_t bufferSize)
{
    if (buffer == nullptr || bufferSize == 0) {
        return SavedResultCode::SAVED_RESULT_INVALID_PARAM;
    }
    if (bufferSize > SAVED_DATA_LIMIT) {
        return SavedResultCode::SAVED_RESULT_EXCEED_UPPER_LIMIT;
    }
    void *dumpBuffer = Utils::Memdup(buffer, bufferSize);
    if (dumpBuffer == nullptr) {
        return SavedResultCode::SAVED_RESULT_ALLOC_ERROR;
    }
    AdapterFree(userSavedData);
    userSavedData = dumpBuffer;
    userSavedDataSize = bufferSize;
    return SavedResultCode::SAVED_RESULT_OK;
}

SavedResultCode AbilitySavedData::GetSavedData(void *buffer, uint16_t bufferSize, uint16_t* getDataSize)
{
    if (getDataSize == nullptr) {
        return SavedResultCode::SAVED_RESULT_INVALID_PARAM;
    }
    if (buffer == nullptr || bufferSize < savedDataSize) {
        *getDataSize = 0;
        return SavedResultCode::SAVED_RESULT_INVALID_PARAM;
    }
    if (savedData == nullptr || savedDataSize == 0) {
        *getDataSize = 0;
        return SavedResultCode::SAVED_RESULT_NO_DATA;
    }
    int32_t ret = memcpy_s(buffer, bufferSize, savedData, savedDataSize);
    if (ret != 0) {
        *getDataSize = 0;
        return SavedResultCode::SAVED_RESULT_INVALID_PARAM;
    }
    *getDataSize = savedDataSize;
    return SavedResultCode::SAVED_RESULT_OK;
}

SavedResultCode AbilitySavedData::GetUserSavedData(void *buffer, uint16_t bufferSize, uint16_t* getDataSize)
{
    if (getDataSize == nullptr) {
        return SavedResultCode::SAVED_RESULT_INVALID_PARAM;
    }
    if (buffer == nullptr || bufferSize < userSavedDataSize) {
        *getDataSize = 0;
        return SavedResultCode::SAVED_RESULT_INVALID_PARAM;
    }
    if (userSavedData == nullptr || userSavedDataSize == 0) {
        *getDataSize = 0;
        return SavedResultCode::SAVED_RESULT_NO_DATA;
    }
    int32_t ret = memcpy_s(buffer, bufferSize, userSavedData, userSavedDataSize);
    if (ret != 0) {
        *getDataSize = 0;
        return SavedResultCode::SAVED_RESULT_INVALID_PARAM;
    }
    *getDataSize = userSavedDataSize;
    return SavedResultCode::SAVED_RESULT_OK;
}

void AbilitySavedData::SetSavedResultCode(SavedResultCode code)
{
    savedResult = code;
}

SavedResultCode AbilitySavedData::GetSavedResultCode() const
{
    return savedResult;
}

void AbilitySavedData::Reset()
{
    AdapterFree(savedData);
    AdapterFree(userSavedData);
    savedDataSize = 0;
    userSavedDataSize = 0;
    savedResult = SavedResultCode::SAVED_RESULT_NO_DATA;
}

} // namespace AbilitySlite
} // namespace OHOS
