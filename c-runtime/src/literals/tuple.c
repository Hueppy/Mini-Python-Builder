#include "literals/tuple.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "function-args.h"
#include "literals/boolean.h"
#include "literals/list.h"
#include "literals/str.h"
#include "mpy_obj.h"
#include "errors.h"
#include "checks.h"
#include "builtins-setup.h"
#include "type-hierarchy/bound-method.h"


typedef struct __MPyTupleContent {
    unsigned int size;
    __MPyObj **values;
    __MPyObj *boolMethod;
    __MPyObj *strMethod;
} __MPyTupleContent;

void __mpy_obj_cleanup_tuple(__MPyObj *self) {
    __MPyTupleContent *content = (__MPyTupleContent*) self->content;

    __mpy_tuple_foreach(self, __mpy_obj_ref_dec);

    free(content->values);
    content->values = NULL;
}

__MPyObj* __mpy_tuple_get_attr_impl(__MPyObj *self, const char *name) {
    __MPyTupleContent *content = (__MPyTupleContent*) self->content;

    if (strcmp("__str__", name) == 0) {
        return content->strMethod;
    }

    return NULL;
}

__MPyObj* __mpy_obj_init_tuple(unsigned int size) {
    __MPyObj *obj = __mpy_obj_new(__MPyType_Object);
    obj->type = __MPyType_Tuple;
    obj->cleanupAction = __mpy_obj_cleanup_tuple;
    obj->attrAccessor = __mpy_tuple_get_attr_impl;

    obj->content = __mpy_checked_malloc(sizeof(__MPyTupleContent));
    __MPyTupleContent *content = (__MPyTupleContent*) obj->content;
    content->size = size;
    content->values = __mpy_checked_malloc(sizeof(__MPyObj*) * size);

    content->boolMethod = __mpy_bind_func(__MPyFunc_Tuple_bool, obj);
    content->strMethod = __mpy_bind_func(__MPyFunc_Tuple_str, obj);

    return __mpy_obj_return(obj);
}

__MPyObj* __mpy_tuple_assign(unsigned int pos, __MPyObj *value, __MPyObj *self) {
    if (self->type != __MPyType_Tuple) {
        // FIXME replace "unknown" with either wrapper for getting type as char*
        fprintf(stderr, "ERROR: __mpy_tuple_assign(%i, %i, %i): called on non-tuple object of type '%s'\n", pos, value->id, self->id, "unknown");
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyTupleContent *content = (__MPyTupleContent*) self->content;

    if (pos >= content->size) {
        fprintf(stderr, "ERROR: __mpy_tuple_assign(%i, %i, %i): called with pos > size (size = '%i')\n", pos, value->id, self->id, content->size);
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyObj **slot = (content->values) + pos;
    *slot = value;
    __mpy_obj_ref_inc(value);

    return self;
}

void __mpy_tuple_foreach(__MPyObj *self, void (*action) (__MPyObj* obj)) {
    if (self->type != __MPyType_Tuple) {
        // FIXME see type error above
        fprintf(stderr, "ERROR: __mpy_tuple_foreach(%i): called on non-tuple object of type '%s'\n", self->id, "unknown");
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyTupleContent *content = (__MPyTupleContent*) self->content;

    __MPyObj **slot = content->values;
    for (unsigned int i = 0; i < content->size; i++) {
        action(*(slot++));
    }
}

unsigned int __mpy_tuple_size(__MPyObj *self) {
    if (self->type != __MPyType_Tuple) {
        // FIXME see above
        fprintf(stderr, "ERROR: __mpy_tuple_size(%i): called with non-tuple object of type '%s'\n", self->id, "unknown");
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyTupleContent *content = (__MPyTupleContent*) self->content;
    return content->size;
}

__MPyObj* __mpy_tuple_get_at(__MPyObj *self, unsigned int pos) {
    if (self->type != __MPyType_Tuple) {
        // FIXME see above
        fprintf(stderr, "ERROR: __mpy_tuple_get_at(%i, %i): called with non-tuple object of type '%s'\n", self->id, pos, "unknown");
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyTupleContent *content = (__MPyTupleContent*) self->content;
    if (pos >= content->size) {
        fprintf(stderr, "IndexError: tuple index out of range\n");
        __mpy_fatal_error(__MPY_ERROR_USER);
    }

    // Note: no __mpy_obj_return call here since there's no need to decrease the refCount (we still have it in the map)
    return *((content->values) + pos);
}

__MPyObj *__mpy_tuple_func_bool_impl(__MPyObj *args, __MPyObj *kwargs) {
    assert(args != NULL && kwargs != NULL);

    __MPyGetArgsState argHelper = __mpy_args_init("tuple.__bool__", args, kwargs, 1);
    __MPyObj *self = __mpy_args_get_positional(&argHelper, 0, "self");
    __mpy_args_finish(&argHelper);

    __mpy_boolean_c_type truth = __mpy_tuple_size(self);

    __mpy_obj_ref_dec(self);
    return __mpy_obj_return(__mpy_obj_init_boolean(truth));
}

__MPyObj* __mpy_tuple_func_str_impl(__MPyObj *args, __MPyObj *kwargs) {
    assert(args != NULL && kwargs != NULL);

    __MPyGetArgsState argHelper = __mpy_args_init("tuple.__str__", args, kwargs, 1);
    __MPyObj *self = __mpy_args_get_positional(&argHelper, 0, "self");
    __mpy_args_finish(&argHelper);

    __MPyTupleContent *content = (__MPyTupleContent*)self->content;

    __MPyObj *strs = __mpy_obj_init_list();
    unsigned int len = 3; // 2 for enclosing [] + 1 for null terminator
    for (unsigned int i = 0; i < content->size; i++) {
        if (i > 0) {
            // account for leading ','
            len++;
        }

        __MPyObj *obj = __mpy_tuple_get_at(self, i);
        __mpy_obj_ref_inc(obj);

        __MPyObj *str = __mpy_call(__mpy_obj_get_attr(obj, "__str__"), __mpy_obj_init_tuple(0), NULL);
        __mpy_obj_ref_inc(str);

        __mpy_list_add(str, strs);

        len += strlen(__mpy_str_as_c_str(str));

        __mpy_obj_ref_dec(str);
        __mpy_obj_ref_dec(obj);
    }

    char *s = __mpy_checked_malloc(len);
    memset(s, 0, len);

    strcat(s, "(");
    for (unsigned int i = 0; i < content->size; i++) {
        if (i > 0) {
            strcat(s, ",");
        }

        __MPyObj *str = __mpy_list_get_at(strs, i);
        __mpy_obj_ref_inc(str);

        strcat(s, __mpy_str_as_c_str(str));

        __mpy_obj_ref_dec(str);
    }
    strcat(s, ")");

    __mpy_obj_ref_dec(strs);

    return __mpy_obj_return(__mpy_obj_init_str_dynamic(s));
}
