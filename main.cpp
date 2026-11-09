#include "geometry.h"
#include "gl.h"
#include "model.h"
#include <algorithm>
#include <cmath>
#include <iostream>

extern mat<4,4> Viewport, Perspective, ModelView;

// Convert from object to view coordiantes
inline vec3 view(const vec3& v) {
    vec4 eye = ModelView * vec4{v.x, v.y, v.z, 1};
    return eye.xyz();
}
// Scale BGRA texture to [-1, 1]
inline vec3 scale(const TGAColor& mapped) {
    return {
        mapped[2] * 2.0 / 255.0 - 1,
        mapped[1] * 2.0 / 255.0 - 1,
        mapped[0] * 2.0 / 255.0 - 1
    };
}
// Scale uv vector from [0, 1] to view coordinates and reverse the y coordinate
inline vec3 scale(const vec3& uv, const TGAImage& buf) {
    return vec3{uv.x * buf.width(), (1 - uv.y) * buf.height(), uv.z};
}

struct PhongShader : IShader {
    const Model model;
    const TGAImage textureImage;
    vec3 light;     // light position in eye coordinates
    vec3 eye;       // camera position in eye coordinates
    Triangle tri;   // Triangle in eye coordinates
    Triangle text;  // Triangle vertex textures in UV coordinates
    Triangle norm;  // Triangle vertex normals in eye coordinates

    PhongShader(const vec3& l, const vec3& cam, const Model& m, const TGAImage& t)
    : model{m},
      textureImage{t}
    {
        light = view(l);
        eye = view(cam);
    }
    vec3 vertex(const int face, const int vertex) {
        // Transform vertex from object coordinates to eye coordinates
        tri[vertex] = view(model.vert(face, vertex));
        text[vertex] = scale(model.vertText(face, vertex), textureImage);    // Scale the uv coordinates
        vec3 n = model.vertNorm(face, vertex);
        norm[vertex] = (ModelView.inverse().transpose() * vec4 {n.x, n.y, n.z, 1}).xyz();
        return transform(model.vert(face, vertex)); // Return Screen Coordinates
    }

    std::pair<bool, TGAColor> fragment(const vec3& bar) const override {
        // Contribution of each component
        constexpr double ambientCoeff{0.3};
        constexpr double diffuseCoeff{0.75};
        constexpr double specularCoeff{0.4};
        constexpr double alpha{35};  // shininess constant
        constexpr double i{0.8};     // light intensity

        TGAColor glFragColor{{255, 255, 255, 255}};
        // Interpolate uv coordinates
        vec3 u {(bar[0] * text[0] + bar[1] * text[1] + bar[2] * text[2])};
        vec3 n {normalize(scale(textureImage.get(std::round(u.x), std::round(u.y))))};      // Normal vector of point
        vec3 l {normalize(light)};              // Vector towards the light source
        vec3 v {normalize(eye)};                // Vector towards the camera
        double cosA = dot(n, l);
        vec3 r {normalize(2 * n * cosA - l)};   // Reflected vector
        double cosB = dot(r, v);

        double Ia{i * ambientCoeff};
        double Id{i * diffuseCoeff * std::max(0.0, cosA)};
        double Is{i * specularCoeff * std::pow(std::max(0.0, cosB), alpha)};
        double illumination = Ia + Id + Is;

        for(int channel : {0, 1, 2}) {
            glFragColor[channel] *= std::min(1.0, illumination);
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

    TGAImage frameBuffer{width, height, TGAImage::GRAYSCALE};
    TGAImage textureImage{};
    // Iterate through all input objects
    for(int i{1}; i < argc; i += 2) {
        Model model{argv[i]};
        textureImage.read_tga_file(argv[i+1]);
        PhongShader shader(light, eye, model, textureImage);
        render(model, shader, frameBuffer);
    }
    frameBuffer.write_tga_file("./output/framebuffer.tga");

    return 0;
}
