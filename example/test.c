/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/json.h"

int json_cmp(struct json_token* tok, const json_char* str)
{
    if (!tok || !str)
        return 1;

    json_size i;
    for (i = 0; (i < tok->len && *str); i++, str++){
        if (tok->str[i] != *str)
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

static void test_array(void)
{
    const json_char *buf = (const json_char*)"{\"list\"=[1,2,3,4]}";
    json_size size = strlen((const char*)buf);

    struct json_iter iter;
    iter = json_begin((json_char*)buf, size);

    json_pair pair;
    iter = json_parse(&iter, pair);
    assert(!iter.err);
    assert(!json_cmp(&pair[JSON_NAME], (json_char*)"list"));
    assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"[1,2,3,4]"));

    struct json_token tok;
    iter = json_begin(pair[1].str, pair[1].len);

    int i = 0;
    const json_char* check = (const json_char*)"1234";
    do {
        iter = json_read(&iter, &tok);
        assert(tok.str[0] == check[i++]);
    } while (!iter.err);
}

static void test_sub(void)
{
    const json_char *buf = (const json_char*)"{\"sub\"={\"a\"=\"b\"}}";
    json_size size = strlen((const char*)buf);

    struct json_iter iter;
    iter = json_begin((json_char*)buf, size);

    json_pair pair;
    iter = json_parse(&iter, pair);
    assert(!iter.err);
    assert(!json_cmp(&pair[JSON_NAME], (json_char*)"sub"));
    assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"{\"a\"=\"b\"}"));

    iter = json_begin(pair[1].str, pair[1].len);
    iter = json_parse(&iter, pair);
    assert(!iter.err);
    assert(!json_cmp(&pair[JSON_NAME], (json_char*)"a"));
    assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"b"));
}

int main(void)
{
    test_str();
    test_num();
    test_utf8();
    test_array();
    test_sub();
    exit(EXIT_SUCCESS);
}

