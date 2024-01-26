#include "cheat.hpp"
#include "stdafx.hpp"
#include "sdk.hpp"

uintptr_t rust::get_base_networkable ( CSharedMemComms* drv )
{
	uintptr_t address = addr::gameassembly + offsets::bn_offset;
	uintptr_t basenetworkable = drv->read<uintptr_t> ( address );

	if ( !basenetworkable )
		return NULL;

	return basenetworkable;
}

uintptr_t rust::get_client_entities ( CSharedMemComms* drv )
{
	uintptr_t address = addr::base_networkable + offsets::cl_ent;
	uintptr_t client_entities = drv->read<uintptr_t> ( address );
	if ( !client_entities )
		return NULL;

	return client_entities;
}

uintptr_t rust::get_entity_realm ( CSharedMemComms* drv )
{
	uintptr_t address = drv->read<uintptr_t> ( addr::client_entities ) + offsets::ent_realm;
	uintptr_t entity_realm = drv->read<uintptr_t> ( address );
	if ( !entity_realm )
		return NULL;

	return entity_realm;
}

uintptr_t rust::get_buffer_list ( CSharedMemComms* drv )
{
	uintptr_t address = addr::entity_realm + offsets::buf_list;
	uintptr_t buffer_list = drv->read<uintptr_t> ( address );
	if ( !buffer_list )
		return NULL;

	return buffer_list;
}

uintptr_t rust::get_object_list ( CSharedMemComms* drv )
{
	uintptr_t address = addr::buffer_list + offsets::obj_list;
	uintptr_t object_list = drv->read<uintptr_t> ( address );
	if ( !object_list )
		return NULL;

	return object_list;
}

unsigned int rust::get_object_size ( CSharedMemComms* drv )
{
	uintptr_t address = addr::buffer_list + offsets::obj_size;
	unsigned int object_size = drv->read<unsigned int> ( address );
	if ( !object_size )
		return NULL;

	return object_size;
}

uintptr_t rust::get_base_entity ( CSharedMemComms* drv, uintptr_t element )
{
	uintptr_t unk = drv->read<uintptr_t> ( element + 0x18 );
	return drv->read<uintptr_t> ( unk + offsets::base_ply );
}

uintptr_t rust::get_model_state ( CSharedMemComms* drv, uintptr_t baseplayer )
{
	return drv->read<uintptr_t> ( baseplayer + offsets::modelState );
}

std::string rust::get_class_name ( CSharedMemComms* drv, uintptr_t element )
{
	uintptr_t object = drv->read<uintptr_t> ( element );
	if ( !object )
		return {};

	uintptr_t pName = drv->read<uintptr_t> ( object + offsets::class_name );
	if ( !pName )
		return {};

	std::string nice = drv->read_ascii ( pName, 32 );

	return nice;
}

uintptr_t rust::get_player_visual ( CSharedMemComms* drv, uintptr_t element )
{
	return drv->read <uintptr_t> ( element + offsets::player_visual );
}

uintptr_t rust::get_object_pos_component ( CSharedMemComms* drv, uintptr_t element )
{
	uintptr_t visual = rust::get_player_visual ( drv, element );
	if ( !visual )
		return NULL;

	return drv->read<uintptr_t> ( visual + offsets::obj_state ); // state
}

Vector3 rust::get_object_position ( CSharedMemComms* drv, uintptr_t element )
{
	uintptr_t state = rust::get_object_pos_component ( drv, element );
	if ( !state )
		return { -1, -1, -1 };

	return drv->read<Vector3> ( state + offsets::obj_pos );
}

Vector3 rust::get_bone_position ( CSharedMemComms* drv, uintptr_t Entity, unsigned int bone )
{
	uintptr_t player_model = drv->read<uintptr_t> ( Entity + offsets::model );
	uintptr_t transform = drv->read<uintptr_t> ( player_model + 0x48 );
	uintptr_t ogbones = drv->read<uintptr_t> ( transform + ( 0x20 + ( ( bone ) * 0x8 ) ) );
	uintptr_t bones = drv->read<uintptr_t> ( ogbones + 0x10 );
	Vector3 result = drv->read< Vector3 > ( bones );
	return result;
}

Vector3 rust::get_bone_position_from_transform ( CSharedMemComms* drv, uintptr_t transform, uintptr_t Entity, unsigned int bone )
{
	uintptr_t ogbones = drv->read<uintptr_t> ( transform + ( 0x20 + ( ( bone ) * 0x8 ) ) );
	uintptr_t bones = drv->read<uintptr_t> ( ogbones + 0x10 );
	return drv->read< Vector3 > ( bones );
}

Matrix4x4 rust::get_view_matrix ( CSharedMemComms* drv )
{
	if ( !addr::camera_instance )
	{
		uintptr_t gameobjectmanager = drv->read<uintptr_t> ( addr::unityplayer + offsets::gom_offset );
		if ( !gameobjectmanager )
			return {};

		uintptr_t tagged_objects = drv->read<uintptr_t> ( gameobjectmanager + 0x8 );
		if ( !tagged_objects )
			return {};

		uintptr_t game_object = drv->read<uintptr_t> ( tagged_objects + 0x10 );
		if ( !game_object )
			return {};

		uintptr_t object_class = drv->read<uintptr_t> ( game_object + 0x30 );
		if ( !object_class )
			return {};

		addr::camera_instance = drv->read<uintptr_t> ( object_class + 0x18 );
	}

	if ( addr::camera_instance )
		return drv->read<Matrix4x4> ( addr::camera_instance + 0x2E4 );
	else
		return {};
}

Vector3 rust::get_camera_position ( CSharedMemComms* drv )
{
	if ( !addr::camera_instance )
	{
		uintptr_t gameobjectmanager = drv->read<uintptr_t> ( addr::unityplayer + offsets::gom_offset );
		if ( !gameobjectmanager )
			return {};

		uintptr_t tagged_objects = drv->read<uintptr_t> ( gameobjectmanager + 0x8 );
		if ( !tagged_objects )
			return {};

		uintptr_t game_object = drv->read<uintptr_t> ( tagged_objects + 0x10 );
		if ( !game_object )
			return {};

		uintptr_t object_class = drv->read<uintptr_t> ( game_object + 0x30 );
		if ( !object_class )
			return {};

		addr::camera_instance = drv->read<uintptr_t> ( object_class + 0x18 );
	}

	if ( addr::camera_instance )
		return drv->read<Vector3> ( addr::camera_instance + 0x42C );
	else
		return {};
}

bool rust::world_to_screen ( const Vector3& element_position, Vector2& screen_position, Matrix4x4 view_matrix, bool ignore_incorrect )
{
	if ( element_position.empty ( ) )
		return false;

	Vector3 trans_vec { view_matrix [0] [3], view_matrix [1] [3], view_matrix [2] [3] };
	Vector3 right_vec { view_matrix [0] [0], view_matrix [1] [0], view_matrix [2] [0] };
	Vector3 up_vec { view_matrix [0] [1], view_matrix [1] [1], view_matrix [2] [1] };

	const float w = trans_vec.dot_product ( element_position ) + view_matrix [3] [3];
	bool bad = false;

	if ( w < 1.f )
	{
		if ( !ignore_incorrect )
			return false;
		else
			bad = true;
	}

	float x = right_vec.dot_product ( element_position ) + view_matrix [3] [0];
	float y = up_vec.dot_product ( element_position ) + view_matrix [3] [1];

	Vector2 screen_pos = { ( monitor::width / 2 ) * ( 1.f + x / w ), ( monitor::height / 2 ) * ( 1.f - y / w ) };
	screen_position = screen_pos;

	if ( screen_pos.x >= monitor::width || screen_pos.y >= monitor::height || screen_pos.x <= 0 || screen_pos.y <= 0 || bad )
		return false;

	return true;
}

inline float bullet_gravity ( const char* bullet_name )
{
	if ( _stricmp ( bullet_name, _ ( "ammo.rifle" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.rifle.hv" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.rifle.explosive" ) ) == NULL )
	{
		return 1.25f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.rifle.incendiary" ) ) == NULL )
	{
		return 1.f;
	}

	if ( _stricmp ( bullet_name, _ ( "ammo.pistol" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.pistol.hv" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.pistol.incendiary" ) ) == NULL )
	{
		return 1.f;
	}

	if ( _stricmp ( bullet_name, _ ( "arrow.hv" ) ) == NULL )
	{
		return 0.5f;
	}
	if ( _stricmp ( bullet_name, _ ( "arrow.bone" ) ) == NULL )
	{
		return 0.75f;
	}
	if ( _stricmp ( bullet_name, _ ( "arrow.wooden" ) ) == NULL )
	{
		return 0.75f;
	}
	if ( _stricmp ( bullet_name, _ ( "arrow.fire" ) ) == NULL )
	{
		return 1.f;
	}

	if ( _stricmp ( bullet_name, _ ( "ammo.handmade.shell" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.shotgun" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.shotgun.fire" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.shotgun.slug" ) ) == NULL )
	{
		return 1.f;
	}

	if ( _stricmp ( bullet_name, _ ( "ammo.nailgun.nails" ) ) == NULL )
	{
		return 0.75f;
	}

	return 1.f;
}

inline float get_bullet_dragg ( const char* bullet_name )
{
	if ( _stricmp ( bullet_name, _ ( "ammo.rifle" ) ) == NULL )
	{
		return 0.6f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.rifle.hv" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.rifle.explosive" ) ) == NULL )
	{
		return 0.6f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.rifle.incendiary" ) ) == NULL )
	{
		return 0.6f;
	}

	if ( _stricmp ( bullet_name, _ ( "ammo.pistol" ) ) == NULL )
	{
		return 0.7f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.pistol.hv" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.pistol.incendiary" ) ) == NULL )
	{
		return 0.7f;
	}

	if ( _stricmp ( bullet_name, _ ( "arrow.hv" ) ) == NULL )
	{
		return 0.005f;
	}
	if ( _stricmp ( bullet_name, _ ( "arrow.bone" ) ) == NULL )
	{
		return 0.01f;
	}
	if ( _stricmp ( bullet_name, _ ( "arrow.wooden" ) ) == NULL )
	{
		return 0.005f;
	}
	if ( _stricmp ( bullet_name, _ ( "arrow.fire" ) ) == NULL )
	{
		return 0.01f;
	}

	if ( _stricmp ( bullet_name, _ ( "ammo.handmade.shell" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.shotgun" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.shotgun.fire" ) ) == NULL )
	{
		return 1.f;
	}
	if ( _stricmp ( bullet_name, _ ( "ammo.shotgun.slug" ) ) == NULL )
	{
		return 0.6f;
	}

	if ( _stricmp ( bullet_name, _ ( "ammo.nailgun.nails" ) ) == NULL )
	{
		return 0.005f;
	}

	return 1.f;
}

inline float get_bullet_velocity ( const char* weapon_name, const char* bullet_name )
{
	/* rifle ammo */

	if ( _stricmp ( weapon_name, _ ( "rifle.ak" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "rifle.lr300" ) ) == NULL )
	{

		if ( _stricmp ( bullet_name, _ ( "ammo.rifle" ) ) == NULL )
		{
			return 375.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.hv" ) ) == NULL )
		{
			return 450.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.explosive" ) ) == NULL )
		{
			return 225.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.incendiary" ) ) == NULL )
		{
			return 225.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "rifle.bolt" ) ) == NULL )
	{
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle" ) ) == NULL )
		{
			return 656.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.hv" ) ) == NULL )
		{
			return 788.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.explosive" ) ) == NULL )
		{
			return 394.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.incendiary" ) ) == NULL )
		{
			return 394.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "rifle.l96" ) ) == NULL )
	{
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle" ) ) == NULL )
		{
			return 1125.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.hv" ) ) == NULL )
		{
			return 1350.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.explosive" ) ) == NULL )
		{
			return 675.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.incendiary" ) ) == NULL )
		{
			return 675.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "rifle.m39" ) ) == NULL )
	{
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle" ) ) == NULL )
		{
			return 469.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.hv" ) ) == NULL )
		{
			return 563.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.explosive" ) ) == NULL )
		{
			return 281.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.incendiary" ) ) == NULL )
		{
			return 281.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "rifle.semiauto" ) ) == NULL )
	{
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle" ) ) == NULL )
		{
			return 375.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.hv" ) ) == NULL )
		{
			return 450.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.explosive" ) ) == NULL )
		{
			return 225.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.incendiary" ) ) == NULL )
		{
			return 225.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "lmg.m249" ) ) == NULL )
	{
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle" ) ) == NULL )
		{
			return 487.5f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.hv" ) ) == NULL )
		{
			return 585.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.explosive" ) ) == NULL )
		{
			return 293.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.rifle.incendiary" ) ) == NULL )
		{
			return 293.f;
		}
	}

	/* pistol ammo */

	if ( _stricmp ( weapon_name, _ ( "smg.2" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "smg.mp5" ) ) == NULL )
	{

		if ( _stricmp ( bullet_name, _ ( "ammo.pistol" ) ) == NULL )
		{
			return 240.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.pistol.hv" ) ) == NULL )
		{
			return 320.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.pistol.incendiary" ) ) == NULL )
		{
			return 180.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "pistol.m92" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "pistol.python" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "pistol.revolver" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "pistol.semiauto" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "smg.thompson" ) ) == NULL )
	{

		if ( _stricmp ( bullet_name, _ ( "ammo.pistol" ) ) == NULL )
		{
			return 300.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.pistol.hv" ) ) == NULL )
		{
			return 400.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.pistol.incendiary" ) ) == NULL )
		{
			return 225.f;
		}
	}

	/* shotgun ammo */

	if ( _stricmp ( weapon_name, _ ( "shotgun.double" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "shotgun.pump" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "shotgun.waterpipe" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "shotgun.spas12" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "pistol.eoka" ) ) == NULL )
	{

		if ( _stricmp ( bullet_name, _ ( "ammo.shotgun.slug" ) ) == NULL )
		{
			return 225.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.shotgun.fire" ) ) == NULL )
		{
			return 100.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.handmade.shell" ) ) == NULL )
		{
			return 100.f;
		}
		if ( _stricmp ( bullet_name, _ ( "ammo.shotgun.slug" ) ) == NULL )
		{
			return 225.f;
		}
	}

	/* monkey arrows and monkey guns */

	if ( _stricmp ( weapon_name, _ ( "bow.hunting" ) ) == NULL )
	{
		if ( _stricmp ( bullet_name, _ ( "arrow.hv" ) ) == NULL )
		{
			return 80.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.bone" ) ) == NULL )
		{
			return 45.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.wooden" ) ) == NULL )
		{
			return 50.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.fire" ) ) == NULL )
		{
			return 40.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "bow.compound" ) ) == NULL )
	{
		if ( _stricmp ( bullet_name, _ ( "arrow.hv" ) ) == NULL )
		{
			return 160.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.bone" ) ) == NULL )
		{
			return 90.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.wooden" ) ) == NULL )
		{
			return 100.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.fire" ) ) == NULL )
		{
			return 80.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "crossbow" ) ) == NULL )
	{
		if ( _stricmp ( bullet_name, _ ( "arrow.hv" ) ) == NULL )
		{
			return 120.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.bone" ) ) == NULL )
		{
			return 68.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.wooden" ) ) == NULL )
		{
			return 75.f;
		}
		if ( _stricmp ( bullet_name, _ ( "arrow.fire" ) ) == NULL )
		{
			return 60.f;
		}
	}

	if ( _stricmp ( weapon_name, _ ( "pistol.nailgun" ) ) == NULL )
	{
		return 50.f;
	}

	return 1337.f;
}

inline float is_prediction_available ( const char* weapon_name )
{

	if ( _stricmp ( weapon_name, _ ( "rifle.ak" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "rifle.lr300" ) ) == NULL )
	{
		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "rifle.bolt" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "rifle.l96" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "rifle.m39" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "rifle.semiauto" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "lmg.m249" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "smg.2" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "smg.mp5" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "pistol.m92" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "pistol.python" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "pistol.revolver" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "pistol.semiauto" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "smg.thompson" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "shotgun.double" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "shotgun.pump" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "shotgun.waterpipe" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "shotgun.spas12" ) ) == NULL
		|| _stricmp ( weapon_name, _ ( "pistol.eoka" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "bow.hunting" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "bow.compound" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "crossbow" ) ) == NULL )
	{

		return true;
	}

	if ( _stricmp ( weapon_name, _ ( "pistol.nailgun" ) ) == NULL )
	{
		return true;
	}

	return false;
}

float rust::get_bullet_speed ( )
{
	return get_bullet_velocity ( game::m_rLocalInfo.m_szWeaponName.c_str ( ), game::m_rLocalInfo.m_szBulletType.c_str ( ) );
}

float rust::get_bullet_gravity ( )
{
	return bullet_gravity ( game::m_rLocalInfo.m_szBulletType.c_str ( ) );
}

float rust::get_bullet_drag ( )
{
	return get_bullet_dragg ( game::m_rLocalInfo.m_szBulletType.c_str ( ) );
}

bool rust::use_weapon_prediction ( )
{
	return is_prediction_available ( game::m_rLocalInfo.m_szWeaponName.c_str ( ) );
}

uintptr_t rust::scan_for_class ( CSharedMemComms* drv, const char* name )
{
	uintptr_t m_pBase = addr::gameassembly;
	IMAGE_DOS_HEADER m_idHeader = drv->read<IMAGE_DOS_HEADER> ( m_pBase );
	IMAGE_SECTION_HEADER m_isHeader = drv->read<IMAGE_SECTION_HEADER> ( m_pBase + m_idHeader.e_lfanew + sizeof ( IMAGE_NT_HEADERS64 ) + ( 3 * 40 ) );
	IMAGE_SECTION_HEADER m_isSection = drv->read<IMAGE_SECTION_HEADER> ( m_pBase + m_idHeader.e_lfanew + sizeof ( IMAGE_NT_HEADERS64 ) + ( 4 * 40 ) );
	ULONG m_lDataSize = m_isSection.VirtualAddress - m_isHeader.VirtualAddress;

	if ( strcmp ( ( char* ) m_isHeader.Name, ".data" ) )
		return NULL;

	for ( uintptr_t offset = m_lDataSize; offset > 0; offset -= 8 )
	{
		char m_szClass [0x100] = { '\0' };
		uintptr_t m_pClass = drv->read<uintptr_t> ( m_pBase + m_isHeader.VirtualAddress + offset );

		if ( !m_pClass )
			continue;

		uintptr_t m_pName = drv->read<uintptr_t> ( m_pClass + 0x10 );
		if ( !m_pName )
			continue;

		std::string m_szClassName = drv->read_ascii ( m_pName, sizeof ( m_szClass ) );

		if ( !strcmp ( m_szClassName.c_str ( ), name ) )
			return m_pClass;
	}

	return NULL;
}

uintptr_t rust::mono_field_static_get_value ( CSharedMemComms* drv, uintptr_t klass, uintptr_t offset )
{
	uintptr_t vTable = drv->read<uintptr_t> ( klass + 0xD0 );
	if ( vTable )
	{
		vTable = drv->read<uintptr_t> ( vTable + 0x8 );
		if ( vTable )
		{
			uintptr_t vTableSize = 0x40 + drv->read<uintptr_t> ( klass + 0x5C ) * 0x8;
			return drv->read<uintptr_t> ( vTable + vTableSize + offset );
		}
	}
	return 0;
}

float normalize_angle ( float angle )
{
	while ( angle > 360 )
		angle -= 360;
	while ( angle < 0 )
		angle += 360;
	return angle;
}

Vector3 normalize_angles ( Vector3 angles )
{
	angles.x = normalize_angle ( angles.x );
	angles.y = normalize_angle ( angles.y );
	angles.z = normalize_angle ( angles.z );
	return angles;
}

Vector3 rust::euler_angles ( Vector4 q1 )
{
	float sqw = q1.w * q1.w;
	float sqx = q1.x * q1.x;
	float sqy = q1.y * q1.y;
	float sqz = q1.z * q1.z;
	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	float test = q1.x * q1.w - q1.y * q1.z;
	Vector3 v;
	if ( test > 0.4995f * unit )
	{ // singularity at north pole
		v.y = 2.0f * atan2 ( q1.y, q1.x );
		v.x = M_PI / 2.f;
		v.z = 0;
		return normalize_angles ( v * 57.2958f );
	}
	if ( test < -0.4995f * unit )
	{ // singularity at south pole
		v.y = -2.0f * atan2 ( q1.y, q1.x );
		v.x = -M_PI / 2.f;
		v.z = 0;
		return normalize_angles ( v * 57.2958f );
	}
	Vector4 q ( q1.w, q1.z, q1.x, q1.y );
	v.y = ( float ) atan2 ( 2.0f * q.x * q.w + 2.0f * q.y * q.z, 1 - 2.0f * ( q.z * q.z + q.w * q.w ) );     // Yaw
	v.x = ( float ) asin ( 2.0f * ( q.x * q.z - q.w * q.y ) );                             // Pitch
	v.z = ( float ) atan2 ( 2.0f * q.x * q.y + 2.0f * q.z * q.w, 1 - 2.0f * ( q.y * q.y + q.z * q.z ) );      // Roll
	return normalize_angles ( v * 57.2958f );
}
//stackoverflow
Vector3 rust::rotate_point ( Vector3 center, Vector3 origin, float angle )
{
	float rad = ( ( angle ) * static_cast< float >( ( M_PI / 180.0f ) ) );
	float s = -sin ( rad );
	float c = cos ( rad );
	origin.x -= center.x;
	origin.z -= center.z;
	float xnew = origin.x * c - origin.z * s;
	float znew = origin.x * s + origin.z * c;
	xnew += center.x;
	znew += center.z;
	return Vector3 ( xnew, origin.y, znew );
}

Vector2 rust::RotatePoint ( Vector2 pointToRotate, Vector2 centerPoint, float angle, bool angleInRadians = false )
{
	float rad = ( ( normalize_angle ( angle ) ) * static_cast< float >( ( M_PI / 180.0f ) ) );
	float s = -sin ( rad );
	float c = cos ( rad );
	pointToRotate.x -= centerPoint.x;
	pointToRotate.y -= centerPoint.y;
	float xnew = pointToRotate.x * c - pointToRotate.y * s;
	float znew = pointToRotate.x * s + pointToRotate.y * c;
	pointToRotate.x = xnew + centerPoint.x;
	pointToRotate.y = znew + centerPoint.y;
	return pointToRotate;
}