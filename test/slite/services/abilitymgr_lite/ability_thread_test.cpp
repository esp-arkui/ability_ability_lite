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
#include "ability_mgr_service_slite.h"
#include "ability_service_interface.h"
#include "ability_thread.h"
#include "los_task.h"

using namespace OHOS;
using namespace OHOS::AbilitySlite;
constexpr int32_t QUEUE_LENGTH = 32;
constexpr int32_t APP_TASK_PRI = 25;
constexpr uint16_t TEST_TOKEN = 123;

class JsAppHostTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
        amsQueueId_ = osMessageQueueNew(QUEUE_LENGTH, sizeof(Request), nullptr);
        Identity identity {
            .serviceId = 0,
            .featureId = 0,
            .queueId = amsQueueId_,
        };
        AbilityMgrServiceSlite* abilityMgrService = AbilityMgrServiceSlite::GetInstance();
        abilityMgrService->Initialize(abilityMgrService, identity);
    }

    static void TearDownTestCase()
    {
        osMessageQueueDelete(amsQueueId_);
    }

    static osMessageQueueId_t amsQueueId_;
};

osMessageQueueId_t JsAppHostTest::amsQueueId_ { nullptr };

TEST_F(JsAppHostTest, JsAppHostDefaultTest)
{
    auto* jsAppHost = new JsAppHost();
    ASSERT_EQ(jsAppHost->GetMessageQueueId(), nullptr);
    JsAppHost::JsAppTaskHandler(0);
    JsAppHost::JsAppTaskHandler(reinterpret_cast<UINT32>((uintptr_t)jsAppHost));
    jsAppHost->ForceDestroy();
    delete jsAppHost;
}

TEST_F(JsAppHostTest, JsAppTaskHandlerLifecycleTest)
{
    auto* jsAppHost = new JsAppHost();
    TSK_INIT_PARAM_S stTskInitParam {};
    LOS_TaskLock();
    stTskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)(JsAppHost::JsAppTaskHandler);
    stTskInitParam.uwStackSize = TASK_STACK_SIZE;
    stTskInitParam.usTaskPrio = OS_TASK_PRIORITY_LOWEST - APP_TASK_PRI;
    stTskInitParam.pcName = const_cast<char *>("AppTask");
    stTskInitParam.uwResved = 0;
    stTskInitParam.uwArg = reinterpret_cast<UINT32>((uintptr_t)jsAppHost);
    UINT32 appTaskId = 0;
    UINT32 ret = LOS_TaskCreate(&appTaskId, &stTskInitParam);
    ASSERT_EQ(ret, LOS_OK);
    osMessageQueueId_t jsAppQueueId = osMessageQueueNew(QUEUE_LENGTH, sizeof(ACELite::AbilityInnerMsg), nullptr);
    jsAppHost->SetMessageQueueId(jsAppQueueId);
    LOS_TaskUnlock();

    ACELite::AbilityInnerMsg innerMsg1;
    innerMsg1.token = TEST_TOKEN;
    innerMsg1.msgId = ACELite::ACTIVE;
    innerMsg1.bundleName = "";
    innerMsg1.path = "";
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg1, 0, 0);
    ASSERT_EQ(ret, LOS_OK);
    Request request1;
    uint8_t prio1 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request1, &prio1, osWaitForever);
    ASSERT_EQ(ret, LOS_OK);
    ASSERT_EQ(request1.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token1 = request1.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state1 = (request1.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token1, TEST_TOKEN);
    ASSERT_EQ(state1, STATE_ACTIVE);

    ACELite::AbilityInnerMsg innerMsg2;
    innerMsg2.token = TEST_TOKEN;
    innerMsg2.msgId = ACELite::BACKGROUND;
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg2, 0, 0);
    ASSERT_EQ(ret, LOS_OK);
    Request request2;
    uint8_t prio2 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request2, &prio2, osWaitForever);
    ASSERT_EQ(ret, LOS_OK);
    ASSERT_EQ(request2.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token2 = request2.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state2 = (request2.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token2, TEST_TOKEN);
    ASSERT_EQ(state2, STATE_BACKGROUND);

    ACELite::AbilityInnerMsg innerMsg3;
    innerMsg3.token = TEST_TOKEN;
    innerMsg3.msgId = ACELite::DESTROY;
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg3, 0, 0);
    ASSERT_EQ(ret, LOS_OK);
    Request request3;
    uint8_t prio3 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request3, &prio3, osWaitForever);
    ASSERT_EQ(ret, LOS_OK);
    ASSERT_EQ(request3.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token3 = request3.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state3 = (request3.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token3, TEST_TOKEN);
    ASSERT_EQ(state3, STATE_UNINITIALIZED);
    ret = osMessageQueueDelete(jsAppQueueId);
    ASSERT_EQ(ret, LOS_OK);
    delete jsAppHost;
}

TEST_F(JsAppHostTest, JsAppTaskHandlerOtherTest)
{
    auto* jsAppHost = new JsAppHost();
    TSK_INIT_PARAM_S stTskInitParam {};
    LOS_TaskLock();
    stTskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)(JsAppHost::JsAppTaskHandler);
    stTskInitParam.uwStackSize = TASK_STACK_SIZE;
    stTskInitParam.usTaskPrio = OS_TASK_PRIORITY_LOWEST - APP_TASK_PRI;
    stTskInitParam.pcName = const_cast<char *>("AppTask");
    stTskInitParam.uwResved = 0;
    stTskInitParam.uwArg = reinterpret_cast<UINT32>((uintptr_t)jsAppHost);
    UINT32 appTaskId = 0;
    UINT32 ret = LOS_TaskCreate(&appTaskId, &stTskInitParam);
    ASSERT_EQ(ret, LOS_OK);
    osMessageQueueId_t jsAppQueueId = osMessageQueueNew(QUEUE_LENGTH, sizeof(ACELite::AbilityInnerMsg), nullptr);
    jsAppHost->SetMessageQueueId(jsAppQueueId);
    LOS_TaskUnlock();

    ACELite::AbilityInnerMsg innerMsg1;
    innerMsg1.token = TEST_TOKEN;
    innerMsg1.msgId = ACELite::ACTIVE;
    innerMsg1.bundleName = "";
    innerMsg1.path = "";
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg1, 0, 0);
    ASSERT_EQ(ret, LOS_OK);
    Request request1;
    uint8_t prio1 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request1, &prio1, osWaitForever);
    ASSERT_EQ(ret, LOS_OK);
    ASSERT_EQ(request1.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token1 = request1.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state1 = (request1.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token1, TEST_TOKEN);
    ASSERT_EQ(state1, STATE_ACTIVE);

    ACELite::AbilityInnerMsg innerMsg2;
    innerMsg2.token = TEST_TOKEN;
    innerMsg2.msgId = ACELite::BACKPRESSED;
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg2, 0, 0);
    ASSERT_EQ(ret, LOS_OK);

    ACELite::AbilityInnerMsg innerMsg3;
    innerMsg3.token = TEST_TOKEN;
    innerMsg3.msgId = ACELite::ASYNCWORK;
    innerMsg3.data = nullptr;
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg3, 0, 0);
    ASSERT_EQ(ret, LOS_OK);

    ACELite::AbilityInnerMsg innerMsg4;
    innerMsg4.token = TEST_TOKEN;
    innerMsg4.msgId = ACELite::TE_EVENT;
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg4, 0, 0);
    ASSERT_EQ(ret, LOS_OK);

    ACELite::AbilityInnerMsg innerMsg5;
    innerMsg5.token = TEST_TOKEN;
    innerMsg5.msgId = ACELite::UNKNOWN;
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg5, 0, 0);
    ASSERT_EQ(ret, LOS_OK);

    ACELite::AbilityInnerMsg innerMsg6;
    innerMsg6.token = TEST_TOKEN;
    innerMsg6.msgId = ACELite::DESTROY;
    ret = osMessageQueuePut(jsAppQueueId, &innerMsg6, 0, 0);
    ASSERT_EQ(ret, LOS_OK);
    Request request6;
    uint8_t prio6 = 0;
    ret = osMessageQueueGet(amsQueueId_, &request6, &prio6, osWaitForever);
    ASSERT_EQ(ret, LOS_OK);
    ASSERT_EQ(request6.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token6 = request6.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state6 = (request6.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token6, TEST_TOKEN);
    ASSERT_EQ(state6, STATE_UNINITIALIZED);
    ret = osMessageQueueDelete(jsAppQueueId);
    ASSERT_EQ(ret, LOS_OK);
    delete jsAppHost;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
