#pragma once
#include <vector>
#include "geometry.h"
#include "tgaimage.h"

struct Model {
    explicit Model(const std::string& path);
    int numVerts() const;
    int numFaces() const;
    vec4 vert(const int i) const;
    vec4 vert(const int iface, const int nthvert) const;
    vec4 vertNorm(const int iface, const int nthvert) const;
    vec2 uv(const int iface, const int nthvert) const;
    vec3 normal(const vec2& uv) const ;
private:
    TGAImage m_normalMap{};

    std::vector<vec4>  m_vertices;      // Array of vertices
    std::vector<vec4>  m_vNormals;      // Array of vertex normals
    std::vector<vec2>  m_vTextures;     // Array of vertex textures coordinates

    std::vector<vec3i> m_facetVert;     // Trangulated vertex coordinates
    std::vector<vec3i> m_facetNorm;     // Trangulated vertex normal coordinates
    std::vector<vec3i> m_facetText;     // Trangulated vertex texture coordinates

};
