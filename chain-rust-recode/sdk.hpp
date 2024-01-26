#pragma once
#include "stdafx.hpp"

class Matrix4x4;
class Vector4;
class Vector3;
class Vector2;

namespace rust
{
	uintptr_t get_base_networkable ( CSharedMemComms* drv );
	uintptr_t get_client_entities ( CSharedMemComms* drv );
	uintptr_t get_entity_realm ( CSharedMemComms* drv );
	uintptr_t get_buffer_list ( CSharedMemComms* drv );
	uintptr_t get_object_list ( CSharedMemComms* drv );
	uintptr_t get_base_entity ( CSharedMemComms* drv, uintptr_t Object );
	uintptr_t get_model_state ( CSharedMemComms* drv, uintptr_t BaseEntity );
	uintptr_t get_player_visual ( CSharedMemComms* drv, uintptr_t Object );
	uintptr_t get_object_pos_component ( CSharedMemComms* drv, uintptr_t Object );


	Vector3 get_object_position ( CSharedMemComms* drv, uintptr_t Object );
	Vector3 get_bone_position ( CSharedMemComms* drv, uintptr_t Entity, unsigned int bone );
	Vector3 get_bone_position_from_transform ( CSharedMemComms* drv, uintptr_t transform, uintptr_t Entity, unsigned int bone );
	Matrix4x4 get_view_matrix ( CSharedMemComms* drv );

	Vector3 get_camera_position ( CSharedMemComms* drv );

	bool world_to_screen ( const Vector3& element_position, Vector2& screen_position, Matrix4x4 view_matrix, bool ignore_incorrect = false );
	bool use_weapon_prediction ( );

	uintptr_t scan_for_class ( CSharedMemComms* drv, const char* name );

	uintptr_t mono_field_static_get_value ( CSharedMemComms* drv, uintptr_t klass, uintptr_t offset );

	Vector3 euler_angles ( Vector4 q1 );

	Vector3 rotate_point ( Vector3 center, Vector3 origin, float angle );

	Vector2 RotatePoint ( Vector2 pointToRotate, Vector2 centerPoint, float angle, bool angleInRadians );

	float get_bullet_speed ( );
	float get_bullet_gravity ( );

	float get_bullet_drag ( );

	unsigned int get_object_size ( CSharedMemComms* drv );

	std::string get_class_name ( CSharedMemComms* drv, uintptr_t Object );
	std::string get_object_name ( CSharedMemComms* drv, uintptr_t Object );
};