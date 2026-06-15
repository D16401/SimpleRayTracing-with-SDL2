#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "simple_cg.h"
#include "cg_math.h"
#include <iostream>

//Ray
Ray::Ray(Vec3 origin, Vec3 direction): origin(origin), direction(direction){
}

//Camera
Vec3 Camera::getPosition()const{
    return position;
}

Vec3 Camera::CanvasToViewport(const Vec2& canvasP) const {
    float v_w = ((canvasP.getX() + 0.5f) / canvasW - 0.5f) * viewportW;
    float v_h = ((canvasP.getY() + 0.5f) / canvasH - 0.5f) * viewportH;
    return position + (direction.normalize() * depth) + Vec3(v_w, v_h, 0);
}

Vec3 Camera::getRayDirection(Vec3& viewportP) const {
    return position - viewportP;
}

//Object
void Object::setPosition(Vec3 Obj_position){
    position = Obj_position;
}
void Object::setColor(SDL_Color Obj_color){
    color = Obj_color;
}
//Sphere
Sphere::Sphere(): Object(){
}
void Sphere::setRadius(float Sphere_radius){
    radius = Sphere_radius;
}
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
void Scene::addObjectPtr(std::unique_ptr<Object> objectPtr){
    objectPtrs.push_back(std::move(objectPtr));
}


SDL_Color SimpleRayTracing(const Camera& camera, const Scene& scene, const Vec2& canvasP){
    Vec3 viewportP = camera.CanvasToViewport(canvasP);
    Ray ray(camera.getPosition(), viewportP);
    float closest_distance = CGMATH_INF;
    size_t objPtr_index = static_cast<size_t>(-1);//哨兵值
    size_t n_object = scene.getObjectPtrs().size();
    for (size_t idx = 0; idx < n_object; idx++){
        float distance;
        if (scene.getObjectPtrs()[idx]->intersecRay(ray, distance)){
            if (distance < closest_distance){
                closest_distance = distance;
                objPtr_index = idx;
            }
        }
    }
    SDL_Color color = scene.getBackgroundColor();
    if (objPtr_index != static_cast<size_t>(-1)){
        color = scene.getObjectPtrs()[objPtr_index]->getColor();
    }
    return color;
}

void PaintPixel(SDL_Renderer*& renderer, Vec2& canvasP, SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, static_cast<int>(canvasP.getX()), static_cast<int>(canvasP.getY()));
}