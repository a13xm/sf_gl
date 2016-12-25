#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"


std::ostream& operator<<(std::ostream &out_s, const ModelFace &mf) {
    for (int i=0;i<3;i++) {
        out_s << mf.m_verts[i].v_idx << "/" << mf.m_verts[i].vt_idx << "/"  << mf.m_verts[i].vn_idx << " ";
    }
    return out_s;
}

Model::Model(const char *filename) {
    std::ifstream in(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec3f vt;
            for (int i=0;i<3;i++) iss >> vt[i];
            t_verts_.push_back(vt);
        } else if (!line.compare(0, 2, "f ")) {
            iss >> trash;
            ModelFace mf(iss);
            faces_.push_back(mf);
        }
    }
    in.close();
    std::cerr << "# v# " << verts_.size() << "# vt# " << t_verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

ModelFace Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

void Model::load_texture(const char *filename) {
    bool load_res = texture_.read_tga_file(filename);
    std::cerr << "texture " << filename << " loading " << (load_res ? "ok" : "failed") << std::endl;
    texture_.flip_vertically();
}

TGAColor Model::get_texture_color(int u, int v) {
    return texture_.get(u, v);
}

Vec2i Model::uv(int t_vert_idx) {
    Vec3f t_vert = t_verts_[t_vert_idx];
    return Vec2i(t_vert.x * texture_.get_width(), t_vert.y * texture_.get_height());
}

