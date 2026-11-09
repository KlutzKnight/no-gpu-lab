#include "geometry.h"
#include "gl.h"
#include "model.h"
#include <algorithm>
#include <cmath>
#include <iostream>

extern mat<4,4> Viewport, Perspective, ModelView;

struct PhongShader : IShader {
    const Model model;
    vec4 light;     // light position in eye coordinates
    vec4 eye;       // camera position in eye coordinates
    Triangle tri;   // Triangle in eye coordinates
    vec2 text[3];   // Triangle vertex textures in UV coordinates

    PhongShader(const vec3& l, const vec3& cam, const Model& m)
    : model{m}
    {
        light = ModelView * vec4{l.x, l.y, l.z, 1};
        eye = ModelView * vec4{cam.x, cam.y, cam.z, 1};
    }
    vec4 vertex(const int face, const int vertex) {
        // Transform vertex from object coordinates to eye coordinates
        tri[vertex] = ModelView * model.vert(face, vertex);
        text[vertex] = model.uv(face, vertex);    // Scale the uv coordinates
        vec4 vert = Perspective * tri[vertex];
        return Viewport * vec4 {vert.x / vert.w, vert.y / vert.w, vert.z / vert.w, 1};  // Return Screen Coordinates
    }

    std::pair<bool, TGAColor> fragment(const vec3& bar) const override {
        // Contribution of each component
        constexpr double ambientCoeff{0.5};
        constexpr double diffuseCoeff{0.75};
        constexpr double specularCoeff{0.4};
        constexpr double alpha{35};  // shininess constant
        constexpr double ia{0.8};    // light intensity

        // Interpolate uv coordinates
        vec2 uv {(bar[0] * text[0] + bar[1] * text[1] + bar[2] * text[2])};

        TGAColor glFragColor = model.diffuse(uv);
        TGAColor specClr = model.specular(uv);

        vec3 n {normalize(ModelView.inverse().transpose() * model.normal(uv)).xyz()};   // Normal vector of point
        vec3 l {normalize(light.xyz())};        // Vector towards the light source
        vec3 v {normalize(eye.xyz())};          // Vector towards the camera
        double cosA = dot(n, l);
        vec3 r {normalize(2 * n * cosA - l)};   // Reflected vector
        double cosB = dot(r, v);

        double Ia{ia * ambientCoeff};
        double Id{diffuseCoeff * std::max(0.0, cosA)};
        double Is{specularCoeff * std::pow(std::max(0.0, cosB), alpha)};

        for(int channel : {0, 1, 2}) {
            glFragColor[channel] = std::min(
                255.0,
                glFragColor[channel] * (Ia + Id) + specClr[channel] * Is
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

    constexpr int width  = 800;     // output image size
    constexpr int height = 800;
    constexpr vec3    eye{-1,0,2};  // camera position
    constexpr vec3 center{0,0,0};   // camera direction
    constexpr vec3     up{0,1,0};   // camera up vector
    constexpr vec3  light{1,1,1};   // light source position

    lookAt(eye, center, up);
    initPerspective(norm(eye - center));
    initViewport(width / 16, height / 16, width * 7/8, height * 7/8);
    initZBuffer(width, height);

    TGAImage frameBuffer{width, height, TGAImage::RGB};
    TGAImage textureImage{};
    // Iterate through all input objects
    for(int i{1}; i < argc; i++) {
        Model model{argv[i]};
        PhongShader shader(light, eye, model);
        render(model, shader, frameBuffer);
    }
    frameBuffer.write_tga_file("./output/framebuffer.tga");

    return 0;
}
