/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "json.h"

int json_cmp(struct json_token* tok, const json_char* str)
{
    if (!tok || !str)
        return 1;

    json_size i;
    for (i = 0; (i < tok->len && str); i++, str++){
        if (tok->str[ i] != *str)
            return 1;
    }
    return 0;
}

static void test_str(void)
{
    const json_char *buf = (json_char*)"{\"name\"=\"value\"}";
    json_size size = strlen((const char*)buf);

    struct json_iter iter;
    iter = json_begin((json_char*)buf, size);

    json_pair pair;
    iter = json_parse(&iter, pair);
    assert(!iter.err);
    assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
    assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"value"));
}

static void test_num(void)
{
    const json_char *buf = (const json_char*)"{\"name\"=1234}";
    const json_size size = strlen((const char*)buf);

    struct json_iter iter;
    iter = json_begin((json_char*)buf, size);

    json_pair pair;
    iter = json_parse(&iter, pair);
    assert(!iter.err);
    assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
    assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"1234"));
}

static void test_utf8(void)
{
    const json_char *buf = (const json_char*)"{\"name\"=\"$¢€𤪤\"}";
    const json_size size = strlen((const char*)buf);

    struct json_iter iter;
    iter = json_begin((json_char*)buf, size);

    json_pair pair;
    iter = json_parse(&iter, pair);
    assert(!iter.err);
    assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
    assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"$¢€𤪤"));
}

int main(void)
{
    test_str();
    test_num();
    test_utf8();
}

