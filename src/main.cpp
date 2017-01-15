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

Matrix trans_sum;
int width = 1024; int height = 1024;
float depth = 255;

void render_model() {
    Model *model = new Model("obj/african_head.obj");
    model->load_texture("obj/african_head_diffuse.tga");
    Rasterizer *rast = new Rasterizer();
    rast->init_frame(width, height, 0, depth);

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
            rast->triangle_aabb(frame_coords, uv_coords, model, intensity);
        }
    }
    rast->draw_z_buffer();
    TGAImage *image = rast->frame_image();
    image->flip_vertically(); // computed as lower-left
    image->write_tga_file("out/output.tga");
    image = NULL;

    TGAImage *z_image = rast->z_buffer_image();
    z_image->flip_vertically(); // computed as lower-left
    z_image->write_tga_file("out/z_output.tga");
    z_image = NULL;

    delete rast;
    rast = NULL;

    delete model;
    model = NULL;
}

int main() {
    Matrix mv = transformations::viewport_matrix(0, 0, width, height, 0, depth);
    trans_sum = mv * transformations::projection_matrix() * transformations::view_matrix() * transformations::model_matrix();
    render_model();
    return 0;
}
