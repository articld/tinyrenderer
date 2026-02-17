#include <tuple>
#include <cmath>
#include <numbers>
#include <vector>

#include "vec.h"
#include "model.h"
#include "tgaimage.h"

constexpr int width  = 800;
constexpr int height = 800;

std::tuple<int,int, int> project(vec3 v) {
    return { (v.x + 1.) *  width/2,
             (v.y + 1.) * height/2, 
             (v.z + 1.) * 255./2 };
}

vec3 rotate(vec3 v){
    constexpr double alpha = std::numbers::pi / 6;
    mat<3, 3> Ry = {{{std::cos(alpha), 0, std::sin(alpha)}, {0,1,0}, {-std::sin(alpha), 0, std::cos(alpha)}}} ;
    return Ry * v;
}

vec3 perspective(vec3 v){
    constexpr double c = 3;
    return v / (1 - v.z/c);
}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy){
    return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
}

void rasterize(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, std::vector<double>& zbuffer,
              TGAImage&framebuffer, TGAColor color) {

    int bbminx = std::min(std::min(ax,bx), cx);
    int bbminy = std::min(std::min(ay,by), cy);
    int bbmaxx = std::max(std::max(ax,bx), cx);
    int bbmaxy = std::max(std::max(ay,by), cy);

    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if(total_area <1 )return; 

    #pragma omp parallel for
    for(int x=bbminx; x<=bbmaxx; x++){
        for(int y=bbminy; y<=bbmaxy; y++){
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha <0 || beta <0 || gamma <0) continue;
            double z =alpha * az + beta * bz + gamma*cz;
            if (z <= zbuffer[x+y * framebuffer.width()]) continue;
            zbuffer[x+y * framebuffer.width()] = z;
            framebuffer.set(x,y, color);
        }
    }
}
/*
void write_zbuffer_img(std::vector<double>& zbuffer, TGAImage& zbuffer_img){
    double max = 0;
    int y = 0;

    for(auto i = zbuffer.begin(); i < zbuffer.end(); i++) max = *i > max ? *i : max;
    for(uint32_t i = 0; i < zbuffer.size(); i++){
        unsigned char z = static_cast<unsigned char>((zbuffer[i] / max) * 255);
        int x = i % width;
        if(i != 0 && x == 0) y++;
        zbuffer_img.set(x,y, {z});
    }
}
*/

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::vector<double> zbuffer(width * height);
    TGAImage zbuffer_img(width, height, TGAImage::GRAYSCALE);


    for (int i=0; i<model.get_nface(); i++) {
        auto [ax, ay, az] = project(perspective(rotate(model.get_vert(i, 0))));
        auto [bx, by, bz] = project(perspective(rotate(model.get_vert(i, 1))));
        auto [cx, cy, cz] = project(perspective(rotate(model.get_vert(i, 2))));
        TGAColor random;
        for(int c :{0, 1, 2}) random[c] = std::rand() % 255;
        rasterize(ax, ay, az, bx, by, bz, cx, cy, cz, zbuffer, framebuffer, random);
    }

    //write_zbuffer_img(zbuffer, zbuffer_img);

    //zbuffer_img.write_tga_file("zbuffer.tga");
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}