/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#include "json.h"

/* remove gcc warning for static init*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winitializer-overrides"

struct json_iter json_begin(const json_char* str, json_size len)
{
    struct json_iter iter;
    memset(&iter, 0, sizeof iter);
    iter.src = str;
    iter.len = len;
    return iter;
}

struct json_iter json_read(const struct json_iter* prev,
                            struct json_token *obj)
{
    static const void *go_struct[] = {
        [0 ... 255] = &&l_fail,
        ['\t']      = &&l_loop,
        ['\r']      = &&l_loop,
        ['\n']      = &&l_loop,
        ['"']       = &&l_qup,
        [':']       = &&l_loop,
        ['=']       = &&l_loop,
        [',']       = &&l_loop,
        ['[']       = &&l_up,
        [']']       = &&l_down,
        ['{']       = &&l_up,
        ['}']       = &&l_down,
        ['-']       = &&l_bare,
        [49 ... 57] = &&l_bare,
        ['t']       = &&l_bare,
        ['f']       = &&l_bare,
        ['n']       = &&l_bare,
    };
    static const void *go_bare[] = {
        [0 ... 255] = &&l_fail,
        [32 ... 126]= &&l_loop,
        ['\t']      = &&l_unbare,
        ['\r']      = &&l_unbare,
        ['\n']      = &&l_unbare,
        [',']       = &&l_unbare,
        [']']       = &&l_unbare,
        ['}']       = &&l_unbare,
    };
    static const void *go_string[] = {
        [0 ... 31]    = &&l_fail,
        [32 ... 126]  = &&l_loop,
        [127]         = &&l_fail,
        ['\\']        = &&l_esc,
        ['"']         = &&l_qdown,
        [128 ... 191] = &&l_fail,
        [192 ... 223] = &&l_utf8_2,
        [224 ... 239] = &&l_utf8_3,
        [240 ... 247] = &&l_utf8_4,
        [248 ... 255] = &&l_fail,
    };
    static const void *go_utf8[] = {
        [0 ... 127]   = &&l_fail,
        [128 ... 191] = &&l_utf_next,
        [192 ... 255] = &&l_fail
    };
    static const void *go_esc[] = {
        [0 ... 255] = &&l_fail,
        ['"']       = &&l_unesc,
        ['\\']      = &&l_unesc,
        ['/']       = &&l_unesc,
        ['b']       = &&l_unesc,
        ['f']       = &&l_unesc,
        ['n']       = &&l_unesc,
        ['r']       = &&l_unesc,
        ['t']       = &&l_unesc,
        ['u']       = &&l_unesc
    };

    if (!prev || !obj || !prev->src || !prev->len || prev->err) {
        struct json_iter it;
        memset(&it, 0, sizeof it);
        memset(&obj, 0, sizeof(*obj));
        it.err = 1;
        return it;
    }

    struct json_iter iter;
    memset(obj, 0, sizeof(*obj));
    memcpy(&iter, prev, sizeof iter);
    iter.err = 0;
    if (!iter.go)
        iter.go = go_struct;

    int len = iter.len;
    const json_char *cur;
    int utf8_remain = 0;
    for (cur = iter.src; len; cur++, len--) {
        goto *iter.go[*cur];
        l_loop:;
    }

    if (!iter.depth) {
        iter.src = NULL;
        iter.len = 0;
        if (obj->str)
            obj->len = cur - obj->str;
        return iter;
    }

l_fail:
    iter.err = 1;
    return iter;

l_up:
    if (iter.depth++ == 1)
        obj->str = cur + 1;
    goto l_loop;

l_down:
    if (iter.depth-- == 1) {
        obj->len = cur - obj->str;
        goto l_yield;
    }
    goto l_loop;

l_qup:
    iter.go = go_string;
    if (iter.depth == 1)
        obj->str = cur + 1;
    goto l_loop;

l_qdown:
    iter.go = go_struct;
    if (iter.depth == 1) {
        obj->len = cur - obj->str;
        goto l_yield;
    }
    goto l_loop;

l_esc:
    iter.go = go_esc;
    goto l_loop;

l_unesc:
    iter.go = go_string;
    goto l_loop;

l_bare:
    if (iter.depth == 1)
        obj->str = cur;
    iter.go = go_bare;
    goto l_loop;

l_unbare:
    if (iter.depth == 1) {
        obj->len = cur - obj->str;
        goto l_yield;
    }
    iter.go = go_struct;
    goto *iter.go[*cur];

l_utf8_2:
    iter.go = go_utf8;
    utf8_remain = 1;
    goto l_loop;

l_utf8_3:
    iter.go = go_utf8;
    utf8_remain = 2;
    goto l_loop;

l_utf8_4:
    iter.go = go_utf8;
    utf8_remain = 3;
    goto l_loop;

l_utf_next:
    if (!--utf8_remain)
        iter.go = go_string;
    goto l_loop;

l_yield:
    if (iter.depth != 1 || !obj->str)
        goto l_loop;
    iter.src = cur + 1;
    iter.len = len - 1;
    return iter;
}

struct json_iter json_parse(const struct json_iter* it, json_pair p)
{
    struct json_iter next;
    next = json_read(it, &p[JSON_NAME]);
    if (next.err)
        return next;
    return json_read(&next, &p[JSON_VALUE]);
}

json_char *json_dup(struct json_token* tok, void*(*alloc)(size_t))
{
    if (!alloc)
        return NULL;

    json_char *str = alloc(tok->len + 1);
    if (!str)
        return NULL;

    memcpy(str, tok->str, tok->len);
    str[tok->len] = '\0';
    return str;
}

int json_cpy(json_char *dst, json_size max, struct json_token* tok)
{
    if (!dst || !max || !tok)
        return -1;

    int result;
    json_size *siz;
    if (max <= (tok->len + 1)) {
        result = max;
        max--;
        siz = &max;
    } else {
        result = tok->len;
        siz = &tok->len;
    }

    memcpy(dst, tok->str, *siz);
    dst[*siz] = '\0';
    return result;
}

