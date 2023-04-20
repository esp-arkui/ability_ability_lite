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
#include "utils_list.h"

namespace OHOS {
namespace AbilitySlite {
class BMSHelper final {
public:
    static BMSHelper &GetInstance()
    {
        static BMSHelper instance;
        return instance;
    }

    BMSHelper() = default;

    ~BMSHelper();

    void RegisterBundleNames(List<char *> &names);

    void Erase();
    
    void tesaa();

    bool IsNativeApp(const char *bundleName);
private:
    List<char *> bundleNames {};
};
} // namespace AbilitySlite
} // namespace OHOS

#endif //ABILITYLITE_BMS_HELPER_H