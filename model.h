#pragma once
#include <vector>
#include "geometry.h"

struct Model {
    explicit Model(const std::string& path);
    int numVerts() const;
    int numFaces() const;
    vec3 vert(const int i) const;
    vec3 vert(const int iface, const int nthvert) const;
    vec3 vertNorm(const int iface, const int nthvert) const;
private:
    std::vector<vec3>  m_vertices;      // Array of vertices
    std::vector<vec3>  m_normals; // Array of vertex normals
    std::vector<vec3i> m_facetVert;     // Trangulated vertex coordinates
    std::vector<vec3i> m_facetNorm;     // Trangulated vertex normal coordinates
};
