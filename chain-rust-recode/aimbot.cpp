#include "aimbot.hpp"
#include "cheat.hpp"
#include "object_loop.hpp"
#include "fnv1a.hpp"
#include "unity.hpp"
#include "stdafx.hpp"
#include "bytes.hpp"

#define GAY_SKIP 	{game::aim_mutex.unlock ( ); \
					sleep_for ( milliseconds ( 150 ) ); \
					continue;}

enum hitbox : int
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_CHEST
};

namespace data::aimbot
{
	uintptr_t m_pTarget;
	bool m_bFound;
	bool m_bActive;
	entity_type m_eType;
	int m_iHitbox;
	Vector3 m_vecPosition;
	float m_flFOV;
	std::vector<Vector3> m_vecBulletSimulation;
	time_t m_tShot;
	bool m_bLocked;
};

float aim_fov;

void erase_aim ( std::pair<uintptr_t, entity_type> object )
{
	game::players.erase ( std::remove ( game::players.begin ( ), game::players.end ( ), object ), game::players.end ( ) );
}

Vector2 center {};

bool find_best_target ( )
{
	float closestDistance = xf ( 99999.f );
	uintptr_t bestTarget = NULL;

	switch ( g_Vars.aimbot.hitbox )
	{
	case 0:
		data::aimbot::m_iHitbox = BoneList::head;
		break;
	case 1:
		data::aimbot::m_iHitbox = BoneList::neck;
		break;
	case 2:
		data::aimbot::m_iHitbox = BoneList::spine2;
		break;
	}

	Matrix4x4 view_matrix = rust::get_view_matrix ( mist::aim.get ( ) );

	for ( const auto& entry : game::players )
	{
		auto& entity = entry.first;
		BasePlayer* player = ( BasePlayer* ) rust::get_base_entity ( mist::aim.get ( ), entity );

		if ( player->is_sleeping ( mist::aim.get ( ) ) )
			continue;

		if ( game::cache [player->get_uid ( mist::aim.get ( ) )].m_bFriend == true && g_Vars.aimbot.ignore_friends )
			continue;

		// fix for shitty aimbot targeting non-esp bots
		if ( entry.second == entity_type::Scientist && !g_Vars.esp.npc )
			continue;

		if ( entry.second == entity_type::TunnelDweller && !g_Vars.esp.tunneldweller )
			continue;

		Vector3 position = player->get_bone_position ( mist::aim.get ( ), BoneList::head );

		if ( data::aimbot::m_bLocked )
		{
			if ( data::aimbot::m_pTarget == entity )
			{
				bestTarget = data::aimbot::m_pTarget;
			}
			continue;
		}

		Vector2 nigga;
		if ( !rust::world_to_screen ( position, nigga, view_matrix ) )
			continue;

		float distance = center.distance ( nigga );

		if ( closestDistance > distance && distance < aim_fov )
		{
			closestDistance = distance;
			bestTarget = entity;
		}
	}

	if ( bestTarget == NULL )
	{
		data::aimbot::m_pTarget = NULL;
		data::aimbot::m_bLocked = false;
		data::aimbot::m_bFound = false;
		return false;
	}

	data::aimbot::m_pTarget = bestTarget;
	data::aimbot::m_bFound = true;
	if ( g_Vars.aimbot.hitbox == 3 && !data::aimbot::m_bLocked )
	{
		do
		{
			BasePlayer* player = ( BasePlayer* ) rust::get_base_entity ( mist::aim.get ( ), bestTarget );
			auto head = player->get_bone_position ( mist::aim.get ( ), BoneList::head );
			auto neck = player->get_bone_position ( mist::aim.get ( ), BoneList::neck );
			auto chest = player->get_bone_position ( mist::aim.get ( ), BoneList::spine2 );
			auto l_upperarm = player->get_bone_position ( mist::aim.get ( ), BoneList::l_upperarm );
			auto r_upperarm = player->get_bone_position ( mist::aim.get ( ), BoneList::r_upperarm );
			auto l_forearm = player->get_bone_position ( mist::aim.get ( ), BoneList::l_forearm );
			auto r_forearm = player->get_bone_position ( mist::aim.get ( ), BoneList::r_forearm );
			auto l_foot = player->get_bone_position ( mist::aim.get ( ), BoneList::l_foot );
			auto r_foot = player->get_bone_position ( mist::aim.get ( ), BoneList::r_foot );

			Vector2 nigga1, nigga2, nigga3, nigga4, nigga5, nigga6, nigga7, nigga8, nigga9;
			if ( !rust::world_to_screen ( head, nigga1, view_matrix ) || !rust::world_to_screen ( neck, nigga2, view_matrix ) || !rust::world_to_screen ( chest, nigga3, view_matrix )
				|| !rust::world_to_screen ( l_upperarm, nigga4, view_matrix ) || !rust::world_to_screen ( r_upperarm, nigga5, view_matrix ) || !rust::world_to_screen ( l_forearm, nigga6, view_matrix )
				|| !rust::world_to_screen ( r_forearm, nigga7, view_matrix ) || !rust::world_to_screen ( l_foot, nigga8, view_matrix ) || !rust::world_to_screen ( r_foot, nigga9, view_matrix ) )
				break;

			float distance1 = center.distance ( nigga1 );
			float distance2 = center.distance ( nigga2 );
			float distance3 = center.distance ( nigga3 );
			float distance4 = center.distance ( nigga4 );
			float distance5 = center.distance ( nigga5 );
			float distance6 = center.distance ( nigga6 );
			float distance7 = center.distance ( nigga7 );
			float distance8 = center.distance ( nigga8 );
			float distance9 = center.distance ( nigga9 );

			float smallest = ( std::min ) ( { distance1, distance2, distance3, distance4, distance5, distance6, distance7, distance8, distance9 } );
			if ( distance1 == smallest )
				data::aimbot::m_iHitbox = BoneList::head;
			else if ( distance2 == smallest )
				data::aimbot::m_iHitbox = BoneList::neck;
			else if ( distance3 == smallest )
				data::aimbot::m_iHitbox = BoneList::spine2;
			else if ( distance4 == smallest )
				data::aimbot::m_iHitbox = BoneList::l_upperarm;
			else if ( distance5 == smallest )
				data::aimbot::m_iHitbox = BoneList::r_upperarm;
			else if ( distance6 == smallest )
				data::aimbot::m_iHitbox = BoneList::l_forearm;
			else if ( distance7 == smallest )
				data::aimbot::m_iHitbox = BoneList::r_forearm;
			else if ( distance8 == smallest )
				data::aimbot::m_iHitbox = BoneList::l_foot;
			else if ( distance9 == smallest )
				data::aimbot::m_iHitbox = BoneList::r_foot;

		} while ( 0 );
	}
	data::aimbot::m_bLocked = true;

	return true;
}

void normalize ( float& pitch, float& yaw )
{
	if ( pitch < -89.f ) pitch = -89.f;
	if ( pitch > 89.f ) pitch = 89.f;

	while ( yaw < -180.f ) yaw += 360.f;
	while ( yaw > 180.f ) yaw -= 360.f;
}

void normalize ( Vector2& angle )
{
	if ( angle.x < -89.f ) angle.x = -89.f;
	if ( angle.x > 89.f ) angle.x = 89.f;

	while ( angle.y < -180.f ) angle.y += 360.f;
	while ( angle.y > 180.f ) angle.y -= 360.f;
}

Vector2 CalcAngle ( const Vector3& src, const Vector3& dst )
{
	Vector3 dir = src - dst;

	const auto sqrtss = [] ( float in )
	{ // thx can
		__m128 reg = _mm_load_ss ( &in );
		return _mm_mul_ss ( reg, _mm_rsqrt_ss ( reg ) ).m128_f32 [0];
	};

	float hyp = sqrtss ( dir.x * dir.x + dir.y * dir.y + dir.z * dir.z );
	float nigga = roundf ( dir.y );

	if ( isnan ( hyp ) )
		hyp = sqrtss ( dir.x * dir.x + nigga * nigga );

	double ax = asin ( dir.y / hyp );
	double ay = -atan2 ( dir.x, -dir.z );

	double x = RAD2DEG ( ax );
	double y = RAD2DEG ( ay );

	return Vector2 { static_cast< float >( x ), static_cast< float >( y ) };
}

void smooth_angle ( Vector2& angles, Vector2& localViewAngles )
{
	Vector2 delta = Vector2 ( angles.x - localViewAngles.x, angles.y - localViewAngles.y );
	normalize ( delta );

	float smoothing_value = 5.5f * ( g_Vars.aimbot.smoothing / 100.f );
	float smooth = powf ( 0.81f + smoothing_value, 0.4f );

	smooth = std::min ( 0.98f, smooth );

	Vector2 toChange = Vector2 ( );

	float coeff = ( 1.0f - smooth ) / delta.length ( ) * 4.f;

	// fast end
	coeff = powf ( coeff, 2.f ) * 80.f / g_Vars.aimbot.smoothing;

	coeff = std::min ( 1.f, coeff );
	toChange = delta * coeff;

	angles = Vector2 ( localViewAngles.x, localViewAngles.y ) + toChange;
}

float get_bullet_drop ( float distance, float projectile_speed, float projectile_gravity )
{
	if ( distance < xf ( 0.001f ) )
		return -1;

	float m_gravity = xf ( 9.81f ) * projectile_gravity;
	float m_time = distance / std::fabs ( projectile_speed );

	return static_cast< float >( xf ( 0.5f ) * m_gravity * m_time * m_time );
}

uintptr_t last_target = NULL;
uintptr_t last_weapon = NULL;

float m_flVelocity;
float m_flDrag;
float m_flGravity;
float m_flVelocityScale;
float m_flVelocityScaleModifier;

struct ProjectileWeaponModModifier
{
	bool enabled;
	float scalar;
	float offset;
};

void update_wpn_info ( )
{
	auto m_pWeapon = game::m_pLocalplayer->get_held_item ( mist::aim.get ( ) );
	auto m_szBullet = m_pWeapon->get_bullet_name ( mist::aim.get ( ) );

	if ( ( uintptr_t ) m_pWeapon != ( uintptr_t ) game::m_rLocalInfo.m_pWeapon || m_szBullet != game::m_rLocalInfo.m_szBulletType || ( uintptr_t ) m_pWeapon != last_weapon )
	{
		game::m_rLocalInfo.m_pWeapon = m_pWeapon;
		game::m_rLocalInfo.m_szWeaponName = m_pWeapon->get_name ( mist::aim.get ( ) );
		game::m_rLocalInfo.m_szBulletType = m_szBullet;
		game::m_rLocalInfo.m_hWeaponName = hash_constfnv64_string ( game::m_rLocalInfo.m_szWeaponName.c_str ( ) );
		game::m_rLocalInfo.m_hBulletType = hash_constfnv64_string ( game::m_rLocalInfo.m_szBulletType.c_str ( ) );
		game::m_rLocalInfo.m_vInfo.m_bPrediction = rust::use_weapon_prediction ( );
		game::m_rLocalInfo.m_vInfo.m_flGravity = rust::get_bullet_gravity ( );
		game::m_rLocalInfo.m_vInfo.m_flSpeed = rust::get_bullet_speed ( );
		auto baseProjectile = m_pWeapon->get_base_projectile ( mist::aim.get ( ) );

		last_weapon = ( uintptr_t ) m_pWeapon;

		uintptr_t mag = mist::aim->read<uintptr_t> ( baseProjectile + offsets::primarymagazine ); // baseproj->primarymagazine;
		uintptr_t ammo = mist::aim->read<uintptr_t> ( mag + 0x20 );
		uintptr_t unk1 = mist::aim->read<uintptr_t> ( ammo + 0x10 );
		uintptr_t unk2 = mist::aim->read<uintptr_t> ( unk1 + 0x30 );
		uintptr_t itemModProjectile = unity::get_component ( mist::aim.get ( ), unk2, _ ( "ItemModProjectile" ) );

		if ( itemModProjectile )
		{
			float projectileVelocity = mist::aim->read<float> ( itemModProjectile + 0x34 );
			uintptr_t projectileObject = mist::aim->read<uintptr_t> ( itemModProjectile + 0x18 );

			unk1 = mist::aim->read<uintptr_t> ( projectileObject + 0x18 );

			unk2 = mist::aim->read<uintptr_t> ( unk1 + 0x10 );

			uintptr_t projectile = unity::get_component ( mist::aim.get ( ), unk2, _ ( "Projectile" ) );

			if ( projectile )
			{
				Vector2 projectileInfo = mist::aim->read<Vector2> ( projectile + 0x24 );

				m_flDrag = projectileInfo.x;
				m_flGravity = projectileInfo.y;

				if ( g_Vars.misc.thickness )
					mist::aim->write<float> ( projectile + 0x2C, g_Vars.misc.flthickness );

				if ( g_Vars.misc.hitsound )
				{
					uintptr_t hitTest = mist::aim->read<uintptr_t> ( projectile + 0x198 );
					bool didHit = mist::aim->read<bool> ( hitTest + 0x66 );

					if ( didHit )
					{
						std::printf ( "\n this->HasHit = 1;" );
						//if ( !g_Vars.global.m_hitsounds.empty ( ) )
						{
							// DIRECTORY XD!!!
							int idx = g_Vars.misc.hitsound_custom;
							if ( idx >= g_Vars.global.m_hitsounds.size ( ) )
								idx = g_Vars.global.m_hitsounds.size ( ) - 1;
							else if ( idx < 0 )
								idx = 0;

							std::string curfile = g_Vars.global.m_hitsounds [idx];
							if ( !curfile.empty ( ) )
							{
								std::string dir = GetDocumentsDirectory ( ).append ( _ ( "\\phantom.tech\\sounds\\" ) ).append ( curfile );

								auto ReadWavFileIntoMemory = [&] ( std::string fname, BYTE** pb, DWORD* fsize )
								{
									std::ifstream f ( fname, std::ios::binary );

									f.seekg ( 0, std::ios::end );
									int lim = f.tellg ( );
									*fsize = lim;

									*pb = new BYTE [lim];
									f.seekg ( 0, std::ios::beg );

									f.read ( ( char* ) *pb, lim );

									f.close ( );
								};

								DWORD dwFileSize;
								BYTE* pFileBytes;
								ReadWavFileIntoMemory ( dir.data ( ), &pFileBytes, &dwFileSize );

								// danke anarh1st47, ich liebe dich
								// dieses code snippet hat mir so sehr geholfen https://i.imgur.com/ybWTY2o.png
								// thanks anarh1st47, you are the greatest
								// loveeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
								// kochamy anarh1st47
								auto modify_volume = [&] ( BYTE* bytes )
								{
									int offset = 0;
									for ( int i = 0; i < dwFileSize / 2; i++ )
									{
										if ( bytes [i] == 'd' && bytes [i + 1] == 'a'
											&& bytes [i + 2] == 't' && bytes [i + 3] == 'a' )
										{
											offset = i;
											break;
										}
									}

									if ( !offset )
										return;

									BYTE* pDataOffset = ( bytes + offset );
									DWORD dwNumSampleBytes = *( DWORD* ) ( pDataOffset + 4 );
									DWORD dwNumSamples = dwNumSampleBytes / 2;

									SHORT* pSample = ( SHORT* ) ( pDataOffset + 8 );
									for ( DWORD dwIndex = 0; dwIndex < dwNumSamples; dwIndex++ )
									{
										SHORT shSample = *pSample;
										shSample = ( SHORT ) ( shSample * ( g_Vars.misc.hitsound_volume / 100.f ) );
										*pSample = shSample;
										pSample++;
										if ( ( ( BYTE* ) pSample ) >= ( bytes + dwFileSize - 1 ) )
											break;
									}
								};

								if ( pFileBytes )
								{
									modify_volume ( pFileBytes );
									PlaySoundA ( ( LPCSTR ) pFileBytes, NULL, SND_MEMORY | SND_ASYNC | SND_NOSTOP );
								}
							}
						}
					}
				}
			}

			if ( g_Vars.misc.noshotgunspread )
			{
				mist::aim->write<float> ( itemModProjectile + 0x30, 0.f );
				mist::aim->write<float> ( itemModProjectile + 0x38, 0.f );
			}

			uintptr_t childrenList = mist::aim->read<uintptr_t> ( baseProjectile + 0x40 );
			int childrenCount = mist::aim->read<int> ( childrenList + 0x18 );
			childrenList = mist::aim->read<uintptr_t> ( childrenList + 0x10 );

			for ( int h = 0; h < childrenCount; h++ )
			{
				uintptr_t childEntity = mist::aim->read<uintptr_t> ( childrenList + 0x20 + h * 0x8 );

				if ( !childEntity )
					continue;

				uintptr_t childEntityName = mist::aim->read<uintptr_t> ( childEntity );
				childEntityName = mist::aim->read<uintptr_t> ( childEntityName + 0x10 );

				std::string entityType = mist::aim->read_ascii ( childEntityName, 24 );
				if ( strcmp ( entityType.c_str ( ), _ ( "ProjectileWeaponMod" ) ) == 0/* && mist::other->rpm<bool>(childEntity + rust::projectileVelocityModifier)*/ )
				{
					//projectileVelocity *= mist::other->rpm<float>(childEntity + rust::projectileVelocityModifier + 0x4); // +0x4

					// todo;
					ProjectileWeaponModModifier projVelModif = mist::aim->read<ProjectileWeaponModModifier> ( childEntity + offsets::projvelocity ); // projectileweaponmod.modifier projectilevelocity;

					if ( projVelModif.enabled )
					{
						projectileVelocity *= projVelModif.scalar;

						break; //only one with vel modifier per weapon, so we can safely exist this for loop
					}
				}
			}
			m_flVelocity = projectileVelocity;
			m_flDrag = m_flDrag ? m_flDrag : rust::get_bullet_drag ( );
			m_flGravity = m_flGravity ? m_flGravity : rust::get_bullet_gravity ( );
			m_flVelocityScale = mist::aim->read<float> ( baseProjectile + offsets::projvelocityscale ); // projectilevelocityscale;
		}
		else
		{
			m_flVelocity = rust::get_bullet_speed ( );
			m_flGravity = rust::get_bullet_gravity ( );
			m_flDrag = rust::get_bullet_drag ( );
			m_flVelocityScale = mist::aim->read<float> ( baseProjectile + offsets::projvelocityscale ); // projectilevelocityscale;
		}
	}
	m_flVelocityScaleModifier = 1.f;
}

void prediction ( const Vector3& from, BasePlayer* who, unsigned int bone )
{
	if ( g_Vars.aimbot.terraincheck )
		if ( !mist::aim->read<bool> ( ( uintptr_t ) who + 0xC8 ) )
		{
			data::aimbot::m_vecPosition = {};
			data::aimbot::m_bLocked = false;
			return;
		}

	Vector3 aimpoint = who->get_bone_position ( mist::aim.get ( ), bone );
	float distance = from.distance ( aimpoint );

	update_wpn_info ( );

	if ( distance < xf ( 0.1f ) || !game::m_rLocalInfo.m_vInfo.m_bPrediction )
	{
		data::aimbot::m_vecPosition = Vector3 {};
		return;
	}

	uintptr_t base_projectile = game::m_rLocalInfo.m_pWeapon->get_base_projectile ( mist::aim.get ( ) );

	float m_flBulletSpeed = m_flVelocity * ( m_flVelocityScale * m_flVelocityScaleModifier );
	float bullet_gravity = rust::get_bullet_gravity ( );
	float bullet_time = distance / m_flBulletSpeed;
	float m_fDrag = rust::get_bullet_drag ( );
	float m_flBulletDrop = get_bullet_drop ( distance, m_flBulletSpeed, bullet_gravity );

	const float m_flTimeStep = xf ( 0.015625f );
	float m_flYTravelled {}, m_flYSpeed {}, m_flBulletTime {}, m_flDivider {};

	float m_flDistanceTo = from.distance ( aimpoint );

	for ( float distance_to_travel = 0.f; distance_to_travel < m_flDistanceTo;)
	{
		float speed_modifier = 1.f - m_flTimeStep * m_fDrag;
		m_flBulletSpeed *= speed_modifier;

		if ( m_flBulletSpeed <= 0.f || m_flBulletSpeed >= 10000.f || m_flYTravelled >= 10000.f || m_flYTravelled < 0.f )
			break;

		if ( m_flBulletTime > 8.f )
			break;

		m_flYSpeed += ( xf ( 9.81f ) * bullet_gravity ) * m_flTimeStep;
		m_flYSpeed *= speed_modifier;

		distance_to_travel += m_flBulletSpeed * m_flTimeStep;
		m_flYTravelled += m_flYSpeed * m_flTimeStep;
		m_flBulletTime += m_flTimeStep;
	}

	Vector3 velocity = who->get_velocity ( mist::aim.get ( ) ) * 0.75f;
	if ( velocity.y > 0.f )
		velocity.y /= 3.25f;

	aimpoint.y += m_flYTravelled;
	aimpoint += velocity * m_flBulletTime;

	data::aimbot::m_vecPosition = aimpoint;
	data::aimbot::m_bLocked = true;

	last_target = data::aimbot::m_pTarget;

	return;
}

void aimbot::hit_sound ( )
{
	//while ( true )
	//{
	//	if ( !g_Vars.misc.hitsound )
	//		continue;

	//	BasePlayer* aimtarget = ( BasePlayer* ) rust::get_base_entity ( mist::aim.get ( ), data::aimbot::m_pTarget );

	//	float lastheadshotsoundtime;

	//	if ( game::m_rLocalInfo.m_pPlayer == 0 )
	//		lastheadshotsoundtime = 0.f;
	//	else
	//		lastheadshotsoundtime = mist::aim->read<float> ( ( uintptr_t ) aimtarget + 0x748 ); // lastHeadshotSoundTime

	//	if ( lastheadshotsoundtime > 0.f ||
	//		InputHelper::Released ( VK_F6 ) )
	//	{
	//		if ( !g_Vars.global.m_hitsounds.empty ( ) )
	//		{
	//			// DIRECTORY XD!!!
	//			int idx = g_Vars.misc.hitsound_custom;
	//			if ( idx >= g_Vars.global.m_hitsounds.size ( ) )
	//				idx = g_Vars.global.m_hitsounds.size ( ) - 1;
	//			else if ( idx < 0 )
	//				idx = 0;

	//			std::string curfile = g_Vars.global.m_hitsounds [idx];
	//			if ( !curfile.empty ( ) )
	//			{
	//				std::string dir = GetDocumentsDirectory ( ).append ( _ ( "\\phantom.tech\\sounds\\" ) ).append ( curfile );

	//				auto ReadWavFileIntoMemory = [&] ( std::string fname, BYTE** pb, DWORD* fsize )
	//				{
	//					std::ifstream f ( fname, std::ios::binary );

	//					f.seekg ( 0, std::ios::end );
	//					int lim = f.tellg ( );
	//					*fsize = lim;

	//					*pb = new BYTE [lim];
	//					f.seekg ( 0, std::ios::beg );

	//					f.read ( ( char* ) *pb, lim );

	//					f.close ( );
	//				};

	//				DWORD dwFileSize;
	//				BYTE* pFileBytes;
	//				ReadWavFileIntoMemory ( dir.data ( ), &pFileBytes, &dwFileSize );

	//				// danke anarh1st47, ich liebe dich
	//				// dieses code snippet hat mir so sehr geholfen https://i.imgur.com/ybWTY2o.png
	//				// thanks anarh1st47, you are the greatest
	//				// loveeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
	//				// kochamy anarh1st47
	//				auto modify_volume = [&] ( BYTE* bytes )
	//				{
	//					int offset = 0;
	//					for ( int i = 0; i < dwFileSize / 2; i++ )
	//					{
	//						if ( bytes [i] == 'd' && bytes [i + 1] == 'a'
	//							&& bytes [i + 2] == 't' && bytes [i + 3] == 'a' )
	//						{
	//							offset = i;
	//							break;
	//						}
	//					}

	//					if ( !offset )
	//						return;

	//					BYTE* pDataOffset = ( bytes + offset );
	//					DWORD dwNumSampleBytes = *( DWORD* ) ( pDataOffset + 4 );
	//					DWORD dwNumSamples = dwNumSampleBytes / 2;

	//					SHORT* pSample = ( SHORT* ) ( pDataOffset + 8 );
	//					for ( DWORD dwIndex = 0; dwIndex < dwNumSamples; dwIndex++ )
	//					{
	//						SHORT shSample = *pSample;
	//						shSample = ( SHORT ) ( shSample * ( g_Vars.misc.hitsound_volume / 100.f ) );
	//						*pSample = shSample;
	//						pSample++;
	//						if ( ( ( BYTE* ) pSample ) >= ( bytes + dwFileSize - 1 ) )
	//							break;
	//					}
	//				};

	//				if ( pFileBytes )
	//				{
	//					modify_volume ( pFileBytes );
	//					PlaySoundA ( ( LPCSTR ) pFileBytes, NULL, SND_MEMORY | SND_ASYNC | SND_NOSTOP );
	//				}
	//			}
	//		}

	//		mist::aim->write<float> ( ( uintptr_t ) aimtarget + 0x748, 0.f );
	//	}

	//	sleep_for ( 1ms );
	//}
}

bool process_aim_data ( )
{
	BasePlayer* aimtarget = ( BasePlayer* ) rust::get_base_entity ( mist::aim.get ( ), data::aimbot::m_pTarget );
	BasePlayer* localplayer = game::m_pLocalplayer;

	if ( !aimtarget || !localplayer )
		return false;

	Vector3 local_pos = rust::get_camera_position ( mist::aim.get ( ) );
	if ( local_pos.empty ( ) )
		return false;

	prediction ( local_pos, aimtarget, data::aimbot::m_iHitbox );

	if ( data::aimbot::m_vecPosition.empty ( ) || !( GetAsyncKeyState ( g_Vars.aimbot.aim_key ) ) )
	{
		data::aimbot::m_bLocked = false;
		return false;
	}

	Vector2 body_angles = localplayer->get_body_angles ( mist::aim.get ( ) );
	if ( body_angles.empty ( ) )
		return false;

	Vector2 offset = CalcAngle ( local_pos, data::aimbot::m_vecPosition ) - body_angles - localplayer->get_recoil_angles ( mist::aim.get ( ) );

	Vector2 aim_angle = body_angles;
	aim_angle = aim_angle + offset;

	smooth_angle ( aim_angle, body_angles );
	normalize ( aim_angle.x, aim_angle.y );

	if ( aim_angle.empty ( ) || isnan ( aim_angle.x ) || isnan ( aim_angle.y ) ||
		offset.x >= xf ( 88.f ) || offset.x <= xf ( -88.f ) || aim_angle.x <= xf ( -88.f )
		|| aim_angle.x >= xf ( 88.f ) )
	{
		data::aimbot::m_bFound = false;
		data::aimbot::m_pTarget = NULL;
		return false;
	}

	if ( g_Vars.aimbot.silent )
	{
		auto eyes = localplayer->get_player_eyes ( mist::aim.get ( ) );
		if ( eyes )
		{
			auto angle = data::aimbot::m_vecPosition - local_pos;
			angle.normalize ( );
			Vector4 quaternionangle = Vector4::QuaternionLookRotation ( angle, { 0,1,0 } );

			eyes->set_body_rotation ( mist::aim.get ( ), quaternionangle );
		}
	}
	else
		localplayer->set_body_angles ( mist::aim.get ( ), aim_angle );

	return true;
}

void aimbot::thread ( )
{
	while ( monitor::width == 0 );
	static float old_fov = g_Vars.aimbot.aim_fov;
	uintptr_t m_pGraphics = mist::misc->read<uintptr_t> ( addr::gameassembly + offsets::fov_offset );
	uintptr_t m_pGraphicsInstance = mist::misc->read<uintptr_t> ( m_pGraphics + 0xB8 );
	float stage1 = static_cast< float >( abs ( tan ( ( g_Vars.aimbot.aim_fov * ( M_PI / 180.f ) ) / 2 ) ) );
	float stage2 = static_cast< float >( abs ( tan ( ( mist::misc->read<float> ( m_pGraphicsInstance + 0x18 ) * ( M_PI / 180.f ) ) / 2 ) ) );
	float final = stage1 / stage2;
	aim_fov = final / 2 * ( monitor::width / 2.f );
	data::aimbot::m_flFOV = aim_fov;

	center = { monitor::width / 2.f, monitor::height / 2.f };

	while ( true )
	{
		if ( !game::m_pLocalplayer || !g_Vars.aimbot.enabled )
		{
			data::aimbot::m_bActive = false;
			data::aimbot::m_bFound = false;
			data::aimbot::m_bLocked = false;
			data::aimbot::m_pTarget = 0;
			data::aimbot::m_vecPosition = { };

			sleep_for ( milliseconds ( 200 ) );
			continue;
		}

		if ( g_Vars.aimbot.aim_fov != old_fov )
		{
			float stage1 = static_cast< float >( abs ( tan ( ( g_Vars.aimbot.aim_fov * ( M_PI / 180.f ) ) / 2 ) ) );
			float stage2 = static_cast< float >( abs ( tan ( ( mist::misc->read<float> ( m_pGraphicsInstance + 0x18 ) * ( M_PI / 180.f ) ) / 2 ) ) );
			float final = stage1 / stage2;
			aim_fov = final / 2 * ( monitor::width / 2.f );
			data::aimbot::m_flFOV = aim_fov;
			old_fov = g_Vars.aimbot.aim_fov;
		}

		game::aim_mutex.lock ( );

		if ( !find_best_target ( ) )
			GAY_SKIP;

		if ( !process_aim_data ( ) )
		{
			game::aim_mutex.unlock ( );

			sleep_for ( milliseconds ( 3 ) );
			continue;
		}
		else

			game::aim_mutex.unlock ( );

		sleep_for ( milliseconds ( 1 ) );
	}

}