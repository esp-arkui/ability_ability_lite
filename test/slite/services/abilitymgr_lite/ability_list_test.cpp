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
#include "ability_record.h"
#include "ability_list.h"

using namespace OHOS::AbilitySlite;

class AbilityListTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
        testRecord = new AbilityRecord;
        testRecord->SetAppName(LAUNCHER_BUNDLE_NAME);
        testRecord->token = LAUNCHER_TOKEN;
        testRecord->taskId = TASK_ID;
    }

    static void TearDownTestCase()
    {
        delete testRecord;
    }
    static AbilityRecord *testRecord;
    static constexpr char LAUNCHER_BUNDLE_NAME[] = "com.ohos.launcher";
    static constexpr char TEST_BUNDLE_NAME[] = "com.xxxxx";
    static constexpr uint16_t LAUNCHER_TOKEN = 10;
    static constexpr uint16_t TASK_ID = 11;
};
AbilityRecord *AbilityListTest::testRecord { nullptr };

TEST_F(AbilityListTest, AbilityListAddAndGet0)
{
    AbilityList abilityList_ {};
    AbilityRecord *point = abilityList_.Get(TEST_BUNDLE_NAME);
    ASSERT_EQ(point, nullptr);
}

TEST_F(AbilityListTest, AbilityListErase)
{
    AbilityList abilityList_ {};
    abilityList_.Add(testRecord);
    AbilityRecord *point = abilityList_.Get(LAUNCHER_TOKEN);
    ASSERT_NE(point, nullptr);
    abilityList_.Erase(LAUNCHER_TOKEN);
    AbilityRecord *point1 = abilityList_.Get(LAUNCHER_TOKEN);
    ASSERT_EQ(point1, nullptr);
}

TEST_F(AbilityListTest, AbilityListAddAndGet1)
{
    AbilityList abilityList_ {};
    abilityList_.Add(testRecord);
    AbilityRecord *point = abilityList_.Get(LAUNCHER_TOKEN);
    ASSERT_NE(point, nullptr);
    ASSERT_EQ(point->token, LAUNCHER_TOKEN);
}

TEST_F(AbilityListTest, AbilityListAddAndGet2)
{
    AbilityList abilityList_ {};
    abilityList_.Add(testRecord);
    AbilityRecord *point = abilityList_.Get(LAUNCHER_BUNDLE_NAME);
    ASSERT_NE(point, nullptr);
    char *appName = point->appName;
    ASSERT_STREQ(appName, LAUNCHER_BUNDLE_NAME);
}

TEST_F(AbilityListTest, AbilityListAddAndGet3)
{
    AbilityList abilityList_ {};
    abilityList_.Add(testRecord);
    AbilityRecord *point = abilityList_.GetByTaskId(TASK_ID);
    ASSERT_NE(point, nullptr);
    char *appName = point->appName;
    ASSERT_STREQ(appName, LAUNCHER_BUNDLE_NAME);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}