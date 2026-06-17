#include <SDL2/SDL.h>
#include <iostream>
#include "cg_math.h"
#include "simple_cg.h"

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
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);//设置颜色:黑色，完全不透明
    SDL_RenderClear(renderer);//全屏绘制颜色
    SDL_RenderPresent(renderer);//显示绘制的内容

    Camera camera;
    camera.setCameraMode(CameraMode::DirectLighting);
    Scene scene;//栈上创建
    scene.setALight(0.2f);

    auto sphereAPtr = std::make_unique<Sphere>();//堆上创建
    sphereAPtr->setColor({255, 0, 0, 255});
    sphereAPtr->setRadius(0.5f);
    sphereAPtr->setPosition(Vec3(0, 0, -2));
    scene.addObjectPtr(std::move(sphereAPtr));//sphereAPtr现在是nullptr

    Vec3 L = Vec3(0,1,-1);
    Vec3 L_v = camera.getForward() * L.dot(camera.getForward())/(L.length() * camera.getForward().length() * camera.getForward().length());
    Vec3 h = L - L_v;
    float r = h.length();

    auto DLight1Ptr = std::make_unique<DLight>(0.8f);
    DLight1Ptr->setDirection(L);
    scene.addDLightPtr(std::move(DLight1Ptr));

    bool isrunning = true;
    int timer = 0;
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
        
        for (int i = 0; i < camera.getCanvasW(); i++){
            for (int j = 0; j < camera.getCanvasH(); j++){
                Vec2 canvasP = Vec2(float(i), float(j));
                SDL_Color color = SimpleRayTracing(camera, scene, canvasP);
                PaintPixel(renderer, canvasP, color);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(160);
        timer++;
        Vec3 newDirection = L_v + Vec3(r*cos(timer*2*M_PI/20), r*sin(timer*2*M_PI/20), 0);
        scene.getDLightPtrs()[0]->setDirection(newDirection);
    }
    return 0;
}