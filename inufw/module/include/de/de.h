#pragma once
#include <inuos.h>

struct Color {
    UByte blue;
    UByte green;
    UByte red;
    UByte alpha;
}
INUPACKED;

struct Color ConsturctColor(UByte red, UByte green, UByte blue);

struct PSFGlyph {
    UByte rows[16];
}
INUPACKED;


Void DeInitialize();
Void DeFillScreen(struct Color color);
Void DeDrawRectangle(UIntPtr startX, UIntPtr startY, UIntPtr x, UIntPtr y, struct Color color);
Void DeFillPixel(UIntPtr x, UIntPtr y, struct Color color);
Void DeDrawLetter(Char value);
Void DeDrawString(const Char *value);
Void DeClearScreen();
Void DeSkipLine();
Void DeReturnCarette();
Void DeLineWind();
