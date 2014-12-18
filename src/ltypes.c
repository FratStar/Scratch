#include "ltypes.h"

arBool RectOlap(arRect a, arRect b)
{
	int left, left2;
	int right, right2;
	int top, top2;
	int bottom, bottom2;

	left = a.x;
	right = a.x + a.w;
	top = a.y;
	bottom = a.y + a.h;

	left2 = b.x;
	right2 = b.x + b.w;
	top2 = b.y;
	bottom2 = b.y + b.h;

	if((right >= left2) && (left <= right2) && (bottom >= top2) && (top <= bottom2))
	{
		printf("Bitch you guessed it");
		return arTrue;
	}
	return arFalse;

}

arRect ar_Rect(arInt x, arInt y, arUint w, arUint h)
{
	arRect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}
