#include <tuple>

#include "vec.h"
#include "model.h"
#include "tgaimage.h"

constexpr int width  = 128;
constexpr int height = 128;

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

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &framebuffer, TGAColor color) {
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
            if(alpha > 0.1 && beta > 0.1 && gamma >0.1)continue;
            unsigned char b = 0, r = 0, g = 0;
            b = static_cast<unsigned char>(alpha * az);
            r = static_cast<unsigned char>(beta * bz);
            g = static_cast<unsigned char>(gamma * cz);
            framebuffer.set(x,y, {b, r, g});
        }
    }
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    int ax = 17, ay = 4, az = 255;
    int bx = 120, by = 110, bz= 255;
    int cx = 23, cy = 90, cz = 255;
    triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer, green);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
