#pragma once

#include <corecrt_math_defines.h>
#include <sstream>
#include <algorithm>

class Vector2;
class Vector3;
class Vector4;
class QAngle;

class Vector3
{
public:
	Vector3 ( )
	{
		x = y = z = 0.f;
	}

	Vector3 ( float fx, float fy, float fz )
	{
		x = fx;
		y = fy;
		z = fz;
	}

	float x, y, z;

	Vector3 operator+( const Vector3& input ) const
	{
		return Vector3 { x + input.x, y + input.y, z + input.z };
	}

	Vector3 operator-( const Vector3& input ) const
	{
		return Vector3 { x - input.x, y - input.y, z - input.z };
	}

	Vector3 operator/( float input ) const
	{
		return Vector3 { x / input, y / input, z / input };
	}

	Vector3 operator*( float input ) const
	{
		return Vector3 { x * input, y * input, z * input };
	}

	Vector3& operator+=( const Vector3& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Vector3& operator-=( const Vector3& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Vector3& operator/=( float input )
	{
		x /= input;
		y /= input;
		z /= input;
		return *this;
	}

	Vector3& operator*=( float input )
	{
		x *= input;
		y *= input;
		z *= input;
		return *this;
	}

	bool operator==( const Vector3& input ) const
	{
		return x == input.x && y == input.y && z == input.z;
	}

	void make_absolute ( )
	{
		x = std::abs ( x );
		y = std::abs ( y );
		z = std::abs ( z );
	}

	float Clamp01 ( float value )
	{
		float result;
		if ( value < 0 )
		{
			result = 0;
		}
		else if ( value > 1.f )
		{
			result = 1.f;
		}
		else
		{
			result = value;
		}
		return result;
	}

	float Lerp ( )
	{
		return x + ( y - x ) * Clamp01 ( z );
	}

	float length_sqr ( ) const
	{
		return ( x * x ) + ( y * y ) + ( z * z );
	}

	float length ( ) const
	{
		return std::sqrt ( length_sqr ( ) );
	}

	float length_2d ( ) const
	{
		return std::sqrt ( ( x * x ) + ( y * y ) );
	}

	Vector3 normalize ( )
	{
		Vector3 out = *this;
		auto l = length ( );
		if ( l == 0 )
			return *this;
		;
		out.x /= l;
		out.y /= l;
		out.z /= l;
		return out;
	}

	float unity_magnitude ( )
	{
		return ( float ) sqrt ( ( double ) ( x * x + y * y + z * z ) );
	}

	Vector3 Cross ( Vector3 rhs )
	{
		return Vector3 ( y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x );
	}

	Vector3 unity_Normalize ( )
	{
		float num = unity_magnitude ( );
		if ( num > 1E-05f )
		{
			x /= num;
			y /= num;
			z /= num;
		}
		else
		{
			x = 0;
			y = 0;
			z = 0;
		}

		return { x,y,z };
	}

	float Magnitude ( ) { return sqrt ( this->x * this->x + this->y * this->y + this->z * this->z ); }
	Vector3 Normalized ( )
	{
		float num = this->Magnitude ( );
		if ( num > 9.99999974737875E-06 )
		{
			return { this->x / num, this->y / num, this->z / num };
		}
		return Vector3 {};
	}


	Vector3 normalized ( ) const
	{
		return { x == 0 ? 0 : x / length ( ), y == 0 ? 0 : y / length ( ), z == 0 ? 0 : z / length ( ) };
	}

	float dot_product ( Vector3 input ) const
	{
		return ( x * input.x ) + ( y * input.y ) + ( z * input.z );
	}

	float distance ( Vector3 input ) const
	{
		return ( *this - input ).length ( );
	}
	float distancesqr ( Vector3 input ) const
	{
		Vector3 dif = { x - input.x, y - input.y, z - input.z };
		return dif.x * dif.x + dif.y * dif.y + dif.z * dif.z;
	}

	float distance_2d ( Vector3 input ) const
	{
		return ( *this - input ).length_2d ( );
	}

	void clamp ( )
	{
		static_cast< void >( std::clamp ( x, -89.f, 89.f ) );
		static_cast< void >( std::clamp ( y, -180.f, 180.f ) );

		z = 0.f;
	}

	bool empty ( ) const
	{
		return ( x == 0.f && y == 0.f && z == 0.f ) || ( x == -1.f && y == -1.f && z == -1.f );
	}

	std::string stringify ( )
	{
		std::stringstream penis;
		penis << std::to_string ( x );
		penis << " ";
		penis << std::to_string ( y );
		penis << " ";
		penis << std::to_string ( z );
		return penis.str ( );
	}

	template<typename T>
	T ToDegrees ( T radians )
	{
		auto RadPi = 3.14159265358979323846;
		auto DegPi = 180.0;

		return ( radians * ( static_cast< T >( DegPi ) / static_cast< T >( RadPi ) ) );
	}

	Vector3 ToEulerAngles ( Vector3* pseudoup = nullptr )
	{
		auto pitch = 0.0f;
		auto yaw = 0.0f;
		auto roll = 0.0f;

		// HEIL HITLER!//
		// SIEG HEIL, SIEG HEIL, SIEG HEIL!
		// ADE, ADE, ADE . . .

		auto length = Vector3 ( x, y, z ).length ( );

		if ( pseudoup )
		{
			auto left = pseudoup->Cross ( *this );

			left.normalize ( );

			pitch = ToDegrees ( std::atan2 ( -this->z, length ) );

			if ( pitch < 0.0f )
				pitch += 360.0f;

			if ( length > 0.001f )
			{
				yaw = ToDegrees ( std::atan2 ( this->y, this->x ) );

				if ( yaw < 0.0f )
					yaw += 360.0f;

				auto up_z = ( this->x * left.y ) - ( this->y * left.x );

				roll = ToDegrees ( std::atan2 ( left.z, up_z ) );

				if ( roll < 0.0f )
					roll += 360.0f;
			}
			else
			{
				yaw = ToDegrees ( std::atan2 ( -left.x, left.y ) );

				if ( yaw < 0.0f )
					yaw += 360.0f;
			}
		}
		else
		{
			if ( this->x == 0.0f && this->y == 0.0f )
			{
				if ( this->z > 0.0f )
					pitch = 270.0f;
				else
					pitch = 90.0f;
			}
			else
			{
				pitch = ToDegrees ( std::atan2 ( -this->z, length ) );

				if ( pitch < 0.0f )
					pitch += 360.0f;

				yaw = ToDegrees ( std::atan2 ( this->y, this->x ) );

				if ( yaw < 0.0f )
					yaw += 360.0f;
			}
		}

		return { pitch, yaw, roll };
	}
};

class Vector4
{
public:
	Vector4 ( )
	{
		x = y = z = w = 0.f;
	}

	Vector4 ( float fx, float fy, float fz, float fw )
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	float x, y, z, w;

	Vector4 operator+( const Vector4& input ) const
	{
		return Vector4 { x + input.x, y + input.y, z + input.z, w + input.w };
	}

	Vector4 operator-( const Vector4& input ) const
	{
		return Vector4 { x - input.x, y - input.y, z - input.z, w - input.w };
	}

	Vector4 operator/( float input ) const
	{
		return Vector4 { x / input, y / input, z / input, w / input };
	}

	Vector4 operator*( float input ) const
	{
		return Vector4 { x * input, y * input, z * input, w * input };
	}

	Vector4& operator-=( const Vector4& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}

	Vector4& operator/=( float input )
	{
		x /= input;
		y /= input;
		z /= input;
		w /= input;
		return *this;
	}

	Vector4& operator*=( float input )
	{
		x *= input;
		y *= input;
		z *= input;
		w *= input;
		return *this;
	}

	bool operator==( const Vector4& input ) const
	{
		return x == input.x && y == input.y && z == input.z && w == input.w;
	}

	void make_absolute ( )
	{
		x = std::abs ( x );
		y = std::abs ( y );
		z = std::abs ( z );
		w = std::abs ( w );
	}

	float length_sqr ( ) const
	{
		return ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
	}

	float length ( ) const
	{
		return std::sqrt ( length_sqr ( ) );
	}

	float length_2d ( ) const
	{
		return std::sqrt ( ( x * x ) + ( y * y ) );
	}

	Vector4 normalized ( ) const
	{
		return { x / length ( ), y / length ( ), z / length ( ), w / length ( ) };
	}

	float dot_product ( Vector4 input ) const
	{
		return ( x * input.x ) + ( y * input.y ) + ( z * input.z ) + ( w * input.w );
	}

	float distance ( Vector4 input ) const
	{
		return ( *this - input ).length ( );
	}

	float distance_2d ( Vector4 input ) const
	{
		return ( *this - input ).length_2d ( );
	}
	void clamp ( )
	{
		static_cast< void >( std::clamp ( x, -89.f, 89.f ) );
		static_cast< void >( std::clamp ( y, -180.f, 180.f ) );

		z = 0.f;
		w = 0.f;
	}

	bool empty ( ) const
	{
		return x == 0.f && y == 0.f && z == 0.f && w == 0.f;
	}

	static Vector4 QuaternionLookRotation ( Vector3 forward, Vector3 up )
	{
		Vector3 vector = forward.unity_Normalize ( );
		Vector3 vector2 = ( up ).Cross ( vector ).unity_Normalize ( );
		Vector3 vector3 = ( vector ).Cross ( vector2 );
		auto m00 = vector2.x;
		auto m01 = vector2.y;
		auto m02 = vector2.z;
		auto m10 = vector3.x;
		auto m11 = vector3.y;
		auto m12 = vector3.z;
		auto m20 = vector.x;
		auto m21 = vector.y;
		auto m22 = vector.z;


		float num8 = ( m00 + m11 ) + m22;
		auto quaternion = Vector4 ( );
		if ( num8 > 0.f )
		{
			auto num = ( float ) sqrt ( num8 + 1.f );
			quaternion.w = num * 0.5f;
			num = 0.5f / num;
			quaternion.x = ( m12 - m21 ) * num;
			quaternion.y = ( m20 - m02 ) * num;
			quaternion.z = ( m01 - m10 ) * num;
			return quaternion;
		}
		if ( ( m00 >= m11 ) && ( m00 >= m22 ) )
		{
			auto num7 = ( float ) sqrt ( ( ( 1.f + m00 ) - m11 ) - m22 );
			auto num4 = 0.5f / num7;
			quaternion.x = 0.5f * num7;
			quaternion.y = ( m01 + m10 ) * num4;
			quaternion.z = ( m02 + m20 ) * num4;
			quaternion.w = ( m12 - m21 ) * num4;
			return quaternion;
		}
		if ( m11 > m22 )
		{
			auto num6 = ( float ) sqrt ( ( ( 1.f + m11 ) - m00 ) - m22 );
			auto num3 = 0.5f / num6;
			quaternion.x = ( m10 + m01 ) * num3;
			quaternion.y = 0.5f * num6;
			quaternion.z = ( m21 + m12 ) * num3;
			quaternion.w = ( m20 - m02 ) * num3;
			return quaternion;
		}
		auto num5 = ( float ) sqrt ( ( ( 1.f + m22 ) - m00 ) - m11 );
		auto num2 = 0.5f / num5;
		quaternion.x = ( m20 + m02 ) * num2;
		quaternion.y = ( m21 + m12 ) * num2;
		quaternion.z = 0.5f * num5;
		quaternion.w = ( m01 - m10 ) * num2;
		return quaternion;
	}



};

class Matrix4x4
{
public:
	inline float* operator[]( int i )
	{
		return m [i];
	}

	inline const float* operator[]( int i ) const
	{
		return m [i];
	}

	inline float* Base ( )
	{
		return &m [0] [0];
	}

	inline const float* Base ( ) const
	{
		return &m [0] [0];
	}

	inline Vector3 MultiplyPoint3x4 ( Vector3 point )
	{
		Vector3 result;
		result.x = m [0] [0] * point.x + m [0] [1] * point.y + m [0] [2] * point.z + m [0] [3];
		result.y = m [1] [0] * point.x + m [1] [1] * point.y + m [1] [2] * point.z + m [1] [3];
		result.z = m [2] [0] * point.x + m [2] [1] * point.y + m [2] [2] * point.z + m [2] [3];
		return result;
	}
public:

	inline Matrix4x4 ( )
	{
		Init (
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f
		);
	}

	inline Matrix4x4 (
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33 )
	{
		Init (
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33
		);
	}

	inline void Init (
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	)
	{
		m [0] [0] = m00;
		m [0] [1] = m01;
		m [0] [2] = m02;
		m [0] [3] = m03;

		m [1] [0] = m10;
		m [1] [1] = m11;
		m [1] [2] = m12;
		m [1] [3] = m13;

		m [2] [0] = m20;
		m [2] [1] = m21;
		m [2] [2] = m22;
		m [2] [3] = m23;

		m [3] [0] = m30;
		m [3] [1] = m31;
		m [3] [2] = m32;
		m [3] [3] = m33;
	}

	Matrix4x4 transpose ( ) const
	{
		return Matrix4x4 (
			m [0] [0], m [1] [0], m [2] [0], m [3] [0],
			m [0] [1], m [1] [1], m [2] [1], m [3] [1],
			m [0] [2], m [1] [2], m [2] [2], m [3] [2],
			m [0] [3], m [1] [3], m [2] [3], m [3] [3] );
	}

	float m [4] [4];
};

inline Vector3 quatmult ( const Vector3* point, Vector4* quat )
{
	float num = quat->x * 2.f;
	float num2 = quat->y * 2.f;
	float num3 = quat->z * 2.f;
	float num4 = quat->x * num;
	float num5 = quat->y * num2;
	float num6 = quat->z * num3;
	float num7 = quat->x * num2;
	float num8 = quat->x * num3;
	float num9 = quat->y * num3;
	float num10 = quat->w * num;
	float num11 = quat->w * num2;
	float num12 = quat->w * num3;
	Vector3 result;
	result.x = ( 1.f - ( num5 + num6 ) ) * point->x + ( num7 - num12 ) * point->y + ( num8 + num11 ) * point->z;
	result.y = ( num7 + num12 ) * point->x + ( 1.f - ( num4 + num6 ) ) * point->y + ( num9 - num10 ) * point->z;
	result.z = ( num8 - num11 ) * point->x + ( num9 + num10 ) * point->y + ( 1.f - ( num4 + num5 ) ) * point->z;
	return result;
}

const Vector3 vecright = { 1.f, 0.f, 0.f };
const Vector3 vecforward = { 0.f, 0.f, 1.f };
const Vector3 vecup = { 0.f, 1.f, 0.f };


namespace math_additional
{
	float Clamp01 ( float value );
	float Lerp ( Vector3 in );
};

class Vector2
{
public:
	Vector2 ( )
	{
		x = y = 0.f;
	}

	Vector2 ( float fx, float fy )
	{
		x = fx;
		y = fy;
	}

	float x, y;

	Vector2 operator+( const Vector2& input ) const
	{
		return Vector2 { x + input.x, y + input.y };
	}

	Vector2 operator-( const Vector2& input ) const
	{
		return Vector2 { x - input.x, y - input.y };
	}

	Vector2 operator+( const Vector3& input ) const
	{
		return Vector2 { x + input.x, y + input.y };
	}

	Vector2 operator-( const Vector3& input ) const
	{
		return Vector2 { x - input.x, y - input.y };
	}

	Vector2 operator/( float input ) const
	{
		return Vector2 { x / input, y / input };
	}

	Vector2 operator*( float input ) const
	{
		return Vector2 { x * input, y * input };
	}

	Vector2& operator-=( const Vector2& v )
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2& operator/=( float input )
	{
		x /= input;
		y /= input;
		return *this;
	}

	Vector2& operator*=( float input )
	{
		x *= input;
		y *= input;
		return *this;
	}

	Vector2& operator += ( const Vector2& v )
	{
		this->x += v.x;
		this->y += v.y;

		return ( *this );
	}

	Vector2 operator + ( float fl ) const
	{
		return { this->x + fl,
				  this->y + fl };
	}

	Vector2 operator - ( float fl ) const
	{
		return { this->x - fl,
				  this->y - fl };
	}

	float length ( ) const
	{
		return std::sqrt ( ( x * x ) + ( y * y ) );
	}

	Vector2 normalized ( ) const
	{
		return { x / length ( ), y / length ( ) };
	}

	float dot_product ( Vector2 input ) const
	{
		return ( x * input.x ) + ( y * input.y );
	}

	float distance ( Vector2 input ) const
	{
		return ( *this - input ).length ( );
	}

	bool empty ( ) const
	{
		return x == 0.f && y == 0.f;
	}

	std::string stringify ( )
	{
		std::stringstream penis;
		penis << std::to_string ( x );
		penis << " ";
		penis << std::to_string ( y );
		penis << " ";
		return penis.str ( );
	}
};

class QAngle
{
public:
	QAngle ( );
	QAngle ( float x, float y, float z );
	QAngle ( const QAngle& v );
	QAngle ( const float* v );

public:
	void Set ( float x = 0.0f, float y = 0.0f, float z = 0.0f );

	void Normalize ( );
	void Clamp ( );

	bool IsZero ( float tolerance = 0.01f );
	bool AngleAreEqual ( QAngle angle, float tolerance = 0.01f );

	QAngle Normalized ( );
	QAngle Clamped ( );

	QAngle Direction ( )
	{
		return QAngle ( cos ( y * 3.14159265358979323846 / 180.0f ) * cos ( x * 3.14159265358979323846 / 180.0f ), sin ( y * 3.14159265358979323846 / 180.0f ) * cos ( x * 3.14159265358979323846 / 180.0f ), sin ( -x * 3.14159265358979323846 / 180.0f ) ).Normalized ( );
	}

	QAngle Forward ( )
	{
		return Direction ( );
	}

	QAngle Up ( )
	{
		return QAngle ( cos ( y * 3.14159265358979323846 / 180.0f ) * cos ( x * 3.14159265358979323846 / 180.0f ), sin ( y * 3.14159265358979323846 / 180.0f ) * cos ( x * 3.14159265358979323846 / 180.0f ), sin ( -( x - 90.0f ) * 3.14159265358979323846 / 180.0f ) ).Normalized ( );
	}

	QAngle Right ( )
	{
		return QAngle ( cos ( ( y + 90.f ) * 3.14159265358979323846 / 180.0f ) * cos ( x * 3.14159265358979323846 / 180.0f ), sin ( ( y + 90.f ) * 3.14159265358979323846 / 180.0f ) * cos ( x * 3.14159265358979323846 / 180.0f ), sin ( -x * 3.14159265358979323846 / 180.0f ) ).Normalized ( );
	}

	Vector3 ToVectors ( Vector3* side = nullptr, Vector3* up = nullptr );
	Vector3 ToVectorsTranspose ( Vector3* side = nullptr, Vector3* up = nullptr );

public:
	float operator [] ( const std::uint32_t index );
	const float operator [] ( const std::uint32_t index ) const;

	QAngle& operator = ( const QAngle& v );
	QAngle& operator = ( const float* v );

	QAngle& operator += ( const QAngle& v );
	QAngle& operator -= ( const QAngle& v );
	QAngle& operator *= ( const QAngle& v );
	QAngle& operator /= ( const QAngle& v );

	QAngle& operator += ( float fl );
	QAngle& operator -= ( float fl );
	QAngle& operator *= ( float fl );
	QAngle& operator /= ( float fl );

	QAngle operator + ( const QAngle& v ) const;
	QAngle operator - ( const QAngle& v ) const;
	QAngle operator * ( const QAngle& v ) const;
	QAngle operator / ( const QAngle& v ) const;

	QAngle operator + ( float fl ) const;
	QAngle operator - ( float fl ) const;
	QAngle operator * ( float fl ) const;
	QAngle operator / ( float fl ) const;

public:
	static QAngle Zero;

public:
	union
	{
		struct
		{
			float pitch;
			float yaw;
			float roll;
		};

		struct
		{
			float x;
			float y;
			float z;
		};
	};
};