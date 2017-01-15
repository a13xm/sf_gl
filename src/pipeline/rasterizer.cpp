#include <cassert>
#include "tgaimage.h"
#include "model.h"
#include "geom/geometry.h"
#include "geom/vector.h"

#include "rasterizer.h"

Rasterizer::Rasterizer () : _width(0), _height(0), _z_depth_min(0.0), _z_depth_max(0.0),
    _frame_image(NULL), _z_buffer_image(NULL), _z_buffer(NULL) {}

Rasterizer::~Rasterizer() {
    clear();
}

void Rasterizer::init_frame(int width, int height, float z_depth_min, float z_depth_max) {
    assert(width > 0);
    assert(height > 0);
    clear();
    _width = width;
    _height = height;
    _z_depth_min = z_depth_min;
    _z_depth_max = z_depth_max;
    _frame_image = new TGAImage(width, height, TGAImage::RGB);
    _z_buffer = new int[width * height];
    for (int x = 0; x<width * height; x++) {
        _z_buffer[x] = std::numeric_limits<int>::min();
    }
    _z_buffer_image = new TGAImage(width, height, TGAImage::GRAYSCALE);
}

void Rasterizer::clear() {
    _width = 0; _height = 0;
    _z_depth_min = 0.0; _z_depth_max = 0.0;
    if (_frame_image) {
        delete _frame_image;
        _frame_image = NULL;
    }
    if (_z_buffer) {
        delete [] _z_buffer;
        _z_buffer = NULL;
    }
    if (_z_buffer_image) { 
        delete _z_buffer_image;
        _z_buffer_image = NULL;
    }
}

void Rasterizer::line(Vec3i t0, Vec3i t1, TGAColor color) {
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
            _frame_image->set(y, x, color);
        } else {
            _frame_image->set(x, y, color);
        }
        error2 += derror2;

        if (error2 > dx) {
            y += (t1.y>t0.y ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void Rasterizer::triangle_aabb(
    Vec3i verts[3], Vec2i uvs[3], Model *model, float intensity) {

    Vec3i v0 = verts[0]; Vec3i v1 = verts[1]; Vec3i v2 = verts[2];
    Vec2i uv0 = uvs[0]; Vec2i uv1 = uvs[1]; Vec2i uv2 = uvs[2];

    if (orientation_2d(v0, v1, v2) < 0) {
        std::swap(v1, v2);
        std::swap(uv1, uv2);
    }

    int x_min = std::max(min_3(v0.x, v1.x, v2.x), 0);
    int y_min = std::max(min_3(v0.y, v1.y, v2.y), 0);
    int x_max = std::min(max_3(v0.x, v1.x, v2.x), _width);
    int y_max = std::min(max_3(v0.y, v1.y, v2.y), _height);

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
                int zb_idx = cur_point.y*_width + cur_point.x;
                if (z > _z_buffer[zb_idx]) {
                    _z_buffer[zb_idx] = z;
                    TGAColor color = model->get_texture_color(u, v);
                    _frame_image->set(cur_point.x, cur_point.y, TGAColor(color.r*intensity, color.g*intensity, color.b*intensity, 255));
                }
            }
        }
    }
}


void Rasterizer::draw_z_buffer() {
    for (int y=0; y<_height; y++) {
        for (int x=0; x<_width; x++) {
            _z_buffer_image->set(x, y, TGAColor(_z_buffer[y*_width + x], 1));
        }
    }
}

TGAImage *Rasterizer::frame_image() {
    return _frame_image;
}

TGAImage *Rasterizer::z_buffer_image() {
    return _z_buffer_image;
}
