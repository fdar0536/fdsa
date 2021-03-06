/*
*    This file is part of fdsa,
*    Copyright(C) 2019-2020 fdar0536.
*
*    fdsa is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as
*    published by the Free Software Foundation, either version 2.1
*    of the License, or (at your option) any later version.
*
*    fdsa is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General
*    Public License along with fdsa. If not, see
*    <http://www.gnu.org/licenses/>.
*/

#include <mutex>
#include <new>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ptrvector.h"

typedef struct fdsa_ptrVector
{
    uint8_t **data = NULL;

    fdsa_freeFunc freeFunc = NULL;

    size_t size = NULL;

    size_t capacity = 0;

    std::mutex mutex;
} fdsa_ptrVector;

extern "C"
{

fdsa_exitstate fdsa_ptrVector_init(fdsa_ptrVector_api *ret)
{
    if (!ret) return fdsa_failed;

    ret->create = fdsa_ptrVector_create;
    ret->destory = fdsa_ptrVector_destroy;
    ret->at = fdsa_ptrVector_at;
    ret->setValue = fdsa_ptrVector_setValue;
    ret->clear = fdsa_ptrVector_clear;
    ret->size = fdsa_ptrVector_size;
    ret->capacity = fdsa_ptrVector_capacity;
    ret->reserve = fdsa_ptrVector_reserve;
    ret->pushBack = fdsa_ptrVector_pushBack;
    ret->resize = fdsa_ptrVector_resize;

    return fdsa_success;
}

FDSA_API fdsa_ptrVector *fdsa_ptrVector_create(fdsa_freeFunc freeFunc)
{
    fdsa_ptrVector *ret = new (std::nothrow) fdsa_ptrVector;
    if (!ret)
    {
        return NULL;
    }

    ret->freeFunc = freeFunc;

    return ret;
}

FDSA_API fdsa_exitstate fdsa_ptrVector_destroy(fdsa_ptrVector *vec)
{
    if (!vec)
    {
        return fdsa_failed;
    }

    vec->mutex.lock();
    if (vec->data)
    {
        size_t i = 0;
        for (i = 0; i < vec->size; ++i)
        {
            if (vec->freeFunc) vec->freeFunc(vec->data[i]);
        }

        delete[] vec->data;
        vec->data = NULL;
    }

    vec->mutex.unlock();
    delete vec;
    return fdsa_success;
}

FDSA_API void *fdsa_ptrVector_at(fdsa_ptrVector *vec, size_t index)
{
    if (!vec) return NULL;

    std::lock_guard<std::mutex> lock(vec->mutex);
    if (index >= vec->size)
    {
        return NULL;
    }

    uint8_t **data = vec->data;
    data += index;

    return data[0];
}

FDSA_API fdsa_exitstate fdsa_ptrVector_setValue(fdsa_ptrVector *vec,
                                                size_t index,
                                                void *src)
{
    if (!vec)
    {
        return fdsa_failed;
    }

    std::lock_guard<std::mutex> lock(vec->mutex);
    if (index >= vec->size)
    {
        return fdsa_failed;
    }

    vec->freeFunc(vec->data[index]);
    vec->data[index] = reinterpret_cast<uint8_t *>(src);

    return fdsa_success;
}

FDSA_API fdsa_exitstate fdsa_ptrVector_clear(fdsa_ptrVector *vec)
{
    if (!vec)
    {
        return fdsa_failed;
    }

    std::lock_guard<std::mutex> lock(vec->mutex);
    size_t i = 0;
    for (i = 0; i < vec->size; ++i)
    {
        vec->freeFunc(vec->data[i]);
        vec->data[i] = NULL;
    }

    vec->size = 0;
    return fdsa_success;
}

FDSA_API fdsa_exitstate fdsa_ptrVector_size(fdsa_ptrVector *vec, size_t *dst)
{
    if (!vec || !dst)
    {
        return fdsa_failed;
    }

    std::lock_guard<std::mutex> lock(vec->mutex);
    *dst = vec->size;

    return fdsa_success;
}

FDSA_API fdsa_exitstate fdsa_ptrVector_capacity(fdsa_ptrVector *vec, size_t *dst)
{
    if (!vec || !dst)
    {
        return fdsa_failed;
    }

    std::lock_guard<std::mutex> lock(vec->mutex);
    *dst = vec->capacity;

    return fdsa_success;
}

FDSA_API fdsa_exitstate fdsa_ptrVector_reserve(fdsa_ptrVector *vec, size_t newSize)
{
    if (!vec)
    {
        return fdsa_failed;
    }

    std::lock_guard<std::mutex> lock(vec->mutex);
    if (newSize <= vec->capacity)
    {
        // do nothing
        return fdsa_success;
    }

    uint8_t **newData = new (std::nothrow) uint8_t*[newSize]();
    if (!newData)
    {
        return fdsa_failed;
    }

    if (vec->data)
    {
        size_t i;
        for (i = 0; i < vec->size; ++i)
        {
            newData[i] = vec->data[i];
        }

        delete[] vec->data;
    }

    vec->data = newData;
    vec->capacity = newSize;

    return fdsa_success;
}

FDSA_API fdsa_exitstate fdsa_ptrVector_pushBack(fdsa_ptrVector *vec, void *src)
{
    if (!vec)
    {
        return fdsa_failed;
    }

    std::lock_guard<std::mutex> lock(vec->mutex);
    if (vec->size == vec->capacity)
    {
        if (fdsa_ptrVector_reserve(vec, vec->capacity + 1) == fdsa_failed)
        {
            return fdsa_failed;
        }
    }

    uint8_t **data = vec->data;

    // array just contain pointer
    data += vec->size;
    data[0] = reinterpret_cast<uint8_t *>(src);
    ++vec->size;

    return fdsa_success;
}

FDSA_API fdsa_exitstate fdsa_ptrVector_resize(fdsa_ptrVector *vec,
                                              size_t amount,
                                              void *src,
                                              void *(*deepCopyFunc)(void *))
{
    if (!vec)
    {
        return fdsa_failed;
    }

    if (vec->capacity < amount)
    {
        if (fdsa_ptrVector_reserve(vec, amount) == fdsa_failed)
        {
            return fdsa_failed;
        }
    }

    size_t i;
    if (vec->size > amount)
    {
        std::lock_guard<std::mutex> lock(vec->mutex);
        for (i = amount; i < vec->size; ++i)
        {
            vec->freeFunc(vec->data[i]);
            vec->data[i] = NULL;
        }

        vec->size = amount;
    }
    else if (vec->size < amount)
    {
        // vec->capacity >= amount

        void *toBeInsert = NULL;
        for (i = vec->size; i < amount; ++i)
        {
            toBeInsert = deepCopyFunc(src);
            if (!toBeInsert)
            {
                return fdsa_failed;
            }

            if (fdsa_ptrVector_pushBack(vec, toBeInsert) == fdsa_failed)
            {
                return fdsa_failed;
            }
        }
    }

    return fdsa_success;
}

} // end extern "C"
