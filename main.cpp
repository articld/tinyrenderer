#include <tuple>

#include "vec.h"
#include "model.h"
#include "tgaimage.h"

constexpr int width  = 800;
constexpr int height = 800;

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void draw_line2d(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax>bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for (int x=ax; x<=bx; x++) {
        if (steep)
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by-ay);
        if (ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx-ax);
        }
    }
}

std::tuple<int,int> project(vec3 v) {
    return { (v.x + 1.) *  width/2,
             (v.y + 1.) * height/2 };
}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy){
    return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    int bbminx = std::min(std::min(ax,bx), cx);
    int bbminy = std::min(std::min(ay,by), cy);
    int bbmaxx = std::max(std::max(ax,bx), cx);
    int bbmaxy = std::max(std::max(ay,by), cy);

    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);

    #pragma omp parallel for
    for(int x=bbminx; x<=bbmaxx; x++){
        for(int y=bbminy; y<=bbmaxy; y++){
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha <0 || beta <0 || gamma <0) continue;
            framebuffer.set(x,y, color);
        }
    }
}


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int i=0; i<model.get_nface(); i++) {
        auto [ax, ay] = project(model.get_vert(i, 0));
        auto [bx, by] = project(model.get_vert(i, 1));
        auto [cx, cy] = project(model.get_vert(i, 2));
        draw_line2d(ax, ay, bx, by, framebuffer, red);
        draw_line2d(bx, by, cx, cy, framebuffer, red);
        draw_line2d(cx, cy, ax, ay, framebuffer, red);
    }

    for (int i=0; i<model.get_nverts(); i++) {
        vec3 v = model.get_vert(i);
        auto [x, y] = project(v);
        framebuffer.set(x, y, white);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}