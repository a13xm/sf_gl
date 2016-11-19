#include <vector>
#include <cmath>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(  0, 255,   0,   0);
const TGAColor blue  = TGAColor(  0,   0, 255, 255);

const int width  = 800;
const int height = 800;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(t0.x-t1.x)<std::abs(t0.y - t1.y)) {
        std::swap(t0.x, t0.y);
        std::swap(t1.x, t1.y);
        steep = true;
    }
    if (t0.x > t1.x) {
        std::swap(t0.x, t1.x);
        std::swap(t0.y, t1.y);
    }
    int dx = t1.x-t0.x;
    int dy = t1.y-t0.y;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = t0.y;
    for (int x=t0.x; x<=t1.x; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;

        if (error2 > dx) {
            y += (t1.y>t0.y ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y==t1.y && t0.y==t2.y) return;
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    int height_r2 = t2.y - t0.y;
    int width_r2 = t2.x - t0.x;
    for (int y = t0.y; y <= t2.y; y++) {
        float r2_curr_part = (float)(y - t0.y)/height_r2;
        int x2 = t0.x + width_r2*r2_curr_part;
        int x1, height_r1, width_r1;

        if (y <= t1.y) {
            height_r1 = t1.y - t0.y;
            width_r1 = t1.x - t0.x;
            float r1_curr_part = height_r1 == 0 ? 0 : (float)(y - t0.y)/height_r1;
            x1 = t0.x + width_r1*r1_curr_part;
        } else {
            height_r1 = t2.y - t1.y;
            width_r1 = t2.x - t1.x;
            float r1_curr_part = height_r1 == 0 ? 0 : (float)(y - t1.y)/height_r1;
            x1 = t1.x + width_r1*r1_curr_part;
        }

        if (x1 > x2) std::swap(x1, x2);
        for (int x = x1; x <= x2; x++) {
            image.set(x, y, color);
        }
    }
}


int main() {
    Model *model = new Model("obj/african_head.obj");
    TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir(0,0,-1);
    for (int i=0; i < model->nfaces(); i++) {
        std::vector<int> edges = model->face(i);
        Vec2i frame_coords[3];
        Vec3f model_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f vertice = model->vert(edges[j]);
            model_coords[j] = vertice;
            frame_coords[j] = Vec2i((vertice.x + 1.)*width/2., (vertice.y + 1.)*height/2.);
        }

        Vec3f edge_vect1 = model_coords[1] - model_coords[0];
        Vec3f edge_vect2 = model_coords[2] - model_coords[0];
        Vec3f triangle_normal = (edge_vect2 ^ edge_vect1).normalize();
        float intensity = triangle_normal * light_dir;
        if (intensity > 0) {
            triangle(frame_coords[0], frame_coords[1], frame_coords[2], image, TGAColor(255*intensity, 255*intensity, 255*intensity, 255));
        }
    }

    delete model;
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("out/output.tga");
    return 0;
}
