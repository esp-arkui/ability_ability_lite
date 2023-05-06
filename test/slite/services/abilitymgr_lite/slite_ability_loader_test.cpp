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
#include "slite_ace_ability.h"
#include "slite_ability.h"
#include "slite_ability_loader.h"

using namespace OHOS::AbilitySlite;

class MyNativeAbility : public SliteAbility
{
public:
int getNativeTest()
{
    return 200;
}
};

class SliteAbilityLoaderTest : public ::testing::Test {
protected:
static void SetUpTestCase()
{
}

static void TearDownTestCase()
{
}

static constexpr char LAUNCHER_BUNDLE_NAME[] = "com.ohos.launcher";

static SliteAbility *CreateJsAbilityThread(const char *bundleName)
{
    auto *jsAbility = new OHOS::ACELite::SliteAceAbility(bundleName);
    return jsAbility;
}

static SliteAbility *CreateNativeAbilityThread(const char *bundleName)
{
    auto *nativeAbility = new MyNativeAbility();
    return nativeAbility;
}
};

TEST_F(SliteAbilityLoaderTest, SliteAbilityLoaderSetNull)
{
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::JS_ABILITY, nullptr);
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::NATIVE_ABILITY, nullptr);
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(static_cast<SliteAbilityType>(3), CreateJsAbilityThread);
    SliteAbility *jsA = SliteAbilityLoader::GetInstance().CreateAbility(SliteAbilityType::JS_ABILITY, LAUNCHER_BUNDLE_NAME);
    ASSERT_EQ(jsA, nullptr);
    SliteAbility *naA = SliteAbilityLoader::GetInstance().CreateAbility(SliteAbilityType::NATIVE_ABILITY, LAUNCHER_BUNDLE_NAME);
    ASSERT_EQ(naA, nullptr);
}

TEST_F(SliteAbilityLoaderTest, SliteAbilityLoader)
{
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::JS_ABILITY, CreateJsAbilityThread);
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::NATIVE_ABILITY, CreateNativeAbilityThread);
    SliteAbility *jsA = SliteAbilityLoader::GetInstance().CreateAbility(SliteAbilityType::JS_ABILITY, LAUNCHER_BUNDLE_NAME);
    ASSERT_NE(jsA, nullptr);
    delete jsA;
    SliteAbility *naA = SliteAbilityLoader::GetInstance().CreateAbility(SliteAbilityType::NATIVE_ABILITY, LAUNCHER_BUNDLE_NAME);
    ASSERT_NE(naA, nullptr);
    MyNativeAbility *myNativeAbility = static_cast<MyNativeAbility *>(naA);
    ASSERT_EQ(myNativeAbility->getNativeTest(), 200);
    SliteAbilityLoader::GetInstance().UnsetAbilityCreatorFunc(SliteAbilityType::JS_ABILITY);
    SliteAbilityLoader::GetInstance().UnsetAbilityCreatorFunc(SliteAbilityType::NATIVE_ABILITY);
    delete naA;
}

TEST_F(SliteAbilityLoaderTest, SliteAbilityLoaderGetWithOtherType)
{
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::JS_ABILITY, CreateJsAbilityThread);
    SliteAbilityLoader::GetInstance().SetAbilityCreatorFunc(SliteAbilityType::NATIVE_ABILITY, CreateNativeAbilityThread);
    SliteAbility *ability = SliteAbilityLoader::GetInstance().CreateAbility(static_cast<SliteAbilityType>(3), LAUNCHER_BUNDLE_NAME);
    SliteAbilityLoader::GetInstance().UnsetAbilityCreatorFunc(SliteAbilityType::JS_ABILITY);
    SliteAbilityLoader::GetInstance().UnsetAbilityCreatorFunc(SliteAbilityType::NATIVE_ABILITY);
    ASSERT_EQ(ability, nullptr);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
