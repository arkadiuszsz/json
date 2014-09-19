/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#ifndef JSON_H_
#define JSON_H_

#include <stdlib.h>
#include <string.h>

#define JSON_NAME 0
#define JSON_VALUE 1

struct json_token;
typedef unsigned char json_char;
typedef unsigned int json_size;

typedef struct json_token {
    const json_char *str;
    json_size len;
} json_pair[2];

struct json_iter {
    int depth;
    int err;
    const void **go;
    const json_char *src;
    json_size len;
};

struct json_iter json_begin(const json_char*, json_size);
struct json_iter json_read(const struct json_iter*, struct json_token*);
struct json_iter json_parse(const struct json_iter*, json_pair);
json_char *json_dup(struct json_token*, void*(*alloc)(size_t));
int json_cpy(json_char *dst, json_size max, struct json_token*);

#endif
