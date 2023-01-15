
/**
 * @file Aliases meant for inclusion in the generated program.
 *
 * Allows to refer to functions by namespaced identifiers in the c code
 * implementing the runtime (e. g. __MPyFunc_print)
 * while refering to them as simply print in c code generated by the builder.
 */

#define print __MPyFunc_print

#define type __MPyFunc_type

#define id __MPyFunc_id

#define input __MPyFunc_input

#define range __MPyFunc_range
