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
#include "adapter.h"
#include "securec.h"

using namespace OHOS::AbilitySlite;

TEST(AbilityDataTest, AbilityDataDefaultTest)
{
    auto *abilityData = new AbilityData();
    ASSERT_EQ(abilityData->wantData, nullptr);
    ASSERT_EQ(abilityData->wantDataSize, 0);
    delete abilityData;
}

TEST(AbilityRecordTest, AbilityRecordDefaultTest)
{
    auto *abilityRecord = new AbilityRecord();
    ASSERT_EQ(abilityRecord->appName, nullptr);
    ASSERT_EQ(abilityRecord->appPath, nullptr);
    ASSERT_EQ(abilityRecord->abilityData, nullptr);
    ASSERT_EQ(abilityRecord->abilityThread, nullptr);
    ASSERT_EQ(abilityRecord->jsAppQueueId, nullptr);
    ASSERT_EQ(abilityRecord->taskId, 0u);
    ASSERT_EQ(abilityRecord->token, 0u);
    ASSERT_EQ(abilityRecord->state, SCHEDULE_STOP);
    ASSERT_EQ(abilityRecord->isTerminated, false);
    delete abilityRecord;
}

TEST(AbilityRecordTest, SetAppNameTest)
{
    auto *abilityRecord = new AbilityRecord();
    const char *TEST_NAME1 = "testName1";
    const char *TEST_NAME2 = "testName2";
    abilityRecord->SetAppName(TEST_NAME1);
    ASSERT_STREQ(abilityRecord->appName, TEST_NAME1);
    ASSERT_NE(abilityRecord->appName, TEST_NAME1);
    abilityRecord->SetAppName(TEST_NAME2);
    ASSERT_STREQ(abilityRecord->appName, TEST_NAME2);
    ASSERT_NE(abilityRecord->appName, TEST_NAME2);
    delete abilityRecord;
}

TEST(AbilityRecordTest, SetAppPathTest)
{
    auto *abilityRecord = new AbilityRecord();
    const char *TEST_PATH1 = "testPath1";
    const char *TEST_PATH2 = "testPath2";
    abilityRecord->SetAppPath(TEST_PATH1);
    ASSERT_STREQ(abilityRecord->appPath, TEST_PATH1);
    ASSERT_NE(abilityRecord->appPath, TEST_PATH1);
    abilityRecord->SetAppPath(TEST_PATH2);
    ASSERT_STREQ(abilityRecord->appPath, TEST_PATH2);
    ASSERT_NE(abilityRecord->appPath, TEST_PATH2);
    delete abilityRecord;
}

TEST(AbilityRecordTest, SetWantDataTest)
{
    auto *abilityRecord = new AbilityRecord();
    const uint16_t buffer1Size = 27;
    void *buffer1 = AdapterMalloc(buffer1Size);
    const uint16_t buffer2Size = 1933;
    void *buffer2 = AdapterMalloc(buffer2Size);
    const uint16_t buffer3Size = 739;
    void *buffer3 = AdapterMalloc(buffer3Size);

    abilityRecord->SetWantData(buffer1, buffer1Size);
    ASSERT_NE(abilityRecord->abilityData, nullptr);
    ASSERT_NE(abilityRecord->abilityData->wantData, nullptr);
    ASSERT_NE(abilityRecord->abilityData->wantData, buffer1);
    ASSERT_EQ(abilityRecord->abilityData->wantDataSize, buffer1Size);
    errno_t res = memcpy_s(abilityRecord->abilityData->wantData, abilityRecord->abilityData->wantDataSize,
        buffer1, buffer1Size);
    ASSERT_EQ(res, EOK);

    abilityRecord->SetWantData(buffer2, buffer2Size);
    ASSERT_NE(abilityRecord->abilityData, nullptr);
    ASSERT_NE(abilityRecord->abilityData->wantData, nullptr);
    ASSERT_NE(abilityRecord->abilityData->wantData, buffer2);
    ASSERT_EQ(abilityRecord->abilityData->wantDataSize, buffer2Size);
    res = memcpy_s(abilityRecord->abilityData->wantData, abilityRecord->abilityData->wantDataSize,
        buffer2, buffer2Size);
    ASSERT_EQ(res, EOK);

    abilityRecord->SetWantData(nullptr, 0);
    ASSERT_NE(abilityRecord->abilityData, nullptr);
    ASSERT_EQ(abilityRecord->abilityData->wantData, nullptr);
    ASSERT_EQ(abilityRecord->abilityData->wantDataSize, 0);

    abilityRecord->SetWantData(buffer3, buffer3Size);
    ASSERT_NE(abilityRecord->abilityData, nullptr);
    ASSERT_NE(abilityRecord->abilityData->wantData, nullptr);
    ASSERT_NE(abilityRecord->abilityData->wantData, buffer3);
    ASSERT_EQ(abilityRecord->abilityData->wantDataSize, buffer3Size);
    res = memcpy_s(abilityRecord->abilityData->wantData, abilityRecord->abilityData->wantDataSize,
        buffer3, buffer3Size);
    ASSERT_EQ(res, EOK);

    AdapterFree(buffer1);
    AdapterFree(buffer2);
    AdapterFree(buffer3);
    delete abilityRecord;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
