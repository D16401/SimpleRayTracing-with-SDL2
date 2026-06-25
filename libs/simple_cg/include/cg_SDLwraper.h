#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <functional>

#include "cg_structure.h"
#include "cg_math.h"

class SDL_Application{
public:
    SDL_Application(){}
    SDL_Application(const char* title);
    SDL_Application(const SDL_Application&) = delete;
    SDL_Application& operator=(const SDL_Application&) = delete;
    ~SDL_Application();
    void LoadCamera(std::shared_ptr<const Camera> camera);
    void LoadScene(std::shared_ptr<const Scene> scene);
    int Init(bool enableTestWindow = false);//根据载入的相机设置，初始化窗口和渲染器
    int Run(std::function<void()> UpdateBlock = [](){});//启动主程序，通过载入的相机渲染载入的场景
    int HandleEvent();//处理事件，窗口关闭和esc按下事件
    int RenderScene();
    void PaintPixel(Vec2& canvasP, SDL_Color color);
    int getTickCount(){return tickCount;}
private:
    const char* title = "Title";
    int canvasW = 200;
    int canvasH = 200;
    bool isCameraLoaded = false;
    bool isSceneLoaded = false;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::shared_ptr<const Camera> cameraPtr;
    std::shared_ptr<const Scene> scenePtr;
    int tickCount = 0;
    int fps = 30;
};

SDL_Color operator*(float scalar, SDL_Color color);
SDL_Color operator*(SDL_Color color, float scalar);
SDL_Color blendByReflectivity(SDL_Color color1, SDL_Color color2, float reflectivity);

