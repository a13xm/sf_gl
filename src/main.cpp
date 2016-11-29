#include <vector>
#include <cmath>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(  0, 255,   0,   0);
const TGAColor blue  = TGAColor(  0,   0, 255, 255);

const int width  = 800;
const int height = 800;
const int depth = 255;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
    bool steep = std::abs(t0.x-t1.x)<std::abs(t0.y - t1.y);
    if (steep) {
        std::swap(t0.x, t0.y);
        std::swap(t1.x, t1.y);
    }
    
    if (t0.x > t1.x) std::swap(t0, t1);
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

template <typename T>
T min_3(T a, T b, T c) {
    return std::min(std::min(a, b), c);
}

template <typename T>
T max_3(T a, T b, T c) {
    return std::max(std::max(a, b), c);
}

int orientation_2d(Vec3i& a, Vec3i& b, Vec3i& c) {
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

float triangle_area(Vec3i a, Vec3i b, Vec3i c) {
    return std::abs(0.5f * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)));
}

void triangle_aabb(Vec3i vertex0, Vec3i vertex1, Vec3i vertex2, TGAImage &image, TGAColor color, int *z_buffer) {

    if (orientation_2d(vertex0, vertex1, vertex2) < 0) {
        std::swap(vertex1, vertex2);
    }

    int x_min = min_3(vertex0.x, vertex1.x, vertex2.x);
    int y_min = min_3(vertex0.y, vertex1.y, vertex2.y);
    int x_max = max_3(vertex0.x, vertex1.x, vertex2.x);
    int y_max = max_3(vertex0.y, vertex1.y, vertex2.y); 

    float total_area_inv = 1.0f/triangle_area(vertex0, vertex1, vertex2);

    Vec3i cur_point;
    for (cur_point.y = y_min; cur_point.y <= y_max; cur_point.y++) {
        for (cur_point.x = x_min; cur_point.x <= x_max; cur_point.x++) {
            int w0 = orientation_2d(vertex0, vertex1, cur_point);
            int w1 = orientation_2d(vertex1, vertex2, cur_point);
            int w2 = orientation_2d(vertex2, vertex0, cur_point);
            
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float area_0_1 = triangle_area(vertex0, vertex1, cur_point);
                float area_0_2 = triangle_area(vertex0, vertex2, cur_point);

                float b2 = area_0_1 * total_area_inv;
                float b1 = area_0_2 * total_area_inv;
                float b0 = 1.0f - b1 - b2;

                float z = vertex0.z + b1*(vertex1.z - vertex0.z) + b2*(vertex2.z - vertex0.z);
                int zb_idx = cur_point.y*width + cur_point.x;
                if (z > z_buffer[zb_idx]) {
                    z_buffer[zb_idx] = z;
                    image.set(cur_point.x, cur_point.y, color);
                }
            }
        }
    }
}


void draw_z_buffer(int *z_buffer, TGAImage &z_image) {
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            z_image.set(x, y, TGAColor(z_buffer[y*width + x], 1));
        }
    }
}

void render_model(TGAImage &image, TGAImage &z_image) {
    Model *model = new Model("obj/african_head.obj");

    int *z_buffer = new int[(width + 1) * (height + 1)]; // +1 while have no fill rule
    for (int x = 0; x<(width + 1) * (height + 1); x++) {
    // int *z_buffer = new int[width * height];
    // for (int x = 0; x<width * height; x++) {
        z_buffer[x] = std::numeric_limits<int>::min();
    }


    Vec3f light_dir(0,0,-1);
    for (int i=0; i < model->nfaces(); i++) {
        std::vector<int> edges = model->face(i);
        Vec3i frame_coords[3];
        Vec3f model_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f vertice = model->vert(edges[j]);
            model_coords[j] = vertice;
            frame_coords[j] = Vec3i((vertice.x + 1.)*width/2., (vertice.y + 1.)*height/2., (vertice.z + 1.)*depth/2.);
        }

        Vec3f edge_vect1 = model_coords[1] - model_coords[0];
        Vec3f edge_vect2 = model_coords[2] - model_coords[0];
        Vec3f triangle_normal = (edge_vect2 ^ edge_vect1).normalize();
        float intensity = triangle_normal * light_dir;

        if (intensity > 0) {
            TGAColor color = TGAColor(255*intensity, 255*intensity, 255*intensity, 255);
            triangle_aabb(frame_coords[0], frame_coords[1], frame_coords[2], image, color, z_buffer);
        }
    }

    delete model;
    draw_z_buffer(z_buffer, z_image);
}

int main() {
    TGAImage image(width, height, TGAImage::RGB);
    TGAImage z_image(width, height, TGAImage::GRAYSCALE);
    render_model(image, z_image);

    image.flip_vertically();
    image.write_tga_file("out/output.tga");

    z_image.flip_vertically();
    z_image.write_tga_file("out/z_output.tga");
    return 0;
}
