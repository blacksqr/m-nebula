#define _LIB
#include "mathlib/nmath.h"
#include "mathlib/vector.h"
#include "mathlib/matrix.h"

template<> const vector2 vector2::one	(1.0f,1.0f);
template<> const vector2 vector2::zero	(0.0f,0.0f);
template<> const vector2 vector2::unit_x(1.0f,0.0f);
template<> const vector2 vector2::unit_y(0.0f,1.0f);

template<> const vector3 vector3::one	(1.0f,1.0f,1.0f);
template<> const vector3 vector3::zero	(0.0f,0.0f,0.0f);
template<> const vector3 vector3::unit_x(1.0f,0.0f,0.0f);
template<> const vector3 vector3::unit_y(0.0f,1.0f,0.0f);
template<> const vector3 vector3::unit_z(0.0f,0.0f,1.0f);

template<> const vector4 vector4::one	(1.0f,1.0f,1.0f,1.0f);
template<> const vector4 vector4::zero	(0.0f,0.0f,0.0f,0.0f);
template<> const vector4 vector4::unit_x(1.0f,0.0f,0.0f,0.0f);
template<> const vector4 vector4::unit_y(0.0f,1.0f,0.0f,0.0f);
template<> const vector4 vector4::unit_z(0.0f,0.0f,1.0f,0.0f);
template<> const vector4 vector4::unit_w(0.0f,0.0f,0.0f,1.0f);

template<> const t_quaternion<float> t_quaternion<float>::zero;
template<> const t_quaternion<double> t_quaternion<double>::zero;


template<> const float  t_matrix33<float>::epsilon = FLT_EPSILON;
template<> const double t_matrix33<double>::epsilon = DBL_EPSILON;
template<> const float  t_matrix44<float>::epsilon = FLT_EPSILON;
template<> const double t_matrix44<double>::epsilon = DBL_EPSILON;

template<> const float t_matrix33<float>::identity[9] = {
	1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
};

template<> const double t_matrix33<double>::identity[9] = {
	1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
};

template<> const float t_matrix44<float>::identity[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

template<> const double t_matrix44<double>::identity[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
};