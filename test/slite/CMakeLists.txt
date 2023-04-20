
# test
set(ABILITY_TEST_DIR "${ABILITY_LITE_DIR}/test/slite")

# bounds_checking_function
set(SECUREC securec)
set(SECUREC_DIR "${ABILITY_LITE_DIR}/../../../third_party/bounds_checking_function")
set(SECUREC_INCLUDE_DIR "${SECUREC_DIR}/include")
aux_source_directory(${SECUREC_DIR}/src SECUREC_SRC)
include_directories(${SECUREC_INCLUDE_DIR})
add_library(${SECUREC} SHARED ${SECUREC_SRC})

# gtest
set(GTEST gtest)
set(GTEST_DIR "${ABILITY_LITE_DIR}/../../../third_party/googletest/googletest")
set(GTEST_INCLUDE_DIR "${GTEST_DIR}/include")
aux_source_directory(${GTEST_DIR}/src GTEST_SRC)
include_directories(${GTEST_INCLUDE_DIR} ${GTEST_DIR})
add_library(${GTEST} STATIC ${GTEST_SRC})

# dep
set(DEP "dep")
set(DEP_DIR "${ABILITY_TEST_DIR}/dep")
set(DEP_INCLUDE_DIR "${DEP_DIR}/include")
aux_source_directory(${DEP_DIR}/src DEP_SRC)
include_directories(${DEP_INCLUDE_DIR} ${SECUREC_INCLUDE_DIR})
add_library(${DEP} STATIC ${DEP_SRC})
target_link_libraries(${DEP} PUBLIC ${SECUREC})

# mock
set(MOCK "mock")
set(MOCK_DIR "${ABILITY_TEST_DIR}/mock")
set(MOCK_INCLUDE_DIR "${MOCK_DIR}/include")
aux_source_directory(${MOCK_DIR}/src MOCK_SRC)
include_directories(
        ${MOCK_INCLUDE_DIR}
        ${ABILITY_LITE_INTERFACES_KITS_WANT_LITE_INCLUDE_DIR}
        ${ABILITY_LITE_INTERFACES_KITS_ABILITY_LITE_SLITE_INCLUDE_DIR}
        ${ABILITY_LITE_INTERFACES_KITS_ABILITY_LITE_INCLUDE_DIR})
add_library(${MOCK} STATIC ${MOCK_SRC})

add_subdirectory(services/abilitymgr_lite)