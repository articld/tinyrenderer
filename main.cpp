#include <cstdlib>
#include "gl.h"
#include "model.h"

extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;     // the depth buffer

struct PhongShader : IShader{
    const Model &model;
    vec4 l;
    vec2 varying_uv[3];
    vec4 tri[3];
    vec4 varying_norm[3];

    PhongShader(const vec3 light, const Model &m): model(m){
        l = normalized((ModelView*vec4{light.x, light.y, light.z, 0.}));
    }

    virtual vec4 vertex(const int face, const int vert){
        varying_uv[vert] = model.get_uv(face,vert);

        vec4 n = model.get_norm(face, vert);
        varying_norm[vert] = ModelView.invert_transpose() * n;

        vec4 gl_position = ModelView * model.get_vert(face, vert);
        tri[vert] = gl_position;
        return Perspective * gl_position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const{
        mat<2,4> E = {tri[1] - tri[0], tri[2] - tri[0]};
        mat<2,2> U = {varying_uv[1] - varying_uv[0], varying_uv[2] - varying_uv[0]};
        mat<2,4> T = U.invert() * E;
        mat<4,4> D = {normalized(T[0]), //vettore tangente
                      normalized(T[1]), //vettore bitangente
                      normalized(varying_norm[0]*bar[0] + varying_norm[1]*bar[1] + varying_norm[2]*bar[2]), //n interpolato
                      {0,0,0,1}};

        vec2 uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
        TGAColor gl_FragColor = model.get_diff_text(uv);
        TGAColor gl_SpecColor = model.get_spec_text(uv);

        vec4 n = normalized(D.invert_transpose() * model.get_norm_text(uv));
        vec4 r = normalized(n * (n * l)*2 - l); // reflected light direction

        double ambient = .3; // ambient light intensity
        double diff = std::max(0., n * l); // diffuse light intensity
        double spec = (3. * gl_SpecColor[0] / 255.) * std::pow(std::max(r.z, 0.), 35); // specular intensity, note that the camera lies on the z-axis (in eye coordinates), therefore simple r.z, since (0,0,1)*(r.x, r.y, r.z) = r.z
        for (int channel : {0,1,2})
            gl_FragColor[channel] *= std::min(1., ambient + .4*diff + .9*spec);
        return {false, gl_FragColor}; // do not discard the pixel
    }
};

/*
struct PhongShader : IShader {
    const Model &model;
    vec3 tri[3];  // triangle in eye coordinates
    vec4 l;
    vec3 varying_norm[3];

    PhongShader(const vec3 light, const Model &m) : model(m) {
        l = normalized((ModelView * vec4{light.x, light.y, light.z , 0.}).xyz());
    }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.get_vert(face, vert);                          // current vertex in object coordinates
        vec3 n = model.get_norm(face, vert);
        varying_norm[vert] = (ModelView.invert_transpose() * vec4{n.x, n.y, n.z, 0.}).xyz();
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();                            // in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        TGAColor color = {255, 255, 255, 255};
        //vec3 n = normalized(cross(tri[1]-tri[0], tri[2]-tri[0]));
        vec3 n = normalized(varying_norm[0] * bar[0] +
                            varying_norm[1] * bar[1] +
                            varying_norm[2] * bar[2]);
        vec3 r = normalized(2* n * (n*l) - l);
        double ambient = 0.3;
        double diffuse = std::max(0. , n * l);
        double specular = std::pow(std::max(r.z ,0.), 35);
        for(int channel: {0,1,2})
            color[channel] *= std::min(1., ambient + .4* diffuse + .9*specular);

        return {false, color};                                    // do not discard the pixel
    }
};
*/

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width  = 6400;
    constexpr int height = 6400;
    constexpr vec3  light{ 1, 1, 1};
    constexpr vec3    eye{-1, 0, 2};
    constexpr vec3 center{ 0, 0, 0};
    constexpr vec3     up{ 0, 1, 0};

    lookat(eye, center, up);
    init_perspective(norm(eye-center));
    init_viewport(width/16, height/16, width*7/8, height*7/8);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    for (int m=1; m<argc; m++) {
        Model model(argv[m]);
        PhongShader shader(light, model);
        for (int f=0; f<model.get_nface(); f++) {
            Triangle clip = { shader.vertex(f, 0),
                              shader.vertex(f, 1),
                              shader.vertex(f, 2) };
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}