#include "stdafx.hpp"
#include "unity.hpp"

#pragma warning( push )
#pragma warning( disable : 4556 )
#pragma warning( push )
#pragma warning( disable : 26451 )

struct Vec4
{
	float x, y, z, w;
};

struct TransformAccessReadOnly
{
	ULONGLONG pTransformData;
	int index;
};

struct TransformData
{
	ULONGLONG pTransformArray;
	ULONGLONG pTransformIndices;
};

struct Matrix34
{
	Vec4 vec0;
	Vec4 vec1;
	Vec4 vec2;
};

uintptr_t unity::get_component ( CSharedMemComms* drv, uintptr_t gameObject, const char* componentNameStr )
{
	if ( !gameObject )
		return NULL;

	uintptr_t componentList = drv->read<uintptr_t> ( gameObject + 0x30 );
	for ( int h = 0; h < 20; h++ ) // Dunno where component count is, don't care. ItemModProjectile is usually 3rd component in list.
	{
		uintptr_t component = drv->read<uintptr_t> ( componentList + ( 0x10 * h + 0x8 ) );
		if ( !component )
			continue;

		uintptr_t unk1 = drv->read<uintptr_t> ( component + 0x28 );
		if ( !unk1 )
			continue;

		uintptr_t componentName = drv->read<uintptr_t> ( unk1 + 0x0 );

		std::string name = drv->read_ascii ( drv->read<uintptr_t> ( componentName + 0x10 ), 18 );
		if ( strcmp ( name.c_str ( ), componentNameStr ) == 0 )
			return unk1;
	}

	return NULL;
}

Vector3 unity::get_position_injected ( CSharedMemComms* drv, const uintptr_t transform )
{
	__m128 result;

	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	TransformAccessReadOnly pTransformAccessReadOnly { };
	pTransformAccessReadOnly = drv->read< TransformAccessReadOnly > ( uintptr_t ( transform + 0x38 ) );

	TransformData transformData { };
	transformData = drv->read< TransformData > ( uintptr_t ( pTransformAccessReadOnly.pTransformData + 0x18 ) );
	if ( !transformData.pTransformArray || !transformData.pTransformIndices )
		return {};

	size_t sizeMatriciesBuf = sizeof ( Matrix34 ) * pTransformAccessReadOnly.index + sizeof ( Matrix34 );
	size_t sizeIndicesBuf = sizeof ( int ) * pTransformAccessReadOnly.index + sizeof ( int );

	// Allocate memory for storing large amounts of data (matricies and indicies)
	PVOID pMatriciesBuf = malloc ( sizeof ( ULONG_PTR ) + sizeMatriciesBuf );
	if ( !pMatriciesBuf )
		return {};

	PVOID pIndicesBuf = malloc ( sizeof ( ULONG_PTR ) + sizeIndicesBuf );
	if ( !pIndicesBuf )
	{
		free ( pMatriciesBuf );
		return {};
	}

	if ( pMatriciesBuf && pIndicesBuf )
	{
		if ( !drv->read ( uintptr_t ( transformData.pTransformArray ), pMatriciesBuf, sizeMatriciesBuf ) )
		{
			free ( pMatriciesBuf );
			free ( pIndicesBuf );
			return {};
		}

		if ( !drv->read ( uintptr_t ( transformData.pTransformIndices ), pIndicesBuf, sizeIndicesBuf ) )
		{
			free ( pMatriciesBuf );
			free ( pIndicesBuf );
			return {};
		}

		result = *( __m128* )( ( ULONGLONG ) pMatriciesBuf + 0x30 * pTransformAccessReadOnly.index );
		int transformIndex = *( int* ) ( ( ULONGLONG ) pIndicesBuf + 0x4 * pTransformAccessReadOnly.index );

		while ( transformIndex >= 0 )
		{
			Matrix34 matrix34 = *( Matrix34* ) ( ( ULONGLONG ) pMatriciesBuf + 0x30 * transformIndex );

			__m128 xxxx = _mm_castsi128_ps ( _mm_shuffle_epi32 ( *( __m128i* )( &matrix34.vec1 ), 0x00 ) );	// xxxx
			__m128 yyyy = _mm_castsi128_ps ( _mm_shuffle_epi32 ( *( __m128i* )( &matrix34.vec1 ), 0x55 ) );	// yyyy
			__m128 zwxy = _mm_castsi128_ps ( _mm_shuffle_epi32 ( *( __m128i* )( &matrix34.vec1 ), 0x8E ) );	// zwxy
			__m128 wzyw = _mm_castsi128_ps ( _mm_shuffle_epi32 ( *( __m128i* )( &matrix34.vec1 ), 0xDB ) );	// wzyw
			__m128 zzzz = _mm_castsi128_ps ( _mm_shuffle_epi32 ( *( __m128i* )( &matrix34.vec1 ), 0xAA ) );	// zzzz
			__m128 yxwy = _mm_castsi128_ps ( _mm_shuffle_epi32 ( *( __m128i* )( &matrix34.vec1 ), 0x71 ) );	// yxwy
			__m128 tmp7 = _mm_mul_ps ( *( __m128* )( &matrix34.vec2 ), result );

			result = _mm_add_ps (
				_mm_add_ps (
					_mm_add_ps (
						_mm_mul_ps (
							_mm_sub_ps (
								_mm_mul_ps ( _mm_mul_ps ( xxxx, mulVec1 ), zwxy ),
								_mm_mul_ps ( _mm_mul_ps ( yyyy, mulVec2 ), wzyw ) ),
							_mm_castsi128_ps ( _mm_shuffle_epi32 ( _mm_castps_si128 ( tmp7 ), 0xAA ) ) ),
						_mm_mul_ps (
							_mm_sub_ps (
								_mm_mul_ps ( _mm_mul_ps ( zzzz, mulVec2 ), wzyw ),
								_mm_mul_ps ( _mm_mul_ps ( xxxx, mulVec0 ), yxwy ) ),
							_mm_castsi128_ps ( _mm_shuffle_epi32 ( _mm_castps_si128 ( tmp7 ), 0x55 ) ) ) ),
					_mm_add_ps (
						_mm_mul_ps (
							_mm_sub_ps (
								_mm_mul_ps ( _mm_mul_ps ( yyyy, mulVec0 ), yxwy ),
								_mm_mul_ps ( _mm_mul_ps ( zzzz, mulVec1 ), zwxy ) ),
							_mm_castsi128_ps ( _mm_shuffle_epi32 ( _mm_castps_si128 ( tmp7 ), 0x00 ) ) ),
						tmp7 ) ), *( __m128* )( &matrix34.vec0 ) );

			transformIndex = *( int* ) ( ( ULONGLONG ) pIndicesBuf + 0x4 * transformIndex );
		}

		free ( pMatriciesBuf );
		free ( pIndicesBuf );
	}

	Vector3 out = *reinterpret_cast< Vector3* >( &result.m128_f32 [0] );
	return out;
}

#pragma warning( pop )
#pragma warning( pop )