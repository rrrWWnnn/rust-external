#include "Config.hpp"
#include "stdafx.hpp"
#include <fstream>
#include <iomanip>
#include "Menu/Framework/GUI.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include <shlobj.h>
#include <sstream>

BOOL ConfigManager::DirectoryExists ( LPCTSTR szPath )
{
	DWORD dwAttrib = GetFileAttributes ( szPath );

	return ( dwAttrib != INVALID_FILE_ATTRIBUTES &&
		( dwAttrib & FILE_ATTRIBUTE_DIRECTORY ) );
}

std::vector<std::string> ConfigManager::GetConfigs ( )
{
	static bool created_cfg = true;
	if ( created_cfg )
	{
		namespace fs = std::experimental::filesystem;
		fs::path full_path ( GetDocumentsDirectory ( ) );
		std::wstring str = full_path.wstring ( ) + _ ( L"\\phantom.tech" );
		std::wstring str2 = full_path.wstring ( ) + _ ( L"\\phantom.tech\\sounds" );

		CreateDirectoryW ( str.c_str ( ), nullptr );
		CreateDirectoryW ( str2.c_str ( ), nullptr );

		str += _ ( L"\\cfg" );
		CreateDirectoryW ( str.c_str ( ), nullptr );

		created_cfg = false;
	}

	std::string config_extension = _ ( ".json" );
	std::vector<std::string> names;

	WIN32_FIND_DATAA find_data;
	HANDLE preset_file = FindFirstFileA ( GetDocumentsDirectory ( ).append ( _ ( "\\phantom.tech\\cfg\\*" ) + config_extension ).c_str ( ), &find_data );

	if ( preset_file != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				continue;

			std::string s = find_data.cFileName;
			int pos = s.find ( _ ( ".json" ) );

			s.erase ( s.begin ( ) + pos, s.begin ( ) + pos + 5 );

			names.push_back ( s.c_str ( ) );
		} while ( FindNextFileA ( preset_file, &find_data ) );

		FindClose ( preset_file );
	}

	return names;
}

void ConfigManager::LoadConfig ( std::string configname )
{
	std::ifstream input_file = std::ifstream ( GetDocumentsDirectory ( ).append ( _ ( "\\phantom.tech\\cfg\\" ) + configname + _ ( ".json" ) ).c_str ( ) );
	if ( !input_file.good ( ) )
		return;

	json cfg = json::parse ( input_file );

	try
	{
		g_Vars.aimbot.enabled = cfg ["aimbot"] ["enabled"];
		g_Vars.aimbot.aim_key = cfg ["aimbot"] ["aimbot_key"];
		g_Vars.aimbot.hitbox = cfg ["aimbot"] ["hitbox"];
		g_Vars.aimbot.aim_fov = cfg ["aimbot"] ["fov"];
		g_Vars.aimbot.smoothing = cfg ["aimbot"] ["smoothing"];
		g_Vars.aimbot.terraincheck = cfg ["aimbot"] ["terrain_check"];

		g_Vars.misc.thickness = cfg ["aimbot"] ["accuracy"] ["big_bullet"];
		g_Vars.misc.flthickness = cfg ["aimbot"] ["accuracy"] ["big_bullet_scale"];
		g_Vars.misc.nosway = cfg ["aimbot"] ["accuracy"] ["remove_scope_sway"];
		g_Vars.misc.nospread = cfg ["aimbot"] ["accuracy"] ["remove_spread"];
		g_Vars.misc.noshotgunspread = cfg ["aimbot"] ["accuracy"] ["remove_shotgun_spread"];
		g_Vars.misc.jumpshoot = cfg ["aimbot"] ["accuracy"] ["shoot_midair"];
		g_Vars.misc.recoil_pitch = cfg ["aimbot"] ["accuracy"] ["recoil_percentage_x"];
		g_Vars.misc.recoil_yaw = cfg ["aimbot"] ["accuracy"] ["recoil_percentage_y"];

		g_Vars.aimbot.ignore_friends = cfg ["aimbot"] ["special"] ["team_check"];
		g_Vars.aimbot.silent = cfg ["aimbot"] ["special"] ["silent_aim"];

		g_Vars.misc.rapidfire = cfg ["aimbot"] ["other"] ["rapid_fire"];
		g_Vars.misc.rapidfire_speed = cfg ["aimbot"] ["other"] ["rapid_fire_speed"];
		g_Vars.misc.eoka = cfg ["aimbot"] ["other"] ["insta_eoka"];
		g_Vars.misc.compound = cfg ["aimbot"] ["other"] ["insta_compound_bow"];
		g_Vars.misc.fastbow = cfg ["aimbot"] ["other"] ["fast_bow"];
		g_Vars.misc.force_automatic = cfg ["aimbot"] ["other"] ["force_automatic"];
		g_Vars.misc.force_semiautomatic = cfg ["aimbot"] ["other"] ["force_semi_automatic"];

		g_Vars.esp.enabled = cfg ["esp"] ["general"] ["enable"];
		g_Vars.esp.chams = cfg ["esp"] ["general"] ["chams"];
		g_Vars.esp.box = cfg ["esp"] ["general"] ["box"];
		g_Vars.esp.name = cfg ["esp"] ["general"] ["name"];
		g_Vars.esp.health = cfg ["esp"] ["general"] ["health"];
		g_Vars.esp.health_override = cfg ["esp"] ["general"] ["health_override"];
		g_Vars.esp.skeleton = cfg ["esp"] ["general"] ["skeleton"];
		g_Vars.esp.distance = cfg ["esp"] ["general"] ["distance"];
		g_Vars.esp.weapon = cfg ["esp"] ["general"] ["weapon"];
		g_Vars.esp.snaplines = cfg ["esp"] ["general"] ["snaplines"];
		g_Vars.esp.npc = cfg ["esp"] ["general"] ["npc"];
		g_Vars.esp.tunneldweller = cfg ["esp"] ["general"] ["tunneldweller"];
		g_Vars.esp.sleeper = cfg ["esp"] ["general"] ["sleeper"];
		g_Vars.esp.teammate = cfg ["esp"] ["general"] ["teammate"];
		g_Vars.esp.highlight_target = cfg ["esp"] ["general"] ["highlight_target"];
		g_Vars.esp.item_weapon = cfg ["esp"] ["general"] ["item_weapon"];
		g_Vars.esp.item_corpse = cfg ["esp"] ["general"] ["item_corpse"];
		g_Vars.esp.item_other = cfg ["esp"] ["general"] ["item_other"];
		g_Vars.esp.animal = cfg ["esp"] ["general"] ["animal"];

		g_Vars.esp.item_info = cfg ["esp"] ["otheresp"] ["hotbar_esp"];
		g_Vars.esp.raid = cfg ["esp"] ["otheresp"] ["raid"];
		g_Vars.esp.hide = cfg ["esp"] ["otheresp"] ["hide"];
		g_Vars.esp.toolcupboard = cfg ["esp"] ["otheresp"] ["toolcupboard"];
		g_Vars.esp.distance_limit = cfg ["esp"] ["otheresp"] ["distance_limit"];

		g_Vars.esp.stash = cfg ["esp"] ["worldenvironment"] ["stash"];

		g_Vars.esp.ores = cfg ["esp"] ["worldenvironment"] ["ores"];
		g_Vars.esp.stone = cfg ["esp"] ["worldenvironment"] ["stone"];
		g_Vars.esp.sulfur = cfg ["esp"] ["worldenvironment"] ["sulfur"];
		g_Vars.esp.metal = cfg ["esp"] ["worldenvironment"] ["metal"];

		g_Vars.esp.vehicles = cfg ["esp"] ["worldenvironment"] ["vehicles"];
		g_Vars.esp.vehicles_boats = cfg ["esp"] ["worldenvironment"] ["vehicles_boats"];
		g_Vars.esp.vehicles_horse = cfg ["esp"] ["worldenvironment"] ["vehicles_horse"];
		g_Vars.esp.vehicles_helicopters = cfg ["esp"] ["worldenvironment"] ["vehicles_helicopters"];
		g_Vars.esp.special = cfg ["esp"] ["worldenvironment"] ["vehicles_special"];

		g_Vars.esp.traps = cfg ["esp"] ["worldenvironment"] ["traps"];
		g_Vars.esp.autoturret = cfg ["esp"] ["worldenvironment"] ["autoturret"];
		g_Vars.esp.flameturret = cfg ["esp"] ["worldenvironment"] ["flameturret"];
		g_Vars.esp.shotguntrap = cfg ["esp"] ["worldenvironment"] ["shotguntrap"];
		g_Vars.esp.landmine = cfg ["esp"] ["worldenvironment"] ["landmine"];
		g_Vars.esp.beartrap = cfg ["esp"] ["worldenvironment"] ["beartrap"];
		g_Vars.esp.samsite = cfg ["esp"] ["worldenvironment"] ["samsite"];

		g_Vars.esp.collectable = cfg ["esp"] ["worldenvironment"] ["collectables"];
		g_Vars.esp.hemp = cfg ["esp"] ["worldenvironment"] ["hemp"];
		g_Vars.esp.mushroom = cfg ["esp"] ["worldenvironment"] ["mushroom"];
		g_Vars.esp.corn = cfg ["esp"] ["worldenvironment"] ["corn"];
		g_Vars.esp.berry = cfg ["esp"] ["worldenvironment"] ["berry"];
		g_Vars.esp.potato = cfg ["esp"] ["worldenvironment"] ["potato"];
		g_Vars.esp.cstone = cfg ["esp"] ["worldenvironment"] ["cstone"];
		g_Vars.esp.csulfur = cfg ["esp"] ["worldenvironment"] ["csulfur"];
		g_Vars.esp.cmetal = cfg ["esp"] ["worldenvironment"] ["cmetal"];
		g_Vars.esp.cwood = cfg ["esp"] ["worldenvironment"] ["cmetal"];

		g_Vars.esp.radtown = cfg ["esp"] ["worldenvironment"] ["radtown"];
		g_Vars.esp.crate = cfg ["esp"] ["worldenvironment"] ["crate"];
		g_Vars.esp.ecrate = cfg ["esp"] ["worldenvironment"] ["ecrate"];
		g_Vars.esp.mcrate = cfg ["esp"] ["worldenvironment"] ["mcrate"];
		g_Vars.esp.barrel = cfg ["esp"] ["worldenvironment"] ["barrel"];
		g_Vars.esp.toolbox = cfg ["esp"] ["worldenvironment"] ["toolbox"];
		g_Vars.esp.oil = cfg ["esp"] ["worldenvironment"] ["oilbarrel"];
		g_Vars.esp.minecart = cfg ["esp"] ["worldenvironment"] ["minecart"];

		g_Vars.misc.alwaysday = cfg ["esp"] ["effects"] ["always_day"];
		g_Vars.misc.alwaysnight = cfg ["esp"] ["effects"] ["alwaysnight"];
		g_Vars.misc.fullbright = cfg ["esp"] ["effects"] ["fullbright"];
		g_Vars.misc.zoom_on_scope = cfg ["esp"] ["effects"] ["zoom_on_scope"];
		g_Vars.misc.fov = cfg ["esp"] ["effects"] ["mfov"];

		g_Vars.misc.admin = cfg ["misc"] ["general"] ["admin_flag"];
		g_Vars.misc.fastloot = cfg ["misc"] ["general"] ["fastloot"];
		g_Vars.misc.spider = cfg ["misc"] ["general"] ["spiderman"];
		g_Vars.misc.bclimb = cfg ["misc"] ["general"] ["climb_assist"];
		g_Vars.misc.infinite_jump = cfg ["misc"] ["general"] ["infinite_jump"];
		g_Vars.misc.bfly = cfg ["misc"] ["general"] ["flyhack"];
		g_Vars.misc.fly = cfg ["misc"] ["general"] ["flyhack_key"];
		g_Vars.misc.noclip_speed = cfg ["misc"] ["general"] ["flyhack_speed"];
		g_Vars.misc.flyhack_indicator = cfg ["misc"] ["general"] ["flyhack_indicator"];
		g_Vars.misc.bview_offset = cfg ["misc"] ["general"] ["bview_offset"];
		g_Vars.misc.view_offset = cfg ["misc"] ["general"] ["view_offset"];
		g_Vars.misc.kill_localplayer = cfg ["misc"] ["general"] ["kill_localplayer"];
		g_Vars.misc.kill_localplayer_key = cfg ["misc"] ["general"] ["kill_localplayer_key"];
		g_Vars.misc.hitsound = cfg ["misc"] ["general"] ["hitsound"];
		g_Vars.misc.hitsound_custom = cfg ["misc"] ["general"] ["hitsound_custom"];
		g_Vars.misc.hitsound_volume = cfg ["misc"] ["general"] ["hitsound_volume"];
		g_Vars.misc.omnisprint = cfg ["misc"] ["general"] ["omnisprint"];
		g_Vars.misc.y = cfg ["misc"] ["general"] ["longneck_y"];
		g_Vars.misc.shoot_from_minicopter = cfg ["misc"] ["general"] ["shoot_while_mounted"];

		g_Vars.esp.box_color.from_json ( cfg ["esp"] ["general"] ["box_color"] );
		g_Vars.esp.name_color.from_json ( cfg ["esp"] ["general"] ["name_color"] );
		g_Vars.esp.skeleton_color.from_json ( cfg ["esp"] ["general"] ["skeleton_color"] );
		g_Vars.esp.health_color.from_json ( cfg ["esp"] ["general"] ["health_color"] );
		g_Vars.esp.distance_color.from_json ( cfg ["esp"] ["general"] ["distance_color"] );
		g_Vars.esp.weapon_color.from_json ( cfg ["esp"] ["general"] ["weapon_color"] );
		g_Vars.esp.snaplines_color.from_json ( cfg ["esp"] ["general"] ["snaplines_color"] );
		g_Vars.esp.highlight_target_color.from_json ( cfg ["esp"] ["general"] ["highlight_target_color"] );
		g_Vars.esp.item_corpse_color.from_json ( cfg ["esp"] ["general"] ["item_corpse_color"] );
		g_Vars.esp.item_weapon_color.from_json ( cfg ["esp"] ["general"] ["item_weapon_color"] );
		g_Vars.esp.item_other_color.from_json ( cfg ["esp"] ["general"] ["item_other_color"] );
		g_Vars.esp.animals_color.from_json ( cfg ["esp"] ["general"] ["animals_color"] );
		g_Vars.esp.sleeper_color.from_json ( cfg ["esp"] ["general"] ["sleeper_color"] );

		g_Vars.esp.raid_color.from_json ( cfg ["esp"] ["otheresp"] ["raid_color"] );
		g_Vars.esp.toolcupboard_color.from_json ( cfg ["esp"] ["otheresp"] ["toolcupboard_color"] );

		g_Vars.esp.stash_color.from_json ( cfg ["esp"] ["worldenvironment"] ["stash_color"] );
		g_Vars.esp.stone_color.from_json ( cfg ["esp"] ["worldenvironment"] ["stone_color"] );
		g_Vars.esp.sulfur_color.from_json ( cfg ["esp"] ["worldenvironment"] ["sulfur_color"] );
		g_Vars.esp.metal_color.from_json ( cfg ["esp"] ["worldenvironment"] ["metal_color"] );

		g_Vars.esp.vehicles_horse_color.from_json ( cfg ["esp"] ["worldenvironment"] ["vehicles_horse_color"] );
		g_Vars.esp.vehicles_boats_color.from_json ( cfg ["esp"] ["worldenvironment"] ["vehicles_boats_color"] );
		g_Vars.esp.vehicles_helicopters_color.from_json ( cfg ["esp"] ["worldenvironment"] ["vehicles_helicopters_color"] );
		g_Vars.esp.special_color.from_json ( cfg ["esp"] ["worldenvironment"] ["special_color"] );

		g_Vars.esp.autoturret_color.from_json ( cfg ["esp"] ["worldenvironment"] ["autoturret_color"] );
		g_Vars.esp.flameturret_color.from_json ( cfg ["esp"] ["worldenvironment"] ["flameturret_color"] );
		g_Vars.esp.shotguntrap_color.from_json ( cfg ["esp"] ["worldenvironment"] ["shotguntrap_color"] );
		g_Vars.esp.landmine_color.from_json ( cfg ["esp"] ["worldenvironment"] ["landmine_color"] );
		g_Vars.esp.beartrap_color.from_json ( cfg ["esp"] ["worldenvironment"] ["beartrap_color"] );
		g_Vars.esp.samsite_color.from_json ( cfg ["esp"] ["worldenvironment"] ["samsite_color"] );

		g_Vars.esp.hemp_color.from_json ( cfg ["esp"] ["worldenvironment"] ["hemp_color"] );
		g_Vars.esp.mushroom_color.from_json ( cfg ["esp"] ["worldenvironment"] ["mushroom_color"] );
		g_Vars.esp.corn_color.from_json ( cfg ["esp"] ["worldenvironment"] ["corn_color"] );
		g_Vars.esp.berry_color.from_json ( cfg ["esp"] ["worldenvironment"] ["berry_color"] );
		g_Vars.esp.potato_color.from_json ( cfg ["esp"] ["worldenvironment"] ["potato_color"] );
		g_Vars.esp.cstone_color.from_json ( cfg ["esp"] ["worldenvironment"] ["cstone_color"] );
		g_Vars.esp.csulfur_color.from_json ( cfg ["esp"] ["worldenvironment"] ["csulfur_color"] );
		g_Vars.esp.cmetal_color.from_json ( cfg ["esp"] ["worldenvironment"] ["cmetal_color"] );
		g_Vars.esp.cwood_color.from_json ( cfg ["esp"] ["worldenvironment"] ["cwood_color"] );

		g_Vars.esp.crate_color.from_json ( cfg ["esp"] ["worldenvironment"] ["crate_color"] );
		g_Vars.esp.ecrate_color.from_json ( cfg ["esp"] ["worldenvironment"] ["ecrate_color"] );
		g_Vars.esp.mcrate_color.from_json ( cfg ["esp"] ["worldenvironment"] ["mcrate_color"] );
		g_Vars.esp.barrel_color.from_json ( cfg ["esp"] ["worldenvironment"] ["barrel_color"] );
		g_Vars.esp.toolbox_color.from_json ( cfg ["esp"] ["worldenvironment"] ["toolbox_color"] );
		g_Vars.esp.oil_color.from_json ( cfg ["esp"] ["worldenvironment"] ["oil_color"] );
		g_Vars.esp.minecart_color.from_json ( cfg ["esp"] ["worldenvironment"] ["minecart_color"] );
		g_Vars.esp.offscreen_color.from_json ( cfg ["esp"] ["worldenvironment"] ["offscreen_color"] );

		g_Vars.esp.offscren_enabled = cfg ["esp"] ["offscreen"] ["offscreen"];
		g_Vars.esp.offscren_distance = cfg ["esp"] ["offscreen"] ["offscreen_dist"];
		g_Vars.esp.offscren_size = cfg ["esp"] ["offscreen"] ["offscreen_size"];
	}
	catch ( ... )
	{

	}
}

void ConfigManager::SaveConfig ( std::string configname )
{
	std::ofstream o ( GetDocumentsDirectory ( ).append ( _ ( "\\phantom.tech\\cfg\\" ) + configname + _ ( ".json" ) ).c_str ( ) );
	if ( !o.is_open ( ) )
		return;

	json cfg;
	cfg ["aimbot"] = {
		{ "enabled", g_Vars.aimbot.enabled },
		{ "aimbot_key", g_Vars.aimbot.aim_key },
		{ "hitbox", g_Vars.aimbot.hitbox },
		{ "fov", g_Vars.aimbot.aim_fov },
		{ "smoothing", g_Vars.aimbot.smoothing },
		{ "terrain_check", g_Vars.aimbot.terraincheck }
	};

	cfg ["aimbot"] ["accuracy"] = {
		{ "big_bullet", g_Vars.misc.thickness },
		{ "big_bullet_scale", g_Vars.misc.flthickness },
		{ "remove_scope_sway", g_Vars.misc.nosway },
		{ "remove_spread", g_Vars.misc.nospread },
		{ "remove_shotgun_spread", g_Vars.misc.noshotgunspread },
		{ "shoot_midair", g_Vars.misc.jumpshoot },
		{ "recoil_percentage_x", g_Vars.misc.recoil_pitch },
		{ "recoil_percentage_y", g_Vars.misc.recoil_yaw }
	};

	cfg ["aimbot"] ["special"] = {
		{ "silent_aim", g_Vars.aimbot.silent },
		{ "team_check", g_Vars.aimbot.ignore_friends }
	};

	cfg ["aimbot"] ["other"] = {
		{ "rapid_fire", g_Vars.misc.rapidfire },
		{ "rapid_fire_speed", g_Vars.misc.rapidfire_speed },
		{ "insta_eoka", g_Vars.misc.eoka },
		{ "insta_compound_bow", g_Vars.misc.compound },
		{ "fast_bow", g_Vars.misc.fastbow },
		{ "force_automatic", g_Vars.misc.force_automatic },
		{ "force_semi_automatic", g_Vars.misc.force_semiautomatic }
	};

	cfg ["esp"] ["general"] = {
		{ "enable", g_Vars.esp.enabled },
		{ "chams", g_Vars.esp.chams },
		{ "box", g_Vars.esp.box },
		{ "name", g_Vars.esp.name },
		{ "health", g_Vars.esp.health },
		{ "health_override", g_Vars.esp.health_override },
		{ "skeleton", g_Vars.esp.skeleton },
		{ "distance", g_Vars.esp.distance },
		{ "weapon", g_Vars.esp.weapon },
		{ "snaplines", g_Vars.esp.snaplines },
		{ "npc", g_Vars.esp.npc },
		{ "tunneldweller", g_Vars.esp.tunneldweller },
		{ "sleeper", g_Vars.esp.sleeper },
		{ "teammate", g_Vars.esp.teammate },
		{ "highlight_target", g_Vars.esp.highlight_target },
		{ "item_corpse", g_Vars.esp.item_corpse },
		{ "item_weapon", g_Vars.esp.item_weapon },
		{ "item_other", g_Vars.esp.item_other },
		{ "animal", g_Vars.esp.animal }
	};

	cfg ["esp"] ["otheresp"] = {
		{ "hotbar_esp", g_Vars.esp.item_info },
		{ "raid", g_Vars.esp.raid },
		{ "hide", g_Vars.esp.hide },
		{ "toolcupboard", g_Vars.esp.toolcupboard },
		{ "distance_limit", g_Vars.esp.distance_limit }
	};

	cfg ["esp"] ["worldenvironment"] = {
		{ "stash", g_Vars.esp.stash },

		{ "ores", g_Vars.esp.ores },
		{ "stone", g_Vars.esp.stone },
		{ "sulfur", g_Vars.esp.sulfur },
		{ "metal", g_Vars.esp.metal },

		{ "vehicles", g_Vars.esp.vehicles },
		{ "vehicles_boats", g_Vars.esp.vehicles_boats },
		{ "vehicles_horse", g_Vars.esp.vehicles_horse },
		{ "vehicles_helicopters", g_Vars.esp.vehicles_helicopters },
		{ "vehicles_special", g_Vars.esp.special },

		{ "traps", g_Vars.esp.traps },
		{ "autoturret", g_Vars.esp.autoturret },
		{ "flameturret", g_Vars.esp.flameturret },
		{ "shotguntrap", g_Vars.esp.shotguntrap },
		{ "landmine", g_Vars.esp.landmine },
		{ "beartrap", g_Vars.esp.beartrap },
		{ "samsite", g_Vars.esp.samsite },

		{ "collectables", g_Vars.esp.collectable },
		{ "hemp", g_Vars.esp.hemp },
		{ "mushroom", g_Vars.esp.mushroom },
		{ "corn", g_Vars.esp.corn },
		{ "berry", g_Vars.esp.berry },
		{ "potato", g_Vars.esp.potato },
		{ "cstone", g_Vars.esp.cstone },
		{ "csulfur", g_Vars.esp.csulfur },
		{ "cmetal", g_Vars.esp.cmetal },
		{ "cwood", g_Vars.esp.cwood },

		{ "radtown", g_Vars.esp.radtown },
		{ "crate", g_Vars.esp.crate },
		{ "ecrate", g_Vars.esp.ecrate },
		{ "mcrate", g_Vars.esp.mcrate },
		{ "barrel", g_Vars.esp.barrel },
		{ "toolbox", g_Vars.esp.toolbox },
		{ "oilbarrel", g_Vars.esp.oil },
		{ "minecart", g_Vars.esp.minecart }
	};

	cfg ["esp"] ["effects"] = {
		{ "always_day", g_Vars.misc.alwaysday },
		{ "alwaysnight", g_Vars.misc.alwaysnight },
		{ "fullbright", g_Vars.misc.fullbright },
		{ "zoom_on_scope", g_Vars.misc.zoom_on_scope },
		{ "mfov", g_Vars.misc.fov }
	};

	cfg ["misc"] ["general"] = {
		{ "admin_flag", g_Vars.misc.admin },
		{ "fastloot", g_Vars.misc.fastloot },
		{ "spiderman", g_Vars.misc.spider },
		{ "climb_assist", g_Vars.misc.bclimb },
		{ "infinite_jump", g_Vars.misc.infinite_jump },
		{ "flyhack", g_Vars.misc.bfly },
		{ "flyhack_key", g_Vars.misc.fly },
		{ "flyhack_speed", g_Vars.misc.noclip_speed },
		{ "flyhack_indicator", g_Vars.misc.flyhack_indicator },
		{ "bview_offset", g_Vars.misc.bview_offset },
		{ "view_offset", g_Vars.misc.view_offset },
		{ "kill_localplayer", g_Vars.misc.kill_localplayer },
		{ "kill_localplayer_key", g_Vars.misc.kill_localplayer_key },
		{ "hitsound", g_Vars.misc.hitsound },
		{ "hitsound_custom", g_Vars.misc.hitsound_custom },
		{ "hitsound_volume", g_Vars.misc.hitsound_volume },
		{ "omnisprint", g_Vars.misc.omnisprint },
		{ "longneck_y", g_Vars.misc.y },
		{ "shoot_while_mounted", g_Vars.misc.shoot_from_minicopter }
	};

	g_Vars.esp.box_color.to_json ( cfg ["esp"] ["general"] ["box_color"] );
	g_Vars.esp.name_color.to_json ( cfg ["esp"] ["general"] ["name_color"] );
	g_Vars.esp.skeleton_color.to_json ( cfg ["esp"] ["general"] ["skeleton_color"] );
	g_Vars.esp.health_color.to_json ( cfg ["esp"] ["general"] ["health_color"] );
	g_Vars.esp.distance_color.to_json ( cfg ["esp"] ["general"] ["distance_color"] );
	g_Vars.esp.weapon_color.to_json ( cfg ["esp"] ["general"] ["weapon_color"] );
	g_Vars.esp.snaplines_color.to_json ( cfg ["esp"] ["general"] ["snaplines_color"] );
	g_Vars.esp.highlight_target_color.to_json ( cfg ["esp"] ["general"] ["highlight_target_color"] );
	g_Vars.esp.item_corpse_color.to_json ( cfg ["esp"] ["general"] ["item_corpse_color"] );
	g_Vars.esp.item_weapon_color.to_json ( cfg ["esp"] ["general"] ["item_weapon_color"] );
	g_Vars.esp.item_other_color.to_json ( cfg ["esp"] ["general"] ["item_other_color"] );
	g_Vars.esp.animals_color.to_json ( cfg ["esp"] ["general"] ["animals_color"] );

	g_Vars.esp.raid_color.to_json ( cfg ["esp"] ["otheresp"] ["raid_color"] );
	g_Vars.esp.toolcupboard_color.to_json ( cfg ["esp"] ["otheresp"] ["toolcupboard_color"] );

	g_Vars.esp.stash_color.to_json ( cfg ["esp"] ["worldenvironment"] ["stash_color"] );
	g_Vars.esp.stone_color.to_json ( cfg ["esp"] ["worldenvironment"] ["stone_color"] );
	g_Vars.esp.sulfur_color.to_json ( cfg ["esp"] ["worldenvironment"] ["sulfur_color"] );
	g_Vars.esp.metal_color.to_json ( cfg ["esp"] ["worldenvironment"] ["metal_color"] );

	g_Vars.esp.vehicles_horse_color.to_json ( cfg ["esp"] ["worldenvironment"] ["vehicles_horse_color"] );
	g_Vars.esp.vehicles_boats_color.to_json ( cfg ["esp"] ["worldenvironment"] ["vehicles_boats_color"] );
	g_Vars.esp.vehicles_helicopters_color.to_json ( cfg ["esp"] ["worldenvironment"] ["vehicles_helicopters_color"] );
	g_Vars.esp.special_color.to_json ( cfg ["esp"] ["worldenvironment"] ["special_color"] );
	g_Vars.esp.sleeper_color.to_json ( cfg ["esp"] ["general"] ["sleeper_color"] );

	g_Vars.esp.autoturret_color.to_json ( cfg ["esp"] ["worldenvironment"] ["autoturret_color"] );
	g_Vars.esp.flameturret_color.to_json ( cfg ["esp"] ["worldenvironment"] ["flameturret_color"] );
	g_Vars.esp.shotguntrap_color.to_json ( cfg ["esp"] ["worldenvironment"] ["shotguntrap_color"] );
	g_Vars.esp.landmine_color.to_json ( cfg ["esp"] ["worldenvironment"] ["landmine_color"] );
	g_Vars.esp.beartrap_color.to_json ( cfg ["esp"] ["worldenvironment"] ["beartrap_color"] );
	g_Vars.esp.samsite_color.to_json ( cfg ["esp"] ["worldenvironment"] ["samsite_color"] );

	g_Vars.esp.hemp_color.to_json ( cfg ["esp"] ["worldenvironment"] ["hemp_color"] );
	g_Vars.esp.mushroom_color.to_json ( cfg ["esp"] ["worldenvironment"] ["mushroom_color"] );
	g_Vars.esp.corn_color.to_json ( cfg ["esp"] ["worldenvironment"] ["corn_color"] );
	g_Vars.esp.berry_color.to_json ( cfg ["esp"] ["worldenvironment"] ["berry_color"] );
	g_Vars.esp.potato_color.to_json ( cfg ["esp"] ["worldenvironment"] ["potato_color"] );
	g_Vars.esp.cstone_color.to_json ( cfg ["esp"] ["worldenvironment"] ["cstone_color"] );
	g_Vars.esp.csulfur_color.to_json ( cfg ["esp"] ["worldenvironment"] ["csulfur_color"] );
	g_Vars.esp.cmetal_color.to_json ( cfg ["esp"] ["worldenvironment"] ["cmetal_color"] );
	g_Vars.esp.cwood_color.to_json ( cfg ["esp"] ["worldenvironment"] ["cwood_color"] );

	g_Vars.esp.crate_color.to_json ( cfg ["esp"] ["worldenvironment"] ["crate_color"] );
	g_Vars.esp.ecrate_color.to_json ( cfg ["esp"] ["worldenvironment"] ["ecrate_color"] );
	g_Vars.esp.mcrate_color.to_json ( cfg ["esp"] ["worldenvironment"] ["mcrate_color"] );
	g_Vars.esp.barrel_color.to_json ( cfg ["esp"] ["worldenvironment"] ["barrel_color"] );
	g_Vars.esp.toolbox_color.to_json ( cfg ["esp"] ["worldenvironment"] ["toolbox_color"] );
	g_Vars.esp.oil_color.to_json ( cfg ["esp"] ["worldenvironment"] ["oil_color"] );
	g_Vars.esp.minecart_color.to_json ( cfg ["esp"] ["worldenvironment"] ["minecart_color"] );
	g_Vars.esp.offscreen_color.to_json ( cfg ["esp"] ["worldenvironment"] ["offscreen_color"] );

	cfg ["esp"] ["offscreen"] = {
		{ "offscreen", g_Vars.esp.offscren_enabled },
		{ "offscreen_dist", g_Vars.esp.offscren_distance },
		{ "offscreen_size", g_Vars.esp.offscren_size }
	};

	o << cfg;
	o.close ( );
}

void ConfigManager::RemoveConfig ( std::string configname )
{
	std::remove ( GetDocumentsDirectory ( ).append ( _ ( "\\phantom.tech\\cfg\\" ) + configname + _ ( ".json" ) ).c_str ( ) );
}

void ConfigManager::CreateConfig ( std::string configname )
{
	std::ofstream o ( GetDocumentsDirectory ( ).append ( _ ( "\\phantom.tech\\cfg\\" ) + configname + _ ( ".json" ) ).c_str ( ) );
}

void ConfigManager::OpenConfigFolder ( )
{
	namespace fs = std::experimental::filesystem;
	fs::path full_path ( GetDocumentsDirectory ( ) );

	std::wstring str = full_path.wstring ( ) + _ ( L"\\phantom.tech\\cfg" );

	PIDLIST_ABSOLUTE pidl;
	if ( SUCCEEDED ( SHParseDisplayName ( str.c_str ( ), 0, &pidl, 0, 0 ) ) )
	{
		// we don't want to actually select anything in the folder, so we pass an empty
		// PIDL in the array. if you want to select one or more items in the opened
		// folder you'd need to build the PIDL array appropriately
		ITEMIDLIST idNull = { 0 };
		LPCITEMIDLIST pidlNull [1] = { &idNull };
		SHOpenFolderAndSelectItems ( pidl, 1, pidlNull, 0 );

		// LIFEEEEHAAAACK BITCH!!! (◣_◢)
		using ILFree_t = void ( __stdcall* )( LPITEMIDLIST );
		static ILFree_t ILFree_fn = ( ILFree_t ) GetProcAddress ( GetModuleHandleA ( _ ( "SHELL32" ) ), _ ( "ILFree" ) );
		ILFree_fn ( pidl );
	}
}