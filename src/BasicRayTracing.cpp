#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include "cg_math.h"
#include "simple_cg.h"

int main(int argc, char* argv[]){
    std::shared_ptr<Camera> cameraPtr = std::make_shared<Camera>();
    cameraPtr->setCameraMode(CameraMode::VisibilityOnly);

    std::shared_ptr<Scene> scenePtr = std::make_shared<Scene>();
    auto sphereAPtr = std::make_unique<Sphere>();//堆上创建
    sphereAPtr->setColor({255, 0, 0, 255});
    sphereAPtr->setRadius(0.5f);
    sphereAPtr->setPosition(Vec3(0, 0, -2));
    scenePtr->addObjectPtr(std::move(sphereAPtr));//sphereAPtr现在是nullptr

    SDL_Application app;
    app.LoadCamera(cameraPtr);//隐式转换为std::shared_ptr<const Camera>
    app.LoadScene(scenePtr);//隐式转换为std::shared_ptr<const scene>
    app.Init();
    app.Run();
    return 0;
}