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
#include "ability_thread.h"
#include "ability_thread_loader.h"
#include "js_ability_thread.h"
#include "native_ability_thread.h"

using namespace OHOS::AbilitySlite;

class AbilityThreadLoaderTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    static AbilityThread *createJsAbilityThread()
    {
        auto *jsThread = new JsAbilityThread();
        jsThread->appTaskId_ = TASK_ID;
        return jsThread;
    }

    static AbilityThread *createNativeAbilityThread()
    {
        auto *nativeThread = new NativeAbilityThread();
        nativeThread->nativeTaskId = TASK_ID;
        return nativeThread;
    }

    static constexpr uint16_t TASK_ID = 12;
};

TEST_F(AbilityThreadLoaderTest, AbilityThreadLoader)
{
    AbilityThreadLoader::GetInstance().SetCreatorFunc(AbilityThreadCreatorType::JS_CREATOR, nullptr);
    AbilityThreadLoader::GetInstance().SetCreatorFunc(AbilityThreadCreatorType::NATIVE_CREATOR, nullptr);
    AbilityThreadLoader::GetInstance().SetCreatorFunc(static_cast<AbilityThreadCreatorType>(3), createJsAbilityThread);
    AbilityThread *jsThread = AbilityThreadLoader::GetInstance().CreateAbilityThread(AbilityThreadCreatorType::JS_CREATOR);
    ASSERT_EQ(jsThread, nullptr);
    delete jsThread;
    AbilityThread *nativeThread = AbilityThreadLoader::GetInstance().CreateAbilityThread(AbilityThreadCreatorType::NATIVE_CREATOR);
    ASSERT_EQ(nativeThread, nullptr);
    delete nativeThread;
    AbilityThread *otherThread = AbilityThreadLoader::GetInstance().CreateAbilityThread(static_cast<AbilityThreadCreatorType>(3));
    ASSERT_EQ(otherThread, nullptr);
    delete otherThread;
    AbilityThreadLoader::GetInstance().SetCreatorFunc(AbilityThreadCreatorType::JS_CREATOR, createJsAbilityThread);
    AbilityThreadLoader::GetInstance().SetCreatorFunc(AbilityThreadCreatorType::NATIVE_CREATOR, createNativeAbilityThread);
    AbilityThread *jsThread1 = AbilityThreadLoader::GetInstance().CreateAbilityThread(AbilityThreadCreatorType::JS_CREATOR);
    ASSERT_NE(jsThread1, nullptr);
    ASSERT_EQ(jsThread1->appTaskId_, TASK_ID);
    delete jsThread1;
    AbilityThread *nativeThread1 = AbilityThreadLoader::GetInstance().CreateAbilityThread(AbilityThreadCreatorType::NATIVE_CREATOR);
    ASSERT_NE(nativeThread1, nullptr);
    NativeAbilityThread *nativeThread2 = static_cast<NativeAbilityThread *>(nativeThread1);
    ASSERT_EQ(nativeThread2->nativeTaskId, TASK_ID);
    delete nativeThread1;
    AbilityThread *otherThread1 = AbilityThreadLoader::GetInstance().CreateAbilityThread(static_cast<AbilityThreadCreatorType>(3));
    ASSERT_EQ(otherThread1, nullptr);
    delete otherThread1;
    AbilityThreadLoader::GetInstance().UnsetCreatorFunc(AbilityThreadCreatorType::JS_CREATOR);
    AbilityThreadLoader::GetInstance().UnsetCreatorFunc(AbilityThreadCreatorType::NATIVE_CREATOR);
}

TEST_F(AbilityThreadLoaderTest, UnsetCreatorFunc)
{
    AbilityThreadLoader::GetInstance().UnsetCreatorFunc(AbilityThreadCreatorType::JS_CREATOR);
    AbilityThreadLoader::GetInstance().UnsetCreatorFunc(AbilityThreadCreatorType::NATIVE_CREATOR);
    AbilityThreadLoader::GetInstance().UnsetCreatorFunc(static_cast<AbilityThreadCreatorType>(3));
    AbilityThread *jsThread = AbilityThreadLoader::GetInstance().CreateAbilityThread(AbilityThreadCreatorType::JS_CREATOR);
    ASSERT_EQ(jsThread, nullptr);
    delete jsThread;
    AbilityThread *nativeThread = AbilityThreadLoader::GetInstance().CreateAbilityThread(AbilityThreadCreatorType::NATIVE_CREATOR);
    ASSERT_EQ(nativeThread, nullptr);
    delete nativeThread;

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}