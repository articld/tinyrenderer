#include <cmath>
#include "tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void draw_line_2d(const int ax, const int ay, const int bx, const int by, TGAImage &framebuffer, TGAColor color){
    for(float t=0; t <= 1; t+=0.02){
        int x = std::round(ax + t*(bx-ax));
        int y = std::round(ay + t*(by-ay));
        framebuffer.set(x, y, color);
    }

}

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    draw_line_2d(ax, ay, bx, by, framebuffer, yellow);
    draw_line_2d(bx, by, cx, cy, framebuffer, blue);
    draw_line_2d(cx, cy, ax, ay, framebuffer, green);
    draw_line_2d(ax, ay, cx, cy, framebuffer, red);

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

