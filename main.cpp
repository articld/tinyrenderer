#include <tuple>
#include <algorithm>

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

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    vec2 vertices[3] = {{ax, ay}, {bx, by}, {cx, cy}};
    int n = sizeof(vertices)/sizeof(vertices[0]);

    std::sort(vertices, vertices + n, [](const vec2& a, const vec2&b){
        return a.y<b.y; //funzione lambda, creo una funzione senza nome, con parametri a,b, restitiusice booleano per sort
    });
    
    draw_line2d(ax, ay, bx, by, framebuffer, color);
    draw_line2d(bx, by, cx, cy, framebuffer, color);
    draw_line2d(cx, cy, ax, ay, framebuffer, color);

    //trinagolo (7,45) (35,100), (45, 60)
}

//TODO:
//ordinare vertici del trianglo per la loro cordinata y
//rasterizzare contemporaneamente il lato sinistro e destro del triangolo
//disegnare righe orizzontali tra il limite sinistro e destro

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}