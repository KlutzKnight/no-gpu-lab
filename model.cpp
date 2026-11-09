#include "model.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>

Model::Model(const std::string& path) {
    std::ifstream file{path};
    if(!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::string line;
    while(std::getline(file, line)) {
        // Skip empty lines and comments
        if(line.empty() || line[0] == '#') continue;

        std::istringstream str{line};
        char trash;
        std::string identifier;
        str >> identifier;

        if(identifier == "v") {
            double x, y, z;
            if(str >> x >> y >> z) {
                m_vertices.push_back({x, y, z});
            }
            else {
                throw std::runtime_error("Invalid vertex format in OBJ file: " + line);
            }
        } else if(identifier == "vt") {
            double x, y, z;
            if(str >> x >> y >> z) {
                m_vTextures.push_back({x, y, z});
            }
            else {
                throw std::runtime_error("Invalid vertex Texture format in OBJ file: " + line);
            }
        } else if(identifier == "vn") {
            double x, y, z;
            if(str >> x >> y >> z) {
                m_vNormals.push_back({x, y, z});
            }
            else {
                throw std::runtime_error("Invalid vertex normal format in OBJ file: " + line);
            }
        } else if(identifier == "f") {
            std::vector<int> vert, text, norm;
            vert.reserve(3);
            text.reserve(3);
            norm.reserve(3);
            int f, t, n, cnt{};
            while(str >> f >> trash >> t >> trash >> n) {
                vert.push_back(f - 1);
                text.push_back(t - 1);
                norm.push_back(n - 1);
                cnt++;
            }

            if(cnt != 3) {
                throw std::runtime_error("Error: the OBJ file is supposed to be triangulated.");
            }
            m_facetVert.push_back({vert[0], vert[1], vert[2]});
            m_facetText.push_back({text[0], text[1], text[2]});
            m_facetNorm.push_back({norm[0], norm[1], norm[2]});
        }
    }

    std::cerr << "# v#" << numVerts() << " f#" << numFaces() << "\n";
}

vec3 Model::vert(const int i) const {
    return m_vertices[i];
}
vec3 Model::vert(const int iface, const int nthvert) const {
    return m_vertices[m_facetVert[iface][nthvert]];
}
vec3 Model::vertNorm(const int iface, const int nthvert) const {
    return m_vNormals[m_facetNorm[iface][nthvert]];
}
vec3 Model::vertText(const int iface, const int nthvert) const {
    return m_vTextures[m_facetText[iface][nthvert]];
}

int Model::numVerts() const {
    return m_vertices.size();
}
int Model::numFaces() const {
    return m_facetVert.size();
}
