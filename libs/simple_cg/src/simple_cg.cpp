#include <vector>
#include <memory>
#include <iostream>

#include <SDL2/SDL.h>

#include "cg_math.h"

#include "simple_cg.h"

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

//Camera
Vec3 Camera::CanvasToViewport(const Vec2& canvasP) const {
    float v_w = ((canvasP.getX() + 0.5f) / canvasW - 0.5f) * viewportW;
    float v_h = ((canvasP.getY() + 0.5f) / canvasH - 0.5f) * viewportH;
    return position + (forward.normalize() * depth) + Vec3(v_w, v_h, 0);
}


//Sphere
bool Sphere::intersecRay(const Ray& ray, float& distance) const{
    Vec3 D = ray.getDirection();
    Vec3 O = ray.getOrigin();
    Vec3 C = this->getPosition();
    float r = radius;
    float a = D.dot(D);
    float b = 2 * D.dot(O - C);
    float c = (O-C).dot(O-C) - r*r;
    float x1,x2;
    int n_root = solveQuadratic(a, b, c, x1, x2);
    if (n_root != 0){
        if (x1 > 0){
            distance = x1;
            return true;
        }else{
            
            return false;
        }
    }else{
        return false;
    }
}


//Scene
void Scene::addObjectPtr(std::unique_ptr<Object> ObjectPtr){
    ObjectPtrs.push_back(std::move(ObjectPtr));
}
void Scene::addPLightPtr(std::unique_ptr<PLight> PLightPtr){
    PLightPtrs.push_back(std::move(PLightPtr));
}
void Scene::addDLightPtr(std::unique_ptr<DLight> DLightPtr){
    DLightPtrs.push_back(std::move(DLightPtr));
}


//algotithm for raytracing
float DiffuseFactor(const Vec3& HitpointNormal, const Vec3& HitLight){
    float N_dot_L = HitpointNormal.dot(HitLight);
    if( N_dot_L > 0){
        float Factor = N_dot_L / (HitpointNormal.length() * HitLight.length());
        return Factor;
    }else{
        return 0;
    }
}

float SpecularFactor(int specular, const Vec3& HitpointNormal, const Vec3& HitLight, const Vec3& ViewDirection){
    if (specular <= 0){
        return 0;
    }else{
        float N_dot_L = HitpointNormal.dot(HitLight);
        Vec3 ReflectLight = HitpointNormal* 2 * N_dot_L  - HitLight;
        float R_dot_V = ReflectLight.dot(ViewDirection); 
        if (R_dot_V > 0){
            float Factor = static_cast<float>(pow((R_dot_V / (ReflectLight.length() * ViewDirection.length())), specular));
            return Factor;
        }else{
            return 0;
        }
    }
}

float ComputeLighting(const Scene& scene, const Vec3& Hitpoint, const Vec3& HitpointNormal, int SurfaceSpecular, const Vec3& cameraPosition){
    float total_intensity = 0;
    Vec3 viewDirection = Hitpoint - cameraPosition;
    size_t n_Plights = scene.getPLightPtrs().size();
    for (size_t idx = 0; idx < n_Plights; idx++){
        Vec3 HitLight = Hitpoint - scene.getPLightPtrs()[idx]->getPosition();
        float IntensityFactor = DiffuseFactor(HitpointNormal, HitLight) + SpecularFactor(SurfaceSpecular, HitpointNormal, HitLight, viewDirection);
        total_intensity = total_intensity + scene.getPLightPtrs()[idx]->getIntensity() * IntensityFactor;
    }
    size_t n_Dlights = scene.getDLightPtrs().size();
    for (size_t idx = 0; idx < n_Dlights; idx++){
        Vec3 HitLight = scene.getDLightPtrs()[idx]->getDirection();
        float IntensityFactor = DiffuseFactor(HitpointNormal, HitLight) + SpecularFactor(SurfaceSpecular, HitpointNormal, HitLight, viewDirection);
        total_intensity = total_intensity + scene.getDLightPtrs()[idx]->getIntensity() * IntensityFactor;
    }
    return total_intensity;
}

SDL_Color SimpleRayTracing(const Camera& camera, const Scene& scene, const Vec2& canvasP){
    Vec3 camearPos = camera.getPosition();
    Vec3 viewportP = camera.CanvasToViewport(canvasP);
    Ray ray(camearPos, viewportP);
    float closest_distance = CGMATH_INF;
    size_t closest_objPtr_index = static_cast<size_t>(-1);//哨兵值
    size_t n_object = scene.getObjectPtrs().size();
    for (size_t objPtr_idx = 0; objPtr_idx < n_object; objPtr_idx++){
        float distance;
        if (scene.getObjectPtrs()[objPtr_idx]->intersecRay(ray, distance)){
            if (distance < closest_distance){
                closest_distance = distance;
                closest_objPtr_index = objPtr_idx;
            }
        }
    }
    SDL_Color color = scene.getBackgroundColor();
    if (closest_objPtr_index != static_cast<size_t>(-1)){
        color = scene.getObjectPtrs()[closest_objPtr_index]->getColor();
    }
    switch (camera.getCameraMode())
    {
        case CameraMode::VisibilityOnly:
            break;
        case CameraMode::DirectLighting:
            float intensity = 0;
            if (closest_objPtr_index != static_cast<size_t>(-1)){
                Vec3 Hitpoint = camearPos + ray.getDirection() * closest_distance;
                Vec3 HitpointNormal = scene.getObjectPtrs()[closest_objPtr_index]->getNormal(Hitpoint);
                int specular = scene.getObjectPtrs()[closest_objPtr_index]->getSpecular();
                intensity = ComputeLighting(scene, Hitpoint, HitpointNormal, specular, camearPos);
            }
            color = color * (scene.getALight() + intensity);
            break;
    }
    return color;
}

void PaintPixel(SDL_Renderer*& renderer, Vec2& canvasP, SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, static_cast<int>(canvasP.getX()), static_cast<int>(canvasP.getY()));
}