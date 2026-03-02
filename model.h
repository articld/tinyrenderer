#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include"vec.h"
#include"tgaimage.h"

class Model{
    private:
        std::vector<vec4> verts = {};
        std::vector<vec4> normal = {};
        std::vector<vec2> uv_coords = {};

        std::vector<int> faces_vrt = {};
        std::vector<int> faces_nrm = {};
        std::vector<int> faces_uv = {};

        TGAImage diffuse = {};
        TGAImage normalmap = {};

    public:
        Model(std::string model_path);
        inline vec4 get_vert(const int iface, const int n) const { return verts[faces_vrt[iface*3+n]]; }
        inline vec4 get_norm(const int iface, const int n) const { return normal[faces_nrm[iface*3+n]]; }
        inline vec2 get_uv(const int iface, const int n) const { return uv_coords[faces_uv[iface*3+n]]; }

        inline int get_nverts() const { return verts.size(); }
        inline int get_nface() const { return faces_vrt.size()/3; }

        vec4 get_norm_text(const vec2 uv) const;
        TGAColor get_diff_text(const vec2 uv) const;
};