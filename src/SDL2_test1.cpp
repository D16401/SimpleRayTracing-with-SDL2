#include <SDL2/SDL.h>
#include <iostream>
#include "cg_math.h"

int main(int argc, char* argv[]){
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "Fail to initialize SDL:" << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Title",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        500,//camera.canvasW,
        500,//camera.canvasH,
        SDL_WINDOW_SHOWN
    );
    if(!window){
        std::cerr << "Fail to create the window:" << SDL_GetError() << std::endl;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if(!renderer){
	    std::cerr << "Fail to create the window:" << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }


    bool isrunning = true;
    SDL_Event event;
    while (isrunning)
    {
        while (SDL_PollEvent(&event))//监测关闭按钮和按键esc是否被按下
        {
            if(event.type == SDL_QUIT){
                isrunning = false;
            }else if(event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        isrunning = false;
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);//设置颜色
        SDL_RenderClear(renderer);//绘制颜色
        SDL_RenderPresent(renderer);//显示平面
    }
    return 0;
}