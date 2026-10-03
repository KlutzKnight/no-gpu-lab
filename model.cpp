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
    std::string identifier;
    while(std::getline(file, line)) {
        std::istringstream str{line};
        std::vector<std::string> tokens;
        // Split the line by whitespace;
        while(str >> identifier) {
            tokens.push_back(identifier);
        }
        if(tokens.empty()) continue;
        if(tokens[0] == "v") {
            vertices.push_back({
                std::stod(tokens[1]),
                std::stod(tokens[2]),
                std::stod(tokens[3])
            });
        }
        else if(tokens[0] == "f") {
            // Extract the first number after each space
            for(size_t i{1}; i < tokens.size(); i++) {
                size_t pos = 0;
                if((pos = tokens[i].find('/')) != std::string::npos) {
                    tokens[i] = tokens[i].substr(0, pos);
                }
            }
            faces.push_back({
                std::stoi(tokens[1]) - 1,
                std::stoi(tokens[2]) - 1,
                std::stoi(tokens[3]) - 1
            });
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