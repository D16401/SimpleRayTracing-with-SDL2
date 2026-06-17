#ifndef SIMPLE_CG_H
#define SIMPLE_CG_H

#include "cg_math.h"
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <algorithm>

#define CG_INF 1e-7

SDL_Color operator*(float scalar, SDL_Color color);
SDL_Color operator*(SDL_Color color, float scalar);
SDL_Color operator+(SDL_Color color1, SDL_Color color2);

class Ray{
public:
    Ray(){}
    Ray(Vec3 origin, Vec3 direction): origin(origin), direction(direction){}
    Vec3 getOrigin() const {return origin;}
    Vec3 getDirection() const {return direction;}
    void setDirection(Vec3 vector_D){direction = vector_D;}
private:
    Vec3 origin = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 direction = Vec3(0.0f, 0.0f, 1.0f);
};

enum class CameraMode : uint8_t {
    VisibilityOnly,     // 仅追踪最近交点，返回 albedo / normal
    DirectLighting,     // + Lambertian / Phong 光照
    HardShadows,        // + shadow ray
    RecursiveReflection // + recursive trace()
};

class Camera{
public:
    Vec3 getForward() const {return forward;}
    Vec3 getUp() const {return up;}
    Vec3 getRight() const {return right;}
    Vec3 getRayDirection(Vec3& viewportP) const {return position - viewportP;}//相机向视口采样点发射的光线方向
    Vec3 CanvasToViewport(const Vec2& canvasP) const;//坐标转换，画布到视口
    Vec3 getPosition() const {return position;}
    int getCanvasW() const {return canvasW;}
    int getCanvasH() const {return canvasH;}
    CameraMode getCameraMode() const {return cameraMode;}
    void setCameraMode(CameraMode mode) {cameraMode = mode;}
private:
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 forward = Vec3(0.0f, 0.0f, -1.0f);
    Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
    Vec3 right = Vec3(1.0f, 0.0f, 0.0f);//forward.cross()up
    float depth = 1.0f;
    float viewportW = 1.0f;
    float viewportH = 1.0f;
    int canvasW = 500;
    int canvasH = 500;
    CameraMode cameraMode = CameraMode::VisibilityOnly;
};

enum class LightType : uint8_t {
    Point,          //点光源
    Directional,     //方向光
};

class Light{
public:
    Light(float intensity, LightType type): intensity(intensity), type(type){}
    float getIntensity(){return std::clamp(intensity, 0.f, 1.f);}
    LightType getType(){return type;}
private:
    float intensity = 0;//光强，值应为0~1
    LightType type;
};

class PLight: public Light{//Point Light，有位置无方向
public:
    PLight(float intensity): Light(intensity, LightType::Point){}
    Vec3 getPosition() const {return position;}
    void setPosition(Vec3 P){position = P;}
private:
    Vec3 position = Vec3(0, 0, 0);
};

class DLight: public Light{//Directional Light，有方向无位置
public:
    DLight(float intensity): Light(intensity, LightType::Directional){}
    Vec3 getDirection() const {return direction;}
    void setDirection(Vec3 D){direction = D;}
private:
    Vec3 direction = Vec3(0, 0, 1);
};

class Object{
public:
    void setPosition(Vec3 Obj_position){position = Obj_position;}
    void setColor(SDL_Color Obj_color){color = Obj_color;};
    Vec3 getPosition() const {return position;}
    SDL_Color getColor() const {return color;}
    int getSpecular() const {return specular;}
    virtual bool intersecRay(const Ray& ray, float& distance) const = 0;
    virtual Vec3 getNormal(const Vec3& Hitpoint) const = 0;
private:
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    SDL_Color color = {0, 0, 0, 255};
    int specular = -1;
};

class Sphere: public Object{
public:
    Sphere(): Object(){};
    void setRadius(float Sphere_radius){radius = Sphere_radius;}
    bool intersecRay(const Ray& ray, float& distance) const override;
    Vec3 getNormal(const Vec3& Hitpoint) const override {return (Hitpoint - this->getPosition());}
private:
    float radius = 1.0;
};

class Scene{
public:
    void addObjectPtr(std::unique_ptr<Object> ObjectPtr);//为Scene实例添加物体（unique_ptr）
    void addPLightPtr(std::unique_ptr<PLight> PLightPtr);//为Scene实例添加光源（unique_ptr）
    void addDLightPtr(std::unique_ptr<DLight> DLightPtr);//为Scene实例添加光源（unique_ptr）
    const std::vector<std::unique_ptr<Object>>& getObjectPtrs() const {return ObjectPtrs;}
    const std::vector<std::unique_ptr<PLight>>& getPLightPtrs() const {return PLightPtrs;}
    const std::vector<std::unique_ptr<DLight>>& getDLightPtrs() const {return DLightPtrs;}
    SDL_Color getBackgroundColor() const {return backgroundColor;}//返回Scene实例的背景色
    float getALight() const {return std::clamp(ALight_intensity, 0.f, 1.f);}
    void setALight(float intensity){ALight_intensity = intensity;};
private:
    std::vector<std::unique_ptr<Object>> ObjectPtrs;
    std::vector<std::unique_ptr<PLight>> PLightPtrs;
    std::vector<std::unique_ptr<DLight>> DLightPtrs;
    SDL_Color backgroundColor = {0, 0, 0, 255};
    float ALight_intensity = 0;//Ambient Light环境光强
};

//algotithm for raytracing
float DiffuseFactor(const Vec3& HitpointNormal, const Vec3& HitLight);
float SpecularFactor(int specular, const Vec3& HitpointNormal, const Vec3& HitLight, const Vec3& ViewDirection);
float ComputeLighting(const Scene& scene, const Vec3& Hitpoint, const Vec3& HitpointNormal, int SurfaceSpecular, const Vec3& cameraPosition);
SDL_Color SimpleRayTracing(const Camera& camera, const Scene& scene, const Vec2& canvasP);//执行光线追踪
void PaintPixel(SDL_Renderer*& renderer, Vec2& canvasP, SDL_Color color);//在坐标处像素绘制颜色

#endif