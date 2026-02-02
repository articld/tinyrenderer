#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include"vec.h"

class Model{
    private:
        std::vector<vec3> verts;
        std::vector<int> faces;

    public:
        Model(std::string model_path);
        inline const vec3 get_vert(int n){ return verts[n];}
        inline const int get_nverts(){ return verts.size(); }
        inline const int get_face(int n){ return faces[n]; }
        inline const int get_nface(){ return faces.size()/3; }

};