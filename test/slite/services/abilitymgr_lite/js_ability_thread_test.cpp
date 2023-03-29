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
#include "ability_errors.h"
#include "ability_inner_message.h"
#include "ability_mgr_service_slite.h"
#include "ability_service_interface.h"
#include "bundle_manager.h"
#include "js_ability_thread.h"
#include "los_task.h"
#include "utils.h"

using namespace OHOS;
using namespace OHOS::AbilitySlite;
constexpr int32_t QUEUE_LENGTH = 32;
constexpr int32_t APP_TASK_PRI = 25;
constexpr uint16_t TEST_TOKEN = 123;

class JsAbilityThreadTest : public ::testing::Test {
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
    }

    static void TearDownTestCase()
    {
        osMessageQueueDelete(amsQueueId_);
    }

    static osMessageQueueId_t amsQueueId_;
};

osMessageQueueId_t JsAbilityThreadTest::amsQueueId_ { nullptr };

TEST_F(JsAbilityThreadTest, JsAbilityThreadDefaultTest)
{
    auto *jsAbilityThread = new JsAbilityThread();
    ASSERT_EQ(jsAbilityThread->messageQueueId_, nullptr);
    ASSERT_EQ(jsAbilityThread->appTaskId_, 0u);
    JsAbilityThread::AppTaskHandler(0);
    delete jsAbilityThread;
}

TEST_F(JsAbilityThreadTest, JsAbilityThreadInitTest)
{
    auto *jsAbilityThread = new JsAbilityThread();
    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), PARAM_CHECK_ERROR);
    ASSERT_EQ(jsAbilityThread->InitAbilityThread(nullptr), PARAM_NULL_ERROR);
    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), PARAM_CHECK_ERROR);
    AbilityRecord record;
    ASSERT_EQ(jsAbilityThread->InitAbilityThread(&record), PARAM_NULL_ERROR);
    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), PARAM_CHECK_ERROR);
    delete jsAbilityThread;
}

TEST_F(JsAbilityThreadTest, JsAbilityThreadNoReleaseTest)
{
    auto *jsAbilityThread = new JsAbilityThread();
    AbilityRecord record;
    record.appName = Utils::Strdup(APP1_BUNDLE_NAME);
    record.token = TEST_TOKEN;
    ASSERT_EQ(jsAbilityThread->InitAbilityThread(&record), ERR_OK);
    osMessageQueueId_t messageQueueId = jsAbilityThread->messageQueueId_;
    ASSERT_NE(messageQueueId, nullptr);

    AbilityInnerMsg innerMsg3;
    innerMsg3.token = TEST_TOKEN;
    innerMsg3.msgId = AbilityMsgId::DESTROY;
    innerMsg3.abilityThread = jsAbilityThread;
    auto ret = osMessageQueuePut(messageQueueId, &innerMsg3, 0, 0);
    ASSERT_EQ(ret, osOK);
    Request request3;
    uint8_t prio3 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request3, &prio3, osWaitForever);
    ASSERT_EQ(ret, osOK);
    ASSERT_EQ(request3.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token3 = request3.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state3 = (request3.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token3, TEST_TOKEN);
    ASSERT_EQ(state3, STATE_INACTIVE);
    delete jsAbilityThread;
}

TEST_F(JsAbilityThreadTest, JsAppTaskHandlerLifecycleTest)
{
    auto *jsAbilityThread = new JsAbilityThread();
    AbilityRecord record;
    record.appName = Utils::Strdup(APP1_BUNDLE_NAME);
    record.token = TEST_TOKEN;
    ASSERT_EQ(jsAbilityThread->InitAbilityThread(&record), ERR_OK);
    osMessageQueueId_t messageQueueId = jsAbilityThread->messageQueueId_;
    ASSERT_NE(messageQueueId, nullptr);

    AbilityInnerMsg innerMsg1;
    innerMsg1.msgId = AbilityMsgId::CREATE;
    innerMsg1.bundleName = "";
    innerMsg1.path = "";
    innerMsg1.abilityThread = jsAbilityThread;
    innerMsg1.want = (Want *) AdapterMalloc(sizeof(Want));
    innerMsg1.want->data = nullptr;
    innerMsg1.want->dataLength = 0;
    innerMsg1.want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    innerMsg1.want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    innerMsg1.want->element->abilityName = nullptr;
    innerMsg1.want->element->deviceId = nullptr;
    auto ret = osMessageQueuePut(messageQueueId, &innerMsg1, 0, 0);
    ASSERT_EQ(ret, osOK);
    Request request1;
    uint8_t prio1 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request1, &prio1, osWaitForever);
    ASSERT_EQ(ret, osOK);
    ASSERT_EQ(request1.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token1 = request1.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state1 = (request1.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token1, TEST_TOKEN);
    ASSERT_EQ(state1, STATE_ACTIVE);

    AbilityInnerMsg innerMsg2;
    innerMsg2.token = TEST_TOKEN;
    innerMsg2.msgId = AbilityMsgId::BACKGROUND;
    innerMsg2.abilityThread = jsAbilityThread;
    ret = osMessageQueuePut(messageQueueId, &innerMsg2, 0, 0);
    ASSERT_EQ(ret, osOK);
    Request request2;
    uint8_t prio2 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request2, &prio2, osWaitForever);
    ASSERT_EQ(ret, osOK);
    ASSERT_EQ(request2.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token2 = request2.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state2 = (request2.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token2, TEST_TOKEN);
    ASSERT_EQ(state2, STATE_BACKGROUND);

    AbilityInnerMsg innerMsg3;
    innerMsg3.token = TEST_TOKEN;
    innerMsg3.msgId = AbilityMsgId::DESTROY;
    innerMsg3.abilityThread = jsAbilityThread;
    ret = osMessageQueuePut(messageQueueId, &innerMsg3, 0, 0);
    ASSERT_EQ(ret, osOK);
    Request request3;
    uint8_t prio3 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request3, &prio3, osWaitForever);
    ASSERT_EQ(ret, osOK);
    ASSERT_EQ(request3.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token3 = request3.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state3 = (request3.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token3, TEST_TOKEN);
    ASSERT_EQ(state3, STATE_INACTIVE);

    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), ERR_OK);
    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), PARAM_CHECK_ERROR);
    delete jsAbilityThread;
}

TEST_F(JsAbilityThreadTest, JsAppTaskHandlerOtherTest)
{
    auto* jsAbilityThread = new JsAbilityThread();
    AbilityRecord record;
    record.appName = Utils::Strdup(APP1_BUNDLE_NAME);
    record.token = TEST_TOKEN;
    ASSERT_EQ(jsAbilityThread->InitAbilityThread(&record), ERR_OK);
    osMessageQueueId_t messageQueueId = jsAbilityThread->messageQueueId_;
    ASSERT_NE(messageQueueId, nullptr);

    AbilityInnerMsg innerMsg1;
    innerMsg1.msgId = AbilityMsgId::CREATE;
    innerMsg1.bundleName = "";
    innerMsg1.path = "";
    innerMsg1.abilityThread = jsAbilityThread;
    innerMsg1.want = (Want *) AdapterMalloc(sizeof(Want));
    innerMsg1.want->data = nullptr;
    innerMsg1.want->dataLength = 0;
    innerMsg1.want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    innerMsg1.want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    innerMsg1.want->element->abilityName = nullptr;
    innerMsg1.want->element->deviceId = nullptr;
    auto ret = osMessageQueuePut(messageQueueId, &innerMsg1, 0, 0);
    ASSERT_EQ(ret, osOK);
    Request request1;
    uint8_t prio1 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request1, &prio1, osWaitForever);
    ASSERT_EQ(ret, osOK);
    ASSERT_EQ(request1.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token1 = request1.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state1 = (request1.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token1, TEST_TOKEN);
    ASSERT_EQ(state1, STATE_ACTIVE);

    AbilityInnerMsg innerMsg5;
    innerMsg5.token = TEST_TOKEN;
    innerMsg5.msgId = AbilityMsgId::UNKNOWN;
    innerMsg5.abilityThread = jsAbilityThread;
    ret = osMessageQueuePut(messageQueueId, &innerMsg5, 0, 0);
    ASSERT_EQ(ret, osOK);

    AbilityInnerMsg innerMsg6;
    innerMsg6.token = TEST_TOKEN;
    innerMsg6.msgId = AbilityMsgId::DESTROY;
    innerMsg6.abilityThread = jsAbilityThread;
    ret = osMessageQueuePut(messageQueueId, &innerMsg6, 0, 0);
    ASSERT_EQ(ret, osOK);
    Request request6;
    uint8_t prio6 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request6, &prio6, osWaitForever);
    ASSERT_EQ(ret, osOK);
    ASSERT_EQ(request6.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token6 = request6.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state6 = (request6.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token6, TEST_TOKEN);
    ASSERT_EQ(state6, STATE_INACTIVE);

    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), ERR_OK);
    delete jsAbilityThread;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
