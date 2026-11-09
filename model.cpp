#include "model.h"
#include "tgaimage.h"
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
                m_vertices.push_back({x, y, z, 1});
            }
            else {
                throw std::runtime_error("Invalid vertex format in OBJ file: " + line);
            }
        } else if(identifier == "vt") {
            double x, y, z;
            if(str >> x >> y >> z) {
                m_vTextures.push_back({x, 1 - y});
            }
            else {
                throw std::runtime_error("Invalid vertex Texture format in OBJ file: " + line);
            }
        } else if(identifier == "vn") {
            double x, y, z;
            if(str >> x >> y >> z) {
                m_vNormals.push_back({x, y, z, 1});
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

    auto load_texture = [&path](const std::string& suffix, TGAImage& img) {
        auto pos = path.find_last_of('.');
        if(pos == std::string::npos) return;
        std::string nmPath = path.substr(0, pos) + suffix;
        std::cerr << "texture file " << nmPath << " loading " << (img.read_tga_file(nmPath) ? "ok." : "failed.") << "\n";
    };

    load_texture("_nm.tga", m_normalMap);
    std::cerr << "# v#" << numVerts() << " f#" << numFaces() << "\n";
}

vec4 Model::vert(const int i) const {
    return m_vertices[i];
}
vec4 Model::vert(const int iface, const int nthvert) const {
    return m_vertices[m_facetVert[iface][nthvert]];
}
vec4 Model::vertNorm(const int iface, const int nthvert) const {
    return m_vNormals[m_facetNorm[iface][nthvert]];
}
vec2 Model::uv(const int iface, const int nthvert) const {
    return m_vTextures[m_facetText[iface][nthvert]];
}
vec3 Model::normal(const vec2& uv) const {
    TGAColor text = m_normalMap.get(uv.x * m_normalMap.width(), uv.y * m_normalMap.height());
    return vec3{
        text[2] * 2.0 / 255.0 - 1,
        text[1] * 2.0 / 255.0 - 1,
        text[0] * 2.0 / 255.0 - 1
    };
}

int Model::numVerts() const {
    return m_vertices.size();
}
int Model::numFaces() const {
    return m_facetVert.size();
}
