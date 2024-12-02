#include "../include/::IDENT::.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef ::IDENT_UPPER::_DEBUG
#define ::IDENT_UPPER::_LOG_LEVEL LOG_ALL
#else
#define ::IDENT_UPPER::_LOG_LEVEL LOG_WARNING
#endif

struct ::IDENT:: *::IDENT::;

int ::LIB::_init(void)
{
	::IDENT:: = calloc(1, sizeof(*::IDENT::));
	if (::IDENT:: == NULL)
	{
		fprintf(stderr, "Kill some Chrome tabs\n");
		return 1;
	}

	int exit_status;

	SetTraceLogLevel(::IDENT_UPPER::_LOG_LEVEL);
	SetConfigFlags(FLAG_WINDOW_UNDECORATED);
	SetTargetFPS(30); // Keeps cpu usage to a miminum
	int monitor = GetCurrentMonitor();
	int width = GetMonitorWidth(monitor);
	int height = GetMonitorHeight(monitor);
	InitWindow(width, height, "::NAME::");
	if (!IsWindowReady())
	{
		TraceLog(LOG_ERROR, "Failed to initialize a window");
		exit_status = 2;
		goto exit_raylib_window;
	}
	SetExitKey(KEY_NULL);

	return 0;

exit_raylib_window:
	CloseWindow();

	return exit_status;
}
