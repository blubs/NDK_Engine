//
// Created by F1 on 3/29/2016.
//

#ifndef ENGINE_MATH_H
#define ENGINE_MATH_H

#include "../common.h"

#define PI 3.14159265359f
#define HALF_PI 1.57079632679f
#define TWO_PI 6.28318530718f
#define DEG_TO_RAD 0.01745329251f
#define RAD_TO_DEG 57.2957795131f

struct Vec3
{
	float x, y, z;

	Vec3 ()
	{
		x = y = z = 0.0f;
	}

	Vec3 (const float a,const float b,const float c)
	{
		x = a;
		y = b;
		z = c;
	}

	Vec3 (const Vec3& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	~Vec3 ()
	{};

	Vec3& operator =(const Vec3& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	//Dot product
	friend float operator* (const Vec3& l,const Vec3& r)
	{
		return l.x*r.x + l.y*r.y + l.z*r.z;
	}
	//Vector Addition
	friend Vec3 operator+(const Vec3& l,const Vec3& r)
	{
		return Vec3(l.x + r.x, l.y + r.y, l.z + r.z);
	}
	//Vector Subtraction
	friend Vec3 operator-(const Vec3& l,const Vec3& r)
	{
		return Vec3(l.x - r.x, l.y - r.y, l.z - r.z);
	}
	//Scalar Multiplication
	friend Vec3 operator*(const float l,const Vec3& r)
	{
		return Vec3(l*r.x, l*r.y, l*r.z);
	}
	friend Vec3 operator*(const Vec3& l,const float r)
	{
		return Vec3(l.x*r, l.y*r, l.z*r);
	}

	//Scalar Division
	friend Vec3 operator/(const float l, const Vec3& r)
	{
		return Vec3(l/r.x, l/r.y, l/r.z);
	}
	friend Vec3 operator/(const Vec3& l,const float r)
	{
		return Vec3(l.x/r, l.y/r, l.z/r);
	}


	float len() const
	{
		return (float)sqrt(x*x + y*y + z*z);
	}
	float len_squared() const
	{
		return x*x + y*y + z*z;
	}

	//Returns a normalized version of this vector
	Vec3 normalized() const
	{
		Vec3 res(*this);
		return res/len();
	}
	//Actually normalizes this vector
	void normalize()
	{
		float inv_l = 1.0f/len();
		x *= inv_l;
		y *= inv_l;
		z *= inv_l;
	}

	static Vec3 cross(const Vec3& a,const Vec3& b)
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
	static Vec3 ZERO()
	{
		Vec3 result;
		return result;
	}

	//For these rotations, we first apply yaw, then pitch, then roll
	//Given pitch yaw and roll as a Vec3, returns the forward vector (no roll)
	/*static Vec3 ANGLES_TO_FORWARD(Vec3 angles)
	{
		Vec3 result;
		float cos_pitch = cosf(angles.x);
		result.x = cos_pitch * cosf(angles.y);
		result.y = cos_pitch * sinf(angles.y);
		result.z = sinf(angles.x);
		return result;
	}
	//Given pitch yaw and roll as a Vec3, returns the right vector
	static Vec3 ANGLES_TO_RIGHT(Vec3 angles)
	{
		Vec3 result;
		result.x = cosf(angles.x) +
		return result;
	}

	//Given pitch yaw and roll as a Vec3, returns the up vector
	static Vec3 ANGLES_TO_UP(Vec3 angles)
	{
		Vec3 result;
		result.x = cosf(angles.x) +
		return result;
	}*/ //wait a sec, I may be able to do this with quaternions instead

	//TODO:
	// vec to angle
	// angle to vec

};

//Quaternion struct
struct Quat
{
	float w;//x,y,z being the i,j,k components
	Vec3 v;
	//i,j,k components
	Quat()
	{
		w = 1.0f;
		v.x = v.y = v.z = 0.0f;
	}

	Quat (const Quat& other)
	{
		v = other.v;
		w = other.w;
	}

	//TODO: functions/operators for quaternions
	//Creates a quaternion from direction and angle about the direction
	Quat(const float theta,const Vec3& dir)
	{
		float half_theta = theta * 0.5f;
		w = cosf(half_theta);
		float sin_half_theta = sinf(half_theta);
		v = dir * sin_half_theta;
	}
	Quat& operator =(const Quat& other)
	{
		w = other.w;
		v.x = other.v.x;
		v.y = other.v.y;
		v.z = other.v.z;
	}
	//Multiplies two quaternions
	friend Quat operator*(const Quat& l,const Quat& r)
	{
		Quat result;

		result.w = l.w * r.w - l.v*r.v;
		result.v = l.v * r.w + r.v * l.w + Vec3::cross(l.v,r.v);

		return result;
	}
	//Returns the inverted quaternion
	Quat inverted() const
	{
		Quat result;
		result.w = w;
		result.v = v*-1.0f;
		return result;
	}
	//Actually inverts this quaternion
	void invert()
	{
		v.x *= -1.0f;
		v.y *= -1.0f;
		v.z *= -1.0f;
	}

	//Multiplies a quaternion and a vector (applies a rotation)
	//The code is functionally equivalent to (q * p * q.invert()).v, where p.w = 0, p.v = r
	friend Vec3 operator*(const Quat& l, const Vec3& r)
	{
		Vec3 vXr = Vec3::cross(l.v,r);
		return r + vXr*(2.0f*l.w) + Vec3::cross(l.v,vXr)*2.0f;
	}

	//Returns a normalized version of this quaternion
	Quat normalized() const
	{
		Quat res(*this);
		float inv_mag = 1.0f/magnitude();
		res.v = res.v / inv_mag;
		res.w = res.w / inv_mag;
		return res;
	}
	//Actually normalizes this quaternion
	void normalize()
	{
		float inv_mag = 1.0f/magnitude();
		v.x *= inv_mag;
		v.y *= inv_mag;
		v.x *= inv_mag;
		w *= inv_mag;
	}
	float magnitude() const
	{
		return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + w*w);
	}
	float magnitude_squared() const
	{
		return v.x*v.x + v.y*v.y + v.z*v.z + w*w;
	}



	//The following is pseudo code for SLERPING quaternions,
	// 	implementation will be unclear until we figure out how we are going to call it
	// let t be the interpolation scalar ranging from 0 to 1
	// let temp1 = final * initial.invert();
	// let temp2 = ( temp1.w * sin(  (t*theta)/2  ), temp1.v * sin (  (t*theta)/2  ));
	// current = initial * temp2;

};

//This implementation is a column major-3x3 matrix (as OGL expects them)
//The following shows the mapping between index and matrix element
//[ 0  3  6 ]
//[ 1  4  7 ]
//[ 2  5  8 ]
struct Mat3
{
	float m[9];

	//Is it more useful to default to zero matrix? or to identity?
	//Default behavior returns 0 matrix
	Mat3()
	{
		m[0] = m[1] = m[2] =
		m[3] = m[4] = m[5] =
		m[6] = m[7] = m[8] = 0.0f;
	}

	Mat3(const float a0,const float a1,const float a2,
	const float a3,const float a4,const float a5,
	const float a6,const float a7,const float a8)
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
	}

	//This assumes ptr has 9 float values
	Mat3(const float* ptr)
	{
		m[0] = ptr[0];
		m[1] = ptr[1];
		m[2] = ptr[2];
		m[3] = ptr[3];
		m[4] = ptr[4];
		m[5] = ptr[5];
		m[6] = ptr[6];
		m[7] = ptr[7];
		m[8] = ptr[8];
	}

	Mat3 operator =(const Mat3& other)
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
		return *this;
	}

	friend Mat3 operator*(const Mat3& l, const Mat3& r)
	{
		Mat3 result;

		result.m[0]  = l.m[0] * r.m[0]  + l.m[3] * r.m[1]  + l.m[6]  * r.m[2];
		result.m[1]  = l.m[1] * r.m[0]  + l.m[4] * r.m[1]  + l.m[7]  * r.m[2];
		result.m[2]  = l.m[2] * r.m[0]  + l.m[5] * r.m[1]  + l.m[8] * r.m[2];

		result.m[3]  = l.m[0] * r.m[3]  + l.m[3] * r.m[4]  + l.m[6] * r.m[5];
		result.m[4]  = l.m[1] * r.m[3]  + l.m[4] * r.m[4]  + l.m[7] * r.m[5];
		result.m[5]  = l.m[2] * r.m[3]  + l.m[5] * r.m[4]  + l.m[8] * r.m[5];

		result.m[6]  = l.m[0] * r.m[6]  + l.m[3] * r.m[7]  + l.m[6] * r.m[8];
		result.m[7]  = l.m[1] * r.m[6]  + l.m[4] * r.m[7]  + l.m[7] * r.m[8];
		result.m[8]  = l.m[2] * r.m[6]  + l.m[5] * r.m[7]  + l.m[8] * r.m[8];

		return result;
	}

	//Left multiplying Vec3
	//Treating vec3 as row vector
	friend Vec3 operator*(const Vec3& l,const Mat3& r)
	{
		Vec3 result;
		result.x =l.x*r.m[0] + l.y*r.m[1] + l.z*r.m[2];
		result.y =l.x*r.m[3] + l.y*r.m[4] + l.z*r.m[5];
		result.z =l.x*r.m[6] + l.y*r.m[7] + l.z*r.m[8];
		return result;
	}

	//Right multiplying Vec3
	//Treating vec3 as column vector
	friend Vec3 operator*(const Mat3& l,const Vec3& r)
	{
		Vec3 result;
		result.x =r.x*l.m[0] + r.y*l.m[3] + r.z*l.m[6];
		result.y =r.x*l.m[1] + r.y*l.m[4] + r.z*l.m[7];
		result.z =r.x*l.m[2] + r.y*l.m[5] + r.z*l.m[8];
		return result;
	}


	//Static method that returns an identity matrix
	static Mat3 IDENTITY()
	{
		Mat3 result;
		result.m[0] = result.m[4] = result.m[8] = 1.0f;
		return result;
	}
};

//This implementation is a column major-matrix (as OGL expects them)
//The following shows the mapping between index and matrix element
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

	Mat4(const float a0,const float a1,const float a2,const float a3,
		const float a4,const float a5,const float a6,const float a7,
		const float a8,const float a9,const float a10,const float a11,
		const float a12,const float a13,const float a14,const float a15)
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

	//This assumes ptr has 16 float values
	Mat4(const float* ptr)
	{
		m[0] = ptr[0];
		m[1] = ptr[1];
		m[2] = ptr[2];
		m[3] = ptr[3];
		m[4] = ptr[4];
		m[5] = ptr[5];
		m[6] = ptr[6];
		m[7] = ptr[7];
		m[8] = ptr[8];
		m[9] = ptr[9];
		m[10] = ptr[10];
		m[11] = ptr[11];
		m[12] = ptr[12];
		m[13] = ptr[13];
		m[14] = ptr[14];
		m[15] = ptr[15];
	}

	Mat4 operator =(const Mat4& other)
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

	friend Mat4 operator*(const Mat4& l, const Mat4& r)
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
	friend Vec3 operator*(const Vec3& l,const Mat4& r)
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
	friend Vec3 operator*(const Mat4& l,const Vec3& r)
	{
		Vec3 result;

		result.x =r.x*l.m[0] + r.y*l.m[4] + r.z*l.m[8]  + l.m[12];
		result.y =r.x*l.m[1] + r.y*l.m[5] + r.z*l.m[9]  + l.m[13];
		result.z =r.x*l.m[2] + r.y*l.m[6] + r.z*l.m[10] + l.m[14];
		return result;
	}

	//The following two functions return the inverse matrix
	//They assume the matrices represent rigid body transformations (i.e. only rotation and translation)
	//This can be used to find view matrix as well
	//This actually inverts the matrix
	Mat4 invert()
	{
		Vec3 u(m[0],m[1],m[2]);
		Vec3 v(m[4],m[5],m[6]);
		Vec3 w(m[8],m[9],m[10]);

		Vec3 t(m[12],m[13],m[14]);

		m[0] = u.x;	m[4] = u.y;	m[8] = u.z;	m[12] = -(u*t);
		m[1] = v.x;	m[5] = v.y;	m[9] = v.z;	m[13] = -(v*t);
		m[2] = w.x;	m[6] = w.y;	m[10]= w.z;	m[14] = -(w*t);
		m[3] = 0.0f;	m[7] = 0.0f;	m[11]= 0.0f;	m[15] = 1.0f;
	}
	//This returns a copy of the inverted matrix
	Mat4 inverted() const
	{
		Mat4 result;

		Vec3 u(m[0],m[1],m[2]);
		Vec3 v(m[4],m[5],m[6]);
		Vec3 w(m[8],m[9],m[10]);

		Vec3 t(m[12],m[13],m[14]);

		result.m[0] = u.x;	result.m[4] = u.y;	result.m[8] = u.z;	result.m[12] = -(u*t);
		result.m[1] = v.x;	result.m[5] = v.y;	result.m[9] = v.z;	result.m[13] = -(v*t);
		result.m[2] = w.x;	result.m[6] = w.y;	result.m[10]= w.z;	result.m[14] = -(w*t);
		result.m[3] = 0.0f;	result.m[7] = 0.0f;	result.m[11]= 0.0f;	result.m[15] = 1.0f;

		return result;
	}

	//This actually transposes the matrix
	Mat4 transpose()
	{
		Mat4 original = *this;
		//0 is unmodified
		m[1] = original.m[4];
		m[2] = original.m[8];
		m[3] = original.m[12];

		m[4] = original.m[1];
		//5 is unmodified
		m[6] = original.m[9];
		m[7] = original.m[13];

		m[8] = original.m[2];
		m[9] = original.m[6];
		//10 is unmodified
		m[11] = original.m[14];

		m[12] = original.m[3];
		m[13] = original.m[7];
		m[14] = original.m[11];
		//15 is unmodified
	}
	//Returns a transposed copy of this matrix
	Mat4 transposed() const
	{
		Mat4 result;
		result.m[0] = m[0];
		result.m[1] = m[4];
		result.m[2] = m[8];
		result.m[3] = m[12];

		result.m[4] = m[1];
		result.m[5] = m[5];
		result.m[6] = m[9];
		result.m[7] = m[13];

		result.m[8] = m[2];
		result.m[9] = m[6];
		result.m[10] = m[10];
		result.m[11] = m[14];

		result.m[12] = m[3];
		result.m[13] = m[7];
		result.m[14] = m[11];
		result.m[15] = m[15];
		return result;
	}

	//Inverts then tranposes this matrix
	Mat4 invert_then_transpose()
	{
		invert();
		transpose();
	}
	//Returns a copy of this matrix inverted then transposed
	Mat4 inverted_then_transposed() const
	{
		Mat4 result = inverted();
		result.transpose();
		return result;
	}

	//Returns the transform position of matrix
	Vec3 get_pos() const
	{
		return Vec3(m[12],m[13],m[14]);
	}


	//returns the upper left 3x3 Matrix of this 4x4 Matrix
	Mat3 get_mat3() const
	{
		Mat3 result;
		result.m[0] = m[0];
		result.m[1] = m[1];
		result.m[2] = m[2];
		result.m[3] = m[4];
		result.m[4] = m[5];
		result.m[5] = m[6];
		result.m[6] = m[8];
		result.m[7] = m[9];
		result.m[8] = m[10];
		return result;
	}

	//Static method that returns an identity matrix
	static Mat4 IDENTITY()
	{
		Mat4 result;
		result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;
		return result;
	}
	//Static method that returns a translation transform matrix given point
	static Mat4 TRANSLATE(const Vec3& pos)
	{
		Mat4 result = IDENTITY();
		result.m[12] = pos.x;
		result.m[13] = pos.y;
		result.m[14] = pos.z;

		return result;
	}

	//Static method that returns a scale transform matrix
	static Mat4 SCALE(const float x,const float y,const float z)
	{
		Mat4 result;
		result.m[0] = x;
		result.m[5] = y;
		result.m[10] = z;
		result.m[15] = 1.0f;
		return result;
	}

	//Static method that returns a rotation matrix given a unit quaternion rotation
	static Mat4 ROTATE(const Quat& q)
	{
		Mat4 result;
		//Precomputing floating point multiplications
		//TODO: test runtime of this versus not precomputing, is there any benefit?
		float xx2,yy2,zz2,xy2,xz2,yz2,wx2,wy2,wz2;
		xx2 = 2.0f * q.v.x * q.v.x;
		yy2 = 2.0f * q.v.y * q.v.y;
		zz2 = 2.0f * q.v.z * q.v.z;
		xz2 = 2.0f * q.v.x * q.v.z;
		xy2 = 2.0f * q.v.x * q.v.y;
		yz2 = 2.0f * q.v.y * q.v.z;
		wx2 = 2.0f * q.w * q.v.x;
		wy2 = 2.0f * q.w * q.v.y;
		wz2 = 2.0f * q.w * q.v.z;

		result.m[0] = 1.0f - yy2 - zz2;
		result.m[1] = xy2 + wz2;
		result.m[2] = xz2 - wy2;
		result.m[4] = xy2 - wz2;
		result.m[5] = 1.0f - xx2 - zz2;
		result.m[6] = yz2 + wx2;
		result.m[8] = xz2 + wy2;
		result.m[9] = yz2 - wx2;
		result.m[10] = 1.0f - xx2 - yy2;
		result.m[15] = 1.0f;

		return result;
	}

	//Calculated transform matrix given angles and position
	static Mat4 ROT_TRANS(const Vec3& angles, const Vec3& pos)
	{
		Quat pitch(angles.x, Vec3::RIGHT());
		//Rotating angles.y about UP vector for yaw
		Quat yaw(angles.y, Vec3::UP());

		Quat rot = yaw*pitch;
		Vec3 forward = rot * Vec3::FRONT();

		//Rotating angles.z about the forward vector for the roll
		Quat roll(angles.z, forward);
		//Adding roll to pitch * yaw
		rot = roll * rot;
		Vec3 right = rot * Vec3::RIGHT();
		Vec3 up = Vec3::cross(right,forward);

		return Mat4::TRANSLATE(pos) * Mat4::ROTATE(rot);
	}

	//Alternate ROT_TRANS, assigns forward right and up vectors that are passed in
	static Mat4 ROT_TRANS(const Vec3& angles, const Vec3& pos, Vec3* right, Vec3* up, Vec3* forward)
	{
		Quat pitch(angles.x, Vec3::RIGHT());
		//Rotating angles.y about UP vector for yaw
		Quat yaw(angles.y, Vec3::UP());

		Quat rot = yaw*pitch;
		*forward = rot * Vec3::FRONT();

		//Rotating angles.z about the forward vector for the roll
		Quat roll(angles.z, *forward);
		//Adding roll to pitch * yaw
		rot = roll * rot;
		*right = rot * Vec3::RIGHT();
		*up = Vec3::cross(*right,*forward);

		return Mat4::TRANSLATE(pos) * Mat4::ROTATE(rot);
	}

	//TODO:
	// rotation matrix

	// Constructs a view matrix from normalized camera direction vectors and position vector
	static Mat4 VIEW(const Vec3 right, const Vec3 up, const Vec3 forward, const Vec3 pos)
	{
		Mat4 result;
		//Top row has right vec
		result.m[0] = right.x;
		result.m[4] = right.y;
		result.m[8] = right.z;

		//2nd row has up vec
		result.m[1] = up.x;
		result.m[5] = up.y;
		result.m[9] = up.z;

		//3rd row has -forward vec
		result.m[2] = -forward.x;
		result.m[6] = -forward.y;
		result.m[10] = -forward.z;

		//last column has undoing position
		result.m[12] = -(right*pos);
		result.m[13] = -(up*pos);
		result.m[14] = forward*pos;
		result.m[15] = 1.0f;
		 return result;
	}

	// Constructs a projection matrix given near plane, far plane, aspect ratio, and fov
	static Mat4 PROJECT_PERSPECTIVE(const float near,const float far,const float aspect,const float fov)
	{
		Mat4 result;
		float inv_tan_fov = 1.0f / (tanf(fov * 0.5f));

		result.m[0] = inv_tan_fov / aspect;
		result.m[5] = inv_tan_fov;
		result.m[10] = (far + near) / (near - far);
		result.m[11] = -1.0f;
		result.m[14] = (2.0f * far * near)/(near - far);

		return result;
	}

	//may or may not change this, but currently ortho projection considers the center to be at (0,0)
	// Constructs an orthographic projection matrix given near plane, far plane, screen width and screen height
	static Mat4 PROJECT_ORTHO(const float near, const float far, const float width, const float height)
	{
		Mat4 result = Mat4::IDENTITY();
		float top = height*0.5f;
		float bottom = -top;
		float right = width*0.5f;
		float left = -right;

		float inv_z_dist = 1.0f/ (far - near);
		float inv_width = 1.0f/(right - left);
		float inv_height = 1.0f/(top-bottom);

		result.m[0] = 2.0f * inv_width;
		result.m[5] = 2.0f * inv_height;
		result.m[10] = 2.0f * inv_z_dist;
		result.m[12] = -(right+left) * inv_width;
		result.m[13] = -(top+bottom) * inv_height;
		result.m[14] = -(far+near) * inv_z_dist;

		return result;
	}
	//Additional methods to perhaps implement (depending on if I ever find the need for them)
	//Scalar multiplication / division of a matrix
	//addition / subtraction of a matrices
	//Transpose Matrix
	//Inverse Matrix
	//Determinant of a matrix
	//Eigen vectors / values (very doubtful I'll find myself in need of these)
};

#endif //ENGINE_MATH_H
