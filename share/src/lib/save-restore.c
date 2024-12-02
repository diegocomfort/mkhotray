#include "../include/::IDENT::.h"

extern struct ::IDENT:: *::IDENT::;

struct ::IDENT:: *::LIB::_save(void)
{
	return ::IDENT::;
}

void ::LIB::_restore(struct ::IDENT:: *state)
{
	::IDENT:: = state;

	TraceLog(LOG_INFO, "Reloaded ::NAME::");
}
