//
// Created by dian on 2023/3/25.
//

#ifndef ABILITYLITE_NATIVE_ABILITY_THREAD_H
#define ABILITYLITE_NATIVE_ABILITY_THREAD_H

#include "ability_thread.h"

namespace OHOS {
namespace AbilitySlite {
class NativeAbilityThread: public AbilityThread{
public:

    NativeAbilityThread();

    ~NativeAbilityThread() override;

    int32_t InitAbilityThread(const AbilityRecord *abilityRecord) override;

    int32_t ReleaseAbilityThread() override;

private:
    static osMessageQueueId_t NativeQueneId;  //静态成员变量
    static uint32_t NativeTaskId;  //静态成员变量
};
}
}

#endif //ABILITYLITE_NATIVE_ABILITY_THREAD_H
