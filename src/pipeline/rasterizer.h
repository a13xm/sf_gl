#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

namespace rasterizer {

    void line(Vec3i t0, Vec3i t1, TGAImage &image, TGAColor color);

    void triangle_aabb(
        Vec3i v0, Vec3i v1, Vec3i v2, 
        Vec2i uv0, Vec2i uv1, Vec2i uv2,
        TGAImage &image, Model *model, float intensity, int *z_buffer);


    void draw_z_buffer(int *z_buffer, TGAImage &z_image);

}

#endif //__RASTERIZER_H__