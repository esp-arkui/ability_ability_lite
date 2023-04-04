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
#include "bms_helper.h"
#include "utils.h"

using namespace OHOS::AbilitySlite;

class BMSHelperTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {

    }

    static void TearDownTestCase()
    {

    }
    const char *BUNDLE_NAME1 = "com.ohos.launcher1";
    const char *BUNDLE_NAME2 = "com.ohos.launcher2";
};

TEST_F(BMSHelperTest, BMSHelperRegisteNull)
{
    OHOS::List<char *> bundleNames {};
    BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
    char *bName1 = OHOS::Utils::Strdup(BUNDLE_NAME1);
    bool isNativeApp = BMSHelper::GetInstance().IsNativeApp(bName1);
    ASSERT_FALSE(isNativeApp);
    AdapterFree(bName1);
}

TEST_F(BMSHelperTest, BMSHelperRegisteList0)
{
    OHOS::List<char *> bundleNames {};
    char *bName1 = OHOS::Utils::Strdup(BUNDLE_NAME1);
    char *bName2 = OHOS::Utils::Strdup(BUNDLE_NAME2);
    bundleNames.PushBack(bName1);
    BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
    bool isNativeApp1 = BMSHelper::GetInstance().IsNativeApp(bName1);
    ASSERT_TRUE(isNativeApp1);
    bool isNativeApp2 = BMSHelper::GetInstance().IsNativeApp(bName2);
    ASSERT_FALSE(isNativeApp2);
    AdapterFree(bName1);
    AdapterFree(bName2);
}

TEST_F(BMSHelperTest, BMSHelperRegisteErase)
{
    OHOS::List<char *> bundleNames {};
    char *bName1 = OHOS::Utils::Strdup(BUNDLE_NAME1);
    bundleNames.PushBack(bName1);
    BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
    bool isNativeApp = BMSHelper::GetInstance().IsNativeApp(bName1);
    ASSERT_TRUE(isNativeApp);
    BMSHelper::GetInstance().Erase();
    bool isNativeApp1 = BMSHelper::GetInstance().IsNativeApp(bName1);
    ASSERT_FALSE(isNativeApp1);
    AdapterFree(bName1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}