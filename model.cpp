#include "model.h"
#include <fstream>
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
        std::string identifier;
        str >> identifier;
        
        if(identifier == "v") {
            double x, y, z;
            if(str >> x >> y >> z) {
                vertices.push_back({x, y, z});
            }
            else {
                throw std::runtime_error("Invalid vertex format in OBJ file: " + line);
            }
        }
        else if(identifier == "f") {
            std::vector<int> face_indices;
            std::string token;

            while(str >> token) {
                // For now we only care about the vertex index before the first '/'
                auto pos = token.find('/');
                if(pos != std::string::npos) {
                    token.resize(pos);
                }

                try {
                    // Convert from 1-based to 0-based index
                    face_indices.push_back(std::stoi(token) - 1);
                }
                catch(const std::exception&) {
                    throw std::runtime_error("Invalid face index in OBJ file: " + token);
                }
            }

            if(face_indices.size() != 3) {
                throw std::runtime_error(
                    "Error: OBJ is supposed to be triangulated (found face with " +
                    std::to_string(face_indices.size()) +
                    "vertices).\n");
            }

            faces.push_back({face_indices[0], face_indices[1], face_indices[2]});
        }
    }
}

vec3 Model::vert(const int i) const {
    return vertices[i];
}
vec3 Model::vert(const int iface, const int nthvert) const {
    return vertices[faces[iface][nthvert]];
}

int Model::numVerts() const {
    return vertices.size();
}
int Model::numFaces() const {
    return faces.size();
}