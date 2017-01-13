#include <cassert>
#include <iostream>
#include "geom/vector.h"
#include "geom/matrix.h"

#include "transformations.h"

namespace transformations {

    Matrix model_matrix() {
        Matrix result = Matrix::identity(4);
        return result;
        // return result * m_yaw(0.4);
    }

    Matrix view_matrix() {
        Matrix result = Matrix::identity(4);
        // result[0][3] = -0.6;
        return result;
    }

    Matrix projection_matrix() {
        Matrix result = Matrix::identity(4);
        return result;
    }

    Matrix viewport_matrix(int vp_x, int vp_y, int vp_width, int vp_height, float z_depth_min, float z_depth_max) {
        Matrix result = Matrix::identity(4);
        result[0][3] = vp_x + vp_width * 0.5;
        result[1][3] = vp_y + vp_height * 0.5;
        result[2][3] = z_depth_min + (z_depth_max - z_depth_min) * 0.5;

        result[0][0] = vp_width * 0.5;
        result[1][1] = vp_height * 0.5;
        result[2][2] = (z_depth_max - z_depth_min) * 0.5;
        return result;
    }

/////////////////////////////////////////////////////////////
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
/////////////////////////////////////////////////////////////

    Matrix rotation(float pitch, float yaw, float roll) {
        return m_pitch(pitch) * m_yaw(yaw) * m_roll(roll);
    }

    Matrix m_pitch(float pitch) {
        float sin_x = sinf(pitch);
        float cos_x = cosf(pitch);
        Matrix result = Matrix::identity(4);
        result[1][1] = cos_x; result[1][2] = -sin_x;
        result[2][1] = sin_x; result[2][2] = cos_x;
        return result;
    }

    Matrix m_yaw(float yaw) {
        float sin_y = sinf(yaw);
        float cos_y = cosf(yaw);
        Matrix result = Matrix::identity(4);
        result[0][0] = cos_y; result[0][2] = sin_y;
        result[2][0] = -sin_y; result[2][2] = cos_y;
        return result;
    }

    Matrix m_roll(float roll) {
        float sin_z = sinf(roll);
        float cos_z = cosf(roll);
        Matrix result = Matrix::identity(4);
        result[0][0] = cos_z; result[0][1] = -sin_z;
        result[1][0] = sin_z; result[1][1] = cos_z;
        return result;
    }

    // void look_at(Matrix &result, Vec3f &camera_point, Vec3f &target_point, Vec3f &up) {

    // }


}