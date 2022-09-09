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
#include "ohos_init.h"
#include "utils.h"


using namespace OHOS;
using namespace OHOS::AbilitySlite;

namespace {
class NativeAbility : public SliteAbility {
public:
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
        onBackgroundDone = true;
        SliteAbility::OnBackground();
    }

    void OnDestroy() override
    {
        SliteAbility::OnDestroy();
    }

    void ResetOnActiveDoneFlag()
    {
        onForegroundDone = false;
    }

    bool WaitOnActiveDone()
    {
        while (!onForegroundDone) {
            usleep(50000); // sleep 50ms
        }
        onForegroundDone = false;
        return true;
    }

private:
    std::atomic<bool> onForegroundDone { false };
    std::atomic<bool> onBackgroundDone { false };
};

NativeAbility g_nativeAbility;
uint32_t tokenCount = 0;
}


class AbilityMsClientTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
        AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
        abilityService.setNativeAbility(&g_nativeAbility);
        OhosInitMgr::GetInstance().Init();
    }

    static void TearDownTestCase()
    {
        OhosInitMgr::GetInstance().Uninit();
    }
};

TEST_F(AbilityMsClientTest, AbilityMsClientTest)
{
    g_nativeAbility.ResetOnActiveDoneFlag();
    AbilityMsClient &abilityMsClient = AbilityMsClient::GetInstance();
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    ASSERT_NE(want, nullptr);
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = nullptr;
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    ASSERT_NE(want->element, nullptr);
    want->element->bundleName = Utils::Strdup(LAUNCHER_BUNDLE_NAME);
    ASSERT_NE(want->element->bundleName, nullptr);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    ASSERT_EQ(abilityMsClient.StartAbility(want), ERR_OK);
    ASSERT_EQ(g_nativeAbility.WaitOnActiveDone(), true);

    ClearWant(want);
    AdapterFree(want);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
