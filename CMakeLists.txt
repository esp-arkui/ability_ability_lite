cmake_minimum_required(VERSION 3.6)
project(abilityLite C CXX)

# If CMAKE_BUILD_TYPE was not specified, default to release.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release)
endif ()
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

# compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_SKIP_INSTALL_RPATH TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fsigned-char -ftree-vectorize -funroll-loops -fstack-protector-all")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c17 -fsigned-char -ftree-vectorize -funroll-loops -fstack-protector-all")
if (CMAKE_BUILD_TYPE MATCHES debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -ftest-coverage -fprofile-arcs --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -ftest-coverage -fprofile-arcs --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -fPIE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -fPIE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -s")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -s")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE")
endif ()
# liteos_m
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -Wno-unused-variable")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -Wno-unused-variable")
# define
add_compile_definitions(__LITEOS_M__ TASK_STACK_SIZE=65536 AMS_TASK_STACK_SIZE=32768 APP_PLATFORM_WATCHGT)
enable_testing()

# ability_lite
set(ABILITY_LITE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# interfaces
set(ABILITY_LITE_INTERFACES_KITS_ABILITY_LITE_DIR "${ABILITY_LITE_DIR}/interfaces/kits/ability_lite")
set(ABILITY_LITE_INTERFACES_KITS_ABILITY_LITE_INCLUDE_DIR "${ABILITY_LITE_INTERFACES_KITS_ABILITY_LITE_DIR}")
set(ABILITY_LITE_INTERFACES_KITS_ABILITY_LITE_SLITE_INCLUDE_DIR "${ABILITY_LITE_INTERFACES_KITS_ABILITY_LITE_DIR}/slite")
set(ABILITY_LITE_INTERFACES_KITS_WANT_LITE_DIR "${ABILITY_LITE_DIR}/interfaces/kits/want_lite")
set(ABILITY_LITE_INTERFACES_KITS_WANT_LITE_INCLUDE_DIR "${ABILITY_LITE_INTERFACES_KITS_WANT_LITE_DIR}")
set(ABILITY_LITE_INTERFACES_INNER_API_DIR "${ABILITY_LITE_DIR}/interfaces/inner_api/abilitymgr_lite")
set(ABILITY_LITE_INTERFACES_INNER_API_INCLUDE_DIR "${ABILITY_LITE_INTERFACES_INNER_API_DIR}")
set(ABILITY_LITE_INTERFACES_INNER_API_SLITE_INCLUDE_DIR "${ABILITY_LITE_INTERFACES_INNER_API_DIR}/slite")
# frameworks
set(ABILITY_LITE_FRAMEWORKS_ABILITY_LITE_DIR "${ABILITY_LITE_DIR}/frameworks/ability_lite")
set(ABILITY_LITE_FRAMEWORKS_ABILITY_LITE_SRC_DIR "${ABILITY_LITE_FRAMEWORKS_ABILITY_LITE_DIR}/src/slite")
set(ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_DIR "${ABILITY_LITE_DIR}/frameworks/abilitymgr_lite")
set(ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_DIR_INCLUDE_DIR "${ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_DIR}/include/slite")
set(ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_SRC_DIR "${ABILITY_LITE_FRAMEWORKS_ABILITYMGR_LITE_DIR}/src/slite")
set(ABILITY_LITE_FRAMEWORKS_WANT_LITE_DIR "${ABILITY_LITE_DIR}/frameworks/want_lite")
set(ABILITY_LITE_FRAMEWORKS_WANT_LITE_INCLUDE_DIR "${ABILITY_LITE_FRAMEWORKS_WANT_LITE_DIR}/include")
set(ABILITY_LITE_FRAMEWORKS_WANT_LITE_SRC_DIR "${ABILITY_LITE_FRAMEWORKS_WANT_LITE_DIR}/src")
# services
set(ABILITY_LITE_SERVICES_DIR "${ABILITY_LITE_DIR}/services/abilitymgr_lite")
set(ABILITY_LITE_SERVICES_SRC_DIR "${ABILITY_LITE_SERVICES_DIR}/src/slite")
set(ABILITY_LITE_SERVICES_INCLUDE_DIR "${ABILITY_LITE_SERVICES_DIR}/include/slite")
set(ABILITY_LITE_SERVICES_UTIL_INCLUDE_DIR "${ABILITY_LITE_SERVICES_DIR}/include/util")

add_subdirectory(test/slite)
