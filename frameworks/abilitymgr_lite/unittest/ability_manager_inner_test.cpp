#include "gtest/gtest.h"
#include "../../../interfaces/innerkits/abilitymgr_lite/slite/ability_manager_inner.h"

#define TDD_CASE_BEGIN() SetUp()
#define TDD_CASE_END() TearDown()

#define HC_SUCCESS 0x00000000

using namespace testing::ext;

typedef int (*StartCheckFunc)(const char *bundleName);

namespace OHOS
{
    class AbilityManagerInnerTest : public testing::Test
    {
    public:
        void SetUp() override
        {
            printf("AbilityManagerInnerTest setup");
        }

        void TearDown() override
        {
            printf("AbilityManagerInnerTest TearDown");
        }

        void AbilityManagerInnerTest001();
        void AbilityManagerInnerTest002();
        void AbilityManagerInnerTest003();
        void AbilityManagerInnerTest004();
        void AbilityManagerInnerTest005();
        void AbilityManagerInnerTest006();
        void AbilityManagerInnerTest007();
        void AbilityManagerInnerTest008();
        void RunTests();
    };

    void AbilityManagerInnerTest::AbilityManagerInnerTest001()
    {
        printf("AbilityManagerInnerTest setup");
        TDD_CASE_BEGIN();
        StartCheckFunc startChecktCallback = getAbilityCallback();
        auto ret = RegAbilityCallback(startChecktCallback);
        EXPECT_TRUE(ret == HC_SUCCESS);
        printf("--------AbilityManagerInnerTest001 RegAbilityCallback end--------");
        TDD_CASE_END();
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest002()
    {
        printf("--------AbilityManagerInnerTest002 SchedulerLifecycleDone begin--------");
        TDD_CASE_BEGIN();
        int state = 2;
        auto token = 0;
        auto ret = SchedulerLifecycleDone(token, state);
        EXPECT_TRUE(ret == HC_SUCCESS);
        printf("--------AbilityManagerInnerTest002 SchedulerLifecycleDone end--------");
        TDD_CASE_END();
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest003()
    {
        printf("--------AbilityManagerInnerTest003 ForceStopBundle begin--------");
        TDD_CASE_BEGIN();
        auto token = 0;
        auto ret = ForceStopBundle(token);
        EXPECT_TRUE(ret == HC_SUCCESS);
        printf("--------AbilityManagerInnerTest003 ForceStopBundle end--------");
        TDD_CASE_END();
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest004()
    {
        printf("--------AbilityManagerInnerTest004 GetTopAbility begin--------");
        TDD_CASE_BEGIN();
        auto ret = GetTopAbility();
        EXPECT_TRUE(ret == nullptr);
        printf("--------AbilityManagerInnerTest004 GetTopAbility end--------");
        TDD_CASE_END();
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest005()
    {
        printf("--------AbilityManagerInnerTest005 ForceStop begin--------");
        TDD_CASE_BEGIN();
        char *bundlename = "com.huawei.launcher";
        auto ret = ForceStop(bundlename);
        EXPECT_TRUE(ret == HC_SUCCESS);
        printf("--------AbilityManagerInnerTest005 ForceStop end--------");
        TDD_CASE_END();
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest006()
    {
        printf("--------AbilityManagerInnerTest006 getAbilityCallback begin--------");
        TDD_CASE_BEGIN();
        auto ret = getAbilityCallback();
        EXPECT_TRUE(ret == nullptr);
        printf("--------AbilityManagerInnerTest006 getAbilityCallback end--------");
        TDD_CASE_END();
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest007()
    {
        printf("--------AbilityManagerInnerTest007 setCleanAbilityDataFlag begin--------");
        TDD_CASE_BEGIN();
        bool cleanFlag = true;
        setCleanAbilityDataFlag(cleanFlag);
        printf("--------AbilityManagerInnerTest007 setCleanAbilityDataFlag end--------");
        TDD_CASE_END();
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest008()
    {
        printf("--------AbilityManagerInnerTest008 getCleanAbilityDataFlag begin--------");
        TDD_CASE_BEGIN();
        auto ret = getCleanAbilityDataFlag();
        EXPECT_TRUE(ret == HC_SUCCESS);
        printf("--------AbilityManagerInnerTest008 getCleanAbilityDataFlag end--------");
        TDD_CASE_END();
    }

    void AbilityManagerInnerTest::RunTests()
    {
        void AbilityManagerInnerTest001();
        void AbilityManagerInnerTest002();
        void AbilityManagerInnerTest003();
        void AbilityManagerInnerTest004();
        void AbilityManagerInnerTest005();
        void AbilityManagerInnerTest006();
        void AbilityManagerInnerTest007();
        void AbilityManagerInnerTest008();
    }

}
