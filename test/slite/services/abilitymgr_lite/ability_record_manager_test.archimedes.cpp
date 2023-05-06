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
#include <mutex>
#include "ability_errors.h"
#include "ability_mgr_service_slite.h"
#include "ability_record_manager.h"
#include "ability_service_interface.h"
#include "bundle_manager.h"
#include "slite_ability.h"
#include "utils.h"

using namespace OHOS;
using namespace OHOS::AbilitySlite;

namespace {
constexpr int32_t QUEUE_LENGTH = 32;

class NativeAbility : public SliteAbility {
public:
    void OnCreate(const Want &want) override
    {
        SliteAbility::OnCreate(want);
    }

    void OnForeground(const Want &want) override
    {
        onForegroundCallCount++;
        SliteAbility::OnForeground(want);
    }

    void OnBackground() override
    {
        onBackgroundCallCount++;
        SliteAbility::OnBackground();
    }

    void OnDestroy() override
    {
        SliteAbility::OnDestroy();
    }

    uint32_t onForegroundCallCount { 0 };
    uint32_t onBackgroundCallCount { 0 };
};

NativeAbility g_nativeAbility;
uint32_t tokenCount = 0;

void LocalSchedulerLifecycleDone(osMessageQueueId_t amsQueueId, uint64_t tokenExpect, uint32_t stateExpect)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    Request request;
    uint8_t prio = 0;
    ASSERT_EQ(osMessageQueueGet(amsQueueId, &request, &prio, osWaitForever), osOK);
    uint64_t token = request.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state = (request.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token, tokenExpect);
    ASSERT_EQ(state, stateExpect);
    ASSERT_EQ(abilityService.SchedulerLifecycleDone(token, state), ERR_OK);
}

void TestTopAbility(const char *bundleName)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    ElementName *elementName = abilityService.GetTopAbility();
    ASSERT_NE(elementName, nullptr);
    ASSERT_STREQ(elementName->bundleName, bundleName);
    ASSERT_EQ(elementName->abilityName, nullptr);
    ASSERT_EQ(elementName->deviceId, nullptr);
    ClearElement(elementName);
    AdapterFree(elementName);
}
}

class AbilityRecordManagerTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
        amsQueueId_ = osMessageQueueNew(QUEUE_LENGTH, sizeof(Request), nullptr);
        Identity identity {
            .serviceId = 0,
            .featureId = 0,
            .queueId = amsQueueId_,
        };
        AbilityMgrServiceSlite *abilityMgrService = AbilityMgrServiceSlite::GetInstance();
        abilityMgrService->Initialize(abilityMgrService, identity);
        std::call_once(onceFlag, []() {
            OHOS::List<char *> bundleNames {};
            char *launcher = Utils::Strdup(LAUNCHER_BUNDLE_NAME);
            bundleNames.PushBack(launcher);
            BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
            AdapterFree(launcher);
            AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
            abilityService.setNativeAbility(&g_nativeAbility);
            abilityService.StartLauncher();
            LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
        });
    }

    static void TearDownTestCase()
    {
        osMessageQueueDelete(amsQueueId_);
    }

    static osMessageQueueId_t amsQueueId_;
    static std::once_flag onceFlag;
};

osMessageQueueId_t AbilityRecordManagerTest::amsQueueId_ { nullptr };
std::once_flag AbilityRecordManagerTest::onceFlag {};

TEST_F(AbilityRecordManagerTest, StartAbilityDefaultTest_001)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    ASSERT_EQ(abilityService.curTask_, 0u);
    Want *want = nullptr;
    ASSERT_EQ(abilityService.StartAbility(want), PARAM_NULL_ERROR);
    want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->element = nullptr;
    want->appPath = nullptr;
    ASSERT_EQ(abilityService.StartAbility(want), PARAM_NULL_ERROR);
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = nullptr;
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    ASSERT_EQ(abilityService.StartAbility(want), PARAM_NULL_ERROR);
    want->element->bundleName = Utils::Strdup("");
    ASSERT_EQ(abilityService.StartAbility(want), PARAM_CHECK_ERROR);

    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(INVALID_APP_BUNDLE_NAME);
    ASSERT_EQ(abilityService.StartAbility(want), PARAM_CHECK_ERROR);

    ClearWant(want);
    AdapterFree(want);
}

TEST_F(AbilityRecordManagerTest, TerminateAbilityDefaultTest_001)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();

    ASSERT_EQ(abilityService.TerminateAbility(0), ERR_OK);
    uint32_t invalidToken = tokenCount + 1;
    ASSERT_EQ(abilityService.TerminateAbility(invalidToken), -1);
}

TEST_F(AbilityRecordManagerTest, ForceStopDefaultTest_001)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();

    ASSERT_EQ(abilityService.ForceStop(nullptr), PARAM_NULL_ERROR);
    ASSERT_EQ(abilityService.ForceStop(LAUNCHER_BUNDLE_NAME), ERR_OK);
    ASSERT_EQ(abilityService.ForceStop(""), PARAM_CHECK_ERROR);
}

// foreground native + start native
TEST_F(AbilityRecordManagerTest, StartAbilityNativeTest_001)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
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

    uint32_t onForegroundCallCountOrig = g_nativeAbility.onForegroundCallCount;
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 1);

    ClearWant(want);
    AdapterFree(want);
}

// foreground native + start app1
TEST_F(AbilityRecordManagerTest, StartAbilityNativeTest_002)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app1
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = Utils::Strdup(APP_BUNDLE_PATH);
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    uint32_t onBackgroundCallCountOrig = g_nativeAbility.onBackgroundCallCount;
    uint32_t onForegroundCallCountOrig = g_nativeAbility.onForegroundCallCount;
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app1Token = ++tokenCount;
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP1_BUNDLE_NAME);

    // terminate app1
    ASSERT_EQ(abilityService.TerminateAbility(app1Token), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_UNINITIALIZED);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 2);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    ClearWant(want);
    AdapterFree(want);
}

// foreground app1, background native + start native
TEST_F(AbilityRecordManagerTest, StartAbilityJsNativeTest_001)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app1
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = nullptr;
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    uint32_t onBackgroundCallCountOrig = g_nativeAbility.onBackgroundCallCount;
    uint32_t onForegroundCallCountOrig = g_nativeAbility.onForegroundCallCount;
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app1Token = ++tokenCount;
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP1_BUNDLE_NAME);

    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(LAUNCHER_BUNDLE_NAME);
    // start native
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 1);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // terminate app1
    ASSERT_EQ(abilityService.TerminateAbility(app1Token), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 2);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_UNINITIALIZED);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 3);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    ClearWant(want);
    AdapterFree(want);
}

// foreground app1, background native + start app1
TEST_F(AbilityRecordManagerTest, StartAbilityJsNativeTest_002)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app1
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = nullptr;
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    uint32_t onBackgroundCallCountOrig = g_nativeAbility.onBackgroundCallCount;
    uint32_t onForegroundCallCountOrig = g_nativeAbility.onForegroundCallCount;
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app1Token = ++tokenCount;
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP1_BUNDLE_NAME);

    // start native
    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    TestTopAbility(APP1_BUNDLE_NAME);

    // terminate app1
    ASSERT_EQ(abilityService.TerminateAbility(app1Token), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_UNINITIALIZED);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 2);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    ClearWant(want);
    AdapterFree(want);
}

// foreground app1, background native + start app2
TEST_F(AbilityRecordManagerTest, StartAbilityJsNativeTest_003)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app1
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = nullptr;
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    uint32_t onBackgroundCallCountOrig = g_nativeAbility.onBackgroundCallCount;
    uint32_t onForegroundCallCountOrig = g_nativeAbility.onForegroundCallCount;
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app1Token = ++tokenCount;
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP1_BUNDLE_NAME);

    // start app2
    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(APP2_BUNDLE_NAME);
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app2Token = ++tokenCount;
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_UNINITIALIZED);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 2);
    LocalSchedulerLifecycleDone(amsQueueId_, app2Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app2Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP2_BUNDLE_NAME);

    // terminate app2
    ASSERT_EQ(abilityService.TerminateAbility(app2Token), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app2Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 2);
    LocalSchedulerLifecycleDone(amsQueueId_, app2Token, SLITE_STATE_UNINITIALIZED);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 3);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    ClearWant(want);
    AdapterFree(want);
}

// foreground native, background app1, start native
TEST_F(AbilityRecordManagerTest, StartAbilityNativeJsTest_001)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app1
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = nullptr;
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    uint32_t onBackgroundCallCountOrig = g_nativeAbility.onBackgroundCallCount;
    uint32_t onForegroundCallCountOrig = g_nativeAbility.onForegroundCallCount;
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app1Token = ++tokenCount;
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP1_BUNDLE_NAME);

    // start native
    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(LAUNCHER_BUNDLE_NAME);
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 1);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start native
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 2);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // terminate app1
    ASSERT_EQ(abilityService.TerminateAbility(app1Token), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 3);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_UNINITIALIZED);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 4);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    ClearWant(want);
    AdapterFree(want);
}

// foreground native, background app1, start app1
TEST_F(AbilityRecordManagerTest, StartAbilityNativeJsTest_002)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app1
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = nullptr;
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    uint32_t onBackgroundCallCountOrig = g_nativeAbility.onBackgroundCallCount;
    uint32_t onForegroundCallCountOrig = g_nativeAbility.onForegroundCallCount;
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app1Token = ++tokenCount;
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP1_BUNDLE_NAME);

    // start native
    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(LAUNCHER_BUNDLE_NAME);
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 1);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app1
    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 2);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP1_BUNDLE_NAME);

    // terminate app1
    ASSERT_EQ(abilityService.TerminateAbility(app1Token), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 2);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_UNINITIALIZED);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 3);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    ClearWant(want);
    AdapterFree(want);
}

// foreground native, background app1, start app2
TEST_F(AbilityRecordManagerTest, StartAbilityNativeJsTest_003)
{
    AbilityRecordManager &abilityService = AbilityRecordManager::GetInstance();
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app1
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = nullptr;
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    uint32_t onBackgroundCallCountOrig = g_nativeAbility.onBackgroundCallCount;
    uint32_t onForegroundCallCountOrig = g_nativeAbility.onForegroundCallCount;
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app1Token = ++tokenCount;
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 1);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP1_BUNDLE_NAME);

    // start native
    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(LAUNCHER_BUNDLE_NAME);
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 1);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    // start app2
    ClearElement(want->element);
    want->element->bundleName = Utils::Strdup(APP2_BUNDLE_NAME);
    ASSERT_EQ(abilityService.StartAbility(want), ERR_OK);
    uint32_t app2Token = ++tokenCount;
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 2);
    LocalSchedulerLifecycleDone(amsQueueId_, app1Token, SLITE_STATE_UNINITIALIZED);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_BACKGROUND);
    ASSERT_EQ(g_nativeAbility.onBackgroundCallCount, onBackgroundCallCountOrig + 2);
    LocalSchedulerLifecycleDone(amsQueueId_, app2Token, SLITE_STATE_INITIAL);
    LocalSchedulerLifecycleDone(amsQueueId_, app2Token, SLITE_STATE_FOREGROUND);
    TestTopAbility(APP2_BUNDLE_NAME);

    // terminate app2
    ASSERT_EQ(abilityService.TerminateAbility(app2Token), ERR_OK);
    LocalSchedulerLifecycleDone(amsQueueId_, app2Token, SLITE_STATE_BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 3);
    LocalSchedulerLifecycleDone(amsQueueId_, app2Token, SLITE_STATE_UNINITIALIZED);
    // FIXME
    LocalSchedulerLifecycleDone(amsQueueId_, 0, SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.GetState(), SLITE_STATE_FOREGROUND);
    ASSERT_EQ(g_nativeAbility.onForegroundCallCount, onForegroundCallCountOrig + 4);
    TestTopAbility(LAUNCHER_BUNDLE_NAME);

    ClearWant(want);
    AdapterFree(want);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
