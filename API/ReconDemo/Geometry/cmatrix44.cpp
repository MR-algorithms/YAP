#include "cmatrix44.h"

CMatrix44::CMatrix44()
{
    Unit();
}

CMatrix44::CMatrix44(const CMatrix44& rhs)
{
    for (int i = 0; i < 4 * 4; i++)
        _elements[i] = rhs._elements[i];
}

void CMatrix44::Unit()
{
    for(int i = 0; i < 4 * 4; i++)
    {
        _elements[i] = 0.0;
    }
    _elements[0*4 + 0] = 1.0;
    _elements[1*4 + 1] = 1.0;
    _elements[2*4 + 2] = 1.0;
    _elements[3*4 + 3] = 1.0;
}

CMatrix44 CMatrix44::FromBasisVectors( const Vector3D& orient0,
                                       const Vector3D& orient1,
                                       const Vector3D& orient2 )
{
    CMatrix44 matrix;
    matrix._00 = orient0.cx, matrix._01 = orient1.cx, matrix._02 = orient2.cx;
    matrix._10 = orient0.cy, matrix._11 = orient1.cy, matrix._12 = orient2.cy;
    matrix._20 = orient0.cz, matrix._21 = orient1.cz, matrix._22 = orient2.cz;

    return matrix;
}

CMatrix44 CMatrix44::GetTranspose() const
{
    CMatrix44 w;
    double   temp;
    for(int index = 0; index < 4 * 4; index++)
        w._elements[index] = _elements[index];
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < i; j++)
        {
            temp = w._elements[i*4 + j];
            w._elements[i*4 + j] = w._elements[j*4 + i];
            w._elements[j*4 + i] = temp;
        }
    }
    return w;
}

CMatrix44 CMatrix44::Translate(const Vector3D& offset)
{
    CMatrix44 matrix;
    matrix.MakeTranslate(offset);
    return matrix;
}

void CMatrix44::MakeTranslate(const Vector3D& point)
{
    Unit();
    _elements[3]  = point.cx;
    _elements[7]  = point.cy;
    _elements[11] = point.cz;
}

CMatrix44 operator * (const CMatrix44& u, const CMatrix44& v)
{
    CMatrix44 w;

    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            w._elements[i * 4 +j] =
                u._elements[i * 4 + 0] * v._elements[0 * 4 + j] +
                u._elements[i * 4 + 1] * v._elements[1 * 4 + j] +
                u._elements[i * 4 + 2] * v._elements[2 * 4 + j] +
                u._elements[i * 4 + 3] * v._elements[3 * 4 + j];
        }
    }
    return w;
}

Point3D operator*(const CMatrix44& matrix,
                  const Point3D& point)
{
    Point3D w;

    w.x = point.x * matrix._elements[0] + point.y * matrix._elements[1] + point.z * matrix._elements[2] + matrix._elements[3];
    w.y = point.x * matrix._elements[4] + point.y * matrix._elements[5] + point.z * matrix._elements[6] + matrix._elements[7];
    w.z = point.x * matrix._elements[8] + point.y * matrix._elements[9] + point.z * matrix._elements[10] + matrix._elements[11];

    return w;
}
