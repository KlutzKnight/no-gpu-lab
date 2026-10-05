#include "tgaimage.h"
#include <algorithm>
#include <cmath>
#include <utility>

constexpr int width  = 128;
constexpr int height = 128;

// ATTENTION! BGRA order
constexpr TGAColor white  = {255, 255, 255, 255};
constexpr TGAColor blue   = {255, 120,  64, 255};
constexpr TGAColor green  = {  0, 255,   0, 255};
constexpr TGAColor red    = {  0,   0, 255, 255};
constexpr TGAColor yellow = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, const TGAColor& color) {
    // If the line is steep (more vertical than horizontal), transpose the line
    bool steep = std::abs(ax - bx) < std::abs(by - ay);
    if(steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    // If the first is bigger, swap them
    if(ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for(int x = ax; x <= bx; x++) {
        if(steep) {
            // if transposed, de-transpose
            framebuffer.set(y, x, color);
        }
        else {
            framebuffer.set(x, y, color);
        }
        ierror += 2 * std::abs(by - ay);
        if(ierror > (bx - ax)) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx - ax);
        }
    }
}

double signedTriangleArea(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, const TGAColor& color) {
    int bottomLeftX = std::min({ax, bx, cx});
    int bottomLeftY = std::min({ay, by, cy});
    int topRightX = std::max({ax, bx, cx});
    int topRightY = std::max({ay, by, cy});

    double totalArea = signedTriangleArea(ax, ay, bx, by, cx, cy);
#pragma omp parallel for
    for(int y = bottomLeftY; y < topRightY; y++) {
        for(int x = bottomLeftX; x < topRightX; x++) {
            double alpha = signedTriangleArea(x, y, ax, ay, bx, by) / totalArea;
            double beta = signedTriangleArea(x, y, bx, by, cx, cy) / totalArea;
            double gamma = signedTriangleArea(x, y, cx, cy, ax, ay) / totalArea;
            if(alpha < 0 || beta < 0 || gamma < 0) continue;
            framebuffer.set(x, y, color);
        }
    }
}

int main() {
    TGAImage framebuffer{width, height, TGAImage::RGB};

    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    
    framebuffer.write_tga_file("./output/framebuffer.tga");

    return 0;
}