#pragma once
class Item;
class Vector3;
class Vector2;
class Vector4;

struct player
{
	char pad [0x20C]; // 0x0
	float health; // 0x20C
	char pad2 [0x3E8]; // 0x210
	int flags; // 0x5F8
	//char pad3[0x0]; // 0x5FC
};

enum PlayerFlags : int
{
	admin = 4,
	snapshot = 8,
	sleeping = 16,
	spectating = 32,
	wounded = 64,
	developer = 128,
	connected = 256,
	thirdperson = 1024,
	eyesviewmode = 2048,
	muted = 4096,
	nosprint = 8192,
	aiming = 16384,
	sash = 32768,
	relaxed = 65536,
	safezone = 131072,
	serverfall = 262144,
	workbench1 = 1048576,
	workbench2 = 2097152,
	workbench3 = 4194304
};

struct PlayerItems
{
	std::string m_szName;
	int m_iAmount;
	bool m_bActive;
};

class ModelState
{
public:
	enum Flag
	{
		Ducked = 1,
		Jumped = 2,
		OnGround = 4,
		Sleeping = 8,
		Sprinting = 16,
		OnLadder = 32,
		Flying = 64,
		Aiming = 128,
		Prone = 256,
		Mounted = 512,
		Relaxed = 1024
	};

	void SetFlag ( CSharedMemComms* drv, Flag f, bool b );

	bool HasFlag ( CSharedMemComms* drv, Flag f );

	bool get_swimming ( CSharedMemComms* drv );

	void set_sprinting ( CSharedMemComms* drv, bool value );

	bool get_ducked ( CSharedMemComms* drv );

	bool get_sprinting ( CSharedMemComms* drv );
};

class PlayerEyes
{
private:
public:
	Vector4 get_body_rotation ( CSharedMemComms* drv );
	void set_body_rotation ( CSharedMemComms* drv, Vector4 rotation );
};

class BasePlayer
{
public:
	std::string get_name ( CSharedMemComms* drv );

	float get_speed ( CSharedMemComms* drv, ModelState* model_state, float running, float ducking );
	void data ( CSharedMemComms* drv, player& dataref );
	float get_health ( CSharedMemComms* drv );
	unsigned long get_uid ( CSharedMemComms* drv );

	Item* get_held_item ( CSharedMemComms* drv );

	Vector3 get_bone_position ( CSharedMemComms* drv, int bone, uintptr_t transform = 0 );
	Vector2 get_recoil_angles ( CSharedMemComms* drv );
	Vector2 get_body_angles ( CSharedMemComms* drv );

	Vector2 get_head_angles ( CSharedMemComms* drv );

	std::vector<PlayerItems> get_item_list ( CSharedMemComms* drv );

	bool set_recoil_angles ( CSharedMemComms* drv, Vector2 angles );
	bool set_body_angles ( CSharedMemComms* drv, Vector2 angles );
	bool set_flag ( CSharedMemComms* drv, int flag );

	bool is_wounded ( CSharedMemComms* drv );
	bool is_sleeping ( CSharedMemComms* drv );

	bool is_teammate ( CSharedMemComms* drv );

	void omni_sprint ( CSharedMemComms* drv );

	Vector3 get_velocity ( CSharedMemComms* drv );
	PlayerFlags get_flags ( CSharedMemComms* drv );
	PlayerEyes* get_player_eyes ( CSharedMemComms* drv );
};