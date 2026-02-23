#include <cstdlib>
#include "gl.h"
#include "model.h"

extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;     // the depth buffer

struct RandomShader : IShader {
    const Model &model;
    TGAColor color = {};
    vec3 tri[3];  // triangle in eye coordinates

    RandomShader(const Model &m) : model(m) {
    }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.get_vert(face, vert);                          // current vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();                            // in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        return {false, color};                                    // do not discard the pixel
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width  = 800;
    constexpr int height = 800;
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
        RandomShader shader(model);
        for (int f=0; f<model.get_nface(); f++) {
            shader.color = { std::rand()%255, std::rand()%255, std::rand()%255, 255 };
            Triangle clip = { shader.vertex(f, 0),
                              shader.vertex(f, 1),
                              shader.vertex(f, 2) };
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}