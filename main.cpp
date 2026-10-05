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

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, const TGAColor& color) {
    // Sort the vertices, a,b,c in ascending order of y coordinate
    if(ay > by) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    if(by > cy) {
        std::swap(bx, cx);
        std::swap(by, cy);
    }
    if(ay > by) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int total_height = cy - ay;
    // If the lower half is not degenerate
    if(ay != by) {
        int segment_height = by - ay;
        for(int y{ay}; y <= by; y++) {
            int xL = ax + ((y - ay) * (cx - ax)) / total_height;
            int xR = ax + ((y - ay) * (bx - ax)) / segment_height;
            // Draw a horizontal Line
            for(int x = std::min(xL, xR); x < std::max(xL, xR); x++) {
                framebuffer.set(x, y, color);
            }
        }
    }
    // If the upper half is not degenerate
    if(by != cy) {
        int segment_height = cy - by;
        for(int y{by}; y <= cy; y++) {
            int xL = ax + ((y - ay) * (cx - ax)) / total_height;
            int xR = bx + ((y - by) * (cx - bx)) / segment_height;
            // Draw a horizontal Line
            for(int x = std::min(xL, xR); x < std::max(xL, xR); x++) {
                framebuffer.set(x, y, color);
            }
        }
    }
}

int main() {
    TGAImage framebuffer{width, height, TGAImage::RGB};

    triangle(  7, 45, 35, 100, 45,  100, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    
    framebuffer.write_tga_file("./output/framebuffer.tga");

    return 0;
}