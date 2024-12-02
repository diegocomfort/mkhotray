#include "../include/::IDENT::.h"

#include <raylib.h>

extern struct ::IDENT:: *::IDENT::;

int ::LIB::_update(void)
{
	BeginDrawing();
	{
		ClearBackground(RAYWHITE);
		DrawText("::NAME::", 190, 200, 20, LIGHTGRAY);
	}
	EndDrawing();
	return 0;
}
