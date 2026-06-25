#include <iostream>
#include <functional>
#include <memory>

#include "cg_interaction.h"
#include "cg_math.h"

#include "cg_SDLwraper.h"

SDL_Application::SDL_Application(const char* Title){
    title = Title;
}
SDL_Application::~SDL_Application() {
    if (renderer) { SDL_DestroyRenderer(renderer); }
    if (window)   { SDL_DestroyWindow(window); }
    SDL_Quit();
}

void SDL_Application::LoadCamera(std::shared_ptr<const Camera> camera_ptr){
    cameraPtr = camera_ptr;
    canvasW = cameraPtr->getCanvasW();
    canvasH = cameraPtr->getCanvasH();
    isCameraLoaded = true;
}
void SDL_Application::LoadScene(std::shared_ptr<const Scene> scene_ptr){
    scenePtr = scene_ptr;
    isSceneLoaded = true;
}
int SDL_Application::Init(bool enableTestWindow){
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "Fail to initialize SDL:" << SDL_GetError() << std::endl;
        return -1;
    }
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        canvasW,//camera.canvasW,
        canvasH,//camera.canvasH,
        SDL_WINDOW_SHOWN
    );
    if(!window){
        std::cerr << "Fail to create the window:" << SDL_GetError() << std::endl;
        return -1;
    }
    renderer = SDL_CreateRenderer(
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
    bool testWindow = enableTestWindow;
    if (testWindow) {
        std::cout << "SDL initialization test mode. Press ESC to continue main program..." << std::endl;
    }
    while(testWindow){
        SDL_SetRenderDrawColor(renderer, randint(1, 255), randint(1, 255), randint(1, 255), 255);//设置颜色
        SDL_RenderClear(renderer);//绘制颜色
        SDL_RenderPresent(renderer);//显示（黑色）屏幕
        if(HandleEvent() == 1){
            testWindow = false;
            std::cout << "Test window is terminated by the user." << std::endl;
        }
        SDL_Delay(1000/fps);
    };
    return 0;
}
int SDL_Application::Run(std::function<void()> UpdateBlock){
    bool isrunning = true;
    Uint64 lastTime = SDL_GetTicks64();
    const Uint64 frameDelay = 1000 / fps;
    double lag = 0.0;
    while (isrunning)//固定30hz的updateblock，渲染越快越好
    {
        Uint64 currentTime = SDL_GetTicks64();
        Uint64 elapsed = currentTime - lastTime;
        lastTime = currentTime;
        lag += elapsed;
        if(HandleEvent() == 1){
            isrunning = false;
            std::cout << "Terminated by the user." << std::endl;
            break;
        }
        while (lag >= frameDelay){
            tickCount++;
            UpdateBlock();
            lag -= frameDelay;
        }
        int renderStatus = RenderScene();
        if (renderStatus == 1){
            isrunning = false;
            std::cerr << "No Camera has been loaded." << std::endl;
        }else if (renderStatus == 2){
            isrunning = false;
            std::cerr << "No Scene has been loaded." << std::endl;
        }else if(renderStatus == 0){//do nothing
        }
    }
    return 0;
}
int SDL_Application::HandleEvent(){
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT){
            return 1;
        }else if(event.type == SDL_KEYDOWN){
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    return 1;
            }
        }
    }
    return 0;
}
int SDL_Application::RenderScene(){
    if (isCameraLoaded == false){
        return 1;
    }else if(isSceneLoaded == false){
        return 2;
    }
    for (int i = 0; i < cameraPtr->getCanvasW(); i++){
        for (int j = 0; j < cameraPtr->getCanvasH(); j++){
            Vec2 canvasP = Vec2(float(i), float(j));
            Vec3 viewportP = CanvasToViewport(*cameraPtr, canvasP);
            SDL_Color color = SimpleRayTracing(*cameraPtr, *scenePtr, viewportP);
            PaintPixel(canvasP, color);
        }
    }
    SDL_RenderPresent(renderer);
    return 0;
}
void SDL_Application::PaintPixel(Vec2& canvasP, SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, static_cast<int>(canvasP.getX()), static_cast<int>(canvasP.getY()));
}


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
SDL_Color blendByReflectivity(SDL_Color color1, SDL_Color color2, float reflectivity){
    return {
        static_cast<Uint8>((color1.r * (1-reflectivity) + color2.r * reflectivity)),
        static_cast<Uint8>((color1.g * (1-reflectivity) + color2.g * reflectivity)),
        static_cast<Uint8>((color1.b * (1-reflectivity) + color2.b * reflectivity)),
        static_cast<Uint8>((color1.a * (1-reflectivity)+ color2.a * reflectivity))
    };
}
