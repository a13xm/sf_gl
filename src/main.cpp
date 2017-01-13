#include <vector>
#include <cmath>
#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include "tgaimage.h"
#include "model.h"
#include "geom/vector.h"
#include "geom/matrix.h"
#include "pipeline/rasterizer.h"
#include "pipeline/transformations.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(  0, 255,   0, 255);
const TGAColor blue  = TGAColor(  0,   0, 255, 255);

const int width  = 1024;
const int height = 1024;
const int depth = 255;

Matrix trans_sum;

void render_model(TGAImage &image, TGAImage &z_image) {
    Model *model = new Model("obj/african_head.obj");
    model->load_texture("obj/african_head_diffuse.tga");
    Rasterizer rast;

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
            frame_coords[j] = Vec3i(Vec3f(trans_sum * Matrix(vertice)));
            uv_coords[j] = model->uv(mf.m_verts[j].vt_idx);
        }

        Vec3f edge_vect1 = model_coords[1] - model_coords[0];
        Vec3f edge_vect2 = model_coords[2] - model_coords[0];
        Vec3f triangle_normal = (edge_vect2 ^ edge_vect1).normalize();
        float intensity = triangle_normal * light_dir;

        if (intensity > 0) {
            rast.triangle_aabb(frame_coords[0], frame_coords[1], frame_coords[2],
                uv_coords[0], uv_coords[1], uv_coords[2],
                image, model, intensity, z_buffer);
        }
    }

    delete model;
    rast.draw_z_buffer(z_buffer, z_image);
    delete [] z_buffer;
}

int main() {
    TGAImage image(width, height, TGAImage::RGB);
    TGAImage z_image(width, height, TGAImage::GRAYSCALE);

    Matrix mv = transformations::viewport_matrix(0, 0, width, height, 0, 255);
    trans_sum = mv * transformations::projection_matrix() * transformations::view_matrix() * transformations::model_matrix();
    render_model(image, z_image);

    image.flip_vertically(); // computed as lower-left
    image.write_tga_file("out/output.tga");

    z_image.flip_vertically(); // computed as lower-left
    z_image.write_tga_file("out/z_output.tga");
    return 0;
}
