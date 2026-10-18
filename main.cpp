#include "model.h"
#include "tgaimage.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <tuple>
#include <utility>

constexpr int width  = 800;
constexpr int height = 800;

// ATTENTION! BGRA order
constexpr TGAColor white  = {255, 255, 255, 255};
constexpr TGAColor blue   = {255, 120,  64, 255};
constexpr TGAColor green  = {  0, 255,   0, 255};
constexpr TGAColor red    = {  0,   0, 255, 255};
constexpr TGAColor yellow = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage& frameBuffer, const TGAColor& color) {
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
            frameBuffer.set(y, x, color);
        }
        else {
            frameBuffer.set(x, y, color);
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

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage& zBuffer, TGAImage& frameBuffer, TGAColor& color) {
    // Calculate the bounding box of the triangle
    int bottomLeftX = std::min({ax, bx, cx});
    int bottomLeftY = std::min({ay, by, cy});
    int topRightX = std::max({ax, bx, cx});
    int topRightY = std::max({ay, by, cy});

    double totalArea = signedTriangleArea(ax, ay, bx, by, cx, cy);
    // Conservative Back-face culling and discarding triangles that cover less than a pixel
    if(totalArea < 1) return;

#pragma omp parallel for
    for(int y = bottomLeftY; y < topRightY; y++) {
        for(int x = bottomLeftX; x < topRightX; x++) {
            double alpha = signedTriangleArea(x, y, ax, ay, bx, by) / totalArea;
            double beta = signedTriangleArea(x, y, bx, by, cx, cy) / totalArea;
            double gamma = signedTriangleArea(x, y, cx, cy, ax, ay) / totalArea;
            // Barycentric Coordinates being negative implies the point is outside the triangle
            if(alpha < 0 || beta < 0 || gamma < 0) continue;

            unsigned char z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            if(z <= zBuffer.get(x, y)[0]) continue;
            zBuffer.set(x, y, {z});
            frameBuffer.set(x, y, color);
        }
    }
}

std::tuple<int, int, int> project(vec3 v) {
    return {
        (v[0] + 1) * 0.5 * width,
        (v[1] + 1) * 0.5 * height,
        (v[2] + 1) * 0.5 * 255
    };
}
unsigned char randBit() {
    return static_cast<unsigned char>(
        rand()%255
    );
}

void render(Model& model, TGAImage& zBuffer, TGAImage& frameBuffer) {
    for(int i{}; i < model.numFaces(); i++) {
        auto [ax, ay, az] = project(model.vert(i, 0));
        auto [bx, by, bz] = project(model.vert(i, 1));
        auto [cx, cy, cz] = project(model.vert(i, 2));

        TGAColor color = {
            randBit(), randBit(), randBit()
        };
        
        triangle(ax, ay, az, bx, by, bz, cx, cy, cz, zBuffer, frameBuffer, color);
    }
}

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << "obj/model.obj\n";
        return -1;
    }
    Model model{argv[1]};
    TGAImage frameBuffer{width, height, TGAImage::RGB};
    TGAImage     zBuffer{width, height, TGAImage::GRAYSCALE};

    render(model, zBuffer, frameBuffer);
    frameBuffer.write_tga_file("./output/framebuffer.tga");
    zBuffer.write_tga_file("./output/zbuffer.tga");

    return 0;
}