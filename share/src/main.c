#include "include/::IDENT::.h"

#include <raylib.h>

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef ::IDENT_UPPER::_HOT_RELOAD
// Pointer to the shared object created by dlopen()
static void *::LIB:: = NULL;
// Declare function pointers for ::IDENT:: functions
#define ::IDENT_UPPER::_FUNCTION(name, ...) ::IDENT_UPPER::_DECLARE(name, __VA_ARGS__)
::IDENT_UPPER::_LIST
#undef  ::IDENT_UPPER::_FUNCTION
// Declare reload_::LIB::() as an actual function that will reload
// the shared object
int reload_::LIB::(void);
#else
// Declare reload_::LIB::() as a macro which always results in a
// success because ::IDENT:: is actually linked in the build process,
// so there is no shared object to reload
#define reload_::LIB::() 0
#endif // ::IDENT_UPPER::_HOT_RELOAD


int main()
{
	int error;

	error = reload_::LIB::();
	if (error)
	{
		return 1;
	}

	error = ::LIB::_init();
	if (error)
	{
		return 2;
	}

	while(!WindowShouldClose())
	{
#ifdef ::IDENT_UPPER::_HOT_RELOAD
		if (IsKeyPressed(KEY_R))
		{
			struct ::IDENT:: *state = ::LIB::_save();
			error = reload_::LIB::();
			if (error)
			{
				::LIB::_close();
				return 3;
			}
			::LIB::_restore(state);
		}
#endif // ::IDENT_UPPER::_HOT_RELOAD

		error = ::LIB::_update();
		if (error)
		{
			::LIB::_close();
			return 4;
		}

	}

		::LIB::_close();
	return 0;
}

#ifdef ::IDENT_UPPER::_HOT_RELOAD
int reload_::LIB::(void)
{
	int error;

	if (::LIB:: != NULL)
	{
		error = dlclose(::LIB::);
		if (error)
		{
			TraceLog(LOG_ERROR, "Failed to to close ::LIB::: %s\n", dlerror());
			return 1;
		}
	}

		::LIB:: = dlopen(::LIB_UPPER::_PATH, RTLD_NOW);
	if (::LIB:: == NULL)
	{
		TraceLog(LOG_ERROR, "Failed to open "::LIB_UPPER::_PATH": %s\n", dlerror());
		return 2;
	}

#define ::IDENT_UPPER::_FUNCTION(name, ...) ::IDENT_UPPER::_RELOAD(name, __VA_ARGS__)
		::IDENT_UPPER::_LIST
#undef  ::IDENT_UPPER::_FUNCTION

		return 0;
}
#endif // ::IDENT_UPPER::_HOT_RELOAD
