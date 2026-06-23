#ifndef CG_STRUCTURE_H
#define CG_STRUCTURE_H

#include <cstdint>
#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>
#include <memory>

#include "cg_math.h"

class Scene;


class Ray{
public:
    Ray(){}
    Ray(Vec3 origin, Vec3 direction): origin(origin), direction(direction){}
    //getter
    Vec3 getOrigin() const {return origin;}
    Vec3 getDirection() const {return direction;}
    //setter
    void setDirection(Vec3 vector_D){direction = vector_D;}
private:
    Vec3 origin = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 direction = Vec3(0.0f, 0.0f, 1.0f);
};


enum class CameraMode : uint8_t {//#include <cstdint>, #include <vector>间接包含？
    VisibilityOnly,     // 仅追踪最近交点，返回 albedo / normal
    DirectLighting,     // + Lambertian / Phong 光照
    HardShadows,        // + shadow ray
    RecursiveReflection // + recursive trace()
};
class Camera{
public:
    //getter
    int getCanvasW() const {return canvasW;}
    int getCanvasH() const {return canvasH;}
    float getDepth() const {return depth;}
    float getViewportW() const {return viewportW;}
    float getViewportH() const {return viewportH;}
    Vec3 getForward() const {return forward;}
    Vec3 getUp() const {return up;}
    Vec3 getRight() const {return right;}
    Vec3 getRayDirection(Vec3& viewportP) const {return position - viewportP;}//相机向视口采样点发射的光线方向
    Vec3 getPosition() const {return position;}
    CameraMode getCameraMode() const {return cameraMode;}
    //setter
    void setCameraMode(CameraMode mode) {cameraMode = mode;}
private:
    //output setting
    int canvasW = 500;
    int canvasH = 500;
    CameraMode cameraMode = CameraMode::VisibilityOnly;
    //sample setting
    float depth = 1.0f;
    float viewportW = 1.0f;
    float viewportH = 1.0f;
    //右手坐标系
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 forward = Vec3(0.0f, 0.0f, -1.0f);
    Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
    Vec3 right = Vec3(1.0f, 0.0f, 0.0f);//forward.cross()up
};


enum class ObjectType : uint8_t {
    NONE,
    Sphere,          //球体
    Plane,     //平面
};
class Object{
public:
    Object(ObjectType type): type(type){}
    //getter
    Vec3 getPosition() const {return position;}
    SDL_Color getColor() const {return color;}
    int getSpecular() const {return specular;}
    ObjectType getType() const {return type;}
    virtual Vec3 getNormal(const Vec3& Hitpoint) const = 0;
    //setter
    void setPosition(Vec3 Obj_position){position = Obj_position;}
    void setColor(SDL_Color Obj_color){color = Obj_color;};
    //virtual interaction
    virtual bool intersectTest(const Ray& ray, float& return_distance) const = 0;
private:
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    SDL_Color color = {0, 0, 0, 255};
    int specular = -1;
    ObjectType type = ObjectType::NONE;
};
class Sphere: public Object{
public:
    Sphere(): Object(ObjectType::Sphere){}
    //getter
    float getRadius() const {return radius;}
    Vec3 getNormal(const Vec3& Hitpoint) const override {return (Hitpoint - this->getPosition());}
    //setter
    void setRadius(float Sphere_radius){radius = Sphere_radius;}
    //virtual interaction
    bool intersectTest(const Ray& ray, float& return_distance) const;
private:
    float radius = 1.0;
};
class Plane: public Object{
public:
    Plane(): Object(ObjectType::Plane){}
    //getter
    Vec3 getNormal() const {return normal;}
    Vec3 getNormal(const Vec3& Hitpoint) const override {return normal;}
    //setter
    void setNormal(Vec3 N) {normal = N;}
    //virtual interaction
    bool intersectTest(const Ray& ray, float& return_distance) const;
private:
    Vec3 normal;
};


enum class LightType : uint8_t {
    Point,          //点光源
    Directional,     //方向光
};
class Light{
public:
    Light(float intensity, LightType type): intensity(intensity), type(type){}
    virtual ~Light() = default;
    //getter
    float getIntensity(){return std::clamp(intensity, 0.f, 1.f);}//clamp, #include <algorithm>
    virtual Vec3 getHitInDirection(const Vec3& HitPoint) const = 0;
    //virtual interaction
    virtual bool Light::OcclusionTest(const Scene& scene, const Vec3& HitPoint) const = 0;
private:
    float intensity = 0;//光强，值应为0~1
    LightType type;
};
class PointLight: public Light{//Point Light，有位置无方向
public:
    PointLight(float intensity): Light(intensity, LightType::Point){}
    //getter
    Vec3 getPosition() const {return position;}
    Vec3 getHitInDirection(const Vec3& HitPoint)const override {return HitPoint - position;};
    //setter
    void setPosition(Vec3 P){position = P;}
    //virtual interaction
    bool OcclusionTest(const Scene& scene, const Vec3& HitPoint) const override;
private:
    Vec3 position = Vec3(0, 0, 0);
};
class DirectionalLight: public Light{//Directional Light，有方向无位置
public:
    DirectionalLight(float intensity): Light(intensity, LightType::Directional){}
    //getter
    Vec3 getDirection() const {return direction;}
    Vec3 getHitInDirection(const Vec3& HitPoint)const override {return direction;};
    //setter
    void setDirection(Vec3 D){direction = D;}
    //virtual interaction
    bool OcclusionTest(const Scene& scene, const Vec3& HitPoint) const override;
private:
    Vec3 direction = Vec3(0, 0, 1);
};



class Scene{
public:
    //getter
    SDL_Color getBackgroundColor() const {return backgroundColor;}//返回Scene实例的背景色
    float getAmbientLight() const {return std::clamp(AmbientLight_intensity, 0.f, 1.f);}//返回当前scene的环境光强
    const std::vector<std::unique_ptr<Object>>& getObjectPtrs() const {return ObjectPtrs;}
    const std::vector<std::unique_ptr<Light>>& getLightPtrs() const {return LightPtrs;}
    //setter
    void setAmbientLight(float intensity){AmbientLight_intensity = intensity;};
    //adder
    void addObjectPtr(std::unique_ptr<Object> ObjectPtr);//为Scene实例添加物体（unique_ptr）//unique_ptr, #include <memory>
    void addLightPtr(std::unique_ptr<Light> LightPtr);//为Scene实例添加光源
private:
    SDL_Color backgroundColor = {0, 0, 0, 255};
    float AmbientLight_intensity = 0;//Ambient Light环境光强
    std::vector<std::unique_ptr<Object>> ObjectPtrs;
    std::vector<std::unique_ptr<Light>> LightPtrs;
};
#endif