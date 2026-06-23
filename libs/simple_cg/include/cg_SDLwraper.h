#ifndef CG_SDLWRAPER
#define CG_SDLWRAPER


#include <SDL2/SDL.h>

#include "cg_math.h"

SDL_Color operator*(float scalar, SDL_Color color);
SDL_Color operator*(SDL_Color color, float scalar);
SDL_Color operator+(SDL_Color color1, SDL_Color color2);

void PaintPixel(SDL_Renderer*& renderer, Vec2& canvasP, SDL_Color color);//在坐标处像素绘制颜色

#endif