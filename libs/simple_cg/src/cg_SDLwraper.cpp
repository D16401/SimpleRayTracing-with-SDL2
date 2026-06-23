#include "cg_SDLwraper.h"

SDL_Color operator*(float scalar, SDL_Color color){
    auto clamp = [](double value) -> Uint8 {
        if (value < 0) return 0;
        if (value > 255) return 255;
        return static_cast<Uint8>(value);
    };
    return {
        clamp(scalar * color.r),
        clamp(scalar * color.g),
        clamp(scalar * color.b),
        color.a
    };
}
SDL_Color operator*(SDL_Color color, float scalar){
    return scalar*color;
}
SDL_Color operator+(SDL_Color color1, SDL_Color color2){
    return {
        static_cast<Uint8>((static_cast<int>(color1.r) + color2.r) / 2),
        static_cast<Uint8>((static_cast<int>(color1.g) + color2.g) / 2),
        static_cast<Uint8>((static_cast<int>(color1.b) + color2.b) / 2),
        static_cast<Uint8>((static_cast<int>(color1.a) + color2.a) / 2)
    };
}

void PaintPixel(SDL_Renderer*& renderer, Vec2& canvasP, SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, static_cast<int>(canvasP.getX()), static_cast<int>(canvasP.getY()));
}