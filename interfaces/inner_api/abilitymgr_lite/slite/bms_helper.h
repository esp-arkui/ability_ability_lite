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

#ifndef ABILITYLITE_BMS_HELPER_H
#define ABILITYLITE_BMS_HELPER_H

#include <cstdint>
#include "bundle_manager.h"
#include "utils_list.h"
#include "want.h"

namespace OHOS {
namespace AbilitySlite {
struct AbilitySvcInfo {
    char *bundleName;
    char *path;
    void *data;
    uint16_t dataLength;
    bool isNativeApp;
    uint32_t mission;
};

class BMSHelper final {
public:
    static BMSHelper &GetInstance()
    {
        static BMSHelper instance;
        return instance;
    }

    int32_t RegisterBundleNames(const List<char *> &names);

    int32_t RegisterStartupBundleName(const char *bundleName);

    const char *GetStartupBundleName();

    int32_t RegisterTemporaryBundleNames(const List<char *> &names);

    bool IsTemporaryBundleName(const char *bundleName);

    void Erase();

    bool IsNativeApp(const char *bundleName);

    uint8_t QueryAbilitySvcInfo(const Want *want, AbilitySvcInfo *svcInfo);

    bool IsValidAbility(const AbilityInfo *abilityInfo);

    void ClearAbilitySvcInfo(AbilitySvcInfo *abilitySvcInfo);

private:
    List<char *> bundleNames_ {};
    List<char *> temporaryBundleNames_ {};
    char *startupBundleName_ = nullptr;

    BMSHelper() = default;

    ~BMSHelper();
};
} // namespace AbilitySlite
} // namespace OHOS

#endif //ABILITYLITE_BMS_HELPER_H
