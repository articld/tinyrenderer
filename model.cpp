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
            vec4 v = {0,0,0,1};
            for (int i : {0,1,2}) iss >> v[i];
            verts.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            int f,t,n, count = 0;
            iss >> trash;
            while (iss >> f >> trash >> t >> trash >> n) {
                faces_vrt.push_back(--f);
                faces_nrm.push_back(--n);
                faces_uv.push_back(--t);
                count++;
            }
            if (count!=3) {
                std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                return;
            }
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            vec4 vn;
            for (int i: {0,1,2}) iss >> vn[i];
            normal.push_back(normalized(vn));
        } else if(!line.compare(0, 3, "vt ")){
            iss >> trash >> trash;
            vec2 uv;
            for (int i:{0, 1}) iss >> uv[i];
            uv_coords.push_back({uv.x, 1. - uv.y});
        }
    }
    std::cerr << "# v# " << get_nverts() << " f# "  << get_nface() << std::endl;

    auto load_texture = [&model_path](const std::string suffix, TGAImage& img){
        size_t dot = model_path.find_last_of(".");
        if(dot==std::string::npos) return;
        std::string texfile = model_path.substr(0,dot) + suffix;
        std::cerr<<"texture file "<< texfile << " loading"<<(img.read_tga_file(texfile.c_str()) ? "ok":"failed")<<std::endl;
    };
    load_texture("_nm_tangent.tga", normalmap);
    load_texture("_diffuse.tga", diffuse);
    load_texture("_spec.tga", specularmap);

}

TGAColor Model::get_diff_text(const vec2 uv) const{
    TGAColor c = diffuse.get(uv[0]*diffuse.width(), uv[1]*diffuse.height());
    return c;
}

TGAColor Model::get_spec_text(const vec2 uv) const{
    TGAColor c = specularmap.get(uv[0]*specularmap.width(), uv[1]*specularmap.height());
    return c;
}

vec4 Model::get_norm_text(const vec2 uv) const{
    TGAColor c = normalmap.get(uv[0]*normalmap.width(), uv[1]*normalmap.height());
    return normalized(vec4{(double)c[2], (double)c[1], (double)c[0], 0} *2. /255. - vec4{1,1,1,0});
}