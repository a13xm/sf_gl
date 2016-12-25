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
const TGAColor green = TGAColor(  0, 255,   0, 255);
const TGAColor blue  = TGAColor(  0,   0, 255, 255);

const int width  = 1024;
const int height = 1024;
const int depth = 255;

void line(Vec3i t0, Vec3i t1, TGAImage &image, TGAColor color) {
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

bool is_top_left_edge(Vec3i va, Vec3i vb, Vec3i vc) { // test that va - vb is top or left edge
    return va.y > vb.y || (va.y == vb.y && va.y > vc.y);
}

void triangle_aabb(
    Vec3i v0, Vec3i v1, Vec3i v2, 
    Vec2i uv0, Vec2i uv1, Vec2i uv2,
    TGAImage &image, Model *model, float intensity, int *z_buffer) {

    if (orientation_2d(v0, v1, v2) < 0) {
        std::swap(v1, v2);
        std::swap(uv1, uv2);
    }

    int x_min = min_3(v0.x, v1.x, v2.x);
    int y_min = min_3(v0.y, v1.y, v2.y);
    int x_max = max_3(v0.x, v1.x, v2.x);
    int y_max = max_3(v0.y, v1.y, v2.y);

    float total_area_inv = 1.0f/triangle_area(v0, v1, v2);

    int bias0 = is_top_left_edge(v1, v2, v0) ? 0 : -1;
    int bias1 = is_top_left_edge(v2, v0, v1) ? 0 : -1;
    int bias2 = is_top_left_edge(v0, v1, v2) ? 0 : -1;

    Vec3i cur_point;
    for (cur_point.y = y_min; cur_point.y <= y_max; cur_point.y++) {
        for (cur_point.x = x_min; cur_point.x <= x_max; cur_point.x++) {
            int w0 = orientation_2d(v1, v2, cur_point) + bias0; // left-top rasterization rule
            int w1 = orientation_2d(v2, v0, cur_point) + bias1; // left-top rasterization rule
            int w2 = orientation_2d(v0, v1, cur_point) + bias2; // left-top rasterization rule

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float area_0_1 = triangle_area(v0, v1, cur_point);
                float area_0_2 = triangle_area(v0, v2, cur_point);

                float b2 = area_0_1 * total_area_inv;
                float b1 = area_0_2 * total_area_inv;
                float b0 = 1.0f - b1 - b2;

                float z = v0.z + b1*(v1.z - v0.z) + b2*(v2.z - v0.z);
                int u = uv0.u + b1*(uv1.u - uv0.u) + b2*(uv2.u - uv0.u);
                int v = uv0.v + b1*(uv1.v - uv0.v) + b2*(uv2.v - uv0.v);
                int zb_idx = cur_point.y*width + cur_point.x;
                if (z > z_buffer[zb_idx]) {
                    z_buffer[zb_idx] = z;
                    TGAColor color = model->get_texture_color(u, v);
                    image.set(cur_point.x, cur_point.y, TGAColor(color.r*intensity, color.g*intensity, color.b*intensity, 255));
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
    model->load_texture("obj/african_head_diffuse.tga");

    int *z_buffer = new int[width * height];
    for (int x = 0; x<width * height; x++) {
        z_buffer[x] = std::numeric_limits<int>::min();
    }

    Vec3f light_dir(0,0,-1);
    for (int i=0; i < model->nfaces(); i++) {
        ModelFace mf = model->face(i);
        Vec3f model_coords[3];
        Vec3i frame_coords[3];
        Vec2i uv_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f vertice = model->vert(mf.m_verts[j].v_idx);
            model_coords[j] = vertice;
            frame_coords[j] = Vec3i((vertice.x + 1.)*width/2., (vertice.y + 1.)*height/2., (vertice.z + 1.)*depth/2.);
            uv_coords[j] = model->uv(mf.m_verts[j].vt_idx);
        }

        Vec3f edge_vect1 = model_coords[1] - model_coords[0];
        Vec3f edge_vect2 = model_coords[2] - model_coords[0];
        Vec3f triangle_normal = (edge_vect2 ^ edge_vect1).normalize();
        float intensity = triangle_normal * light_dir;

        if (intensity > 0) {
            triangle_aabb(frame_coords[0], frame_coords[1], frame_coords[2],
                uv_coords[0], uv_coords[1], uv_coords[2],
                image, model, intensity, z_buffer);
        }
    }

    delete model;
    draw_z_buffer(z_buffer, z_image);
    delete [] z_buffer;
}

int main() {
    TGAImage image(width, height, TGAImage::RGB);
    TGAImage z_image(width, height, TGAImage::GRAYSCALE);
    render_model(image, z_image);

    image.flip_vertically(); // computed as lower-left
    image.write_tga_file("out/output.tga");

    z_image.flip_vertically(); // computed as lower-left
    z_image.write_tga_file("out/z_output.tga");
    return 0;
}
