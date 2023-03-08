//
// Created by dian on 2023/3/9.
//

#ifndef ABILITYLITE_ABILITY_THREAD_H
#define ABILITYLITE_ABILITY_THREAD_H

#include <stdint.h>

namespace OHOS {
class AbilityThread {
public:
    AbilityThread() = default;

    virtual ~AbilityThread() = default;

    virtual int GetAbilityThreadType() = 0;

    virtual void SetAbilityThreadType(int32_t type) = 0;

protected:
    int32_t threadType_ = 0;
};
} // namespace OHOS
#endif // ABILITYLITE_ABILITY_THREAD_H