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
        printf("======================================================\n"); \
        printf("== Result:  %3d Total   %3d Passed      %3d Failed  ==\n", \
                pass_count  + fail_count, pass_count, fail_count); \
        printf("======================================================\n"); \
    } while (0)

static int pass_count;
static int fail_count;

int main(void)
{
    test_section("str")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=\"value\"}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"value"));
    }

    test_section("num")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=1234}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"1234"));
    }

    test_section("utf8")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=\"$¢€𤪤\"}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"$¢€𤪤"));
    }

    test_section("map")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=\"test\", \"age\"=42, \"utf8\"=\"äöü\", \"alive\"=true}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"name"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"test"));

        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"age"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"42"));

        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"utf8"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"äöü"));

        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"alive"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"true"));
    }

    test_section("array")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"list\"=[1,2,3,4]}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"list"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"[1,2,3,4]"));

        int i = 0;
        iter = json_begin(pair[1].str, pair[1].len);
        const json_char check[] = "1234";
        do {
            struct json_token tok;
            iter = json_read(&iter, &tok);
            test_assert(tok.str[0] == check[i++]);
        } while (!iter.err);
    }

    test_section("sub")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\"={\"a\"=\"b\"}}";
        iter = json_begin(buf, sizeof buf);

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

    test_section("subarray")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\"={\"a\"=[1,2,3,4]}}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"sub"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"{\"a\"=[1,2,3,4]}"));

        iter = json_begin(pair[1].str, pair[1].len);
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"a"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"[1,2,3,4]"));

        int i = 0;
        iter = json_begin(pair[1].str, pair[1].len);
        const json_char check[] = "1234";
        do {
            struct json_token tok;
            iter = json_read(&iter, &tok);
            test_assert(tok.str[0] == check[i++]);
        } while (!iter.err);
    }

    test_section("list")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\"={\"a\"=\"b\"}, \"list\"={\"c\"=\"d\"}}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"sub"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"{\"a\"=\"b\"}"));

        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"list"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"{\"c\"=\"d\"}"));
    }

    test_section("table")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\"={\"a\"= \"b\"}, \"list\"=[1,2,3,4], \"a\"=true}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"sub"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"{\"a\"= \"b\"}"));

        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"list"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"[1,2,3,4]"));

        iter = json_parse(&iter, pair);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (json_char*)"a"));
        test_assert(!json_cmp(&pair[JSON_VALUE], (json_char*)"true"));
    }

    test_result();
    exit(EXIT_SUCCESS);
}

