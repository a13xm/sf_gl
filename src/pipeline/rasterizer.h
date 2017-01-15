#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

class Rasterizer {
private:
    int _width, _height;
    float _z_depth_min, _z_depth_max;
    TGAImage *_frame_image;
    TGAImage *_z_buffer_image;
    int *_z_buffer;

    void clear();
public:
    Rasterizer();
    ~Rasterizer();
    void init_frame(int width, int height, float z_depth_min, float z_depth_max);
    void line(Vec3i t0, Vec3i t1, TGAColor color);
    void triangle_aabb(Vec3i verts[3], Vec2i uvs[3], Model *model, float intensity);
    void draw_z_buffer();
    TGAImage *frame_image();
    TGAImage *z_buffer_image();
};

#endif //__RASTERIZER_H__