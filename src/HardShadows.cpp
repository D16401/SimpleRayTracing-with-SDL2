#include <SDL2/SDL.h>
#include <iostream>
#include "cg_math.h"
#include "simple_cg.h"

int main(int argc, char* argv[]){
    std::shared_ptr<Camera> cameraPtr = std::make_shared<Camera>();
    cameraPtr->setCameraMode(CameraMode::HardShadows);
    
    std::shared_ptr<Scene> scenePtr = std::make_shared<Scene>();
    scenePtr->setAmbientLight(0.2f);

    auto sphereAPtr = std::make_unique<Sphere>();//堆上创建
    sphereAPtr->setColor({255, 0, 0, 255});
    sphereAPtr->setRadius(0.5f);
    sphereAPtr->setPosition(Vec3(0, 0, -2));
    scenePtr->addObjectPtr(std::move(sphereAPtr));//sphereAPtr现在是nullptr

    auto planePtr = std::make_unique<Plane>();
    planePtr->setColor({255, 255, 0, 255});
    planePtr->setNormal(Vec3(0, 2, 0));
    planePtr->setPosition(Vec3(0, -1.3f, -2));
    scenePtr->addObjectPtr(std::move(planePtr));

    Vec3 L = Vec3(0,1,-1);
    Vec3 L_v = cameraPtr->getForward() * L.dot(cameraPtr->getForward())/(cameraPtr->getForward().length() * cameraPtr->getForward().length());
    Vec3 h = L - L_v;
    float r = h.length();

    auto PLightPtr = std::make_unique<PointLight>(0.8f);
    PLightPtr->setPosition(L);
    scenePtr->addLightPtr(std::move(PLightPtr));

    SDL_Application app;
    app.LoadCamera(cameraPtr);
    app.LoadScene(scenePtr);
    app.Init(false);
    app.Run([&]()
        {
            Vec3 newPosition = (
                L_v + 
                Vec3(static_cast<float>(r*cos(app.getFramesCount()*2*M_PI/40)),
                    static_cast<float>(r*sin(app.getFramesCount()*2*M_PI/40)),
                    0));
            auto* light = scenePtr->getLightPtrs()[0].get();
            if (!light) {
                // 空指针保护
                std::cerr << "Fail to modify the light. Nullptr error!" << std::endl;
                return;
            }
            if (auto* pointLight = dynamic_cast<PointLight*>(light)) {
                pointLight->setPosition(newPosition);
            }
        }
    );
    return 0;
}