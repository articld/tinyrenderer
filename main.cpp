#include <cmath>
#include <cstdlib>
#include <ctime>

#include "tgaimage.h"
#include "model.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void draw_line_2d(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color){
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep){
        std::swap(ax,ay);
        std::swap(bx,by);
    }
    if(ax>bx){
        std::swap(ax,bx);
        std::swap(ay,by);
    }
    int y = ay;
    int ierror = 0;
    for(int x=ax; x<=bx; x++){
        if(steep)
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);

        ierror += 2* std::abs(by-ay)/static_cast<float>(bx-ax);
        if (ierror> ax-bx){
            y += by-ay ? 1: -1;
            ierror -= 2 *(bx-ax);
        }
    }

}

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Model model("./obj/diablo3_pose/diablo3_pose.obj");
    model.get_file();

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

