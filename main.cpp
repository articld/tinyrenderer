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

    int total_height = vertices[2].y - vertices[0].y; 

    if(vertices[0].y != vertices[1].y){//controllo faccia non degenere
        int segment_height = vertices[1].y - vertices[0].y;
        //faccio passare la linea orizzontale da ay a by
        for(int y = vertices[0].y ; y <= vertices[1].y; y++){ //rasterizzazione bordo
            int x1 = vertices[0].x + ((vertices[2].x - vertices[0].x)*(y - vertices[0].y)) / total_height;
            int x2 = vertices[0].x + ((vertices[1].x - vertices[0].x)*(y - vertices[0].y)) / segment_height;
            for (int x = std::min(x1, x2); x < std::max(x1,x2); x++) //rasterizza solo la parte inferiore
                framebuffer.set(x, y, color);
        }
    } 

    if(vertices[1].y != vertices[2].y){//controllo faccia non degenere
        int segment_height = vertices[2].y - vertices[1].y;
        //passo la linea orizzontale da by a cy
        for(int y = vertices[1].y ; y <= vertices[2].y; y++){ //rasterizzazione bordo
            int x1 = vertices[0].x + ((vertices[2].x - vertices[0].x)*(y - vertices[0].y)) / total_height;
            int x2 = vertices[1].x + ((vertices[2].x - vertices[1].x)*(y - vertices[1].y)) / segment_height;
            for (int x = std::min(x1, x2); x < std::max(x1,x2); x++)
                framebuffer.set(x, y, color);
        }
    } 
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}