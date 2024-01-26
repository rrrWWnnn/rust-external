#include "object_loop.hpp"
#include "stdafx.hpp"
#include "sdk.hpp"
#include "baseplayer.hpp"
#include "cheat.hpp"

#define FERR_HASH_IMPL 1
#define HE(x, y) (x == y)
#include "fnv1a.hpp"
#include "overlay.hpp"
#include "aimbot.hpp"

ULONG64 SteamID3To64 ( ULONG64 id )
{
	return ( ( 1ULL << 56 ) | ( 1ULL << 52 ) | ( 1ULL << 32 ) | id );
}

namespace addr
{
	uintptr_t unityplayer;
	uintptr_t gameassembly;

	uintptr_t base_networkable;
	uintptr_t client_entities;
	uintptr_t entity_realm;
	uintptr_t buffer_list;
	uintptr_t object_list;
	uintptr_t camera_instance;
	uintptr_t tod_sky;
	std::size_t object_size;
};

namespace game
{
	BasePlayer* m_pLocalplayer;
	rLocal m_rLocalInfo;
	entSet players;
	staticStringColorSet stashes, animals, items, collectibles, ores, vehicles, special, barrel, traps, bag;
	staticStringListColorSet cupboard;
	raidSet raid;
	PlayerIdName user_cache;
	std::unordered_map<ULONG, rCache> cache;
	std::unordered_set<ULONG> friendlist;
	std::mutex draw_mutex, aim_mutex;
};

void object::setup ( )
{
	addr::unityplayer = mist::obj->get_module_base_address ( _ ( "UnityPlayer.dll" ) );
	addr::gameassembly = mist::obj->get_module_base_address ( _ ( "GameAssembly.dll" ) );

	addr::base_networkable = rust::get_base_networkable ( mist::obj.get ( ) );
	addr::client_entities = rust::get_client_entities ( mist::obj.get ( ) );
	addr::entity_realm = rust::get_entity_realm ( mist::obj.get ( ) );
	addr::buffer_list = rust::get_buffer_list ( mist::obj.get ( ) );
	addr::object_list = rust::get_object_list ( mist::obj.get ( ) );
	addr::object_size = rust::get_object_size ( mist::obj.get ( ) );
	//

	//if ( addr::gameassembly != 0 )
	//	logging::print ( TYPE_DEBUG, "UnityPlayer.dll: 0x%p", addr::unityplayer );
	//else
	//	logging::print ( TYPE_ERROR, "Couldn't find UnityPlayer.dll!" );
	//if ( addr::unityplayer != 0 )
	//	logging::print ( TYPE_DEBUG, "GameAssembly.dll: 0x%p", addr::gameassembly );
	//else
	//	logging::print ( TYPE_ERROR, "Couldn't find GameAssembly.dll!" );

	//logging::print ( TYPE_DEBUG, "Object list: 0x%p (%d)", addr::object_list, addr::object_size );

	std::thread game ( object::refresh_game );
	game.detach ( );
}

void object::refresh_game ( )
{
	uintptr_t gameobjectmanager = mist::obj->read<uintptr_t> ( addr::unityplayer + offsets::gom_offset );
	if ( !gameobjectmanager )
		return;

	while ( true )
	{

		addr::base_networkable = rust::get_base_networkable ( mist::obj.get ( ) );
		addr::client_entities = rust::get_client_entities ( mist::obj.get ( ) );
		addr::entity_realm = rust::get_entity_realm ( mist::obj.get ( ) );
		addr::buffer_list = rust::get_buffer_list ( mist::obj.get ( ) );
		addr::object_list = rust::get_object_list ( mist::obj.get ( ) );
		addr::object_size = rust::get_object_size ( mist::obj.get ( ) );

		uintptr_t tagged_objects = mist::obj->read<uintptr_t> ( gameobjectmanager + 0x8 );
		if ( !tagged_objects )
		{

			sleep_for ( 20ms );
			continue;
		}

		uintptr_t game_object = mist::obj->read<uintptr_t> ( tagged_objects + 0x10 );
		if ( !game_object )
		{

			sleep_for ( 20ms );
			continue;
		}

		uintptr_t object_class = mist::obj->read<uintptr_t> ( game_object + 0x30 );
		if ( !object_class )
		{

			sleep_for ( 20ms );
			continue;
		}

		addr::camera_instance = mist::obj->read<uintptr_t> ( object_class + 0x18 );

		if ( !addr::tod_sky )
		{
			uintptr_t m_pTODClass = rust::scan_for_class ( mist::obj.get ( ), _ ( "TOD_Sky" ) );
			uintptr_t m_pInstanceList = mist::obj->read<uintptr_t> ( m_pTODClass + 0xB8 );
			m_pInstanceList = mist::obj->read<uintptr_t> ( m_pInstanceList );
			uintptr_t m_pInstances = mist::obj->read<uintptr_t> ( m_pInstanceList + 0x10 );
			addr::tod_sky = mist::obj->read<uintptr_t> ( m_pInstances + 0x20 );
		}

		sleep_for ( seconds ( 4 ) );
	}
}

void raid_esp ( )
{
	uintptr_t m_pEffectClass = rust::scan_for_class ( mist::obj.get ( ), _ ( "EffectNetwork" ) );
	if ( !m_pEffectClass )
	{
		return;
	}

	while ( true )
	{
		if ( !g_Vars.esp.raid )
		{
			sleep_for ( milliseconds ( 800 ) );
			continue;
		}

		uintptr_t m_pEffect = mist::obj->read<uintptr_t> ( m_pEffectClass + 0xB8 );
		if ( !m_pEffect )
		{

			std::this_thread::sleep_for ( std::chrono::nanoseconds ( 500 ) );
			continue;
		}

		m_pEffect = mist::obj->read<uintptr_t> ( m_pEffect );
		if ( !m_pEffect )
		{

			std::this_thread::sleep_for ( std::chrono::nanoseconds ( 200 ) );
			continue;
		}

		Vector3 m_vecOrigin = mist::obj->read<Vector3> ( m_pEffect + 0x5C );
		if ( m_vecOrigin.empty ( ) )
		{

			std::this_thread::sleep_for ( std::chrono::nanoseconds ( 200 ) );
			continue;
		}

		std::string element_name = rust::get_class_name ( mist::obj.get ( ), m_pEffect );
		uintptr_t m_pName = mist::obj->read <uintptr_t> ( m_pEffect + 0x88 );
		std::wstring m_szPrefab = mist::obj->wread_unicode ( m_pName + 0x14, 128 );

		std::string m_szTranslated;

		if ( m_szPrefab.find ( _ ( L"explosion" ) ) == std::string::npos )
			continue;

		if ( m_szPrefab.find ( _ ( L"patrol" ) ) != std::string::npos )
			continue;

		if ( m_szPrefab.find ( _ ( L"c4" ) ) != std::string::npos )
			m_szTranslated = _ ( "C4" );

		else if ( m_szPrefab.find ( _ ( L"rocket" ) ) != std::string::npos )
			m_szTranslated = _ ( "ROCKET" );

		else if ( m_szPrefab.find ( _ ( L"satchel" ) ) != std::string::npos )
			m_szTranslated = _ ( "SATCHEL" );
		else
			continue;

		auto m_tpVars = std::make_tuple ( m_pEffect, m_szTranslated, m_vecOrigin, std::chrono::steady_clock::now ( ) );
		game::raid.emplace_back ( m_tpVars );

		std::this_thread::sleep_for ( std::chrono::microseconds ( 1200 ) );
	}
}

void object::main ( )
{
	while ( addr::object_size == 0 )
		sleep_for ( milliseconds ( 50 ) );

	uintptr_t m_pTODClass = rust::scan_for_class ( mist::obj.get ( ), _ ( "TOD_Sky" ) );
	uintptr_t m_pInstanceList = mist::obj->read<uintptr_t> ( m_pTODClass + 0xB8 );
	m_pInstanceList = mist::obj->read<uintptr_t> ( m_pInstanceList );
	uintptr_t m_pInstances = mist::obj->read<uintptr_t> ( m_pInstanceList + 0x10 );
	addr::tod_sky = mist::obj->read<uintptr_t> ( m_pInstances + 0x20 );

	std::thread raid ( raid_esp );
	raid.detach ( );

	/* ORES */
	constexpr auto metal = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/resource/ores/metal-ore.prefab" );
	constexpr auto metal2 = hashc_constfnv64_string ( "assets/bundled/prefabs/radtown/ore_metal.prefab" );
	constexpr auto metal3 = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/resource/ores_snow/metal-ore.prefab" );
	constexpr auto sulfur = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/resource/ores/sulfur-ore.prefab" );
	constexpr auto sulfur2 = hashc_constfnv64_string ( "assets/bundled/prefabs/radtown/ore_sulfur.prefab" );
	constexpr auto sulfur3 = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/resource/ores_snow/sulfur-ore.prefab" );
	constexpr auto stone = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/resource/ores/stone-ore.prefab" );
	constexpr auto stone2 = hashc_constfnv64_string ( "assets/bundled/prefabs/radtown/ore_stone.prefab" );
	constexpr auto stone3 = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/resource/ores_snow/stone-ore.prefab" );

	/* COLLECTABLES*/
	constexpr auto hemp = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/hemp/hemp-collectable.prefab" );
	constexpr auto mushroom = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/mushrooms/mushroom-cluster-6.prefab" );
	constexpr auto pumpkin = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/pumpkin/pumpkin-collectable.prefab" );
	constexpr auto corn = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/corn/corn-collectable.prefab" );
	constexpr auto berry = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/berry-white/berry-white-collectable.prefab" );
	constexpr auto potato = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/potato/potato-collectable.prefab" );
	constexpr auto csulfur = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/stone/sulfur-collectable.prefab" );
	constexpr auto cstone = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/stone/stone-collectable.prefab" );
	constexpr auto wood = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/wood/wood-collectable.prefab" );
	constexpr auto cmetal = hashc_constfnv64_string ( "assets/bundled/prefabs/autospawn/collectable/stone/metal-collectable.prefab" );

	/* ITEM FILTER */
	constexpr auto waterjug = hashc_constfnv64_string ( "waterjug (world)" );
	constexpr auto waterbucket = hashc_constfnv64_string ( "bucket.water (world)" );
	constexpr auto arrow = hashc_constfnv64_string ( "arrow.wooden (world)" );
	constexpr auto nail = hashc_constfnv64_string ( "ammo.nailgun.nails (world)" );

	/* CLASSES */
	constexpr auto item_hash = hashc_constfnv64_string ( "DroppedItem" );
	constexpr auto corpse_hash = hashc_constfnv64_string ( "PlayerCorpse" );
	constexpr auto ore_hash = hashc_constfnv64_string ( "OreResourceEntity" );
	constexpr auto heli_hash = hashc_constfnv64_string ( "BaseHelicopter" );
	constexpr auto bradley_hash = hashc_constfnv64_string ( "BradleyAPC" );
	constexpr auto container_hash = hashc_constfnv64_string ( "LootContainer" );
	constexpr auto collectible_hash = hashc_constfnv64_string ( "CollectibleEntity" );
	constexpr auto stash_hash = hashc_constfnv64_string ( "StashContainer" );
	//player entities
	constexpr auto player_hash = hashc_constfnv64_string ( "BasePlayer" );
	constexpr auto sc1 = hashc_constfnv64_string ( "Scientist" );
	constexpr auto sc2 = hashc_constfnv64_string ( "ScientistNPC" );
	constexpr auto sc3 = hashc_constfnv64_string ( "HTNAnimal" );
	constexpr auto sc4 = hashc_constfnv64_string ( "HumanNPC" );
	constexpr auto sc5 = hashc_constfnv64_string ( "VehicleVendor" );
	constexpr auto sc6 = hashc_constfnv64_string ( "NPCShopKeeper" );
	constexpr auto sc7 = hashc_constfnv64_string ( "TunnelDweller" );

	//animals
	constexpr auto bear_hash = hashc_constfnv64_string ( "Bear" );
	constexpr auto boar_hash = hashc_constfnv64_string ( "Boar" );
	constexpr auto chicken_hash = hashc_constfnv64_string ( "Chicken" );
	constexpr auto horse_hash = hashc_constfnv64_string ( "Horse" );
	constexpr auto stag_hash = hashc_constfnv64_string ( "Stag" );
	constexpr auto wolf_hash = hashc_constfnv64_string ( "Wolf" );
	constexpr auto zombie_hash = hashc_constfnv64_string ( "Zombie" );
	//vehicles
	constexpr auto minicopter_hash = hashc_constfnv64_string ( "MiniCopter" );
	constexpr auto scrapheli_hash = hashc_constfnv64_string ( "ScrapTransportHelicopter" );
	constexpr auto boat = hashc_constfnv64_string ( "MotorRowboat" );
	constexpr auto rhib = hashc_constfnv64_string ( "RHIB" );
	constexpr auto horse = hashc_constfnv64_string ( "RidableHorse" );
	//traps
	constexpr auto turret_hash = hashc_constfnv64_string ( "AutoTurret" );
	constexpr auto landmine_hash = hashc_constfnv64_string ( "Landmine" );
	constexpr auto beartrap_hash = hashc_constfnv64_string ( "BearTrap" );
	constexpr auto guntrap_hash = hashc_constfnv64_string ( "GunTrap" );
	constexpr auto flameturret_hash = hashc_constfnv64_string ( "FlameTurret" );
	constexpr auto samsite_hash = hashc_constfnv64_string ( "SamSite" );
	constexpr auto toolcupboard_hash = hashc_constfnv64_string ( "BuildingPrivlidge" );
	constexpr auto sleepingbag_hash = hashc_constfnv64_string ( "SleepingBag" );

	while ( true )
	{
		game::entSet players;
		game::staticStringColorSet stashes, animals, items, collectibles, ores, vehicles, special, barrel, traps, sleepingbag;
		game::staticStringListColorSet cupboard;

		addr::object_list = rust::get_object_list ( mist::obj.get ( ) );
		addr::object_size = rust::get_object_size ( mist::obj.get ( ) );

		for ( uint64_t i = 0x0; i < addr::object_size; i++ )
		{
			uintptr_t element = mist::obj->read<uintptr_t> ( addr::object_list + 0x20 + ( i * 0x08 ) );
			if ( !element )
				continue;

			std::string element_name = rust::get_class_name ( mist::obj.get ( ), element );
			uint64_t element_hash = hash_constfnv64_string ( element_name.c_str ( ) );
			if ( element_hash != item_hash &&
				element_hash != corpse_hash &&
				element_hash != ore_hash &&
				element_hash != heli_hash &&
				element_hash != bradley_hash &&
				element_hash != container_hash &&
				element_hash != collectible_hash &&
				element_hash != stash_hash &&
				element_hash != bear_hash &&
				element_hash != boar_hash &&
				element_hash != chicken_hash &&
				element_hash != horse_hash &&
				element_hash != stag_hash &&
				element_hash != wolf_hash &&
				element_hash != zombie_hash &&
				element_hash != player_hash &&
				element_hash != sc1 &&
				element_hash != sc2 &&
				element_hash != sc3 &&
				element_hash != sc4 &&
				element_hash != sc5 &&
				element_hash != sc6 &&
				element_hash != sc7 &&
				element_hash != minicopter_hash &&
				element_hash != scrapheli_hash &&
				element_hash != boat &&
				element_hash != rhib &&
				element_hash != horse &&
				element_hash != turret_hash &&
				element_hash != landmine_hash &&
				element_hash != beartrap_hash &&
				element_hash != guntrap_hash &&
				element_hash != flameturret_hash &&
				element_hash != samsite_hash &&
				element_hash != toolcupboard_hash &&
				element_hash != sleepingbag_hash
				)

				continue;

			uintptr_t base_mono_object = mist::obj->read<uintptr_t> ( element + 0x10 );
			if ( !base_mono_object )
				continue;

			uintptr_t object = mist::obj->read<uintptr_t> ( base_mono_object + 0x30 );
			if ( !object )
				continue;

			uintptr_t objectClass = mist::obj->read<uintptr_t> ( object + 0x30 );
			if ( !objectClass )
				continue;

			USHORT tag = mist::obj->read<USHORT> ( object + 0x54 );

			if ( tag == 6 )
			{
				BasePlayer* player = ( BasePlayer* ) rust::get_base_entity ( mist::obj.get ( ), objectClass );
				//player->mist_override ( mist::obj.get ( ) );
				if ( !player )
					continue;

				if ( i == 0x0 )
				{
					game::m_rLocalInfo.m_pPlayer = objectClass;
					game::m_pLocalplayer = player;

					Item* weapon = game::m_pLocalplayer->get_held_item ( mist::obj.get ( ) );

					if ( game::m_rLocalInfo.m_pWeapon != weapon )
					{
						game::m_rLocalInfo.m_pWeapon = weapon;
						game::m_rLocalInfo.m_pModelState = ( ModelState* ) mist::obj->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::modelState );
						game::m_rLocalInfo.m_szWeaponName = weapon->get_nice_name ( mist::obj.get ( ) );
						game::m_rLocalInfo.m_szBulletType = weapon->get_bullet_name ( mist::obj.get ( ) );
						game::m_rLocalInfo.m_hWeaponName = hash_constfnv64_string ( game::m_rLocalInfo.m_szWeaponName.c_str ( ) );
						game::m_rLocalInfo.m_hBulletType = hash_constfnv64_string ( game::m_rLocalInfo.m_szBulletType.c_str ( ) );
						game::m_rLocalInfo.m_vecLocal = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
						game::m_rLocalInfo.m_vInfo.m_bPrediction = rust::use_weapon_prediction ( );
						game::m_rLocalInfo.m_vInfo.m_flGravity = rust::get_bullet_gravity ( );
						game::m_rLocalInfo.m_vInfo.m_flSpeed = rust::get_bullet_speed ( );
					}
				}
				else
				{
					ULONG uid = player->get_uid ( mist::obj.get ( ) );

					if ( game::cache.find ( uid ) == game::cache.end ( ) )
					{
						Item* weapon = player->get_held_item ( mist::obj.get ( ) );
						rCache cache;
						cache.m_szName = player->get_name ( mist::obj.get ( ) );
						cache.m_szWeaponName = weapon->get_nice_name ( mist::obj.get ( ) );
						cache.m_pWeapon = weapon;
						cache.m_pLastWeapon = weapon;
						cache.m_iAmmo = weapon->get_bullet_amount ( mist::obj.get ( ) );
						cache.m_iMaxAmmo = weapon->get_bullet_max ( mist::obj.get ( ) );
						cache.player_model = mist::obj->read<uintptr_t> ( ( uintptr_t ) player + offsets::model );
						cache.transform = mist::obj->read<uintptr_t> ( cache.player_model + 0x48 );
						cache.m_bWhitelist = false;
						cache.m_bFriend = player->is_teammate ( mist::obj.get ( ) );
						cache.m_bSleeper = player->is_sleeping ( mist::obj.get ( ) );
						if ( game::friendlist.find ( uid ) != game::friendlist.end ( ) )
							cache.m_bFriend = true;

						if ( ( element_name.length ( ) > 9 && ( element_name [9] == 'N' ) || ( element_name [0] == 'S' && element_name [1] == 'c' ) ) || ( element_name [0] == 'H' && element_name [1] == 'T' ) || ( element_name [0] == 'N' && element_name [3] == 'S' ) || ( element_name [0] == 'V' && element_name [7] == 'V' ) ) // Scientist
							cache.m_szName = _ ( "Scientist" );
						if ( ( element_name [0] == 'T' && element_name [6] == 'D' ) ) // TunnelDweller
							cache.m_szName = _ ( "Tunnel Dwellers" );

						if ( game::user_cache.find ( SteamID3To64 ( uid ) ) == game::user_cache.end ( ) )
							game::user_cache.emplace ( SteamID3To64 ( uid ), cache.m_szName );

						game::cache.emplace ( uid, cache );
					}
					Item* weapon = player->get_held_item ( mist::obj.get ( ) );
					game::cache [uid].m_bFriend = player->is_teammate ( mist::obj.get ( ) );
					game::cache [uid].m_pWeapon = weapon;
					game::cache [uid].m_szWeaponName = weapon->get_nice_name ( mist::obj.get ( ) );
					game::cache [uid].m_iAmmo = weapon->get_bullet_amount ( mist::obj.get ( ) );
					game::cache [uid].m_iMaxAmmo = weapon->get_bullet_max ( mist::obj.get ( ) );

					game::cache [uid].player_model = mist::obj->read<uintptr_t> ( ( uintptr_t ) player + offsets::model );
					game::cache [uid].transform = mist::obj->read<uintptr_t> ( game::cache [uid].player_model + 0x48 );
					game::cache [uid].m_bSleeper = player->is_sleeping ( mist::obj.get ( ) );

					entity_type type;

					if ( game::cache [uid].m_szName == _ ( "Scientist" ) )
					{
						type = entity_type::Scientist;
					}
					else if ( game::cache [uid].m_szName == _ ( "Tunnel Dwellers" ) )
					{
						type = entity_type::TunnelDweller;
					}
					else
					{
						type = entity_type::BasePlayer;
					}

					auto pair = std::make_pair ( objectClass, type );

					players.emplace_back ( pair );
				}
			}
			else if ( g_Vars.esp.item && ( element_hash == item_hash || element_hash == corpse_hash ) ) // DroppedItem PlayerCorpse
			{
				uintptr_t pObjName = mist::obj->read<uintptr_t> ( object + 0x60 );
				std::string object_name = mist::obj->read_ascii ( pObjName, 128 );
				const auto object_hash = hash_constfnv64_string ( object_name.c_str ( ) );

				if ( object_hash == waterjug || object_hash == waterbucket )
					continue;

				std::string m_szItemName = object_name;

				if ( object_hash == arrow || object_hash == nail )
					continue;

				constexpr auto corpse = hashc_constfnv64_string ( "assets/prefabs/player/player_corpse.prefab" );

				std::string_view str1 = _ ( " (world)" );
				std::string::size_type i = m_szItemName.find ( str1 );
				if ( i != std::string::npos )
					m_szItemName.erase ( i, str1.length ( ) );
				if ( object_hash == corpse )
					m_szItemName = _ ( "corpse" );
				else if ( m_szItemName.find ( _ ( "backpack" ) ) != std::string::npos )
					m_szItemName = _ ( "backpack" );
				else if ( m_szItemName.find ( _ ( "itemdrop" ) ) != std::string::npos )
					m_szItemName = _ ( "inventory sash" );

				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				FloatColor clr = { };
				if ( ( m_szItemName [0] == 'r' && m_szItemName [1] == 'i' ) ||
					( m_szItemName [0] == 's' && m_szItemName [2] == 'm' && m_szItemName [2] == 'g' ) ||
					( m_szItemName [0] == 'p' && m_szItemName [5] == 'l' ) ||
					( m_szItemName [0] == 'b' && m_szItemName [2] == 'w' ) ||
					( m_szItemName [0] == 'l' && m_szItemName [1] == 'm' ) )
				{
					if ( !g_Vars.esp.item_weapon )
						continue;

					clr = g_Vars.esp.item_weapon_color;
				}

				else if ( m_szItemName.length ( ) > 8 &&
					( ( m_szItemName [0] == 'r' && m_szItemName [7] == 'l' ) ||
						( m_szItemName [0] == 'c' && m_szItemName [7] == 'w' ) || // crossbow
						( m_szItemName [0] == 's' && m_szItemName [4] == 'g' ) ) )
				{
					if ( !g_Vars.esp.item_weapon )
						continue;

					clr = g_Vars.esp.item_weapon_color;
				}
				else if ( ( m_szItemName [0] == 'c' && m_szItemName [2] == 'r' && m_szItemName [4] == 's' ) ||
					( m_szItemName [0] == 'b' && m_szItemName [3] == 'k' && m_szItemName [4] == 'p' ) )
				{
					if ( !g_Vars.esp.item_corpse )
						continue;

					clr = g_Vars.esp.item_corpse_color;
				}
				else
				{
					if ( !g_Vars.esp.item_other )
						continue;

					clr = g_Vars.esp.item_other_color;
				}

				auto pair = std::make_tuple ( objectClass, m_szItemName, position, clr );

				items.emplace_back ( pair );
			}
			else if ( g_Vars.esp.ores && ( element_hash == ore_hash ) ) // OreResourceEntity
			{
				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				uintptr_t pObjName = mist::obj->read<uintptr_t> ( object + 0x60 );
				std::string object_name = mist::obj->read_ascii ( pObjName, 128 );
				const auto object_hash = hash_constfnv64_string ( object_name.c_str ( ) );

				std::string translated_name = "";
				FloatColor clr = { };

				if ( g_Vars.esp.metal && ( object_hash == metal || object_hash == metal2 || object_hash == metal3 ) )
				{
					clr = g_Vars.esp.metal_color;

					translated_name = _ ( "METAL ORE" );
				}
				else if ( g_Vars.esp.sulfur && ( object_hash == sulfur || object_hash == sulfur2 || object_hash == sulfur3 ) )
				{
					clr = g_Vars.esp.sulfur_color;

					translated_name = _ ( "SULFUR ORE" );
				}
				else if ( g_Vars.esp.stone && ( object_hash == stone || object_hash == stone2 || object_hash == stone3 ) )
				{
					clr = g_Vars.esp.stone_color;

					translated_name = _ ( "STONE ORE" );
				}
				else
					continue;

				auto pair = std::make_tuple ( objectClass, translated_name, position, clr );

				ores.emplace_back ( pair );
			} // BaseHelicopter
			else if ( g_Vars.esp.special && ( element_hash == heli_hash || element_hash == bradley_hash ) ) // BaseHelicopter Bradley
			{
				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				FloatColor clr = { };

				clr = g_Vars.esp.special_color;

				auto pair = std::make_tuple ( objectClass, element_name, position, clr );
				special.emplace_back ( pair );
			}
			else if ( g_Vars.esp.radtown && element_hash == container_hash ) // LootContainer
			{
				uintptr_t pObjName = mist::obj->read<uintptr_t> ( object + 0x60 );
				std::string object_name = mist::obj->read_ascii ( pObjName, 128 );
				if ( object_name [23] == 'r' )
				{
					FloatColor clr;
					std::string translated_name = "";
					if ( object_name [31] == 'l' && object_name [36] == 'b' )
					{
						if ( !g_Vars.esp.barrel )
							continue;
						translated_name = _ ( "barrel" );
						clr = g_Vars.esp.barrel_color;
					}
					else if ( object_name [31] == 'c' )
					{
						if ( object_name [37] == 'e' )
						{
							if ( !g_Vars.esp.ecrate )
								continue;
							translated_name = _ ( "elite crate" );
							clr = g_Vars.esp.ecrate_color;
						}
						else if ( object_name [44] == '2' || object_name [37] == 'm' )
						{
							if ( !g_Vars.esp.crate )
								continue;
							translated_name = _ ( "crate" );
							clr = g_Vars.esp.crate_color;
						}
						else if ( object_name [37] == 't' )
						{
							if ( !g_Vars.esp.toolbox )
								continue;
							translated_name = _ ( "toolbox" );
							clr = g_Vars.esp.toolbox_color;
						}
						else
						{
							if ( !g_Vars.esp.mcrate )
								continue;
							translated_name = _ ( "military crate" );
							clr = g_Vars.esp.mcrate_color;
						}
					}
					else if ( object_name [31] == 'o' )
					{
						if ( !g_Vars.esp.oil )
							continue;
						translated_name = _ ( "oil" );
						clr = g_Vars.esp.oil_color;
					}
					else if ( object_name [31] == 'm' )
					{
						if ( !g_Vars.esp.minecart )
							continue;
						translated_name = _ ( "minecart" );
						clr = g_Vars.esp.minecart_color;
					}
					else
						continue;

					Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
					if ( position.empty ( ) )
						continue;

					float m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );
					if ( m_flDistance > 180.f )
						continue;

					if ( m_flDistance > g_Vars.esp.distance_limit )
						continue;

					auto pair = std::make_tuple ( objectClass, translated_name, position, clr );
					barrel.emplace_back ( pair );
				}
				continue;
			}
			else if ( g_Vars.esp.animals && ( ( element_hash == bear_hash ) || ( element_hash == boar_hash ) || ( element_hash == chicken_hash ) || ( element_hash == horse_hash ) || ( element_hash == stag_hash ) || ( element_hash == wolf_hash ) ) )
			{
				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				FloatColor clr = { };
				clr = g_Vars.esp.animals_color;

				const auto pair = std::make_tuple ( objectClass, element_name, position, clr );
				animals.emplace_back ( pair );
			}
			else if ( g_Vars.esp.collectable && element_hash == collectible_hash ) // CollectibleEntity
			{
				uintptr_t pObjName = mist::obj->read<uintptr_t> ( object + 0x60 );
				std::string object_name = mist::obj->read_ascii ( pObjName, 128 );
				std::string translated_name;
				if ( object_name.length ( ) > 50 && ( object_name [23] == 'a' && object_name [33] == 'c' ) )
				{
					const auto object_hash = hash_constfnv64_string ( object_name.c_str ( ) );

					if ( object_hash == hemp ) // assets/bundled/prefabs/autospawn/collectable/hemp/hemp-collectable.prefab
						translated_name = _ ( "hemp" );
					else if ( object_hash == mushroom ) // assets/bundled/prefabs/autospawn/collectable/mushrooms/mushroom-cluster-6.prefab
						translated_name = _ ( "mushroom" );
					else if ( object_hash == pumpkin ) // assets/bundled/prefabs/autospawn/collectable/pumpkin/pumpkin-collectable.prefab
						translated_name = _ ( "pumpkin" );
					else if ( object_hash == corn ) // assets/bundled/prefabs/autospawn/collectable/corn/corn-collectable.prefab
						translated_name = _ ( "corn" );
					else if ( object_hash == berry ) // assets/bundled/prefabs/autospawn/collectable/berry-white/berry-white-collectable.prefab
						translated_name = _ ( "berry" );
					else if ( object_hash == potato ) // assets/bundled/prefabs/autospawn/collectable/potato/potato-collectable.prefab
						translated_name = _ ( "potato" );

					else if ( object_hash == csulfur )
						translated_name = _ ( "sulfur" );
					else if ( object_hash == cstone )
						translated_name = _ ( "stone" );
					else if ( object_hash == wood )
						translated_name = _ ( "wood" );
					else if ( object_hash == cmetal )
						translated_name = _ ( "metal" );
					else
						continue;
				}
				else
					continue;

				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				FloatColor collColor;

				if ( translated_name [0] == 'h' && g_Vars.esp.hemp ) // hemp
					collColor = g_Vars.esp.hemp_color;
				else if ( translated_name [0] == 'm' && g_Vars.esp.mushroom ) // mushroom
					collColor = g_Vars.esp.mushroom_color;
				else if ( translated_name [0] == 'c' && g_Vars.esp.corn ) //  corn
					collColor = g_Vars.esp.corn_color;
				else if ( translated_name [0] == 'b' && g_Vars.esp.berry ) // berry
					collColor = g_Vars.esp.berry_color;
				else if ( translated_name [2] == 't' && g_Vars.esp.potato ) // potato
					collColor = g_Vars.esp.potato_color;
				else if ( translated_name [3] == 'f' && g_Vars.esp.csulfur ) // sulfur
					collColor = g_Vars.esp.csulfur_color;
				else if ( translated_name [1] == 't' && g_Vars.esp.cstone ) // stone
					collColor = g_Vars.esp.cstone_color;
				else if ( translated_name [4] == 'l' && g_Vars.esp.cmetal ) // metal
					collColor = g_Vars.esp.cmetal_color;
				else if ( translated_name [0] == 'w' && g_Vars.esp.cwood )
					collColor = g_Vars.esp.cwood_color;
				else
					continue;

				float m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );
				if ( m_flDistance > 250.f )
					continue;

				if ( m_flDistance > g_Vars.esp.distance_limit )
					continue;

				const auto pair = std::make_tuple ( objectClass, translated_name, position, collColor );
				collectibles.emplace_back ( pair );
			}
			else if ( g_Vars.esp.stash && element_hash == stash_hash )
			{
				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				FloatColor clr = { };
				clr = g_Vars.esp.stash_color;

				const auto pair = std::make_tuple ( objectClass, _ ( "stash" ), position, clr );

				stashes.emplace_back ( pair );
			}
			/*
				element_hash != minicopter_hash &&
				element_hash != scrapheli_hash &&
				element_hash != boat &&
				element_hash != rhib &&
				element_hash != horse
			*/
			else if ( g_Vars.esp.vehicles && ( element_hash == minicopter_hash || element_hash == scrapheli_hash || element_hash == boat || element_hash == rhib || element_hash == horse ) )
			{
				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				uintptr_t pObjName = mist::obj->read<uintptr_t> ( object + 0x60 );
				std::string object_name = mist::obj->read_ascii ( pObjName, 128 );
				const auto object_hash = hash_constfnv64_string ( object_name.c_str ( ) );

				std::string translated_name = "";
				FloatColor clr = { };

				if ( g_Vars.esp.vehicles_helicopters && ( element_hash == minicopter_hash || element_hash == scrapheli_hash ) )
				{
					clr = g_Vars.esp.vehicles_helicopters_color;
					translated_name = ( element_hash == minicopter_hash ? _ ( "Minicopter" ) : _ ( "ScrapHelicopter" ) );
				}
				else if ( g_Vars.esp.vehicles_boats && ( element_hash == boat || element_hash == rhib ) )
				{
					clr = g_Vars.esp.vehicles_boats_color;
					translated_name = ( element_hash == boat ? _ ( "Boat" ) : _ ( "RHIB" ) );
				}
				else if ( g_Vars.esp.vehicles_horse && ( element_hash == horse ) )
				{
					clr = g_Vars.esp.vehicles_horse_color;
					translated_name = _ ( "Horse" );
				}
				else
					continue;

				auto pair = std::make_tuple ( objectClass, translated_name, position, clr );
				vehicles.emplace_back ( pair );
			}
			else if ( g_Vars.esp.traps && ( element_hash == turret_hash || element_hash == flameturret_hash || element_hash == landmine_hash || element_hash == beartrap_hash || element_hash == samsite_hash || element_hash == guntrap_hash ) )
			{
				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				uintptr_t pObjName = mist::obj->read<uintptr_t> ( object + 0x60 );
				std::string object_name = mist::obj->read_ascii ( pObjName, 128 );
				const auto object_hash = hash_constfnv64_string ( object_name.c_str ( ) );
				std::string translated_name = "";
				FloatColor clr { };

				if ( g_Vars.esp.autoturret && HE ( element_hash, turret_hash ) )
				{
					translated_name = _ ( "Auto-turret" );
					clr = g_Vars.esp.autoturret_color;
				}
				else if ( g_Vars.esp.flameturret && HE ( element_hash, flameturret_hash ) )
				{
					translated_name = _ ( "Flame-turret" );
					clr = g_Vars.esp.flameturret_color;
				}
				else if ( g_Vars.esp.landmine && HE ( element_hash, landmine_hash ) )
				{
					translated_name = _ ( "Land-mine" );
					clr = g_Vars.esp.landmine_color;
				}
				else if ( g_Vars.esp.beartrap && HE ( element_hash, beartrap_hash ) )
				{
					translated_name = _ ( "Bear-trap" );
					clr = g_Vars.esp.beartrap_color;
				}
				else if ( g_Vars.esp.samsite && HE ( element_hash, samsite_hash ) )
				{
					translated_name = _ ( "SAM-site" );
					clr = g_Vars.esp.samsite_color;
				}
				else if ( g_Vars.esp.shotguntrap && HE ( element_hash, guntrap_hash ) )
				{
					translated_name = _ ( "Shotgun-trap" );
					clr = g_Vars.esp.shotguntrap_color;
				}
				else
					continue;

				auto pair = std::make_tuple ( objectClass, translated_name, position, clr );
				traps.emplace_back ( pair );
			}
			else if ( g_Vars.esp.toolcupboard && element_hash == toolcupboard_hash )
			{
				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				uintptr_t pObjName = mist::obj->read<uintptr_t> ( object + 0x60 );
				std::string object_name = mist::obj->read_ascii ( pObjName, 128 );
				const auto object_hash = hash_constfnv64_string ( object_name.c_str ( ) );
				FloatColor clr = g_Vars.esp.toolcupboard_color;
				std::vector<std::string> usernames;
				if ( g_Vars.esp.toolcupboard_list )
				{
					uintptr_t m_pList = mist::obj->read<uintptr_t> ( element + 0x568 );
					uintptr_t m_pAuthorizedPlayersList = mist::obj->read<uintptr_t> ( m_pList + 0x10 );
					uint32_t m_pAuthorizedPlayersSize = mist::obj->read<uint32_t> ( m_pList + 0x18 );
					for ( uint32_t i = 0; i < m_pAuthorizedPlayersSize; ++i )
					{
						uintptr_t m_pEntry = mist::obj->read<uintptr_t> ( m_pAuthorizedPlayersList + 0x20 + ( i * 0x08 ) ); CASSERT ( m_pEntry );
						std::string username = mist::obj->read_unicode ( mist::obj->read<uintptr_t> ( m_pEntry + 0x18 ) + 0x14, 32 );

						if ( !username.empty ( ) )
							usernames.emplace_back ( username );
					}
				}

				auto pair = std::make_tuple ( objectClass, usernames, position, clr );
				cupboard.emplace_back ( pair );
			}
			else if ( g_Vars.esp.sleepingbag && element_hash == sleepingbag_hash )
			{
				Vector3 position = mist::obj->read<Vector3> ( rust::get_object_pos_component ( mist::obj.get ( ), objectClass ) + 0x90 );
				if ( position.empty ( ) )
					continue;

				uintptr_t pObjName = mist::obj->read<uintptr_t> ( object + 0x60 );
				std::string object_name = mist::obj->read_ascii ( pObjName, 128 );
				const auto object_hash = hash_constfnv64_string ( object_name.c_str ( ) );
				std::string translated_name = _ ( "bag" );
				FloatColor clr { 255, 255, 255 };

				ULONG64 owner = SteamID3To64 ( mist::obj->read<ULONG64> ( element + 0x3F0 ) );
				std::string owner_name;
				if ( game::user_cache.find ( owner ) == game::user_cache.end ( ) )
				{
					//steam::get_username_steam64 ( owner, owner_name );
					game::user_cache.emplace ( owner, owner_name );
				}
				else
					owner_name = game::user_cache [owner];

				translated_name = _ ( "bag (" ) + owner_name + _ ( ")" );

				auto pair = std::make_tuple ( objectClass, translated_name, position, clr );
				sleepingbag.emplace_back ( pair );
			}

			_mm_pause ( );
		}

		game::players = players;
		game::items = items;
		game::ores = ores;
		game::collectibles = collectibles;
		game::barrel = barrel;
		game::stashes = stashes;
		game::animals = animals;
		game::special = special;
		game::vehicles = vehicles;
		game::traps = traps;
		game::bag = sleepingbag;
		game::cupboard = cupboard;

		sleep_for ( 1s );
	}
}