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

#ifndef ABILITYLITE_NATIVE_ABILITY_THREAD_H
#define ABILITYLITE_NATIVE_ABILITY_THREAD_H

#include "ability_thread.h"

namespace OHOS {
namespace AbilitySlite {
class NativeAbilityThread : public AbilityThread {
public:
    NativeAbilityThread();

    ~NativeAbilityThread() override;

    int32_t InitAbilityThread(const AbilityRecord *abilityRecord) override;

    int32_t ReleaseAbilityThread() override;

    osMessageQueueId_t GetMessageQueueId() const override;

    UINT32 GetAppTaskId() const override;

    static void Reset();

    static void NativeAppTaskHandler(UINT32 uwArg);
private:
    static osMessageQueueId_t nativeQueueId_;
    static UINT32 nativeTaskId_;
    static SliteAbility *nativeAbility_;
};
}
}

#endif //ABILITYLITE_NATIVE_ABILITY_THREAD_H
