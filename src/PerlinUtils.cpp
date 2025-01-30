#include "PerlinUtils.hpp"

namespace perlin {

int simpleHash(int i, int j, int N) {
   return (42043 * i + 15299 * j) % N;
}

vec2d random2DGrad(UniformUnitGenerator& unif) {
   auto theta = unif.get() * 2 * M_PI;
   return vec2d{cos(theta), sin(theta)};
}

vec2d random2DGrad() {
   return random2DGrad(AppConfig::getInstance().getUnif());
}

vec3d random3DGrad() {
   return random3DGrad(AppConfig::getInstance().getUnif());
}

vec3d random3DGrad(UniformUnitGenerator& unif) {
   auto cosphi = unif.get() * 2 - 1;
   auto theta = unif.get() * 2 * M_PI;
   auto h = sqrt(1 - cosphi * cosphi);
   return vec3d{h * cos(theta), h * sin(theta), cosphi};
}

double dot(const vec2d& x, const vec2d& y) {
   return x[0] * y[0] + x[1] * y[1];
}

double dot(const vec3d& x, const vec3d& y) {
   return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
}

double fade(const double t) {
   return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(const double a, const double b, const double t) {
   return a + t * (b - a);
}

} // namespace perlin