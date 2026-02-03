#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include"vec.h"

class Model{
    private:
        std::vector<vec3> verts = {};
        std::vector<int> faces = {};

    public:
        Model(std::string model_path);
        inline vec3 get_vert(const int n) const { return verts[n];}
        inline vec3 get_vert(const int iface, const int n) const { return verts[faces[iface*3+n]];}
        inline int get_nverts() const { return verts.size(); }
        inline int get_nface() const { return faces.size()/3; }

};