#ifndef ::IDENT_UPPER::_H
#define ::IDENT_UPPER::_H

#include <raylib.h>

// State of the app
struct ::IDENT::
{
	char placeholder;
};

//                identifier       return type,   arguments type(s)
#define ::IDENT_UPPER::_LIST						\
	::IDENT_UPPER::_FUNCTION(::LIB::_init,    int,                void) \
	::IDENT_UPPER::_FUNCTION(::LIB::_save,    struct ::IDENT:: *, void) \
	::IDENT_UPPER::_FUNCTION(::LIB::_update,  int,                void) \
	::IDENT_UPPER::_FUNCTION(::LIB::_restore, void,               struct ::IDENT:: *) \
	::IDENT_UPPER::_FUNCTION(::LIB::_close,   void,          void)

#define ::IDENT_UPPER::_TYPEDEF(name, ret, ...) typedef ret (name##_t)(__VA_ARGS__);
#define ::IDENT_UPPER::_DECLARE_FUNCTION(name, ret, ...) ret name(__VA_ARGS__);
#define ::IDENT_UPPER::_DECLARE(name, ...) name##_t *name = NULL;
#define ::IDENT_UPPER::_RELOAD(name, ...)			\
	*(void**)&name = dlsym(::LIB::, #name);			\
	if (name == NULL)						\
	{								\
		TraceLog(LOG_ERROR, "Failed to get addres of "	\
			 #name ": %s", dlerror());			\
		return 1;						\
	}

#ifdef ::IDENT_UPPER::_HOT_RELOAD
// Declare all the function types
#define ::IDENT_UPPER::_FUNCTION(name, ret, ...)	\
	::IDENT_UPPER::_TYPEDEF(name, ret, __VA_ARGS__)
#else
// Declare the functions regularly
#define ::IDENT_UPPER::_FUNCTION(name, ret, ...)		\
	::IDENT_UPPER::_DECLARE_FUNCTION(name, ret, __VA_ARGS__)
#endif // ::IDENT_UPPER::_HOT_RELOAD

::IDENT_UPPER::_LIST
#undef  ::IDENT_UPPER::_FUNCTION

#endif // ::IDENT_UPPER::_H
