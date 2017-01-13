#ifndef __TRANSFORMATIONS_H__
#define __TRANSFORMATIONS_H__

namespace transformations {

    Matrix model_matrix();
    Matrix view_matrix();
    Matrix projection_matrix();
    Matrix viewport_matrix(int vp_x, int vp_y, int vp_width, int vp_height, float z_depth_min, float z_depth_max);

    Matrix rotation(float pitch, float yaw, float roll);
    Matrix m_pitch(float pitch);
    Matrix m_yaw(float yaw);
    Matrix m_roll(float roll);

}



#endif //__TRANSFORMATIONS_H__