#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <vector>

template <class t> struct Vec3;
typedef Vec3<float> Vec3f;

class Matrix {
private:
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int r=4, int c=4);
    Matrix(Vec3f v);
    int nrows();
    int ncols();
    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    Matrix operator*(const Matrix& a);
    Matrix transpose();
    Matrix inverse();
    friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};


#endif //__MATRIX_H__