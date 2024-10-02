#include <inuos.h>
#include <de/de.h>
#include <executive/rt/api.h>
#include <de/unifont.h>
#include <inufw/api.h>

#define PSF1_HEADER_SIZE 4

INUEXTERN Void* FbAddress();
INUEXTERN UIntPtr FbX();
INUEXTERN UIntPtr FbY();

INUSTATIC struct PSFGlyph* base;
INUSTATIC UIntPtr xLength;
INUSTATIC UIntPtr yLength;
INUSTATIC Void *framebufferAddress;

INUSTATIC UIntPtr Bpp;
INUSTATIC UIntPtr xFontLength;
INUSTATIC UIntPtr yFontLength;
INUSTATIC Boolean initialized = false;
INUSTATIC UIntPtr currentX;
INUSTATIC UIntPtr currentY;

struct Color ConsturctColor(UByte red, UByte green, UByte blue) {
    struct Color target;
    target.blue = blue;
    target.red = red;
    target.green = green;
    target.alpha = 0;
    return target;
}

Void DEPrint(const Char *string) {
    if (initialized) {
        DeDrawString(string);
    }
}

Void DeInitialize() {
    framebufferAddress = FbAddress();
    xLength = FbX();
    yLength = FbY();
    currentX = 0;
    currentY = 1;
    xFontLength = 8;
    yFontLength = 16;
    Bpp = 32;
    initialized = true;
    DeFillScreen(ConsturctColor(0,0,0));
}

Void DeFillScreen(const struct Color color) {
    struct Color *fb = framebufferAddress;
    for (UIntPtr i = 0; i < xLength * yLength; ++i) {
        fb[i] = color;
    }
}

Void DeDrawRectangle(UIntPtr startX, UIntPtr startY, UIntPtr x, UIntPtr y, struct Color color) {
    struct Color *fb = (framebufferAddress);
    fb+=(startY*xLength);
    fb+=startX;
    for (UIntPtr i = 0; i < y; ++i) {
        for (UIntPtr z = 0; z < x; ++z) {
            fb[(i*xLength)+z] = color;
        }
    }
}

Void DeFillPixel(UIntPtr x, UIntPtr y, struct Color color) {
    struct Color *fb = framebufferAddress;
    fb+=(y*xLength);
    fb+=x;
    *fb = color;
}

Void DeDrawLetter(const Char value) {
    base = ((struct PSFGlyph*)(__Unifont_APL8x16_15_1_05_psf+PSF1_HEADER_SIZE));
    if (currentX > xLength-xFontLength) {
        currentX = 0;
        currentY += yFontLength;
    }
    if (currentY >= yLength-yFontLength) {
        currentY = yLength-yFontLength;
        DeLineWind();
    }
    currentX+=8;
    const struct PSFGlyph target = base[value];
    for (int i = yFontLength-1; i >= 0; --i) {
        for (int z = xFontLength; z >= 0; --z) {
            if (target.rows[i] & (1 << z)) {
                DeFillPixel(currentX-z,currentY+i,ConsturctColor(255,191,0));
            }
        }
    }
}

Void DeDrawString(const Char *value) {
    const UIntPtr length = wstrlen(value);
    for (UIntPtr i = 0; i < length; ++i) {
        if (value[i] == '\r') {
            DeReturnCarette();
        }
        else if (value[i] == '\n') {
            DeSkipLine();
        }
        else {
            DeDrawLetter(value[i]);
        }
    }
}

Void DeClearScreen() {
    DeFillScreen(ConsturctColor(0,0,0));
}

Void DeSkipLine() {
    currentY += yFontLength;
    if (currentY > yLength-yFontLength) {
        currentY = yLength-yFontLength;
        DeLineWind();
    }
}

Void DeReturnCarette() {
    currentX = 0;
}

Void DeLineWind() {
    //currentY -= yFontLength;
    currentY = 0;
    //Void* fbAdress = FbAddress();
    //UIntPtr oneLineSize = 4*xLength;
    //memmove(fbAdress,fbAdress+oneLineSize*(yFontLength),oneLineSize*yLength-oneLineSize*yFontLength);
    DeClearScreen();
}
