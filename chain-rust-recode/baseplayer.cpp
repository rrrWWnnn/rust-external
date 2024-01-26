#include "stdafx.hpp"
#include "baseplayer.hpp"
#include "cheat.hpp"
#include "unity.hpp"

void ModelState::SetFlag ( CSharedMemComms* drv, Flag f, bool b )
{
	auto flags = drv->read<int> ( reinterpret_cast< uintptr_t >( this ) + offsets::flags );
	if ( b )
		flags |= ( int ) f;
	else
		flags &= ~( int ) f;

	drv->write<int> ( reinterpret_cast< uintptr_t >( this ) + offsets::flags, flags );
}

bool ModelState::HasFlag ( CSharedMemComms* drv, Flag f )
{
	auto flags = drv->read<int> ( reinterpret_cast< uintptr_t >( this ) + offsets::flags );

	return ( flags & ( int ) f ) == ( int ) f;
}

bool ModelState::get_swimming ( CSharedMemComms* drv )
{
	if ( this != NULL )
	{
		return drv->read<float> ( reinterpret_cast< uintptr_t >( this ) + offsets::waterLevel ) >= 0.65f;
	}
	return false;
}

void ModelState::set_sprinting ( CSharedMemComms* drv, bool value )
{
	SetFlag ( drv, Flag::Sprinting, value );
}

bool ModelState::get_ducked ( CSharedMemComms* drv )
{
	return HasFlag ( drv, Flag::Ducked );
}

bool ModelState::get_sprinting ( CSharedMemComms* drv )
{
	return HasFlag ( drv, Flag::Sprinting );
}

std::string BasePlayer::get_name ( CSharedMemComms* drv )
{
	std::string name;
	uintptr_t nameptr = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::display_name );
	if ( !nameptr )
		return _ ( "" );
	name = drv->read_unicode ( nameptr + 0x14, 32 );

	return name;
}

float BasePlayer::get_speed ( CSharedMemComms* drv, ModelState* model_state, float running, float ducking )
{
	float num = 1.f;
	num -= drv->read<float> ( reinterpret_cast< uintptr_t >( this ) + offsets::clothingMoveSpeedReduction );

	if ( model_state->get_swimming ( drv ) )
	{
		num += drv->read<float> ( reinterpret_cast< uintptr_t >( this ) + offsets::clothingWaterSpeedBonus );
	}

	return math_additional::Lerp ( { math_additional::Lerp ( { 2.8f, 5.5f, running } ), 1.7f, ducking } ) * num;
}

void BasePlayer::omni_sprint ( CSharedMemComms* drv )
{
	auto model_state = ( ModelState* ) drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::modelState );
	uintptr_t m_pMovement = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::basemovement );

	auto flags = drv->read<int> ( reinterpret_cast< uintptr_t >( this ) + offsets::flags );

	int m_bBusy = model_state->get_ducked ( drv ) | model_state->get_swimming ( drv );

	float m_flSpeed = get_speed ( drv, model_state, 1.f, m_bBusy );

	Vector3 m_vecVelocity = drv->read<Vector3> ( m_pMovement + offsets::target_movement );
	if ( !m_vecVelocity.empty ( ) )
	{
		Vector3 m_vecXZ = Vector3 { m_vecVelocity.x, 0.f, m_vecVelocity.z }.normalized ( ) * m_flSpeed;
		m_vecVelocity = { m_vecXZ.x, m_vecVelocity.y, m_vecXZ.z };

		drv->write<int> ( reinterpret_cast< uintptr_t >( model_state ) + offsets::flags, flags |= ( int ) 16 );
		drv->write<float> ( m_pMovement + 0x40, 1.f );

		drv->write<Vector3> ( m_pMovement + offsets::target_movement, m_vecVelocity );

		drv->write<int> ( reinterpret_cast< uintptr_t >( model_state ) + offsets::flags, flags |= ( int ) 16 );
		drv->write<float> ( m_pMovement + 0x40, 1.f );
	}
}

void BasePlayer::data ( CSharedMemComms* drv, player& m_data )
{
	m_data = drv->read<player> ( reinterpret_cast< uintptr_t >( this ) );
}

float BasePlayer::get_health ( CSharedMemComms* drv )
{
	return drv->read<float> ( reinterpret_cast< uintptr_t >( this ) + offsets::health );
}

unsigned long BasePlayer::get_uid ( CSharedMemComms* drv )
{

	return drv->read<unsigned long> ( reinterpret_cast< uintptr_t >( this ) + offsets::uid );
}

Vector3 BasePlayer::get_bone_position ( CSharedMemComms* drv, int bone, uintptr_t transform )
{
	uintptr_t bones;
	uintptr_t _transform;
	if ( transform == 0 )
	{
		uintptr_t player_model = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::model );
		_transform = drv->read<uintptr_t> ( player_model + 0x48 );
	}
	else
		_transform = transform;

	uintptr_t ogbones = drv->read<uintptr_t> ( _transform + static_cast< uintptr_t >( 0x20 + ( ( bone ) * 0x8 ) ) );
	bones = drv->read<uintptr_t> ( ogbones + 0x10 );
	return unity::get_position_injected ( drv, bones );
}

Item* BasePlayer::get_held_item ( CSharedMemComms* drv )
{
	int active_id;
	active_id = drv->read<int> ( reinterpret_cast< uintptr_t >( this ) + offsets::active_item ); // clActiveItem

	if ( !active_id )
		return NULL;

	Inventory* plyInventory;

	plyInventory = ( Inventory* ) drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::inventory );
	if ( !plyInventory )
		return NULL;

	ItemContainer* plyBelt = plyInventory->get_belt ( drv );
	if ( !plyBelt )
		return NULL;

	for ( int i = 0; i < 6; i++ )
	{
		Item* weapon = plyBelt->get_item ( drv, i );
		CASSERT ( weapon );
		int uid = weapon->get_uid ( drv );
		if ( !uid )
			continue;

		if ( uid == active_id )
			return weapon;
	}

	return NULL;
}

std::vector<PlayerItems> BasePlayer::get_item_list ( CSharedMemComms* drv )
{
	UINT m_iActiveItem;
	m_iActiveItem = drv->read<int> ( reinterpret_cast< uintptr_t >( this ) + offsets::active_item );

	Inventory* m_pInventory = ( Inventory* ) drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::inventory );
	ItemContainer* m_pContainer = m_pInventory->get_belt ( drv );
	std::vector<PlayerItems> m_vecItems;

	for ( UINT i = 0; i < 6; ++i )
	{
		Item* m_pItem = m_pContainer->get_item ( drv, i );
		UINT m_iUID = 0;
		if ( m_pItem )
			m_iUID = m_pItem->get_uid ( drv );

		if ( !m_iUID )
		{
			PlayerItems m_piEntry;
			m_piEntry.m_szName = "";
			m_piEntry.m_iAmount = 0;
			m_piEntry.m_bActive = false;
			m_vecItems.emplace_back ( m_piEntry );
			continue;
		}

		PlayerItems m_piEntry;

		m_piEntry.m_szName = m_pItem->get_nice_name ( drv ); // m_pItem->get_name ( drv );
		m_piEntry.m_iAmount = m_pItem->get_amount ( drv );
		m_piEntry.m_bActive = ( m_iActiveItem == m_iUID );
		m_vecItems.emplace_back ( m_piEntry );
	}

	return m_vecItems;
}

Vector2 BasePlayer::get_recoil_angles ( CSharedMemComms* drv )
{
	uintptr_t player_input = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::player_input );
	if ( !player_input )
		return {};
	return drv->read<Vector2> ( player_input + offsets::recoil_angles );
}

Vector2 BasePlayer::get_body_angles ( CSharedMemComms* drv )
{
	uintptr_t player_input = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::player_input );
	return drv->read<Vector2> ( player_input + offsets::body_angles );
}

Vector2 BasePlayer::get_head_angles ( CSharedMemComms* drv )
{
	uintptr_t player_input = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::player_input );
	return drv->read<Vector2> ( player_input + offsets::head_angles );
}

bool BasePlayer::set_recoil_angles ( CSharedMemComms* drv, Vector2 angles )
{
	uintptr_t player_input = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::player_input );
	if ( !player_input )
		return false;

	return drv->write<Vector2> ( player_input + offsets::recoil_angles, angles );
}

bool BasePlayer::set_body_angles ( CSharedMemComms* drv, Vector2 angles )
{
	uintptr_t player_input = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::player_input );
	if ( !player_input )
		return false;

	return drv->write<Vector2> ( player_input + offsets::body_angles, angles );
}

bool BasePlayer::set_flag ( CSharedMemComms* drv, int flag )
{
	return drv->write<int> ( reinterpret_cast< uintptr_t >( this ) + offsets::PlayerFlags, flag );
}

bool BasePlayer::is_wounded ( CSharedMemComms* drv )
{
	PlayerFlags flags = this->get_flags ( drv );
	return ( flags & wounded );
}

bool BasePlayer::is_sleeping ( CSharedMemComms* drv )
{
	PlayerFlags flags = this->get_flags ( drv );
	return ( flags & sleeping );
}

bool BasePlayer::is_teammate ( CSharedMemComms* drv )
{
	uintptr_t m_pLocalTeam = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( game::m_pLocalplayer ) + 0x5A0 ); ASSERT ( m_pLocalTeam ); // clientTeam;
	uintptr_t m_pMemberList = drv->read<uintptr_t> ( m_pLocalTeam + 0x30 ); ASSERT ( m_pMemberList ); // List<PlayerTeam.TeamMember>
	uintptr_t m_vecMemberList = drv->read<uintptr_t> ( m_pMemberList + 0x10 ); ASSERT ( m_vecMemberList );
	UINT m_iSize = drv->read<UINT> ( m_pMemberList + 0x18 ); ASSERT ( m_iSize );

	for ( UINT i = 0; i < m_iSize; ++i )
	{
		uintptr_t m_pTeamMember = drv->read<uintptr_t> ( m_vecMemberList + 0x20 + ( i * 0x08 ) ); CASSERT ( m_pTeamMember );
		ULONG m_ulSteamID = drv->read<ULONG> ( m_pTeamMember + 0x20 ); CASSERT ( m_ulSteamID );

		if ( m_ulSteamID == this->get_uid ( drv ) )
			return true;
	}

	return false;
}

Vector3 BasePlayer::get_velocity ( CSharedMemComms* drv )
{
	uintptr_t playermodel = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::player_model );
	Vector3 velocity = drv->read<Vector3> ( playermodel + offsets::oldvelocity ); // velocity
	return velocity;
}

PlayerFlags BasePlayer::get_flags ( CSharedMemComms* drv )
{
	int flags;
	flags = drv->read<int> ( reinterpret_cast< uintptr_t >( this ) + offsets::PlayerFlags );

	PlayerFlags flag = static_cast< PlayerFlags >( flags );
	return flag;
}

Vector4 PlayerEyes::get_body_rotation ( CSharedMemComms* drv )
{
	return drv->read<Vector4> ( reinterpret_cast< uintptr_t >( this ) + 0x44 );
}

void PlayerEyes::set_body_rotation ( CSharedMemComms* drv, Vector4 rotation )
{
	drv->write ( reinterpret_cast< uintptr_t >( this ) + 0x44, rotation );
}

PlayerEyes* BasePlayer::get_player_eyes ( CSharedMemComms* drv )
{
	return drv->read<PlayerEyes*> ( reinterpret_cast< uintptr_t >( this ) + offsets::eyes );
}