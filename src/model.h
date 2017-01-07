#ifndef __MODEL_H__
#define __MODEL_H__

#include <iostream>
#include <sstream>
#include <vector>
#include "geom/geometry.h"
#include "tgaimage.h"


struct ModelFace {
    union {
        struct {
            struct { unsigned int v_idx, vt_idx, vn_idx; } m_vert1;
            struct { unsigned int v_idx, vt_idx, vn_idx; } m_vert2;
            struct { unsigned int v_idx, vt_idx, vn_idx; } m_vert3;
        };
        unsigned int raw[9];
        struct { unsigned int v_idx, vt_idx, vn_idx; } m_verts[3];
    };
    ModelFace(std::istringstream &wvf_line) {
        char trash;
        for (int i=0;i<3;i++) { //v1_idx/vt1_idx/vn1_idx v2_idx/vt2_idx/vn2_idx v3_idx/vt3_idx/vn3_idx
            int v_idx_, vt_idx_, vn_idx_;
            wvf_line >> v_idx_;
            wvf_line >> trash >> vt_idx_;
            wvf_line >> trash >> vn_idx_;

            m_verts[i].v_idx = --v_idx_; // wavefront indexes starts from 1
            m_verts[i].vt_idx = --vt_idx_; // wavefront indexes starts from 1
            m_verts[i].vn_idx = --vn_idx_; // wavefront indexes starts from 1
        }
    }
};

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<Vec3f> t_verts_;
    std::vector<ModelFace> faces_;
    TGAImage texture_;
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    Vec3f t_vert(int i);
    ModelFace face(int idx);
    void load_texture(const char *filename);
    Vec2i uv(int t_vert_idx);
    TGAColor get_texture_color(int u, int v);
};

#endif //__MODEL_H__
