#pragma once
#include <vector>
#include "geometry.h"

struct Model {
    explicit Model(const std::string& path);
    int numVerts() const;
    int numFaces() const;
    vec3 vert(const int i) const;
    vec3 vert(const int iface, const int nthvert) const;
private:
    std::vector<vec3>  vertices; // Array of vertices
    std::vector<vec3i> faces;    // Triangle index in the above array
};

