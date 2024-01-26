#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <TlHelp32.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <type_traits>
#include <WinUser.h>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <shlobj.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")
#define D3_DFVF_CUSTOMVERTEX        ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

#include "rendering.hpp"
#include "communication.hpp"
#include "xorstr.hpp"

#include "Menu/Helpers/InputSys.hpp"
#include "Menu/Helpers/InputHelper.h"
#include "Renderer/Render.hpp"

#include "cheat.hpp"
#include "lazy_importer.hpp"
#include "object_loop.hpp"
#include "sdk.hpp"
#include "math.hpp"
#include "baseplayer.hpp"
#include "misc.hpp"
#include "item.hpp"
#include "xorfloat.hpp"
#include "aimbot.hpp"

#include "color.hpp"

#include "json.hpp"
using json = nlohmann::json;

using namespace std::chrono;
using namespace std::this_thread;

class CVariables
{
public:
	struct _GLOBAL
	{
		int ConfigSelected;
		bool menuOpen = false;
		int scrollbar = 0;
		std::vector<std::string> m_hitsounds;
		float m_fDpiScale = 1.5f;
	}global;

	struct _AIM
	{
		bool enabled;
		int hitbox;
		float aim_fov;
		int aim_key;
		bool silent;
		bool terraincheck;
		bool ignore_friends;
		float smoothing;
	}aimbot;

	struct _ESP
	{
		bool enabled;
		bool chams;
		bool players = true;
		bool sleeper;
		FloatColor sleeper_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool npc;
		bool tunneldweller;
		bool special;
		FloatColor special_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool animal;
		bool fov_circle;
		bool raid;
		FloatColor raid_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );

		bool name;
		FloatColor name_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool box;
		FloatColor box_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool skeleton;
		FloatColor skeleton_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool health;
		bool health_override;
		FloatColor health_color = FloatColor ( 0.0f, 1.0f, 0.0f, 1.0f );

		bool distance;
		FloatColor distance_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool ammo_bar;
		FloatColor ammo_bar_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool snaplines;
		FloatColor snaplines_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool teammate;
		bool highlight_target;
		FloatColor highlight_target_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool weapon;
		FloatColor weapon_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool item_info;
		bool item = true;
		bool item_weapon;
		FloatColor item_weapon_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool item_other;
		FloatColor item_other_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool item_corpse;
		FloatColor item_corpse_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );

		bool offscren_enabled = true;
		float offscren_size = 16;
		float offscren_distance = 50;
		FloatColor offscreen_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		FloatColor offscreen_outline_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );

		bool ores;
		bool sulfur, stone, metal;
		FloatColor sulfur_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		FloatColor stone_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		FloatColor metal_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool stash;
		FloatColor stash_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool radtown;
		bool collectable;
		bool animals;
		FloatColor animals_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool hide;

		bool vehicles;
		bool vehicles_horse;
		FloatColor vehicles_horse_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool vehicles_boats;
		FloatColor vehicles_boats_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool vehicles_helicopters;
		FloatColor vehicles_helicopters_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );

		bool traps;
		bool shotguntrap;
		FloatColor shotguntrap_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool autoturret;
		FloatColor autoturret_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool flameturret;
		FloatColor flameturret_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool landmine;
		FloatColor landmine_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool beartrap;
		FloatColor beartrap_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool samsite;
		FloatColor samsite_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );

		bool crate;
		FloatColor crate_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool ecrate;
		FloatColor ecrate_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool mcrate;
		FloatColor mcrate_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool barrel;
		FloatColor barrel_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool toolbox;
		FloatColor toolbox_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool oil;
		FloatColor oil_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool minecart;
		FloatColor minecart_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );

		bool toolcupboard;
		FloatColor toolcupboard_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );

		bool toolcupboard_list;
		bool sleepingbag;

		bool hemp;
		FloatColor hemp_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool mushroom;
		FloatColor mushroom_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool corn;
		FloatColor corn_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool berry;
		FloatColor berry_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool potato;
		FloatColor potato_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool csulfur;
		FloatColor csulfur_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool cstone;
		FloatColor cstone_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool cmetal;
		FloatColor cmetal_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );
		bool crosshair;
		bool cwood;
		FloatColor cwood_color = FloatColor ( 1.0f, 1.0f, 1.0f, 1.0f );

		float distance_limit = 4000.f;
	} esp;

	struct _MISC
	{
		float recoil_pitch = 100.f;
		float recoil_yaw = 100.f;
		bool force_automatic;
		bool force_semiautomatic;
		bool nosway;
		bool nospread;
		bool noshotgunspread;
		bool spider;
		bool rapidfire;
		float rapidfire_speed;
		bool infinite_jump;

		bool shoot_from_minicopter;

		int kill_localplayer_key;
		bool kill_localplayer;

		bool remove_clothing_restriction;
		bool long_melee;
		bool eoka;
		bool compound;
		bool fullbright;
		bool zoom_on_scope;
		bool alwaysday;
		bool admin;
		bool fastloot;
		bool noslide;
		bool turboclip;
		float noclip_speed = 10.f;
		bool bgravity;
		int gravity;
		bool btimescale;
		int timescale;
		float timescale_speed = 3.f;
		bool bfly;
		int fly;
		bool radar = true;
		int scale = 100;
		bool flyhack_indicator;
		bool bview_offset;
		int view_offset;
		bool bclimb;
		int assist;
		bool fakelag;
		bool fastbow;
		bool jumpshoot;
		bool omnisprint;
		bool alwayswhite;
		bool alwaysnight;
		bool hitsound;
		int hitsound_custom;
		float hitsound_volume;
		bool thickness;
		float flthickness;
		float fov = 90.f;
		float y = .75f;
	} misc;

	struct _MENU
	{
		FloatColor menu_color = FloatColor ( 0.647058824f, 0.57254902f, 0.952941176f, 1.0f );
	} menu;
};

extern CVariables g_Vars;

namespace offsets
{
	constexpr uint64_t gom_offset = 0x17C1F18;
	constexpr uint64_t bn_offset = 0x3115CB0; // BaseNetworkable_c*
	constexpr uint64_t fov_offset = 0x31138F0; // ConVar_Graphics_c*
	constexpr uint64_t global_offset = 0x3116358; // ConVar_Global_c*
	constexpr uint64_t admin_offset = 0x310F610; // ConVar_Admin_c*
	constexpr uint64_t ui_inventory = 0x3113DE8; // UIInventory_c*
	constexpr uint64_t console_system_typeinfo = 0x310DCA8; // ConsoleSystem_Command_c*
	constexpr uint64_t cl_ent = 0xB8;
	constexpr uint64_t ent_realm = 0x10;
	constexpr uint64_t buf_list = 0x28;
	constexpr uint64_t obj_list = 0x18;
	constexpr uint64_t obj_size = 0x10;
	constexpr uint64_t base_ply = 0x28;
	constexpr uint64_t class_name = 0x10;
	constexpr uint64_t player_visual = 0x08;
	constexpr uint64_t obj_state = 0x38;
	constexpr uint64_t obj_pos = 0x90;
	constexpr uint64_t model = 0x128; // public Model model
	constexpr uint64_t player_model = 0x4C0; // public PlayerModel playerModel;
	constexpr uint64_t player_skin_type = 0x1BC; // public int skinType;
	constexpr uint64_t multi_mesh = 0x2C0; // private SkinnedMultiMesh _multiMesh;
	constexpr uint64_t element = 0x20;

	constexpr uint64_t clothingBlocksAiming = 0x750; // public bool clothingBlocksAiming;
	constexpr uint64_t clothingMoveSpeedReduction = 0x754; // public float clothingMoveSpeedReduction;
	constexpr uint64_t clothingWaterSpeedBonus = 0x758; // public float clothingWaterSpeedBonus;
	constexpr uint64_t modelState = 0x5F8; // public ModelState modelState;
	constexpr uint64_t basemovement = 0x4E8; // public BaseMovement movement;
	constexpr uint64_t groundangle = 0xC4; // private float groundAngle;
	constexpr uint64_t groundangle_new = 0xC8; // private float groundAngleNew;
	constexpr uint64_t gravity = 0x74;
	constexpr uint64_t flying = 0x14C; // private bool flying;
	constexpr uint64_t swimming = 0x146; // private bool swimming;
	constexpr uint64_t climbing = 0x142; // private bool climbing;
	constexpr uint64_t groundtime = 0xCC; // private float groundTime;
	constexpr uint64_t maxangle_climb = 0x90; // public float maxAngleClimbing;
	constexpr uint64_t maxangle_walk = 0x8C; // public float maxAngleWalking;

	constexpr uint64_t waterLevel = 0x14;
	constexpr uint64_t target_movement = 0x34;

	constexpr uint64_t flags = 0x24;

	constexpr uint64_t player_input = 0x4E0; // public PlayerInput input; // 0x4E0
	constexpr uint64_t body_angles = 0x3C; // private Vector3 bodyAngles;
	constexpr uint64_t head_angles = 0x58; // private Vector3 headAngles;
	constexpr uint64_t recoil_angles = 0x64; // public Vector3 recoilAngles;

	constexpr uint64_t eyes = 0x688; // public PlayerEyes eyes;

	constexpr uint64_t velocity = 0x214; // private Vector3 newVelocity;
	constexpr uint64_t oldvelocity = 0x22C; // internal Vector3 velocity;
	constexpr uint64_t magazine = 0x2B8; // public BaseProjectile.Magazine primaryMagazine;
	constexpr uint64_t ammotype = 0x20;
	constexpr uint64_t shortname = 0x20;
	constexpr uint64_t active_item = 0x5D0; // private uint clActiveItem;

	constexpr uint64_t base_projectile = 0x98; // public BaseProjectile baseProjectile;
	constexpr uint64_t attack_entity = 0x20; // public AttackEntity Weapon;

	constexpr uint64_t sway = 0x2B8;
	constexpr uint64_t aimcone = 0x2D0;
	constexpr uint64_t repeat_delay = 0x1F4; // public float repeatDelay;

	constexpr uint64_t recoil_properties = 0x2D8; // public RecoilProperties recoil; 

	constexpr uint64_t PlayerFlags = 0x680; // public BasePlayer.PlayerFlags playerFlags;

	constexpr uint64_t jumpTime = 0xD0; // private float jumpTime;
	constexpr uint64_t groundTime = 0xCC; // private float groundTime;
	constexpr uint64_t landTime = 0xD4; // private float landTime;

	constexpr uint64_t successfraction = 0x360; // successfraction
	constexpr uint64_t didSparkThisFrame = 0x370; // _didSparkThisFrame
	constexpr uint64_t isStriking = 0x371; // _isStriking

	constexpr uint64_t wasFalling = 0x14B; // wasFalling
	constexpr uint64_t previousVelocity = 0xE4; // previousVelocity

	constexpr uint64_t clienttickinterval = 0x650; // clienttickinterval

	constexpr uint64_t stringmaxduration = 0x378; // stringHoldDurationMax
	constexpr uint64_t attackReady = 0x360; // attackReady
	constexpr uint64_t wasAiming = 0x370; // wasAiming
	constexpr uint64_t aimSway = 0x2D0; // aimSway
	constexpr uint64_t aimSwaySpeed = 0x2D4; // aimSwaySpeed
	constexpr uint64_t hipAimCone = 0x2EC; // hipAimCone
	constexpr uint64_t aimConePenaltyMax = 0x2F4; // aimConePenaltyMax
	constexpr uint64_t aimconePenaltyPerShot = 0x2F0; // aimconePenaltyPerShot
	constexpr uint64_t automatic = 0x288; // automatic
	constexpr uint64_t maxDistance = 0x290; // maxDistance

	constexpr uint64_t primarymagazine = 0x2B8; // primarymagazine //0x2A0
	constexpr uint64_t projvelocity = 0x178; // 0x178
	constexpr uint64_t projvelocityscale = 0x284; // 0x26C

	constexpr uint64_t inventory = 0x690; // public PlayerInventory inventory;
	constexpr uint64_t display_name = 0x6E0; // protected string _displayName;
	constexpr uint64_t uid = 0x6C8; // public ulong userID;
	constexpr uint64_t health = 0x224; // protected float _health;
	constexpr uint64_t team = 0x598; // public ulong currentTeam;
	constexpr uint64_t timescale = 0x17C1CD0;
	constexpr uint64_t effect = 0x1D98DF4;

	//constexpr uint64_t gom_offset = 0x17C1F18;
	//constexpr uint64_t bn_offset = 0x29BF3E0; // BaseNetworkable_c*
	//constexpr uint64_t fov_offset = 0x29BE5D0; // ConVar_Graphics_c*
	//constexpr uint64_t global_offset = 0x29BFC88; // ConVar_Global_c*
	//constexpr uint64_t admin_offset = 0x29BB848; // ConVar_Admin_c*
	//constexpr uint64_t ui_inventory = 0x29BE9A0; // UIInventory_c*
	//constexpr uint64_t console_system_typeinfo = 0x29BA2B8; // ConsoleSystem_Command_c*
	//constexpr uint64_t cl_ent = 0xB8;
	//constexpr uint64_t ent_realm = 0x10;
	//constexpr uint64_t buf_list = 0x28;
	//constexpr uint64_t obj_list = 0x18;
	//constexpr uint64_t obj_size = 0x10;
	//constexpr uint64_t base_ply = 0x28;
	//constexpr uint64_t class_name = 0x10;
	//constexpr uint64_t player_visual = 0x08;
	//constexpr uint64_t obj_state = 0x38;
	//constexpr uint64_t obj_pos = 0x90;
	//constexpr uint64_t model = 0x118; // public Model model
	//constexpr uint64_t player_model = 0x4A8; // public PlayerModel playerModel;
	//constexpr uint64_t player_skin_type = 0x18C; // public int skinType;
	//constexpr uint64_t multi_mesh = 0x280; // private SkinnedMultiMesh _multiMesh;
	//constexpr uint64_t element = 0x20;

	//constexpr uint64_t clothingBlocksAiming = 0x6B0; // public bool clothingBlocksAiming;
	//constexpr uint64_t clothingMoveSpeedReduction = 0x6B4; // public float clothingMoveSpeedReduction;
	//constexpr uint64_t clothingWaterSpeedBonus = 0x6B8; // public float clothingWaterSpeedBonus;
	//constexpr uint64_t modelState = 0x588; // public ModelState modelState;
	//constexpr uint64_t basemovement = 0x4D0; // public BaseMovement movement;
	//constexpr uint64_t groundangle = 0xB4; // private float groundAngle;
	//constexpr uint64_t groundangle_new = 0xB8; // private float groundAngleNew;
	//constexpr uint64_t gravity = 0x74;
	//constexpr uint64_t flying = 0x13C; // private bool flying;
	//constexpr uint64_t swimming = 0x136; // private bool swimming;
	//constexpr uint64_t climbing = 0x132; // private bool climbing;
	//constexpr uint64_t groundtime = 0xBC; // private float groundTime;
	//constexpr uint64_t maxangle_climb = 0x80; // public float maxAngleClimbing;
	//constexpr uint64_t maxangle_walk = 0x7C; // public float maxAngleWalking;

	//constexpr uint64_t waterLevel = 0x14;
	//constexpr uint64_t target_movement = 0x34;

	//constexpr uint64_t flags = 0x24;

	//constexpr uint64_t player_input = 0x4C8; // public PlayerInput input; // 0x4E0
	//constexpr uint64_t body_angles = 0x3C; // private Vector3 bodyAngles;
	//constexpr uint64_t head_angles = 0x58; // private Vector3 headAngles;
	//constexpr uint64_t recoil_angles = 0x64; // public Vector3 recoilAngles;

	//constexpr uint64_t eyes = 0x600; // public PlayerEyes eyes;

	//constexpr uint64_t velocity = 0x1FC; // private Vector3 newVelocity;
	//constexpr uint64_t oldvelocity = 0x1E4; // internal Vector3 velocity;
	//constexpr uint64_t magazine = 0x2A0; // public BaseProjectile.Magazine primaryMagazine;
	//constexpr uint64_t ammotype = 0x20;
	//constexpr uint64_t shortname = 0x20;
	//constexpr uint64_t active_item = 0x570; // private uint clActiveItem;

	//constexpr uint64_t base_projectile = 0x90; // public BaseProjectile baseProjectile;
	//constexpr uint64_t attack_entity = 0x20; // public AttackEntity Weapon;

	//constexpr uint64_t sway = 0x2B8;
	//constexpr uint64_t aimcone = 0x2D0;
	//constexpr uint64_t repeat_delay = 0x1DC; // public float repeatDelay;

	//constexpr uint64_t recoil_properties = 0x2C0; // public RecoilProperties recoil; 

	//constexpr uint64_t PlayerFlags = 0x5F8; // public BasePlayer.PlayerFlags playerFlags;

	//constexpr uint64_t jumpTime = 0xC0; // private float jumpTime;
	//constexpr uint64_t groundTime = 0xBC; // private float groundTime;
	//constexpr uint64_t landTime = 0xC4; // private float landTime;

	//constexpr uint64_t successfraction = 0x340; // successfraction
	//constexpr uint64_t didSparkThisFrame = 0x350; // _didSparkThisFrame
	//constexpr uint64_t isStriking = 0x351; // _isStriking

	//constexpr uint64_t wasFalling = 0x13B; // wasFalling
	//constexpr uint64_t previousVelocity = 0xD4; // previousVelocity

	//constexpr uint64_t clienttickinterval = 0x5C8; // clienttickinterval

	//constexpr uint64_t stringmaxduration = 0x358; // stringHoldDurationMax
	//constexpr uint64_t attackReady = 0x340; // attackReady
	//constexpr uint64_t wasAiming = 0x350; // wasAiming
	//constexpr uint64_t aimSway = 0x2B8; // aimSway
	//constexpr uint64_t aimSwaySpeed = 0x2BC; // aimSwaySpeed
	//constexpr uint64_t hipAimCone = 0x2D4; // hipAimCone
	//constexpr uint64_t aimConePenaltyMax = 0x2DC; // aimConePenaltyMax
	//constexpr uint64_t aimconePenaltyPerShot = 0x2D8; // aimconePenaltyPerShot
	//constexpr uint64_t automatic = 0x270; // automatic
	//constexpr uint64_t maxDistance = 0x278; // maxDistance

	//constexpr uint64_t primarymagazine = 0x2A0; // primarymagazine //0x2A0
	//constexpr uint64_t projvelocity = 0x160; // 0x178
	//constexpr uint64_t projvelocityscale = 0x26C; // 0x26C

	//constexpr uint64_t inventory = 0x608; // public PlayerInventory inventory;
	//constexpr uint64_t display_name = 0x650; // protected string _displayName;
	//constexpr uint64_t uid = 0x640; // public ulong userID;
	//constexpr uint64_t health = 0x20C; // protected float _health;
	//constexpr uint64_t team = 0x538; // public ulong currentTeam;
	//constexpr uint64_t timescale = 0x17C1CD0;
	//constexpr uint64_t effect = 0x1D98DF4;
};

namespace addr
{
	extern uintptr_t unityplayer;
	extern uintptr_t gameassembly;

	extern uintptr_t base_networkable;
	extern uintptr_t client_entities;
	extern uintptr_t entity_realm;
	extern uintptr_t buffer_list;
	extern uintptr_t object_list;
	extern uintptr_t camera_instance;
	extern uintptr_t tod_sky;
	extern std::size_t object_size;
};

namespace monitor
{
	extern float width, height;
};

namespace d3d9
{
	extern int screen_width;
	extern int screen_height;

	extern HWND game_window;
	extern HWND overlay_hwnd;
	extern RECT window_rect;
	extern ID3DXLine* dx9_line;
	extern LPDIRECT3DDEVICE9 dx9_device;
	extern LPD3DXFONT verdana_13;
	extern LPD3DXFONT tahoma_11;
	extern LPD3DXFONT small_fonts_9;
	extern MARGINS margin;
}

enum : char
{
	TYPE_LOG,
	TYPE_DEBUG,
	TYPE_ERROR,
	TYPE_WARNING,
	TYPE_UNKNOWN
};

constexpr inline const char* GET_PREFIX ( const char type )
{
	switch ( type )
	{
	case TYPE_LOG:
		return "[-] ";
		break;
	case TYPE_DEBUG:
		return "[+] ";
		break;
	case TYPE_ERROR:
		return "[x] ";
		break;
	case TYPE_WARNING:
		return "[!] ";
		break;
	case TYPE_UNKNOWN:
		return "[?] ";
		break;
	default:
		return "";
		break;
	}
}

namespace logging
{
	inline void print ( const char type, const char* format, ... )
	{
		char buffer [256];
		va_list arguments;
		va_start ( arguments, format );
		vsnprintf_s ( buffer, 256, format, arguments );

		const char* PREFIX = GET_PREFIX ( type );

		HANDLE console = GetStdHandle ( STD_OUTPUT_HANDLE );

		int len = 0, x, y = 1;

		switch ( type )
		{
		case TYPE_LOG:
			y = 10;
			break;
		case TYPE_DEBUG:
			y = 8;
			break;
		case TYPE_ERROR:
			y = 12;
			break;
		case TYPE_WARNING:
			y = 14;
			break;
		case TYPE_UNKNOWN:
			y = 9;
			break;
		}

		SetConsoleTextAttribute ( console, y );

		std::string text = buffer;
		len = text.length ( );
		std::cout << std::endl << _ ( "\t\t" ) << PREFIX;

		for ( x = 0; x < len; x++ )
		{
			std::cout << text [x];

			Sleep ( 50 );
		}

		SetConsoleTextAttribute ( console, 15 ); // set color to black background, white chars
	}
};

#define VASSERT(x) \
if ( !x ) \
return;

#define CASSERT(x) \
if ( !x ) \
continue;

#define ASSERT(x) \
if ( !x ) \
return false;

#define BASSERT(x) \
if ( !x ) \
break;

class NonCopyable
{
protected:
	NonCopyable ( ) { }
	~NonCopyable ( ) { }

private:
	NonCopyable ( const NonCopyable& ) = delete;
	NonCopyable& operator=( const NonCopyable& ) = delete;

	NonCopyable ( NonCopyable&& ) = delete;
	NonCopyable& operator=( NonCopyable&& ) = delete;
};

struct KeyBind_t
{
	KeyBind_t ( ) { }

	int key = 0, cond = 0;
	bool enabled = false;

	//void to_json ( nlohmann::json& j )
	//{
	//	j = nlohmann::json {
	//		{ ( XorStr ( "key" ) ), key },
	//		{ ( XorStr ( "cond" ) ), cond },
	//	};
	//}

	//void from_json ( nlohmann::json& j )
	//{
	//	j.at ( XorStr ( "key" ) ).get_to ( key );
	//	j.at ( XorStr ( "cond" ) ).get_to ( cond );
	//}
};

extern std::vector<KeyBind_t*> g_keybinds;

__forceinline std::string GetDocumentsDirectory ( )
{
	char my_documents [MAX_PATH];
	HRESULT result = SHGetFolderPath ( NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents );

	return std::string ( my_documents );
}