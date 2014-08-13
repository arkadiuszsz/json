/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#ifndef JSON_H_
#define JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

typedef unsigned char json_char;
typedef unsigned int json_size;

struct json_token {
    const json_char *str;
    json_size len;
};

#define JSON_NAME 0
#define JSON_VALUE 1

typedef struct json_token json_pair[2];

struct json_iter {
    int depth;
    int err;
    const void **go;
    const json_char *src;
    json_size len;
};

/* parser */
struct json_iter json_begin(const json_char*, json_size);
struct json_iter json_read(const struct json_iter*, struct json_token*);
struct json_iter json_parse(const struct json_iter*, json_pair);
#define json_err(it) ((iter)->err)

/* type */
#define json_isobj(t) ((s)->str[0] == '{')
#define json_islist(t) ((s)->str[0] == '[')

/* util */
json_char *json_dup(struct json_token*, void*(*alloc)(size_t));
int json_cpy(json_char *dst, json_size max, struct json_token*);

#ifdef __cplusplus
}
#endif

#endif
