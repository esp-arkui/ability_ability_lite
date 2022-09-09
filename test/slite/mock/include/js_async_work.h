/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_JS_ASYNC_WORK_H
#define OHOS_ACELITE_JS_ASYNC_WORK_H

#include "message_queue_utils.h"
#include <cstdint>

namespace OHOS {
namespace ACELite {
typedef void *QueueHandler;

/**
 * Function pointer type used for async work.
 * Note: this type is deprecated, use AsyncHandler alternatively
 */
typedef void (*AsyncWorkHandler)(void *data);

/**
 * Function pointer type used for async work.
 */
typedef void (*AsyncHandler)(void *data, int8_t statusCode);

typedef bool (*FatalHandleFunc)();

struct AsyncWork {
    AsyncWorkHandler workHandler;
    AsyncHandler handler;
    void* data;
    AsyncWork() : workHandler(nullptr), handler(nullptr), data(nullptr) {}
    AsyncWork(const AsyncWork &) = delete;
    AsyncWork &operator=(const AsyncWork &) = delete;
    AsyncWork(AsyncWork &&) = delete;
    AsyncWork &operator=(AsyncWork &&) = delete;
};

class JsAsyncWork {
public:
    static void ExecuteAsyncWork(AsyncWork *&asyncWork, int8_t statusCode = 0);

    static void SetAppQueueHandler(const QueueHandler handler);

};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_JS_ASYNC_WORK_H
