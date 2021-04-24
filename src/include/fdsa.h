/*
 * This file is part of fDSA,
 * Copyright(C) 2019-2021 fdar0536.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "internal/defines.h"
#include "internal/ptrmap.h"
#include "internal/ptrvector.h"
#include "internal/vector.h"

#if defined _WIN32 || defined __CYGWIN__
#ifdef __MINGW32__
#define FDSA_API __attribute__((dllexport))
#else
#define FDSA_API __declspec(dllexport)
#endif // __MINGW32__
#else
#define FDSA_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @struct fDSA
 * This is main object for fDSA.
 * All APIs are included in it.
 */
typedef struct fDSA
{

    fdsa_ptrMap *ptrMap;

    fdsa_ptrVector *ptrVector;

    fdsa_vector *vector;

    /**
     * Get object's type id.
     * @param handle the object pointer return from fdsa APIs
     * @param dst the pointer for store result
     * @return please refer to fdsa_exitstate
     */
    fdsa_exitstate (*getHandleType)(fdsa_handle handle, fdsa_types *dst);

    /**
     * free the fdsa_handle
     * @param handle the object pointer return from fdsa APIs
     * @return please refer to fdsa_exitstate
     */
    fdsa_exitstate (*closeHandle)(fdsa_handle handle);

    /**
     * @return it returns fdsa API's version.
     */
    const char *(*version)();
} fDSA;

/**
 * Initialize fdsa APIs. If fail, it will return NULL.
 */
FDSA_API fDSA *fdsa_init();

/**
 * Destroy fdsa APIs' entries.
 */
FDSA_API void fdsa_destroy(fDSA *);

#ifdef __cplusplus
}
#endif
