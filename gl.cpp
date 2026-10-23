#include "geometry.h"
#include "gl.h"
#include "tgaimage.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>


mat<4, 4> Viewport, Perspective, ModelView;     // "OpenGL" state matrices
std::vector<double> zBuffer;                    // Depth Buffer

vec3 transform(const vec3& v) {
    vec4 per = Perspective * ModelView * vec4{v.x, v.y, v.z, 1};
    vec4 res = Viewport * vec4{per.x / per.w, per.y / per.w, per.z / per.w, 1};
    return {res.x, res.y, res.z};
}

void lookAt(const vec3& eye, const vec3& center, const vec3& up) {
    vec3 n = normalize(eye - center);
    vec3 l = normalize(cross(up, n));
    vec3 m = normalize(cross(n, l));
    ModelView = mat<4, 4> {{{l.x, l.y, l.z, 0},
                            {m.x, m.y, m.z, 0},
                            {n.x, n.y, n.z, 0},
                            { 0, 0, 0, 1}}}
                * mat<4, 4> {{{1, 0, 0, -center.x},
                            {0, 1, 0, -center.y},
                            {0, 0, 1, -center.z},
                            {0, 0, 0, 1}}};
}
void initViewport(const int x, const int y, const int w, const int h) {
    Viewport = {{
        {w/2.0, 0, 0,x + w/2.0},
        { 0,h/2.0, 0,y + h/2.0},
        { 0, 0, 1, 0},
        { 0, 0, 0, 1},
    }};
}
void initPerspective(const double f) {
    Perspective = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, -1/f, 1},
    }};
}
void initZBuffer(const int width, const int height) {
    zBuffer = std::vector(width * height, std::numeric_limits<double>::lowest());
}

void line(vec2i& a, vec2i& b, TGAImage& frameBuffer, const TGAColor& color) {
    // If the line is steep (more vertical than horizontal), transpose the line
    bool steep = std::abs(a.x - b.x) < std::abs(b.y - a.y);
    if(steep) {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
    }
    // If the first is bigger, swap them
    if(a.x > b.x) {
        std::swap(a.x, b.x);
        std::swap(a.y, b.y);
    }
    int y = a.y;
    int ierror = 0;
    for(int x = a.x; x <= b.x; x++) {
        if(steep) {
            // if transposed, de-transpose
            frameBuffer.set(y, x, color);
        }
        else {
            frameBuffer.set(x, y, color);
        }
        ierror += 2 * std::abs(b.y - a.y);
        if(ierror > (b.x - a.x)) {
            y += b.y > a.y ? 1 : -1;
            ierror -= 2 * (b.x - a.x);
        }
    }
}

void rasterize(const Triangle& screen, const IShader& shader, TGAImage& frameBuffer) {
    // Triangle defined by the three vertices
    mat<3, 3> ABC = {{
        {screen[0].x, screen[0].y, 1},
        {screen[1].x, screen[1].y, 1},
        {screen[2].x, screen[2].y, 1}
    }};
    // Conservative Back-face culling and discarding triangles that cover less than a pixel
    if(ABC.determinant() < 1) return;

    // Calculate the bounding box of the triangle
    const auto [bbMinX, bbMaxX] = std::minmax({screen[0].x, screen[1].x, screen[2].x});
    const auto [bbMinY, bbMaxY] = std::minmax({screen[0].y, screen[1].y, screen[2].y});
#pragma omp parallel for
    for(int y = std::max<int>(bbMinY, 0); y <= std::min<int>(bbMaxY, frameBuffer.height() - 1); y++) {
        for(int x = std::max<int>(bbMinX, 0); x <= std::min<int>(bbMaxX, frameBuffer.width() - 1); x++) {
            // Barycentric Coordinates of (x, y) w.r.t the triangle
            vec3 bc = ABC.transpose().inverse() * vec3 {static_cast<double>(x), static_cast<double>(y), 1};
            // Barycentric Coordinates being negative implies the point is outside the triangle
            if(bc[0] < 0 || bc[1] < 0 || bc[2] < 0) continue;

            // Linear interpolation of depth
            double z = dot(bc, vec3 {screen[0].z, screen[1].z, screen[2].z});
            if(z <= zBuffer[x + y * frameBuffer.width()]) continue; // Discard fragments too deep w.r.t. z-buffer
            auto [discard, color] = shader.fragment(bc);
            if(discard) continue;           // fragment shader can discard current fragment

            zBuffer[x + y * frameBuffer.width()] = z;   // update z-buffer
            frameBuffer.set(x, y, color);               // update the framebuffer
        }
    }
}
