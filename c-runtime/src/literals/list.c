#include "literals/list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "function-args.h"
#include "literals/boolean.h"
#include "mpy_obj.h"
#include "errors.h"
#include "checks.h"
#include "builtins-setup.h"
#include "type-hierarchy/bound-method.h"
#include "literals/str.h"
#include "literals/tuple.h"


typedef struct __MPyListContent {
    unsigned int capacity;
    unsigned int size;
    __MPyObj **values;
    __MPyObj *strMethod;
} __MPyListContent;

void __mpy_obj_cleanup_list(__MPyObj *self) {
    __MPyListContent *content = (__MPyListContent*) self->content;

    __mpy_list_foreach(self, __mpy_obj_ref_dec);

    free(content->values);
    content->values = NULL;
}

__MPyObj* __mpy_list_get_attr_impl(__MPyObj *self, const char *name) {
    __MPyListContent *content = (__MPyListContent*) self->content;

    if (strcmp("__str__", name) == 0) {
        return content->strMethod;
    }

    return NULL;
}

__MPyObj* __mpy_obj_init_list() {
    __MPyObj *obj = __mpy_obj_new();
    obj->type = __MPyType_List;
    obj->cleanupAction = __mpy_obj_cleanup_list;
    obj->attrAccessor = __mpy_list_get_attr_impl;

    obj->content = __mpy_checked_malloc(sizeof(__MPyListContent));
    __MPyListContent *content = (__MPyListContent*) obj->content;
    content->capacity = 1;
    content->size = 0;
    content->values = __mpy_checked_malloc(sizeof(__MPyObj*) * content->capacity);

    content->strMethod = __mpy_bind_func(__MPyFunc_List_str, obj);

    return __mpy_obj_return(obj);
}

void __mpy_list_resize(__MPyObj *self, unsigned int new_capacity) {
    __MPyListContent *content = (__MPyListContent*) self->content;

    content->capacity = new_capacity;
    content->values = __mpy_checked_realloc(content->values, sizeof(__MPyObj*) * content->capacity);
}

__MPyObj* __mpy_list_add(__MPyObj *value, __MPyObj *self) {
    if (self->type != __MPyType_List) {
        // FIXME replace "unknown" with either wrapper for getting type as char*
        fprintf(stderr, "ERROR: __mpy_list_add(%i, %i): called on non-list object of type '%s'\n", value->id, self->id, "unknown");
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyListContent *content = (__MPyListContent*) self->content;

    if (content->size >= content->capacity) {
        __mpy_list_resize(self, content->capacity * 2);
    }

    __MPyObj **slot = (content->values) + content->size;
    *slot = value;
    __mpy_obj_ref_inc(value);

    content->size++;

    return self;
}

void __mpy_list_foreach(__MPyObj *self, void (*action) (__MPyObj* obj)) {
    if (self->type != __MPyType_List) {
        // FIXME see type error above
        fprintf(stderr, "ERROR: __mpy_list_foreach(%i): called on non-list object of type '%s'\n", self->id, "unknown");
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyListContent *content = (__MPyListContent*) self->content;

    __MPyObj **slot = content->values;
    for (unsigned int i = 0; i < content->size; i++) {
        action(*(slot++));
    }
}

unsigned int __mpy_list_size(__MPyObj *self) {
    if (self->type != __MPyType_List) {
        // FIXME see above
        fprintf(stderr, "ERROR: __mpy_list_size(%i): called with non-list object of type '%s'\n", self->id, "unknown");
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyListContent *content = (__MPyListContent*) self->content;
    return content->size;
}

__MPyObj* __mpy_list_get_at(__MPyObj *self, unsigned int pos) {
    if (self->type != __MPyType_List) {
        // FIXME see above
        fprintf(stderr, "ERROR: __mpy_list_get_at(%i, %i): called with non-list object of type '%s'\n", self->id, pos, "unknown");
        __mpy_fatal_error(__MPY_ERROR_INTERNAL);
    }

    __MPyListContent *content = (__MPyListContent*) self->content;
    if (pos >= content->size) {
        fprintf(stderr, "IndexError: list index out of range\n");
        __mpy_fatal_error(__MPY_ERROR_USER);
    }

    // Note: no __mpy_obj_return call here since there's no need to decrease the refCount (we still have it in the map)
    return *((content->values) + pos);
}

__MPyObj* __mpy_list_func_str_impl(__MPyObj *args, __MPyObj *kwargs) {
    assert(args != NULL && kwargs != NULL);

    __MPyGetArgsState argHelper = __mpy_args_init("list.__str__", args, kwargs, 1);
    __MPyObj *self = __mpy_args_get_positional(&argHelper, 0, "self");
    __mpy_args_finish(&argHelper);

    __MPyListContent *content = (__MPyListContent*)self->content;

    __MPyObj *strs = __mpy_obj_init_list();
    unsigned int len = 3; // 2 for enclosing [] + 1 for null terminator
    for (unsigned int i = 0; i < content->size; i++) {
        if (i > 0) {
            // account for leading ','
            len++;
        }

        __MPyObj *obj = __mpy_list_get_at(self, i);
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

    strcat(s, "[");
    for (unsigned int i = 0; i < content->size; i++) {
        if (i > 0) {
            strcat(s, ",");
        }

        __MPyObj *str = __mpy_list_get_at(strs, i);
        __mpy_obj_ref_inc(str);

        strcat(s, __mpy_str_as_c_str(str));

        __mpy_obj_ref_dec(str);
    }
    strcat(s, "]");

    __mpy_obj_ref_dec(strs);

    return __mpy_obj_return(__mpy_obj_init_str_dynamic(s));
}
