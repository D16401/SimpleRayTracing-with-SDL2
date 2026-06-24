#pragma once

#include "cg_structure.h"

Vec3 CanvasToViewport(const Camera& camera, const Vec2& canvasP);//坐标转换，画布到视口

size_t FindClosestIntersection(const Scene& scene, Ray& intersectRay, float& closest_distance);
float DiffuseFactor(const Vec3& HitPointNormal, const Vec3& HitInDirection);
float SpecularFactor(int specular, const Vec3& HitPointNormal, const Vec3& HitInDirection, const Vec3& ViewDirection);
float ComputeLighting(const Camera& camera, const Scene& scene, const Vec3& HitPoint, const Vec3& HitPointNormal, int SurfaceSpecular, bool enableOcclusionTest);
SDL_Color SimpleRayTracing(const Camera& camera, const Scene& scene, const Vec2& canvasP);
