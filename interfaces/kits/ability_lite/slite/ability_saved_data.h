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

#ifndef OHOS_ABILITY_SLITE_ABILITY_SAVED_DATA_H
#define OHOS_ABILITY_SLITE_ABILITY_SAVED_DATA_H

#include <cstdint>

namespace OHOS {
namespace AbilitySlite {
enum class SavedResultCode : uint8_t {
    SAVED_RESULT_OK = 0,
    SAVED_RESULT_INVALID_PARAM,
    SAVED_RESULT_EXCEED_UPPER_LIMIT,
    SAVED_RESULT_ALLOC_ERROR,
    SAVED_RESULT_NO_DATA,
};

constexpr uint16_t SAVED_DATA_LIMIT = 1024;

class AbilitySavedData {
public:
    AbilitySavedData();

    ~AbilitySavedData();

    SavedResultCode SetSavedData(const void *buffer, uint16_t bufferSize);

    SavedResultCode SetUserSavedData(const void *buffer, uint16_t bufferSize);

    SavedResultCode GetSavedData(void *buffer, uint16_t bufferSize, uint16_t *getDataSize);

    SavedResultCode GetUserSavedData(void *buffer, uint16_t bufferSize, uint16_t *getDataSize);

    void SetSavedResultCode(SavedResultCode code);

    SavedResultCode GetSavedResultCode() const;

    void Reset();

private:
    void *savedData = nullptr;
    void *userSavedData = nullptr;
    uint16_t savedDataSize = 0;
    uint16_t userSavedDataSize = 0;
    SavedResultCode savedResult = SavedResultCode::SAVED_RESULT_NO_DATA;
};
} // namespace AbilitySlite
} // namespace OHOS
#endif // OHOS_ABILITY_SLITE_ABILITY_SAVED_DATA_H
