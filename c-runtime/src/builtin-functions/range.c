#include "builtin-functions/range.h"

#include <assert.h>
#include <stdio.h>

#include "errors.h"
#include "literals/int.h"
#include "literals/list.h"
#include "literals/tuple.h"
#include "mpy_obj.h"

__MPyObj* __mpy_func_range(__MPyObj *args, __MPyObj *kwargs) {
    assert(args != NULL && kwargs != NULL);

    // FIXME check for empty kwargs (or use them correctly)
    __MPY_NOTE("(bug) ignoring kwargs");
    if (kwargs != NULL) {
        __mpy_obj_ref_dec(kwargs);
    }

    unsigned int argNr = 0;
    if (args != NULL) {
        argNr = __mpy_tuple_size(args);
    }

    int start = 0;
    int stop;
    int step = 1;

    switch (argNr) {
        case 1:
            stop = __mpy_int_value(__mpy_tuple_get_at(args, 0));
            break;
        case 2:
            start = __mpy_int_value(__mpy_tuple_get_at(args, 0));
            stop = __mpy_int_value(__mpy_tuple_get_at(args, 1));
            break;
        case 3:
            start = __mpy_int_value(__mpy_tuple_get_at(args, 0));
            stop = __mpy_int_value(__mpy_tuple_get_at(args, 1));
            step = __mpy_int_value(__mpy_tuple_get_at(args, 2));
            break;
        default:
            fprintf(stderr, "Invalid number of arguments in range(): %d", argNr);
            __mpy_fatal_error(__MPY_ERROR_INTERNAL);
            break;
    }

    __MPyObj *list = __mpy_obj_init_list();
    for (int i = start; i < stop; i+=step) {
        __mpy_list_add(__mpy_obj_init_int(i), list);
    }

    __mpy_obj_ref_dec(args);
    // FIXME return None
    return __mpy_obj_return(list);
}

