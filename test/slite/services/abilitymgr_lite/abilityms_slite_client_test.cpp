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

#include <gtest/gtest.h>
#include "abilityms_slite_client.h"
#include "ability_errors.h"
#include "ability_record_manager.h"
#include "adapter.h"
#include "bms_helper.h"
#include "bundle_manager.h"
#include "native_ability_thread.h"
#include "ohos_init.h"
#include "slite_ability_loader.h"
#include "utils.h"

using namespace OHOS;
using namespace OHOS::AbilitySlite;

namespace {
class MyNativeAbility;

class MyJsAbility;

MyJsAbility *g_myJsAbility = nullptr;
MyNativeAbility *g_myNativeAbility = nullptr;

class MyNativeAbility : public SliteAbility {
public:
    explicit MyNativeAbility(const char *bundleName) : SliteAbility(bundleName)
    {
    }

    void OnCreate(const Want &want) override
    {
        SliteAbility::OnCreate(want);
    }

    void OnForeground(const Want &want) override
    {
        onForegroundDone = true;
        SliteAbility::OnForeground(want);
    }

    void OnBackground() override
    {
        SliteAbility::OnBackground();
    }

    void OnDestroy() override
    {
        onDestroyDone = true;
        SliteAbility::OnDestroy();
    }

    uint32_t GetToken()
    {
        return token_;
    }

    static bool WaitOnActiveDone()
    {
        while (g_myNativeAbility == nullptr) {
            // usleep(50000); // sleep 50ms
        }
        while (!g_myNativeAbility->onForegroundDone) {
            // usleep(50000); // sleep 50ms
        }
        g_myNativeAbility->onForegroundDone = false;
        return true;
    }

    static bool WaitOnDestroyDone()
    {
        while (g_myNativeAbility == nullptr) {
            // usleep(50000); // sleep 50ms
        }
        while (!g_myNativeAbility->onDestroyDone) {
            // usleep(50000); // sleep 50ms
        }
        g_myNativeAbility->onDestroyDone = false;
        return true;
    }

private:
    std::atomic<bool> onForegroundDone { false };
    std::atomic<bool> onDestroyDone { false };
};

class MyJsAbility : public SliteAbility {
public:
    explicit MyJsAbility(const char *bundleName) : SliteAbility(bundleName)
    {
    }

    void OnCreate(const Want &want) override
    {
        SliteAbility::OnCreate(want);
    }

    void OnForeground(const Want &want) override
    {
        onForegroundDone = true;
        SliteAbility::OnForeground(want);
    }

    void OnBackground() override
    {
        SliteAbility::OnBackground();
    }

    void OnDestroy() override
    {
        onDestroyDone = true;
        SliteAbility::OnDestroy();
    }

    uint32_t GetToken()
    {
        return token_;
    }

    static bool WaitOnActiveDone()
    {
        while (g_myJsAbility == nullptr) {
            // usleep(50000); // sleep 50ms
        }
        while (!g_myJsAbility->onForegroundDone) {
            // usleep(50000); // sleep 50ms
        }
        g_myJsAbility->onForegroundDone = false;
        return true;
    }

    static bool WaitOnDestroyDone()
    {
        while (g_myJsAbility == nullptr) {
            // usleep(50000); // sleep 50ms
        }
        while (!g_myJsAbility->onDestroyDone) {
            // usleep(50000); // sleep 50ms
        }
        g_myJsAbility->onDestroyDone = false;
        return true;
    }

private:
    std::atomic<bool> onForegroundDone { false };
    std::atomic<bool> onDestroyDone { false };
};

SliteAbility *CreateNativeAbility(const char *bundleName)
{
    g_myNativeAbility = new MyNativeAbility(bundleName);
    return g_myNativeAbility;
}

SliteAbility *CreateJsAbility(const char *bundleName)
{
    g_myJsAbility = new MyJsAbility(bundleName);
    return g_myJsAbility;
}
}


class AbilityMsClientTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
        std::call_once(onceFlag, [&]() {
            SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::NATIVE_ABILITY,
                CreateNativeAbility);
            OHOS::List<char *> bundleNames {};
            char *launcher = Utils::Strdup(LAUNCHER_BUNDLE_NAME);
            bundleNames.PushBack(launcher);
            BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
            AdapterFree(launcher);
            OhosInitMgr::GetInstance().Init();
            SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::JS_ABILITY, CreateJsAbility);
            ASSERT_EQ(MyNativeAbility::WaitOnActiveDone(), true);
        });
    }

    static void TearDownTestCase()
    {
    }

    static std::once_flag onceFlag;
};

std::once_flag AbilityMsClientTest::onceFlag {};

TEST_F(AbilityMsClientTest, AbilityMsClientTest)
{
    AbilityMsClient &abilityMsClient = AbilityMsClient::GetInstance();
    Want *want = (Want *)AdapterMalloc(sizeof(Want));
    ASSERT_NE(want, nullptr);
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = nullptr;
    want->element = (ElementName *)AdapterMalloc(sizeof(ElementName));
    ASSERT_NE(want->element, nullptr);
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    ASSERT_NE(want->element->bundleName, nullptr);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    ASSERT_EQ(abilityMsClient.StartAbility(want), ERR_OK);
    ASSERT_EQ(MyNativeAbility::WaitOnDestroyDone(), true);
    ASSERT_EQ(MyJsAbility::WaitOnActiveDone(), true);
    ASSERT_EQ(abilityMsClient.TerminateAbility(g_myJsAbility->GetToken()), ERR_OK);
    ASSERT_EQ(MyNativeAbility::WaitOnActiveDone(), true);
    g_myJsAbility = nullptr;
    usleep(50000); // sleep 50ms

    ClearWant(want);
    AdapterFree(want);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int32_t result = RUN_ALL_TESTS();
    NativeAbilityThread::Reset();
    OhosInitMgr::GetInstance().Uninit();
    return result;
}
