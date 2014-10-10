/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

#define test_section(desc) \
    do { \
        printf("--------------- {%s} ---------------\n", desc);\
    } while (0);

#define test_assert(cond) \
    do { \
        int pass = cond; \
        printf("[%s] %s:%d: ", pass ? "PASS" : "FAIL", __FILE__, __LINE__);\
        printf((strlen(#cond) > 50 ? "%.47s...\n" : "%s\n"), #cond);\
        if (pass) pass_count++; else fail_count++; \
    } while (0)

#define test_result()\
    do { \
        printf("------------------------------------------------------\n"); \
        printf("-- Result:  %3d Total   %3d Passed      %3d Failed  --\n", \
                pass_count  + fail_count, pass_count, fail_count); \
        printf("------------------------------------------------------\n"); \
    } while (0)

static int pass_count;
static int fail_count;

int main(void)
{
    test_section("str")
    {
        const json_char *buf = (json_char*)"{\"name\"=\"value\"}";
        json_size size = strlen((const char*)buf);

        struct json_iter iter;
        iter = json_begin((json_char*)buf, size);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"value"));
    }

    test_section("num")
    {
        const json_char *buf = (const json_char*)"{\"name\"=1234}";
        const json_size size = strlen((const char*)buf);

        struct json_iter iter;
        iter = json_begin((json_char*)buf, size);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"1234"));
    }

    test_section("utf8")
    {
        const json_char *buf = (const json_char*)"{\"name\"=\"$¢€𤪤\"}";
        const json_size size = strlen((const char*)buf);

        struct json_iter iter;
        iter = json_begin((json_char*)buf, size);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"$¢€𤪤"));
    }

    test_section("array")
    {
        const json_char *buf = (const json_char*)"{\"list\"=[1,2,3,4]}";
        json_size size = strlen((const char*)buf);

        struct json_iter iter;
        iter = json_begin((json_char*)buf, size);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"list"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"[1,2,3,4]"));

        struct json_token tok;
        iter = json_begin(pair[1].str, pair[1].len);

        int i = 0;
        const json_char* check = (const json_char*)"1234";
        do {
            iter = json_read(&iter, &tok);
            test_assert(tok.str[0] == check[i++]);
        } while (!iter.err);
    }

    test_section("sub")
    {
        const json_char *buf = (const json_char*)"{\"sub\"={\"a\"=\"b\"}}";
        json_size size = strlen((const char*)buf);

        struct json_iter iter;
        iter = json_begin((json_char*)buf, size);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"sub"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"{\"a\"=\"b\"}"));

        iter = json_begin(pair[1].str, pair[1].len);
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"a"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"b"));
    }
    test_result();
    exit(EXIT_SUCCESS);
}

