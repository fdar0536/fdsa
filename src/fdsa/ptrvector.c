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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdsa/types.h"
#include "ptrvector.h"

typedef struct vector
{
    fdsa_types id;

    uint8_t **data;

    fdsa_freeFunc freeFunc;

    size_t size;

    size_t capacity;
} vector;

fdsa_ptrVector *fdsa_ptrVector_init()
{
    fdsa_ptrVector *ret = calloc(1, sizeof(fdsa_ptrVector));
    if (!ret)
    {
        return NULL;
    }

    ret->create = fdsa_ptrVector_create;
    ret->at = fdsa_ptrVector_at;
    ret->setValue = fdsa_ptrVector_setValue;
    ret->clear = fdsa_ptrVector_clear;
    ret->size = fdsa_ptrVector_size;
    ret->capacity = fdsa_ptrVector_capacity;
    ret->reserve = fdsa_ptrVector_reserve;
    ret->pushback = fdsa_ptrVector_pushback;
    ret->resize = fdsa_ptrVector_resize;

    return ret;
}

fdsa_handle fdsa_ptrVector_create(fdsa_freeFunc freeFunc)
{
    if (!freeFunc)
    {
        return NULL;
    }

    vector *ret = calloc(1, sizeof(vector));
    if (!ret)
    {
        return NULL;
    }

    ret->id = fdsa_types_ptrVector;
    ret->data = NULL;
    ret->freeFunc = freeFunc;
    ret->size = 0;
    ret->capacity = 0;

    return ret;
}

fdsa_exitstate fdsa_ptrVector_destroy(fdsa_handle in)
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return fdsa_failed;
    }

    vector *vec = (vector *)in;
    if (vec->data)
    {
        size_t i = 0;
        for (i = 0; i < vec->size; ++i)
        {
            vec->freeFunc(vec->data[i]);
        }

        free(vec->data);
    }

    free(in);
    return fdsa_success;
}

void *fdsa_ptrVector_at(fdsa_handle in, size_t index)
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return NULL;
    }

    vector *vec = (vector *)in;
    if (index >= vec->size)
    {
        return NULL;
    }

    uint8_t **data = vec->data;
    data += index;

    return data[0];
}

fdsa_exitstate fdsa_ptrVector_setValue(fdsa_handle in,
                                       size_t index,
                                       void *src)
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return fdsa_failed;
    }

    if (!src)
    {
        return fdsa_failed;
    }

    vector *vec = (vector *)in;
    if (index >= vec->size)
    {
        return fdsa_failed;
    }

    vec->freeFunc(vec->data[index]);
    vec->data[index] = src;

    return fdsa_success;
}

fdsa_exitstate fdsa_ptrVector_clear(fdsa_handle in)
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return fdsa_failed;
    }

    vector *vec = (vector *)in;

    size_t i = 0;
    for (i = 0; i < vec->size; ++i)
    {
        vec->freeFunc(vec->data[i]);
        vec->data[i] = NULL;
    }

    vec->size = 0;
    return fdsa_success;
}

fdsa_exitstate fdsa_ptrVector_size(fdsa_handle in, size_t *dst)
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return fdsa_failed;
    }

    if (!dst)
    {
        return fdsa_failed;
    }

    vector *vec = (vector *)in;
    *dst = vec->size;

    return fdsa_success;
}

fdsa_exitstate fdsa_ptrVector_capacity(fdsa_handle in, size_t *dst)
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return fdsa_failed;
    }

    if (!dst)
    {
        return fdsa_failed;
    }

    vector *vec = (vector *)in;
    *dst = vec->capacity;

    return fdsa_success;
}

fdsa_exitstate fdsa_ptrVector_reserve(fdsa_handle in, size_t newSize)
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return fdsa_failed;
    }

    vector *vec = (vector *)in;
    if (newSize <= vec->capacity)
    {
        // do nothing
        return fdsa_success;
    }

    uint8_t **newData = calloc(newSize, sizeof(uint8_t *));
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

        free(vec->data);
    }

    vec->data = newData;
    vec->capacity = newSize;

    return fdsa_success;
}

fdsa_exitstate fdsa_ptrVector_pushback(fdsa_handle in, void *src)
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return fdsa_failed;
    }

    if (!src)
    {
        return fdsa_failed;
    }

    vector *vec = (vector *)in;
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
    data[0] = src;
    ++vec->size;

    return fdsa_success;
}

fdsa_exitstate fdsa_ptrVector_resize(fdsa_handle in,
                                     size_t amount,
                                     void *src,
                                     void *(*deepCopyFunc)(void *))
{
    if (fdsa_checkInput(in, fdsa_types_ptrVector))
    {
        return fdsa_failed;
    }

    if (!src)
    {
        return fdsa_failed;
    }

    vector *vec = (vector *)in;
    if (vec->capacity < amount)
    {
        if (fdsa_ptrVector_reserve(in, amount) == fdsa_failed)
        {
            return fdsa_failed;
        }
    }

    size_t i;
    if (vec->size > amount)
    {
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

            if (fdsa_ptrVector_pushback(vec, toBeInsert) == fdsa_failed)
            {
                return fdsa_failed;
            }
        }
    }

    return fdsa_success;
}