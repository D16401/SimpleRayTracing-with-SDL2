#ifndef SIMPLE_CG_H
#define SIMPLE_CG_H

#include "cg_math.h"
#include <SDL2/SDL.h>
#include <vector>
#include <memory>

#define CG_INF 1e-7

class Ray{
public:
    Ray(Vec3 origin, Vec3 direction);

    Vec3 getOrigin() const{return origin;}
    Vec3 getDirection() const{return direction;}
private:
    Vec3 origin = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 direction = Vec3(0.0f, 0.0f, 1.0f);
};

class Camera{
public:
    Vec3 getRayDirection(Vec3& viewportP)const;//相机向视口采样点发射的光线方向
    Vec3 CanvasToViewport(const Vec2& canvasP) const;//坐标转换，画布到视口
    Vec3 getPosition()const;
    int getCanvasW()const {return canvasW;}
    int getCanvasH()const {return canvasH;}
private:
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 direction = Vec3(0.0f, 0.0f, 1.0f);
    float depth = 1.0f;
    float viewportW = 1.0f;
    float viewportH = 1.0f;
    int canvasW = 500;
    int canvasH = 500;
};

class Object{
public:
    void setPosition(Vec3 Obj_position);
    void setColor(SDL_Color Obj_color);
    Vec3 getPosition()const {return position;}
    SDL_Color getColor()const {return color;}
    virtual bool intersecRay(const Ray& ray, float& distance) const = 0;
private:
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    SDL_Color color = {0, 0, 0, 255};
};

class Sphere: public Object{
public:
    Sphere();
    void setRadius(float Sphere_radius);
    bool intersecRay(const Ray& ray, float& distance) const override;
private:
    float radius = 1.0;
};

class Scene{
public:
    void addObjectPtr(std::unique_ptr<Object> objectPtr);
    const std::vector<std::unique_ptr<Object>>& getObjectPtrs()const{return objectPtrs;};
    SDL_Color getBackgroundColor() const{return backgroundColor;}
private:
    std::vector<std::unique_ptr<Object>> objectPtrs;
    SDL_Color backgroundColor = {0, 0, 0, 255};
};


SDL_Color SimpleRayTracing(const Camera& camera, const Scene& scene, const Vec2& canvasP);
void PaintPixel(SDL_Renderer*& renderer, Vec2& canvasP, SDL_Color color);

#endif