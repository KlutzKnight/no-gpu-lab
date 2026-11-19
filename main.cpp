#include "geometry.h"
#include "gl.h"
#include "model.h"
#include "vec.h"
#include <algorithm>
#include <cmath>
#include <iostream>

extern mat<4,4> Viewport, Perspective, ModelView;

struct PhongShader : IShader {
    const Model model;
    vec3 l;         // light direction in eye coordinates
    Triangle tri;   // Triangle in eye coordinates
    vec2 text[3];   // Triangle vertex textures in UV coordinates
    double w[3];    // Clip space w component

    PhongShader(const vec3& light, const Model& m)
    : model{m}
    {
        l = normalize((ModelView * vec4{light.x, light.y, light.z, 0}).xyz());
    }
    vec4 vertex(const int face, const int vertex) {
        // Transform vertex from object coordinates to eye coordinates
        tri[vertex] = ModelView * model.vert(face, vertex);
        text[vertex] = model.uv(face, vertex);    // Get the uv coordinates of the fragment
        vec4 vert = Perspective * tri[vertex];
        w[vertex] = vert.w;
        return Viewport * vec4 {vert.x / vert.w, vert.y / vert.w, vert.z / vert.w, 1};  // Return Screen Coordinates
    }

    std::pair<bool, TGAColor> fragment(const vec3& bar) const override {
        // Interpolate uv coordinates
        vec2 uv = (bar[0] * text[0] / w[0] + bar[1] * text[1] / w[1] + bar[2] * text[2] / w[2]) / (bar[0] / w[0] + bar[1] / w[1] + bar[2] / w[2]);

        vec3 n {normalize((ModelView.inverse().transpose() * model.normal(uv)).xyz())};   // Normal vector of the point
        double cosA = dot(n, l);
        vec3 r {normalize(2 * cosA * n - l)};       // Reflected vector

        constexpr double ambient{0.4};
        double diffuse{0.8 * std::max(0.0, cosA)};
        double spec[3]{};
        for(int channel : {0, 1, 2}) {
            spec[channel] = 3 * model.specular(uv)[0]/255.0 * std::pow(std::max(0.0, r.z), 35);
        }

        TGAColor glFragColor = model.diffuse(uv);
        for(int channel : {0, 1, 2}) {
            glFragColor[channel] = std::min(
                255.0,
                glFragColor[channel] * (ambient + diffuse + spec[channel])
            );
        }
        return {false, glFragColor};
    }
};

void render(Model& model, PhongShader& shader, TGAImage& frameBuffer) {
    // Iterate through all facets
    for(int i{}; i < model.numFaces(); i++) {
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

    constexpr int  width = 800;     // output image size
    constexpr int height = 800;
    constexpr vec3    eye{-1,0,2};  // camera position
    constexpr vec3 center{0,0,0};   // camera direction
    constexpr vec3     up{0,1,0};   // camera up vector
    constexpr vec3  light{1,1,1};   // light direction vector

    lookAt(eye, center, up);
    initPerspective(norm(eye - center));
    initViewport(width / 16, height / 16, width * 7/8, height * 7/8);
    initZBuffer(width, height);

    TGAImage frameBuffer{width, height, TGAImage::RGB};
    TGAImage textureImage{};
    // Iterate through all input objects
    for(int i{1}; i < argc; i++) {
        Model model{argv[i]};
        PhongShader shader(light, model);
        render(model, shader, frameBuffer);
    }
    frameBuffer.write_tga_file("./output/framebuffer.tga");

    return 0;
}
