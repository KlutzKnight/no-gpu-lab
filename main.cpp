#include "model.h"
#include "tgaimage.h"
#include <cmath>
#include <utility>

constexpr int width  = 800;
constexpr int height = 800;

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

std::tuple<int, int> project(vec3 v) {
    // (x, y) is an orthogonal projection of the vector (x,y,z)
    // The input models are scaled to fit in [-1, 1]^3
    // we shift and scale the vector to fit the entire screen
    return {
        (v.x + 1) * width  / 2,
        (v.y + 1) * height / 2
    };
}

void render(Model& model, TGAImage& framebuffer) {
    for(int i{}; i < model.numFaces(); i++) {
        auto [ax, ay] = project(model.vert(i, 0));
        auto [bx, by] = project(model.vert(i, 1));
        auto [cx, cy] = project(model.vert(i, 2));

        line(ax, ay, bx, by, framebuffer, red);
        line(bx, by, cx, cy, framebuffer, red);
        line(ax, ay, cx, cy, framebuffer, red);
    }
    for(int i{}; i < model.numVerts(); i++) {
        auto [ax, ay] = project(model.vert(i));
        framebuffer.set(ax, ay, white);
    }
}

int main() {
    TGAImage framebuffer{width, height, TGAImage::RGB};
    Model diabloPose{"./obj/diablo3_pose/diablo3_pose.obj"};

    render(diabloPose, framebuffer);
    framebuffer.write_tga_file("./output/framebuffer.tga");

    return 0;
}