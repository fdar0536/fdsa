/*
 * This file is part of fDSA,
 * Copyright(C) 2020-2021 fdar0536.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fdsa.h"

typedef struct Testing
{
    int a;
    int b;
} Testing;

Testing *createTesting()
{
    Testing *ret = calloc(1, sizeof(Testing));
    if (!ret)
    {
        return NULL;
    }

    ret->a = 0;
    ret->b = 0;

    return ret;
}

void freeTesting(void *in)
{
    if (!in)
    {
        return;
    }

    Testing *testing = (Testing *)in;
    free(testing);
}

int cmpKey(const void *lhs, const void *rhs)
{
    if (!lhs && !rhs)
    {
        return 0;
    }
    else if (!lhs && rhs)
    {
        return -1;
    }
    else if (lhs && !rhs)
    {
        return 1;
    }

    return strcmp(lhs, rhs);
}

void dumpData(fdsa_ptrMap *mapApi, fdsa_handle map, void *key)
{
    Testing *res = mapApi->at(map, key);
    if (res)
    {
        printf("%d, %d\n", res->a, res->b);
    }
    else
    {
        puts("Data is not exist.");
    }
}

int main()
{
    fDSA *api = fdsa_init();
    if (!api)
    {
        fputs("Fail to create api entry.\n", stderr);
        return 1;
    }

    fdsa_ptrMap *mapApi = api->ptrMap;
    fdsa_handle map = mapApi->create(cmpKey, NULL, freeTesting);
    if (!map)
    {
        fputs("Fail to create map.\n", stderr);
        fdsa_destroy(api);
        return 1;
    }

    Testing *data = createTesting();
    if (!data)
    {
        fputs("Fail to allocate memory.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    }

    if (mapApi->insertNode(map, "123", data) == fdsa_failed)
    {
        fputs("Fail to insert node.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    };

    data = createTesting();
    if (!data)
    {
        fputs("Fail to allocate memory.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    }

    data->a = 1;
    data->b = 2;
    if (mapApi->insertNode(map, "456", data) == fdsa_failed)
    {
        fputs("Fail to insert node.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    };

    data = createTesting();
    if (!data)
    {
        fputs("Fail to allocate memory.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    }

    data->a = 2;
    data->b = 3;
    if (mapApi->insertNode(map, "789", data) == fdsa_failed)
    {
        fputs("Fail to insert node.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    };

    dumpData(mapApi, map, "123");
    dumpData(mapApi, map, "456");
    dumpData(mapApi, map, "789");
    dumpData(mapApi, map, "012");
    printf("\n");

    data = createTesting();
    if (!data)
    {
        fputs("Fail to allocate memory.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    }

    data->a = 5;
    data->b = 5;
    if (mapApi->insertNode(map, "123", data) == fdsa_failed)
    {
        fputs("Fail to replace node.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    };

    if (mapApi->deleteNode(map, "456") == fdsa_failed)
    {
        fputs("Fail to delete node.", stderr);
        if (api->closeHandle(map) == fdsa_failed)
        {
            fputs("Fail to close handle.\n", stderr);
        }

        fdsa_destroy(api);
        return 1;
    }

    dumpData(mapApi, map, "123");
    dumpData(mapApi, map, "456");
    dumpData(mapApi, map, "789");
    dumpData(mapApi, map, "012");
    printf("\n");

    if (api->closeHandle(map) == fdsa_failed)
    {
        fputs("Fail to close handle.\n", stderr);

        fdsa_destroy(api);
        return 1;
    }

    fdsa_destroy(api);

    return 0;
}