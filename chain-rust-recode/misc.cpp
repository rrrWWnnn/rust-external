#include "misc.hpp"
#include <unordered_map>
#include "fnv1a.hpp"
#include "unity.hpp"
#include "stdafx.hpp"

uintptr_t m_pBaseProjectile = 0;

std::unordered_map<std::string, Vector4> m_vecRecoilProperties;
uintptr_t m_pEffect;

void spoof_admin ( )
{
	if ( g_Vars.misc.admin && !GetAsyncKeyState ( g_Vars.misc.fly ) )
	{
		int flags = game::m_pLocalplayer->get_flags ( mist::misc.get ( ) );
		if ( flags == 0 )
			return;

		flags |= ( int ) player_flags::admin;
		game::m_pLocalplayer->set_flag ( mist::misc.get ( ), flags );
	}
}

// Servers can send console commands to the client. Some of these are potentially dangerous while cheating.
// Rust severs commonly send "noclip" to trigger an insta "banned: cheat detected".
// We can just loop the command array and disable ones we don't want the server to run.
void CheckAndDisallowDangerousCommands ( uintptr_t command )
{
	uintptr_t pCommandName = mist::misc->read< uintptr_t > ( command + 0x10 );
	std::wstring command_name = mist::misc->wread_unicode ( pCommandName + 0x14, 36 );

	bool isDangerousCommand = command_name.find ( _ ( L"noclip" ) ) != std::wstring::npos ||
		command_name.find ( _ ( L"debugcamera" ) ) != std::wstring::npos ||
		command_name.find ( _ ( L"admintime" ) ) != std::wstring::npos ||
		command_name.find ( _ ( L"camlerp" ) ) != std::wstring::npos ||
		command_name.find ( _ ( L"camspeed" ) ) != std::wstring::npos;

	//auto command_NoClip = _ ( L"debugcamera" );

	//bool isDangerousCommand = wcscmp ( command_name.c_str ( ), command_NoClip ) == 0;

	if ( isDangerousCommand )
	{
		mist::misc->write< bool > ( command + 0x58, false );

		std::printf ( "\n Disallowed command [%S] from running from the server!", command_name );
	}

	for ( int i = 0; i < 36; i++ )
		command_name [i] = '\0';
}

void Main2 ( )
{
	while ( true )
	{
		uintptr_t ConsoleSystemIndex = mist::misc->read<uintptr_t> ( addr::gameassembly + offsets::console_system_typeinfo );
		uintptr_t ConsoleSystemIndex_StaticFields = mist::misc->read<uintptr_t> ( ConsoleSystemIndex + 0xB8 );

		for ( int i = 0; i < 500; i++ )
		{
			uintptr_t x_command = mist::misc->read<uintptr_t> ( ConsoleSystemIndex_StaticFields + 0x10 + ( i * 0x8 ) );

			CheckAndDisallowDangerousCommands ( x_command );
		}

		sleep_for ( 1ms );
	}
}

void no_recoil ( )
{
	Item* m_pWeapon = game::m_pLocalplayer->get_held_item ( mist::misc.get ( ) ); VASSERT ( m_pWeapon );

	m_pBaseProjectile = m_pWeapon->get_base_projectile ( mist::misc.get ( ) );
	if ( !m_pBaseProjectile || !game::m_rLocalInfo.m_pWeapon->is_weapon ( mist::misc.get ( ) ) )
		return;

	uintptr_t m_pRecoilProperties = mist::misc->read<uintptr_t> ( m_pBaseProjectile + offsets::recoil_properties );
	if ( !m_pRecoilProperties )
		return;

	Vector4 m_vecRecoil = mist::misc->read<Vector4> ( m_pRecoilProperties + 0x18 );
	std::string m_hWeapon = m_pWeapon->get_name ( mist::misc.get ( ) );
	auto pair = std::make_pair ( m_hWeapon, m_vecRecoil );

	if ( m_vecRecoilProperties.find ( m_hWeapon ) == m_vecRecoilProperties.end ( ) )
		m_vecRecoilProperties.emplace ( m_hWeapon, m_vecRecoil );

	const Vector4 m_vecDefault = m_vecRecoilProperties [m_hWeapon];

	if ( m_pWeapon->is_weapon ( mist::misc.get ( ) ) && ( g_Vars.misc.recoil_yaw != 100.f && g_Vars.misc.recoil_pitch != 100.f ) )
		mist::misc->write<Vector4> ( m_pRecoilProperties + 0x18, { m_vecDefault.x / 100.f * g_Vars.misc.recoil_yaw, m_vecDefault.y / 100.f * g_Vars.misc.recoil_yaw, m_vecDefault.z / 100.f * g_Vars.misc.recoil_pitch, m_vecDefault.w / 100.f * g_Vars.misc.recoil_pitch } );
}

void hitsound ( )
{
	//uintptr_t m_pHitTest = mist::misc->read<uintptr_t> ( addr::gameassembly + 0x3111250 );
	//uintptr_t m_pHitTestInstance = mist::misc->read<uintptr_t> ( m_pHitTest + 0xB8 );

	//if ( mist::misc->read<bool> ( m_pHitTestInstance + 0x66 ) )
	//{
	//	std::printf ( "did hit = true yes nigga" );
	//	//PlaySoundA ( reinterpret_cast< LPCSTR >( hitmarkersound.data ( ) ), nullptr, SND_MEMORY | SND_ASYNC | SND_NOSTOP );
	//}
}

void movement ( )
{
	static bool do_once = false;
	uintptr_t movement = mist::misc->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::basemovement ); VASSERT ( movement );
	if ( movement && g_Vars.misc.spider )
		mist::misc->write<float> ( movement + offsets::groundangle_new, 0.f );

	if ( movement && g_Vars.misc.noslide )
		mist::misc->write<Vector2> ( movement + offsets::maxangle_walk, { 99999.f, 99999.f } ); // walk && climb

	if ( g_Vars.misc.bgravity && GetAsyncKeyState ( g_Vars.misc.gravity ) )
	{
		mist::misc->write<float> ( movement + offsets::gravity, 1.68f );
		do_once = true;
	}
	else if ( do_once )
	{
		mist::misc->write<float> ( movement + offsets::gravity, 2.5f );
		do_once = false;
	}

	if ( movement && g_Vars.misc.infinite_jump )
	{
		mist::misc->write<float> ( movement + offsets::jumpTime, 0.f ); // jumpTime

		auto m_flGroundTime = mist::misc->read<float> ( movement + offsets::groundTime ) + 0.5f;
		mist::misc->write<float> ( movement + offsets::groundTime, m_flGroundTime ); // groundTime

		mist::misc->write<float> ( movement + offsets::landTime, 0.f ); // landTime
	}

	uintptr_t basemounted = mist::misc->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + 0x600 ); VASSERT ( basemounted ); // BaseMounted
	if ( basemounted && g_Vars.misc.shoot_from_minicopter )
		mist::misc->write<bool> ( basemounted + 0x2B0, true );
	else
		mist::misc->write<bool> ( basemounted + 0x2B0, false );
}

void fast ( )
{
	Item* m_pLastWeapon = 0;
	while ( true )
	{
		Item* m_pCurrentWeapon = game::m_pLocalplayer->get_held_item ( mist::wep.get ( ) ); CASSERT ( m_pCurrentWeapon );
		if ( m_pCurrentWeapon != m_pLastWeapon || m_pCurrentWeapon != game::m_rLocalInfo.m_pWeapon )
		{
			game::m_rLocalInfo.m_pWeapon = m_pCurrentWeapon;
			game::m_rLocalInfo.m_szWeaponName = m_pCurrentWeapon->get_name ( mist::wep.get ( ) );
			game::m_rLocalInfo.m_szBulletType = m_pCurrentWeapon->get_bullet_name ( mist::wep.get ( ) );
			game::m_rLocalInfo.m_hWeaponName = hash_constfnv64_string ( game::m_rLocalInfo.m_szWeaponName.c_str ( ) );
			game::m_rLocalInfo.m_hBulletType = hash_constfnv64_string ( game::m_rLocalInfo.m_szBulletType.c_str ( ) );
			game::m_rLocalInfo.m_vInfo.m_bPrediction = rust::use_weapon_prediction ( );
			game::m_rLocalInfo.m_vInfo.m_flGravity = rust::get_bullet_gravity ( );
			game::m_rLocalInfo.m_vInfo.m_flSpeed = rust::get_bullet_speed ( );

			m_pLastWeapon = m_pCurrentWeapon;
			m_pBaseProjectile = m_pCurrentWeapon->get_base_projectile ( mist::wep.get ( ) );
		}

		if ( !m_pBaseProjectile || !m_pCurrentWeapon->is_weapon ( mist::wep.get ( ) ) )
		{
			sleep_for ( milliseconds ( 300 ) );
			continue;
		}

		if ( game::m_rLocalInfo.m_szWeaponName == _ ( "pistol.eoka" ) && g_Vars.misc.eoka )
		{
			mist::wep->write<float> ( m_pBaseProjectile + offsets::successfraction, 1.f ); // flint->successfraction
			if ( GetAsyncKeyState ( VK_LBUTTON ) & 0x8000 )
			{
				mist::wep->write<uint8_t> ( m_pBaseProjectile + offsets::didSparkThisFrame, 1 ); //_didSparkThisFrame
				mist::wep->write<uint8_t> ( m_pBaseProjectile + offsets::isStriking, 1 ); //_isStriking
			}
			else
				mist::wep->write<uint8_t> ( m_pBaseProjectile + offsets::isStriking, 0 ); //_isStriking
		}

		if ( game::m_rLocalInfo.m_szWeaponName == _ ( "bow.compound" ) && g_Vars.misc.compound )
			mist::wep->write<float> ( m_pBaseProjectile + offsets::stringmaxduration, 0.f ); // compound->stringmaxduration

		if ( game::m_rLocalInfo.m_szWeaponName == _ ( "bow.hunting" ) && g_Vars.misc.fastbow )
		{
			mist::wep->write<bool> ( m_pBaseProjectile + offsets::attackReady, true ); // attackReady
			mist::wep->write<bool> ( m_pBaseProjectile + offsets::wasAiming, true ); // wasAiming
		}

		if ( g_Vars.misc.nosway )
		{
			mist::wep->write<float> ( m_pBaseProjectile + offsets::aimSway, 0.f );//aimSway
			mist::wep->write<float> ( m_pBaseProjectile + offsets::aimSwaySpeed, 0.f );//aimSwaySpeed
		}

		if ( g_Vars.misc.nospread )
		{
			mist::wep->write<float> ( m_pBaseProjectile + offsets::aimcone, 0.f );//aimCone
			mist::wep->write<float> ( m_pBaseProjectile + offsets::hipAimCone, 0.f );//hipAimCone
			mist::wep->write<float> ( m_pBaseProjectile + offsets::aimConePenaltyMax, 0.f );//aimConePenaltyMax
			mist::wep->write<float> ( m_pBaseProjectile + offsets::aimconePenaltyPerShot, 0.f );//aimconePenaltyPerShot
			mist::wep->write<float> ( m_pBaseProjectile + 0x300, 0.f );//stancePenaltyScale
		}

		if ( g_Vars.misc.rapidfire )
			mist::wep->write<float> ( m_pBaseProjectile + offsets::repeat_delay, g_Vars.misc.rapidfire_speed );//repeatDelay

		if ( g_Vars.misc.force_automatic )
			mist::wep->write<bool> ( m_pBaseProjectile + offsets::automatic, true );//automatic

		if ( g_Vars.misc.force_semiautomatic )
			mist::wep->write<bool> ( m_pBaseProjectile + offsets::automatic, false );//automatic

		if ( g_Vars.misc.long_melee && !game::m_rLocalInfo.m_pWeapon->is_weapon ( mist::misc.get ( ) ) )
			mist::wep->write<float> ( m_pBaseProjectile + offsets::maxDistance, 5.f );//maxDistance

		//if ( g_Vars.misc.remove_clothing_restriction )
		//{
		//	mist::wep->write<bool> ( m_pBaseProjectile + 0x750, false );//clothingBlocksAiming
		//	mist::wep->write<bool> ( m_pBaseProjectile + 0x760, false );//equippingBlocked
		//}

		mist::wep->write<float> ( ( uintptr_t ) game::m_pLocalplayer + offsets::clienttickinterval, g_Vars.misc.fakelag ? 0.4 : 0.05f );
		sleep_for ( milliseconds ( 150 ) );
	}
}

void atmosphere ( )
{
	while ( true )
	{
		uintptr_t gameobjectmanager = mist::misc->read<uintptr_t> ( addr::unityplayer + offsets::gom_offset );
		if ( !gameobjectmanager )
			continue;

		uintptr_t Obj = mist::misc->read<uintptr_t> ( gameobjectmanager + 0x8 );

		( Obj && ( Obj ) != mist::misc->read<uintptr_t> ( gameobjectmanager ) );

		Obj = mist::misc->read<uintptr_t> ( Obj + 0x8 );

		uintptr_t GameObject = mist::misc->read<uintptr_t> ( Obj + 0x10 );
		uintptr_t ObjClass = mist::misc->read<uintptr_t> ( GameObject + 0x30 );
		uintptr_t Entity1 = mist::misc->read<uintptr_t> ( ObjClass + 0x18 );
		uintptr_t Dome = mist::misc->read<uintptr_t> ( Entity1 + 0x28 );
		uintptr_t TodCycle = mist::misc->read<uintptr_t> ( Dome + 0x38 );


		uint64_t test1 = mist::misc->read<uint64_t> ( GameObject + 0x30 );//night
		uint64_t test2 = mist::misc->read<uint64_t> ( test1 + 0x18 ); //world
		uint64_t test3 = mist::misc->read<uint64_t> ( test2 + 0x28 ); //day
		uint64_t AtmosphereParameters = mist::misc->read<uint64_t> ( test3 + 0x48 ); //light
		uint64_t CycleParameters = mist::misc->read<uint64_t> ( test3 + 0x38 ); //sun

		uint64_t TOD_CycleParameters = mist::misc->read<uint64_t> ( test3 + 0x38 );
		uint64_t TOD_DayParameters = mist::misc->read<uint64_t> ( test3 + 0x50 );
		uint64_t TOD_NightParameters = mist::misc->read<uint64_t> ( test3 + 0x58 );
		uint64_t TOD_CloudParamaters = mist::misc->read<uint64_t> ( test3 + 0x78 );
		uint64_t TOD_AmbientParameters = mist::misc->read<uint64_t> ( test3 + 0x90 );

		if ( g_Vars.misc.fullbright )
		{
			mist::misc->write<float> ( TOD_AmbientParameters + 0x18, 1.f ); // AmbientMultiplier
			mist::misc->write<float> ( TOD_NightParameters + 0x50, 6.f ); // AmbientMultiplier
			mist::misc->write<float> ( TOD_NightParameters + 0x54, 1.f ); // ReflectionMultiplier
			mist::misc->write<float> ( TOD_DayParameters + 0x50, 1.f ); // AmbientMultiplier
			mist::misc->write<float> ( TOD_DayParameters + 0x54, 1.f ); // ReflectionMultiplier
		}

		//if ( Settings::skyColorBool )
		//	mist::misc->write<float> ( AtmosphereParameters + 0x10, 1 );

		if ( g_Vars.misc.alwaysday )
			mist::misc->write<float> ( TodCycle + 0x10, 12.f );
		else if ( g_Vars.misc.alwaysnight )
			mist::misc->write<float> ( TodCycle + 0x10, 0.f );

		sleep_for ( milliseconds ( 1 ) );
	}
}

Vector3 m_vecEyes {};
uintptr_t m_pMovement = 0;
bool stop = false;
bool go_back = false, go_forward = false, go_up = false;

void picus1 ( )
{
	while ( !m_pMovement )
		sleep_for ( milliseconds ( 50 ) );

	while ( true )
	{
		if ( m_pMovement )

			if ( g_Vars.misc.bclimb && GetAsyncKeyState ( g_Vars.misc.assist ) )
			{
				go_forward = true;
				sleep_for ( milliseconds ( 600 ) );
				go_forward = false; go_back = true;
				sleep_for ( milliseconds ( 65 ) );
				go_back = false; go_up = true;
				sleep_for ( milliseconds ( 500 ) );
				go_up = false;
			}

		sleep_for ( milliseconds ( 50 ) );
	}
}
bool nigger = false;

void flyhack ( )
{
	while ( true )
	{
		uintptr_t m_pLocalPlayer = ( uintptr_t ) game::m_pLocalplayer;
		if ( !m_pLocalPlayer )
		{
			sleep_for ( 400ms );
			continue;
		}

		uintptr_t m_pEyes = mist::misc->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::eyes );
		if ( !m_pEyes )
		{
			sleep_for ( 400ms );
			continue;
		}
		m_pMovement = mist::misc->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::basemovement );
		if ( !m_pMovement )
		{
			Beep ( 250, 250 );
			sleep_for ( 400ms );
			continue;
		}

		if ( g_Vars.misc.jumpshoot )
		{
			ModelState* model_state = ( ModelState* ) mist::misc->read<uintptr_t> ( reinterpret_cast< uintptr_t >( game::m_pLocalplayer ) + offsets::modelState );

			mist::misc->write<int> ( ( uintptr_t ) model_state + 0x24, 4 );
			mist::misc->write<float> ( m_pMovement + 0x4C, 1.f );
		}

		if ( ( g_Vars.misc.bfly && GetAsyncKeyState ( g_Vars.misc.fly ) ) || ( g_Vars.misc.bclimb && GetAsyncKeyState ( g_Vars.misc.assist ) ) )
		{
			//mist::misc->write<uint8_t> ( m_pMovement + 0x14C, 1 ); // flying BOOL
			ModelState* model_state = ( ModelState* ) mist::misc->read<uintptr_t> ( reinterpret_cast< uintptr_t >( game::m_pLocalplayer ) + offsets::modelState );
			mist::misc->write<float> ( ( uintptr_t ) model_state + 0x14, 1.f );
			mist::misc->write<float> ( m_pMovement + 0x68, -999.f ); // noclip ;)
			mist::misc->write<float> ( m_pMovement + 0x6C, -999.f );

			Vector3 targetmovement {};
			PlayerEyes* eyes = ( PlayerEyes* ) m_pEyes;

			Vector4 rotation = mist::misc->read<Vector4> ( m_pEyes + 0x44 );

			model_state->set_sprinting ( mist::misc.get ( ), true );

			if ( GetAsyncKeyState ( 'W' ) || go_forward )
				targetmovement += quatmult ( &vecforward, &rotation );
			if ( GetAsyncKeyState ( 'S' ) || go_back )
				targetmovement -= quatmult ( &vecforward, &rotation );
			if ( GetAsyncKeyState ( 'A' ) )
				targetmovement -= quatmult ( &vecright, &rotation );
			if ( GetAsyncKeyState ( 'D' ) )
				targetmovement += quatmult ( &vecright, &rotation );

			if ( GetAsyncKeyState ( VK_SPACE ) || go_up )
				targetmovement.y += 1.f;

			float d = 5.f;
			if ( GetAsyncKeyState ( VK_CONTROL ) )
				d = 2.5f;
			if ( GetAsyncKeyState ( VK_SHIFT ) )
				d = g_Vars.misc.noclip_speed;

			if ( !( targetmovement == Vector3 { 0.f, 0.f, 0.f } ) )
			{
				mist::misc->write<Vector3> ( m_pMovement + offsets::target_movement, targetmovement * d );
			}

			model_state->set_sprinting ( mist::misc.get ( ), true );

			nigger = true;
		}
		else
		{
			if ( nigger )
			{
				mist::misc->write<float> ( m_pMovement + 0x68, 1.f );
				mist::misc->write<float> ( m_pMovement + 0x6C, 0.55f );
				//mist::misc->write<float> ( m_pMovement + 0x74, 2.5f );
				nigger = false;
			}
		}
		mist::misc->write<bool> ( m_pMovement + 0x18, 0 );
		sleep_for ( 1ms );
	}
}

void fast_loot ( )
{
	while ( true )
	{
		uintptr_t m_pTime = mist::misc->read<uintptr_t> ( addr::unityplayer + offsets::timescale + 7 * 8 );
		if ( !m_pTime )
		{
			sleep_for ( milliseconds ( 700 ) );
			continue;
		}

		if ( g_Vars.misc.fastloot )
		{
			uintptr_t UIInventory = mist::misc->readchain<uintptr_t> ( addr::gameassembly, { offsets::ui_inventory, 0xB8 } );

			if ( mist::misc->read<bool> ( UIInventory ) &&
				!( GetAsyncKeyState ( 'W' ) || GetAsyncKeyState ( 'A' ) || GetAsyncKeyState ( 'S' ) || GetAsyncKeyState ( 'D' ) || GetAsyncKeyState ( VK_LBUTTON ) ) &&
				!game::m_rLocalInfo.m_pModelState->get_sprinting ( mist::misc.get ( ) ) )
				mist::misc->write<float> ( m_pTime + 0xFC, 5.0f );
			else
				mist::misc->write<float> ( m_pTime + 0xFC, 1.f );
		}

		sleep_for ( 1ms );
	}
}

void slow ( )
{
	std::thread pica ( picus1 );
	pica.detach ( );

	bool pico = false;
	bool nigger = false;
	while ( true )
	{
		uintptr_t m_pTime = mist::misc->read<uintptr_t> ( addr::unityplayer + offsets::timescale + 7 * 8 );
		if ( !m_pTime )
		{
			sleep_for ( milliseconds ( 700 ) );
			continue;
		}

		m_pMovement = mist::misc->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::basemovement );
		if ( !m_pMovement )
		{
			sleep_for ( milliseconds ( 400 ) );
			continue;
		}

		if ( g_Vars.misc.fakelag )
			mist::misc->write<float> ( ( uintptr_t ) game::m_pLocalplayer + offsets::clienttickinterval, 0.15f ); // baseply->clienttickinterval
		else
			mist::misc->write<float> ( ( uintptr_t ) game::m_pLocalplayer + offsets::clienttickinterval, 0.05f );

		if ( g_Vars.misc.btimescale && GetAsyncKeyState ( g_Vars.misc.timescale ) )
		{
			mist::misc->write<float> ( m_pTime + 0xFC, 3.f );
			mist::misc->write<float> ( ( uintptr_t ) game::m_pLocalplayer + offsets::clienttickinterval, 0.05f );
			pico = true;
		}
		else if ( pico == true )
		{
			mist::misc->write<float> ( m_pTime + 0xFC, 1.f );

			pico = false;
		}

		uintptr_t m_pEyes = mist::misc->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::eyes );
		if ( g_Vars.misc.bview_offset && GetAsyncKeyState ( g_Vars.misc.view_offset ) )
			mist::misc->write<Vector3> ( m_pEyes + 0x38, { m_vecEyes.x, m_vecEyes.y + g_Vars.misc.y, m_vecEyes.z } );
		else
			m_vecEyes = mist::misc->read<Vector3> ( m_pEyes + 0x38 );

		if ( g_Vars.misc.fov != 70.f && !g_Vars.misc.zoom_on_scope )
		{
			uintptr_t m_pGraphics = mist::misc->read<uintptr_t> ( addr::gameassembly + offsets::fov_offset );
			uintptr_t m_pGraphicsInstance = mist::misc->read<uintptr_t> ( m_pGraphics + 0xB8 );
			mist::misc->write<float> ( m_pGraphicsInstance + 0x18, g_Vars.misc.fov );
		}

		if ( g_Vars.misc.zoom_on_scope )
		{
			uintptr_t m_pGraphics = mist::misc->read<uintptr_t> ( addr::gameassembly + offsets::fov_offset );
			uintptr_t m_pGraphicsInstance = mist::misc->read<uintptr_t> ( m_pGraphics + 0xB8 );

			float m_flDesiredFov = g_Vars.misc.fov;
			float m_flDesiredZoom = g_Vars.misc.fov / 1.5;
			float m_flCurrentFov = mist::misc->read<float> ( m_pGraphicsInstance + 0x18 );

			static bool m_bShouldAnimate = true;
			if ( GetAsyncKeyState ( VK_RBUTTON ) )
			{
				if ( m_bShouldAnimate )
				{
					if ( m_flCurrentFov <= m_flDesiredZoom )
					{
						mist::misc->write<float> ( m_pGraphicsInstance + 0x18, m_flDesiredZoom );
						m_bShouldAnimate = false;
						continue;
					}

					mist::misc->write<float> ( m_pGraphicsInstance + 0x18, m_flCurrentFov - 1.f );
				}
			}
			else
			{
				if ( m_flCurrentFov >= m_flDesiredFov )
				{
					mist::misc->write<float> ( m_pGraphicsInstance + 0x18, m_flDesiredFov );
					m_bShouldAnimate = true;
					continue;
				}

				mist::misc->write<float> ( m_pGraphicsInstance + 0x18, m_flCurrentFov + 1.f );
			}
		}

		sleep_for ( milliseconds ( 5 ) );
	}
}

void experimental ( )
{
	uintptr_t m_pMetabolism = mist::other->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + 0x618 );
	uintptr_t m_pMetabolismAttributeRadiation = mist::other->read<uintptr_t> ( m_pMetabolism + 0x50 );
	mist::other->write<float> ( m_pMetabolismAttributeRadiation + 0x20, 0.f );
	//logging::print ( TYPE_DEBUG, "%f", m_pRadiationValue );
}

namespace keybinds
{
	bool menu = true;
};

void chams ( )
{
	for ( const auto& entry : game::players )
	{
		const auto& entity = entry.first;
		if ( !entity )
			continue;

		if ( g_Vars.esp.chams )
		{
			auto entity2 = rust::get_base_entity ( mist::misc.get ( ), entity );
			auto player_model = mist::misc->read<uintptr_t> ( entity2 + offsets::player_model );
			auto player_skin_type = mist::misc->read<uintptr_t> ( player_model + offsets::player_skin_type );
			auto player_skin_component = mist::misc->read<uintptr_t> ( player_model + 0x190 + ( ( int ) player_skin_type * 0x8 ) );

			auto skin = mist::misc->read<uintptr_t> ( player_skin_component + 0x18 );
			auto skin_sz = mist::misc->read<uintptr_t> ( skin + 0x18 );

			for ( int i = NULL; i <= ( int ) skin_sz; i++ )
			{
				auto current_skin = mist::misc->read<uintptr_t> ( skin + 0x20 + ( i * 0x8 ) );

				mist::misc->write<uintptr_t> ( current_skin + 0x68, NULL );
				mist::misc->write<uintptr_t> ( current_skin + 0x70, NULL );
				mist::misc->write<uintptr_t> ( current_skin + 0x78, NULL );
			}
		}
	}
}

void killlocalplayer ( )
{
	uintptr_t movement = mist::misc->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::basemovement ); VASSERT ( movement );

	if ( g_Vars.misc.kill_localplayer && GetAsyncKeyState ( g_Vars.misc.kill_localplayer_key ) )
	{
		mist::misc->write<bool> ( movement + offsets::wasFalling, true ); // wasFalling
		mist::misc->write<Vector3> ( movement + offsets::previousVelocity, Vector3 ( 0, -20, 0 ) ); // previousVelocity
		mist::misc->write<float> ( movement + offsets::groundtime, 0.f );
	}
}

int misc::thread ( )
{
	uintptr_t m_pAmbientParams = mist::misc->read<uintptr_t> ( addr::tod_sky + 0x90 );
	mist::misc->write<float> ( m_pAmbientParams + 0x18, 5.f );

	std::thread nosleep ( fast );
	nosleep.detach ( );

	std::thread bigsleep ( slow );
	bigsleep.detach ( );

	std::thread atmo ( atmosphere );
	atmo.detach ( );

	std::thread fly ( flyhack );
	fly.detach ( );

	std::thread floot ( fast_loot );
	floot.detach ( );

	//std::thread noclipfix ( Main2 );
	//noclipfix.detach ( );

	while ( true )
	{
		spoof_admin ( );
		movement ( );
		no_recoil ( );
		chams ( );
		//hitsound ( );
		killlocalplayer ( );
		sleep_for ( milliseconds ( 15 ) );
	}
	return 1;
}