#ifndef TYPE_HIERARCHY_LIST_H
#define TYPE_HIERARCHY_LIST_H

#include "mpy_obj.h"

__MPyObj* __mpy_obj_init_list();

__MPyObj* __mpy_list_add(__MPyObj *value, __MPyObj *self);

unsigned int __mpy_list_size(__MPyObj *self);

__MPyObj* __mpy_list_get_at(__MPyObj *self, unsigned int pos);

void __mpy_list_foreach(__MPyObj *self, void (*action) (__MPyObj* obj));

//__MPyObj *__mpy_tuple_func_bool_impl(__MPyObj *args, __MPyObj *kwargs);

__MPyObj* __mpy_list_func_str_impl(__MPyObj *args, __MPyObj *kwargs);
#endif

