#include "tgaimage.h"
#include <cmath>
#include <ctime>
#include <utility>

// ATTENTION! BGRA order
constexpr TGAColor white  = {255, 255, 255, 255};
constexpr TGAColor blue   = {255, 120,  64, 255};
constexpr TGAColor green  = {  0, 255,   0, 255};
constexpr TGAColor red    = {  0,   0, 255, 255};
constexpr TGAColor yellow = {  0, 200, 255, 255};

void drawLine(int ax, int ay, int bx, int by, TGAImage& framebuffer, const TGAColor& color) {
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

int main() {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer{width, height, TGAImage::RGB};

    std::srand(std::time({}));
    auto rand_int = []{ return static_cast<uint8_t>(rand()%255); };
    for (int i=0; i<(1<<24); i++) {
        int ax = rand()%width, ay = rand()%height;
        int bx = rand()%width, by = rand()%height;
        drawLine(ax, ay, bx, by, framebuffer, { rand_int(), rand_int(), rand_int(), rand_int() });
    }

    framebuffer.write_tga_file("./output/framebuffer.tga");

    return 0;
}