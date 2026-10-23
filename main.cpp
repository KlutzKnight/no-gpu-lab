#include "geometry.h"
#include "gl.h"
#include "model.h"
#include <iostream>


struct RandomShader : IShader {
    const Model model;
    TGAColor color;

    RandomShader(const Model& m)
    : model{m}
    {
    }
    virtual vec3 vertex(const int face, const int vertex) {
        // Transform vertex from object coordinates to screen coordinates
        return transform(model.vert(face, vertex));
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3&) const {
        return {false, color};
    }
};

unsigned char randBit() {
    return static_cast<unsigned char>(
        rand()%255
    );
}

void render(Model& model, RandomShader& shader, TGAImage& frameBuffer) {
    // Iterate through all facets
    for(int i{}; i < model.numFaces(); i++) {
        shader.color = {{randBit(), randBit(), randBit()}};
        Triangle screen = {         // Assemble the primitive
            shader.vertex(i, 0),
            shader.vertex(i, 1),
            shader.vertex(i, 2)
        };

        rasterize(screen, shader, frameBuffer);
    }
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << "obj/model.obj\n";
        return 1;
    }

    constexpr int width  = 800;     // output image size
    constexpr int height = 800;
    constexpr vec3    eye{-1,0,2};  // camera position
    constexpr vec3 center{0,0,0};   // camera direction
    constexpr vec3     up{0,1,0};   // camera up vector

    lookAt(eye, center, up);
    initPerspective(norm(eye - center));
    initViewport(width / 16, height / 16, width * 7/8, height * 7/8);
    initZBuffer(width, height);

    TGAImage frameBuffer{width, height, TGAImage::RGB};
    // Iterate through all input objects
    for(int i{1}; i < argc; i++) {
        Model model{argv[i]};
        RandomShader shader(model);
        render(model, shader, frameBuffer);
    }
    frameBuffer.write_tga_file("./output/framebuffer.tga");

    return 0;
}