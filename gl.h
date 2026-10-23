#pragma once

#include "geometry.h"
#include "tgaimage.h"

vec3 transform(const vec3& v);

void lookAt(const vec3& eye, const vec3& center, const vec3& up);
void initViewport(const int x, const int y, const int w, const int h);
void initPerspective(const double f);
void initZBuffer(const int width, const int height);

struct IShader {
    virtual std::pair<bool, TGAColor> fragment(const vec3& bar) const = 0;
};

using Triangle = vec3[3];   // Triangle primitive made of 3 ordered points

void line(vec2i& a, vec2i& b, TGAImage& frameBuffer, const TGAColor& color);
void rasterize(const Triangle& screen, const IShader& shader, TGAImage& frameBuffer);
