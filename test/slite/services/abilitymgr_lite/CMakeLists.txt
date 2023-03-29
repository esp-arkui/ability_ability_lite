find_package(Threads REQUIRED)
include(GoogleTest)

include_directories(
        ${ABILITY_LITE_INTERFACES_KITS_ABILITY_LITE_SLITE_INCLUDE_DIR}
        ${ABILITY_LITE_INTERFACES_KITS_WANT_LITE_INCLUDE_DIR}
        ${ABILITY_LITE_INTERFACES_INNER_API_INCLUDE_DIR}
        ${ABILITY_LITE_INTERFACES_INNER_API_SLITE_INCLUDE_DIR}
        ${ABILITY_LITE_FRAMEWORKS_WANT_LITE_INCLUDE_DIR}
        ${ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_DIR_INCLUDE_DIR}
        ${ABILITY_LITE_SERVICES_INCLUDE_DIR}
        ${ABILITY_LITE_SERVICES_UTIL_INCLUDE_DIR}
        ${SECUREC_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIR}
        ${MOCK_INCLUDE_DIR})

aux_source_directory(${ABILITY_LITE_SERVICES_SRC_DIR} ABILITY_LITE_SERVICES_SRC)
aux_source_directory(${ABILITY_LITE_FRAMEWORKS_WANT_LITE_SRC_DIR} ABILITY_LITE_FRAMEWORKS_WANT_LITE_SRC)
aux_source_directory(${ABILITY_LITE_FRAMEWORKS_ABILITY_LITE_SRC_DIR} ABILITY_LITE_FRAMEWORKS_ABILITY_LITE_SRC)
aux_source_directory(${ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_SRC_DIR} ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_SRC)
set(ABILITY_SERVICE_SRC
        ${ABILITY_LITE_SERVICES_SRC}
        ${ABILITY_LITE_FRAMEWORKS_WANT_LITE_SRC}
        ${ABILITY_LITE_FRAMEWORKS_ABILITY_LITE_SRC}
        ${ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_SRC})
add_executable(ability_record_manager_test ability_record_manager_test.cpp ${ABILITY_SERVICE_SRC})
target_link_libraries(ability_record_manager_test PUBLIC ${GTEST} Threads::Threads ${SECUREC} ${MOCK} ${DEP})

add_executable(abilityms_slite_client_test abilityms_slite_client_test.cpp ${ABILITY_SERVICE_SRC})
target_link_libraries(abilityms_slite_client_test PUBLIC ${GTEST} Threads::Threads ${SECUREC} ${MOCK} ${DEP})

add_executable(ability_record_test ability_record_test.cpp ${ABILITY_LITE_SERVICES_SRC_DIR}/ability_record.cpp)
target_link_libraries(ability_record_test PUBLIC ${GTEST} Threads::Threads ${SECUREC} ${MOCK} ${DEP})

add_executable(js_ability_thread_test js_ability_thread_test.cpp ${ABILITY_SERVICE_SRC})
target_link_libraries(js_ability_thread_test PUBLIC ${GTEST} Threads::Threads ${SECUREC} ${MOCK} ${DEP})

set(GTEST_REPEAT 10)
gtest_discover_tests(ability_record_manager_test EXTRA_ARGS --gtest_shuffle --gtest_repeat=${GTEST_REPEAT})
gtest_discover_tests(abilityms_slite_client_test EXTRA_ARGS --gtest_shuffle --gtest_repeat=${GTEST_REPEAT})
gtest_discover_tests(ability_record_test EXTRA_ARGS --gtest_shuffle --gtest_repeat=${GTEST_REPEAT})
gtest_discover_tests(js_ability_thread_test EXTRA_ARGS --gtest_shuffle --gtest_repeat=${GTEST_REPEAT})
