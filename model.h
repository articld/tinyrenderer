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
        std::vector<vec3> verts_normal = {};
        std::vector<int> faces = {};
        std::vector<int> faces_normals = {};

    public:
        Model(std::string model_path);
        inline vec3 get_vert(const int n) const { return verts[n];}
        inline vec3 get_vert(const int iface, const int n) const { return verts[faces[iface*3+n]];}
        inline vec3 get_vert_norm(const int n) const { return verts_normal[n];}
        inline vec3 get_vert_norm(const int iface, const int n) const { return verts_normal[faces[iface*3+n]];}
        inline int get_nverts() const { return verts.size(); }
        inline int get_nface() const { return faces.size()/3; }

};