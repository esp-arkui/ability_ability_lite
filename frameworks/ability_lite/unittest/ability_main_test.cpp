//#include <gtest/gtest.h>
//#include "../../../interfaces/innerkits/abilitymgr_lite/ability_main.h"


#define TDD_CASE_BEGIN() SetUp()
#define TDD_CASE_END() TearDown()

#define HC_SUCCESS 0x00000000

// using namespace testing::ext;


namespace OHOS
{
    class AbilityMainTest //: public testing::Test
    {
    public:
        // void SetUp() override
        // {
        //     printf("AbilityMainTest setup");
        // }

        // void TearDown() override
        // {
        //     printf("AbilityMainTest TearDown");
        // }

        void AbilityMainTest001();

    };

    void AbilityMainTest::AbilityMainTest001()
    {
        // printf("AbilityMainTest setup");
        // TDD_CASE_BEGIN();
        // const char *token = "com.huawei.launcher_BM70W1/aVSbkx+uI/WT/mO9NqmtEBx9esLAogYAid75/gTMpKWqrNUT5hS9Cj"
        //                       "Bq6kt1OcxgZzdCJ4HuVyS4dP8w=";
        // auto ret = AbilityMain(token);
        // EXPECT_TRUE(ret == HC_SUCCESS);
        // printf("--------AbilityMainTest001  end--------");
        // TDD_CASE_END();
    }

}
