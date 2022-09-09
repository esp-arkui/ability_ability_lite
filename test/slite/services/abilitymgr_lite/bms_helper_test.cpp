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
#include "bms_helper.h"
#include "utils.h"
#include "bundle_manager.h"

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
    const char *BUNDLE_NAME3 = "com.ohos.test.app1";
};

TEST_F(BMSHelperTest, BMSHelperRegisteNull)
{
    OHOS::List<const char *> bundleNames {};
    BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
    bool isNativeApp = BMSHelper::GetInstance().IsNativeApp(BUNDLE_NAME1);
    ASSERT_FALSE(isNativeApp);
}

TEST_F(BMSHelperTest, BMSHelperRegisteList0)
{
    OHOS::List<const char *> bundleNames {};
    bundleNames.PushBack(BUNDLE_NAME1);
    BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
    bool isNativeApp1 = BMSHelper::GetInstance().IsNativeApp(BUNDLE_NAME1);
    ASSERT_TRUE(isNativeApp1);
    bool isNativeApp2 = BMSHelper::GetInstance().IsNativeApp(BUNDLE_NAME2);
    ASSERT_FALSE(isNativeApp2);
    bool isNativeApp3 = BMSHelper::GetInstance().IsNativeApp(nullptr);
    ASSERT_FALSE(isNativeApp3);
}

TEST_F(BMSHelperTest, BMSHelperRegisteErase)
{
    OHOS::List<const char *> bundleNames {};
    bundleNames.PushBack(BUNDLE_NAME1);
    BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
    bool isNativeApp = BMSHelper::GetInstance().IsNativeApp(BUNDLE_NAME1);
    ASSERT_TRUE(isNativeApp);
    BMSHelper::GetInstance().Erase();
    bool isNativeApp1 = BMSHelper::GetInstance().IsNativeApp(BUNDLE_NAME1);
    ASSERT_FALSE(isNativeApp1);
}

TEST_F(BMSHelperTest, BMSHelperQueryAbilitySvcInfo1)
{
    Want *want = nullptr;
    auto *info = static_cast<AbilitySvcInfo *>(AdapterMalloc(sizeof(AbilitySvcInfo)));
    uint8_t queryRet0 = BMSHelper::GetInstance().QueryAbilitySvcInfo(want, info);
    ASSERT_NE(queryRet0, ERR_OK);
    want = (Want *) AdapterMalloc(sizeof(Want));
    want->element = nullptr;
    uint8_t queryRet1 = BMSHelper::GetInstance().QueryAbilitySvcInfo(want, info);
    ASSERT_NE(queryRet1, ERR_OK);
    want->element = reinterpret_cast<ElementName *>(AdapterMalloc(sizeof(ElementName)));
    want->element->bundleName = nullptr;
    uint8_t queryRet2 = BMSHelper::GetInstance().QueryAbilitySvcInfo(want, info);
    ASSERT_NE(queryRet2, ERR_OK);
    char *queryName = OHOS::Utils::Strdup(BUNDLE_NAME1);
    want->element->bundleName = queryName;
    uint8_t queryRet3 = BMSHelper::GetInstance().QueryAbilitySvcInfo(want, info);
    ASSERT_NE(queryRet3, ERR_OK);
    OHOS::List<const char *> bundleNames {};
    bundleNames.PushBack(BUNDLE_NAME1);
    BMSHelper::GetInstance().RegisterBundleNames(bundleNames);
    uint8_t queryRet4 = BMSHelper::GetInstance().QueryAbilitySvcInfo(want, info);
    ASSERT_EQ(queryRet4, ERR_OK);
    ASSERT_TRUE(info->isNativeApp);
    BMSHelper::GetInstance().Erase();
    AdapterFree(want->element->bundleName);
    AdapterFree(want->element);
    AdapterFree(want);
    AdapterFree(info->bundleName);
    AdapterFree(info->path);
    AdapterFree(info);
}

TEST_F(BMSHelperTest, BMSHelperQueryAbilitySvcInfo2)
{
    Want *want = (Want *) AdapterMalloc(sizeof(Want));
    want->element = reinterpret_cast<ElementName *>(AdapterMalloc(sizeof(ElementName)));
    char *bName1 = OHOS::Utils::Strdup(BUNDLE_NAME3);
    want->element->bundleName = bName1;
    auto *info = static_cast<AbilitySvcInfo *>(AdapterMalloc(sizeof(AbilitySvcInfo)));
    uint8_t queryRet1 = BMSHelper::GetInstance().QueryAbilitySvcInfo(want, info);
    ASSERT_EQ(queryRet1, ERR_OK);
    ASSERT_FALSE(info->isNativeApp);
    AdapterFree(want->element->bundleName);
    AdapterFree(want->element);
    AdapterFree(want);
    AdapterFree(info->bundleName);
    AdapterFree(info->path);
    AdapterFree(info);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
