#ifndef _
#include <libintl.h>
#include <locale.h>

#ifdef __cplusplus
#define _(string) gettext(string)
#else
// For C just map this to an empty macro to allow translation of static structs
#define _(string) string
#endif

#endif

