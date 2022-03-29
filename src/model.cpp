#include "model.h"

#include <fstream>
#include <sstream>
#include <string>

#include "util.h"

Model::Model()
{}

Model::~Model()
{}

bool Model::Load(const char* filename)
{
    std::ifstream ifs{filename};
    if (!ifs.is_open()) {
        return false;
    }

    std::string line;
    while (!ifs.eof()) {
        std::getline(ifs, line);
        std::istringstream iss(line);
        char trash;
        if (line.compare(0, 2, "v ") == 0) {
            iss >> trash;
            Vec3f v;
            iss >> v.x;
            iss >> v.y;
            iss >> v.z;
            m_verts.push_back(v);

        } else if (line.compare(0, 2, "f ") == 0) {
            std::vector<uint32_t> f;
            int32_t itrash, index;
            iss >> trash;
            while (iss >> index >> trash >> itrash >> trash >> itrash) {
                --index;
                f.push_back(index);
            }
            m_faces.push_back(std::move(f));
        }
    }
    INFOF("# v# %zu f# %zu", m_verts.size(), m_faces.size());
    return true;
}
