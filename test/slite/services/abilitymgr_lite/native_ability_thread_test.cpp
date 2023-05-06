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

#define protected public

#include <gtest/gtest.h>
#include "ability_errors.h"
#include "ability_inner_message.h"
#include "ability_mgr_service_slite.h"
#include "ability_service_interface.h"
#include "bundle_manager.h"
#include "native_ability_thread.h"
#include "slite_ability_loader.h"
#include "utils.h"

using namespace OHOS;
using namespace OHOS::AbilitySlite;

class NativeAbilityThreadTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    static SliteAbility *CreateAbility(const char* bundleName)
    {
        return new SliteAbility();
    }

    static SliteAbility *CreateNullAbility(const char* bundleName)
    {
        return nullptr;
    }

    const char *APP_NAME = "com.ohos.test";
};

// test init ability thread, the parameter abilityRecord is nullptr
TEST_F(NativeAbilityThreadTest, InitAbilityThreadTest001)
{
    auto *abilityThread = new NativeAbilityThread();
    ASSERT_NE(abilityThread, nullptr);
    ASSERT_EQ(abilityThread->InitAbilityThread(nullptr), PARAM_NULL_ERROR);
    delete abilityThread;
}

// test init ability thread, abilityRecord has nullptr appName
TEST_F(NativeAbilityThreadTest, InitAbilityThreadTest002)
{
    auto *abilityThread = new NativeAbilityThread();
    ASSERT_NE(abilityThread, nullptr);
    auto *record = new AbilityRecord();
    ASSERT_NE(record, nullptr);
    ASSERT_EQ(abilityThread->InitAbilityThread(record), PARAM_NULL_ERROR);
    delete record;
    delete abilityThread;
}

// test init ability thread, wrong state
TEST_F(NativeAbilityThreadTest, InitAbilityThreadTest003)
{
    auto *abilityThread = new NativeAbilityThread();
    ASSERT_NE(abilityThread, nullptr);
    auto *record = new AbilityRecord();
    ASSERT_NE(record, nullptr);
    record->SetAppName(APP_NAME);
    abilityThread->state_ = AbilityThreadState::ABILITY_THREAD_INITIALIZED;
    ASSERT_EQ(abilityThread->InitAbilityThread(record), PARAM_CHECK_ERROR);
    delete record;
    delete abilityThread;
}

// test init ability thread, nullptr nativeAbilityCreatorFunc
TEST_F(NativeAbilityThreadTest, InitAbilityThreadTest004)
{
    auto *abilityThread = new NativeAbilityThread();
    ASSERT_NE(abilityThread, nullptr);
    auto *record = new AbilityRecord();
    ASSERT_NE(record, nullptr);
    record->SetAppName(APP_NAME);
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::NATIVE_ABILITY, CreateNullAbility);
    ASSERT_EQ(abilityThread->InitAbilityThread(record), MEMORY_MALLOC_ERROR);
    NativeAbilityThread::Reset();
    delete record;
    delete abilityThread;
}

// test init ability thread, nullptr nativeAbilityCreatorFunc
TEST_F(NativeAbilityThreadTest, InitAbilityThreadTest005)
{
    auto *abilityThread = new NativeAbilityThread();
    ASSERT_NE(abilityThread, nullptr);
    auto *record = new AbilityRecord();
    ASSERT_NE(record, nullptr);
    record->SetAppName(APP_NAME);
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::NATIVE_ABILITY, CreateAbility);
    ASSERT_EQ(abilityThread->InitAbilityThread(record), ERR_OK);
    NativeAbilityThread::Reset();
    usleep(50000); // sleep 50ms
    delete record;
    delete abilityThread;
}

// test init ability thread, ReleaseAbilityThread
TEST_F(NativeAbilityThreadTest, ReleaseAbilityThreadTest001)
{
    auto *abilityThread = new NativeAbilityThread();
    ASSERT_NE(abilityThread, nullptr);
    abilityThread->state_ = AbilityThreadState::ABILITY_THREAD_UNINITIALIZED;
    ASSERT_EQ(abilityThread->ReleaseAbilityThread(), PARAM_CHECK_ERROR);
    delete abilityThread;
}

// test init ability thread, ReleaseAbilityThread
TEST_F(NativeAbilityThreadTest, ReleaseAbilityThreadTest002)
{
    auto *abilityThread = new NativeAbilityThread();
    ASSERT_NE(abilityThread, nullptr);
    abilityThread->state_ = AbilityThreadState::ABILITY_THREAD_INITIALIZED;
    abilityThread->ability_ = new SliteAbility();
    ASSERT_EQ(abilityThread->ReleaseAbilityThread(), ERR_OK);
    delete abilityThread;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
