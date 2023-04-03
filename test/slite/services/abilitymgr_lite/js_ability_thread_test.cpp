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
#include "utils.h"

using namespace OHOS;
using namespace OHOS::AbilitySlite;
namespace {
constexpr int32_t QUEUE_LENGTH = 32;
constexpr uint16_t TEST_TOKEN = 123;

uint32_t AbilityMsgIdToState(SliteAbilityMsgId id)
{
    switch (id) {
        case SliteAbilityMsgId::CREATE:
            return SLITE_STATE_INITIAL;
        case SliteAbilityMsgId::FOREGROUND:
            return SLITE_STATE_FOREGROUND;
        case SliteAbilityMsgId::BACKGROUND:
            return SLITE_STATE_BACKGROUND;
        case SliteAbilityMsgId::DESTROY:
            return SLITE_STATE_UNINITIALIZED;
        default:
            return SLITE_STATE_UNKNOWN;
    }
}

void LocalSchedulerLifecycleDone(osMessageQueueId_t amsQueueId, osMessageQueueId_t messageQueueId,
    AbilityThread *abilityThread, SliteAbilityMsgId id, Want *want = nullptr)
{
    SliteAbilityInnerMsg innerMsg;
    innerMsg.token = TEST_TOKEN;
    innerMsg.msgId = id;
    innerMsg.abilityThread = abilityThread;
    innerMsg.want = want;
    auto ret = osMessageQueuePut(messageQueueId, &innerMsg, 0, 0);
    ASSERT_EQ(ret, osOK);
    Request request;
    uint8_t prio = 0;
    ret = osMessageQueueGet(amsQueueId, &request, &prio, osWaitForever);
    ASSERT_EQ(ret, osOK);
    ASSERT_EQ(request.msgId, ABILITY_TRANSACTION_DONE);
    uint32_t token = request.msgValue & TRANSACTION_MSG_TOKEN_MASK;
    uint32_t state = (request.msgValue >> TRANSACTION_MSG_STATE_OFFSET) & TRANSACTION_MSG_STATE_MASK;
    ASSERT_EQ(token, TEST_TOKEN);
    ASSERT_EQ(state, AbilityMsgIdToState(id));
}

Want *InitWant()
{
    auto *want = (Want *) AdapterMalloc(sizeof(Want));
    want->data = nullptr;
    want->dataLength = 0;
    want->appPath = Utils::Strdup(APP_BUNDLE_PATH);
    want->element = (ElementName *) AdapterMalloc(sizeof(ElementName));
    want->element->bundleName = Utils::Strdup(APP1_BUNDLE_NAME);
    want->element->abilityName = nullptr;
    want->element->deviceId = nullptr;
    return want;
}
}


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

    LocalSchedulerLifecycleDone(amsQueueId_, messageQueueId, jsAbilityThread, SliteAbilityMsgId::DESTROY);
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

    LocalSchedulerLifecycleDone(amsQueueId_, messageQueueId, jsAbilityThread, SliteAbilityMsgId::CREATE, InitWant());
    LocalSchedulerLifecycleDone(amsQueueId_, messageQueueId, jsAbilityThread, SliteAbilityMsgId::FOREGROUND, InitWant());
    LocalSchedulerLifecycleDone(amsQueueId_, messageQueueId, jsAbilityThread, SliteAbilityMsgId::BACKGROUND);
    LocalSchedulerLifecycleDone(amsQueueId_, messageQueueId, jsAbilityThread, SliteAbilityMsgId::DESTROY);

    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), ERR_OK);
    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), PARAM_CHECK_ERROR);
    delete jsAbilityThread;
}

TEST_F(JsAbilityThreadTest, JsAppTaskHandlerOtherTest)
{
    auto *jsAbilityThread = new JsAbilityThread();
    AbilityRecord record;
    record.appName = Utils::Strdup(APP1_BUNDLE_NAME);
    record.token = TEST_TOKEN;
    ASSERT_EQ(jsAbilityThread->InitAbilityThread(&record), ERR_OK);
    osMessageQueueId_t messageQueueId = jsAbilityThread->messageQueueId_;
    ASSERT_NE(messageQueueId, nullptr);

    LocalSchedulerLifecycleDone(amsQueueId_, messageQueueId, jsAbilityThread, SliteAbilityMsgId::CREATE, InitWant());

    SliteAbilityInnerMsg innerMsg5;
    innerMsg5.token = TEST_TOKEN;
    innerMsg5.msgId = SliteAbilityMsgId::UNKNOWN;
    innerMsg5.abilityThread = jsAbilityThread;
    auto ret = osMessageQueuePut(messageQueueId, &innerMsg5, 0, 0);
    ASSERT_EQ(ret, osOK);

    LocalSchedulerLifecycleDone(amsQueueId_, messageQueueId, jsAbilityThread, SliteAbilityMsgId::DESTROY);
    ASSERT_EQ(jsAbilityThread->ReleaseAbilityThread(), ERR_OK);
    delete jsAbilityThread;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
