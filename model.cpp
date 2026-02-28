#include "model.h"

Model::Model(const std::string model_path) {
    std::ifstream in;
    in.open(model_path, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            vec3 v;
            for (int i : {0,1,2}) iss >> v[i];
            verts.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            int f,t,n, count = 0;
            iss >> trash;
            while (iss >> f >> trash >> t >> trash >> n) {
                faces.push_back(--f);
                faces_normals.push_back(--n);
                count++;
            }
            if (count!=3) {
                std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                return;
            }
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            vec3 vn;
            for (int i: {0,1,2}) iss >> vn[i];
            normal.push_back(normalized(vn));
        }
    }
}