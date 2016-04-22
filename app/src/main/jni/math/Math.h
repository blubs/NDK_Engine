//
// Created by F1 on 3/29/2016.
//

#ifndef ENGINE_MATH_H
#define ENGINE_MATH_H

#include "../common.h"

struct Vec3
{
	float x,y,z;

	Vec3()
	{
		x = y = z = 0.0f;
	}
	Vec3(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}
	~Vec3(){};
	//Dot product
	friend float operator*(Vec3& l, Vec3& r)
	{
		return l.x*r.x + l.y*r.y + l.z*r.z;
	}
	//Vector Addition
	friend Vec3 operator+(Vec3& l, Vec3& r)
	{
		return Vec3(l.x + r.x, l.y + r.y, l.z + r.z);
	}
	//Vector Subtraction
	friend Vec3 operator-(Vec3& l, Vec3& r)
	{
		return Vec3(l.x - r.x, l.y - r.y, l.z - r.z);
	}
	//Scalar Multiplication
	friend Vec3 operator*(float l, Vec3& r)
	{
		return Vec3(l*r.x, l*r.y, l*r.z);
	}
	friend Vec3 operator*(Vec3& l, float r)
	{
		return Vec3(l.x*r, l.y*r, l.z*r);
	}

	//Scalar Division
	friend Vec3 operator/(float l, Vec3& r)
	{
		return Vec3(l/r.x, l/r.y, l/r.z);
	}
	friend Vec3 operator/(Vec3& l, float r)
	{
		return Vec3(l.x/r, l.y/r, l.z/r);
	}


	float len()
	{
		return (float)sqrt(x*x + y*y + z*z);
	}
	float len_squared()
	{
		return x*x + y*y + z*z;
	}

	Vec3 normalize()
	{
		return *this/len();
	}

	static Vec3 cross(Vec3 a, Vec3 b)
	{
		Vec3 res(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
		return res;
	}


	static Vec3 FRONT()
	{
		Vec3 result;
		result.y = 1.0f;
		return result;
	}
	static Vec3 RIGHT()
	{
		Vec3 result;
		result.x = 1.0f;
		return result;
	}
	static Vec3 UP()
	{
		Vec3 result;
		result.z = 1.0f;
		return result;
	}

	//TODO:
	// cross product
	// vec to angle
	// angle to vec
};

//Quaternion struct
struct Quat
{
	float w,x,y,z;//x,y,z being the i,j,k components

	//TODO: a lot of functions for quaternions

};

//This implementation is a column major-matrix (as OGL expects them)
//The following shows the mapping from index to matrix element
//[ 0  4  8  12 ]
//[ 1  5  9  13 ]
//[ 2  6  10 14 ]
//[ 3  7  11 15 ]
struct Mat4
{
	float m[16];

	//Is it more useful to default to zero matrix? or to identity?
	//Default behavior returns 0 matrix
	Mat4()
	{
		m[0] = m[1] = m[2] = m[3] =
		m[4] = m[5] = m[6] = m[7] =
		m[8] = m[9] = m[10] = m[11] =
		m[12] = m[13] = m[14] = m[15] = 0.0f;
	}

	Mat4(float a0,  float a1,  float a2,  float a3,	float a4,  float a5,  float a6,  float a7,
		float a8,  float a9,  float a10, float a11,	float a12, float a13, float a14, float a15)
	{
		m[0] = a0;
		m[1] = a1;
		m[2] = a2;
		m[3] = a3;
		m[4] = a4;
		m[5] = a5;
		m[6] = a6;
		m[7] = a7;
		m[8] = a8;
		m[9] = a9;
		m[10] = a10;
		m[11] = a11;
		m[12] = a12;
		m[13] = a13;
		m[14] = a14;
		m[15] = a15;
	}

	Mat4& operator =(const Mat4& other)
	{
		m[0] = other.m[0];
		m[1] = other.m[1];
		m[2] = other.m[2];
		m[3] = other.m[3];
		m[4] = other.m[4];
		m[5] = other.m[5];
		m[6] = other.m[6];
		m[7] = other.m[7];
		m[8] = other.m[8];
		m[9] = other.m[9];
		m[10] = other.m[10];
		m[11] = other.m[11];
		m[12] = other.m[12];
		m[13] = other.m[13];
		m[14] = other.m[14];
		m[15] = other.m[15];
		return *this;
	}

	friend Mat4& operator*(Mat4& l, Mat4& r)
	{
		Mat4 result;

		result.m[0]  = l.m[0] * r.m[0]  + l.m[4] * r.m[1]  + l.m[8]  * r.m[2]  + l.m[12] * r.m[3];
		result.m[1]  = l.m[1] * r.m[0]  + l.m[5] * r.m[1]  + l.m[9]  * r.m[2]  + l.m[13] * r.m[3];
		result.m[2]  = l.m[2] * r.m[0]  + l.m[6] * r.m[1]  + l.m[10] * r.m[2]  + l.m[14] * r.m[3];
		result.m[3]  = l.m[3] * r.m[0]  + l.m[7] * r.m[1]  + l.m[11] * r.m[2]  + l.m[15] * r.m[3];

		result.m[4]  = l.m[0] * r.m[4]  + l.m[4] * r.m[5]  + l.m[8]  * r.m[6]  + l.m[12] * r.m[7];
		result.m[5]  = l.m[1] * r.m[4]  + l.m[5] * r.m[5]  + l.m[9]  * r.m[6]  + l.m[13] * r.m[7];
		result.m[6]  = l.m[2] * r.m[4]  + l.m[6] * r.m[5]  + l.m[10] * r.m[6]  + l.m[14] * r.m[7];
		result.m[7]  = l.m[3] * r.m[4]  + l.m[7] * r.m[5]  + l.m[11] * r.m[6]  + l.m[15] * r.m[7];

		result.m[8]  = l.m[0] * r.m[8]  + l.m[4] * r.m[9]  + l.m[8]  * r.m[10] + l.m[12] * r.m[11];
		result.m[9]  = l.m[1] * r.m[8]  + l.m[5] * r.m[9]  + l.m[9]  * r.m[10] + l.m[13] * r.m[11];
		result.m[10] = l.m[2] * r.m[8]  + l.m[6] * r.m[9]  + l.m[10] * r.m[10] + l.m[14] * r.m[11];
		result.m[11] = l.m[3] * r.m[8]  + l.m[7] * r.m[9]  + l.m[11] * r.m[10] + l.m[15] * r.m[11];

		result.m[12] = l.m[0] * r.m[12] + l.m[4] * r.m[13] + l.m[8]  * r.m[14] + l.m[12] * r.m[15];
		result.m[13] = l.m[1] * r.m[12] + l.m[5] * r.m[13] + l.m[9]  * r.m[14] + l.m[13] * r.m[15];
		result.m[14] = l.m[2] * r.m[12] + l.m[6] * r.m[13] + l.m[10] * r.m[14] + l.m[14] * r.m[15];
		result.m[15] = l.m[3] * r.m[12] + l.m[7] * r.m[13] + l.m[11] * r.m[14] + l.m[15] * r.m[15];

		return result;
	}

	//Left multiplying Vec3
	//Treating vec3 as row vector, treating 4th component of vector (w) as 1
	//if w = 1, full transformation, if w = 0, only rotate the point
	friend Vec3& operator*(Vec3& l, Mat4& r)
	{
		Vec3 result;

		result.x =l.x*r.m[0] + l.y*r.m[1] + l.z*r.m[2]  + r.m[3];
		result.y =l.x*r.m[4] + l.y*r.m[5] + l.z*r.m[6]  + r.m[7];
		result.z =l.x*r.m[8] + l.y*r.m[9] + l.z*r.m[10] + r.m[11];
		return result;
	}

	//Right multiplying Vec3
	//Treating vec3 as column vector, treating 4th component of vector (w) as 1
	//if w = 1, full transformation, if w = 0, only rotate the point
	friend Vec3& operator*(Mat4& l, Vec3& r)
	{
		Vec3 result;

		result.x =r.x*l.m[0] + r.y*l.m[4] + r.z*l.m[8]  + l.m[12];
		result.y =r.x*l.m[1] + r.y*l.m[5] + r.z*l.m[9]  + l.m[13];
		result.z =r.x*l.m[2] + r.y*l.m[6] + r.z*l.m[10] + l.m[14];
		return result;
	}

	//Static method that returns an identity matrix
	static Mat4 IDENTITY()
	{
		Mat4 result;
		result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;
		return result;
	}
	//Static method that returns a translation transform matrix given points.
	static Mat4 TRANSLATE(float x,float y,float z)
	{
		Mat4 result = IDENTITY();
		result.m[12] = x;
		result.m[13] = y;
		result.m[14] = z;
		return result;
	}

	//Static method that returns a scale transform matrix.
	static Mat4 SCALE(float x, float y, float z)
	{
		Mat4 result;
		result.m[0] = x;
		result.m[5] = y;
		result.m[10] = z;
		result.m[15] = 1.0f;
		return result;
	}

	//Static method that returns a rotation matrix given a unit quaternion rotation
	static Mat4 QUAT_TO_MAT4(Quat q)
	{
		Mat4 result = IDENTITY();
		//Precomputing floating point multiplications
		//TODO: test runtime of this versus not precomputing, is there any benefit?
		float xx2,yy2,zz2,xy2,xz2,yz2,wx2,wy2,wz2;
		xx2 = 2.0f * q.x * q.x;
		yy2 = 2.0f * q.y * q.y;
		zz2 = 2.0f * q.z * q.z;
		xz2 = 2.0f * q.x * q.z;
		xy2 = 2.0f * q.x * q.y;
		yz2 = 2.0f * q.y * q.z;
		wx2 = 2.0f * q.w * q.x;
		wy2 = 2.0f * q.w * q.y;
		wz2 = 2.0f * q.w * q.z;

		result.m[0] = 1.0f - yy2 - zz2;
		result.m[1] = xy2 + wz2;
		result.m[2] = xz2 - wy2;
		result.m[4] = xy2 - wz2;
		result.m[5] = 1.0f - xx2 - zz2;
		result.m[6] = yz2 + wx2;
		result.m[8] = xz2 + wy2;
		result.m[9] = yz2 - wx2;
		result.m[10] = 1.0f - xx2 - yy2;

		return result;
	}
	//TODO:
	// rotation matrix
	// view matrix
	// projection matrix

	//Additional methods to perhaps implement (depending on if I ever find the need for them)
	//Scalar multiplication / division of a matrix
	//addition / subtraction of a matrices
	//Transpose Matrix
	//Inverse Matrix
	//Determinant of a matrix
	//Eigen vectors / values (very doubtful I'll find myself in need of these)
};

#endif //ENGINE_MATH_H
