#include "stdafx.hpp"
#include "overlay.hpp"
#include <array>
#include <iostream>     // std::cout
#include <algorithm>    // std::max
#include "Menu/Elements.h"
#include "Menu/Framework/GUI.h"

template<typename T>
const T& max2 ( const T& a, const T& b )
{
	return ( a < b ) ? b : a;
}

template<typename T>
const T& min2 ( const T& a, const T& b )
{
	return ( b < a ) ? b : a;
}

namespace d3d9
{
	int screen_width;
	int screen_height;

	LPDIRECT3DDEVICE9 dx9_device;

	LPD3DXFONT verdana_13 = 0;
	LPD3DXFONT tahoma_11 = 0;
	LPD3DXFONT small_fonts_9 = 0;

	HWND game_window;
	HWND overlay_hwnd;
	RECT window_rect;
	ID3DXLine* dx9_line;
	MARGINS margin = { -1 };
}

namespace monitor
{
	float width, height;
};

Vector3 _CalcAngle ( Vector3 src, Vector3 dst, bool bruh )
{
	//xd
	if ( bruh )
	{
		Vector3 qAngles;
		Vector3 delta = Vector3 ( ( src.x - dst.x ), ( src.y - dst.y ), ( src.z - dst.z ) );
		double hyp = std::sqrtf ( delta.x * delta.x + delta.y * delta.y );
		qAngles.x = ( float ) ( std::atan ( delta.z / hyp ) * ( 180.0 / M_PI ) );
		qAngles.y = ( float ) ( std::atan ( delta.y / delta.x ) * ( 180.0 / M_PI ) );
		qAngles.z = 0.f;
		if ( delta.x >= 0.f )
			qAngles.y += 180.f;

		return qAngles;
	}
	else
	{
		Vector3 angles;
		Vector3 delta = src - dst;

		angles = delta.ToEulerAngles ( );

		angles.normalize ( );

		return angles;
	}
}

D3DCOLOR color_to_d3dcolor ( Color color )
{
	return D3DCOLOR_RGBA ( color.r ( ), color.g ( ), color.b ( ), color.a ( ) );
}

void DrawCornerBox ( int X, int Y, int W, int H, Color inline_color, Color outline_color )
{
	float lineW = ( W / 5 );
	float lineH = ( H / 6 );
	float lineT = 1;

	// cancer but whatever

	// outline
	{
		lineW = ( W / 5 ) + 1;
		lineH = ( H / 6 ) + 1;
		lineT = 1;

		// bottom right
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y - lineT, X + lineW, Y - lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y - lineT, X - lineT, Y + lineH, color_to_d3dcolor ( outline_color ) );

		// top right
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y + H - lineH, X - lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y + H + lineT, X + lineW, Y + H + lineT, color_to_d3dcolor ( outline_color ) );

		// bottom left
		rendering::c_renderer::get ( )->draw_line ( X + W - lineW, Y - lineT, X + W + lineT, Y - lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X + W + lineT, Y - lineT, X + W + lineT, Y + lineH, color_to_d3dcolor ( outline_color ) );

		// top left
		rendering::c_renderer::get ( )->draw_line ( X + W + lineT, Y + H - lineH, X + W + lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X + W - lineW, Y + H + lineT, X + W + lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
	}

	// outline
	{
		lineW = ( W / 5 ) + 1;
		lineH = ( H / 6 ) + 1;
		lineT = -1;

		// bottom right
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y - lineT, X + lineW, Y - lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y - lineT, X - lineT, Y + lineH, color_to_d3dcolor ( outline_color ) );

		// top right
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y + H - lineH, X - lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y + H + lineT, X + lineW, Y + H + lineT, color_to_d3dcolor ( outline_color ) );

		// bottom left
		rendering::c_renderer::get ( )->draw_line ( X + W - lineW, Y - lineT, X + W + lineT, Y - lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X + W + lineT, Y - lineT, X + W + lineT, Y + lineH, color_to_d3dcolor ( outline_color ) );

		// top left
		rendering::c_renderer::get ( )->draw_line ( X + W + lineT, Y + H - lineH, X + W + lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X + W - lineW, Y + H + lineT, X + W + lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
	}

	// outline
	{
		lineW = ( W / 5 ) + 1;
		lineH = ( H / 6 ) + 1;
		lineT = 0;

		// bottom right
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y - lineT, X + lineW, Y - lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y - lineT, X - lineT, Y + lineH, color_to_d3dcolor ( outline_color ) );

		// top right
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y + H - lineH, X - lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X - lineT, Y + H + lineT, X + lineW, Y + H + lineT, color_to_d3dcolor ( outline_color ) );

		// bottom left
		rendering::c_renderer::get ( )->draw_line ( X + W - lineW, Y - lineT, X + W + lineT, Y - lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X + W + lineT, Y - lineT, X + W + lineT, Y + lineH, color_to_d3dcolor ( outline_color ) );

		// top left
		rendering::c_renderer::get ( )->draw_line ( X + W + lineT, Y + H - lineH, X + W + lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
		rendering::c_renderer::get ( )->draw_line ( X + W - lineW, Y + H + lineT, X + W + lineT, Y + H + lineT, color_to_d3dcolor ( outline_color ) );
	}

	// middle
	{
		lineW = ( W / 5 );
		lineH = ( H / 6 );

		// top left
		rendering::c_renderer::get ( )->draw_line ( static_cast< float >( X ), static_cast< float >( Y ), static_cast< float >( X ), static_cast< float >( Y + lineH ), color_to_d3dcolor ( inline_color ) );
		rendering::c_renderer::get ( )->draw_line ( static_cast< float >( X ), static_cast< float >( Y ), static_cast< float >( X + lineW ), static_cast< float >( Y ), color_to_d3dcolor ( inline_color ) );

		// top right
		rendering::c_renderer::get ( )->draw_line ( static_cast< float >( X + W - lineW ), static_cast< float >( Y ), static_cast< float >( X + W ), static_cast< float >( Y ), color_to_d3dcolor ( inline_color ) );
		rendering::c_renderer::get ( )->draw_line ( static_cast< float >( X + W ), static_cast< float >( Y ), static_cast< float >( X + W ), static_cast< float >( Y + lineH ), color_to_d3dcolor ( inline_color ) );

		// bottom left
		rendering::c_renderer::get ( )->draw_line ( static_cast< float >( X ), static_cast< float >( Y + H - lineH ), static_cast< float >( X ), static_cast< float >( Y + H ), color_to_d3dcolor ( inline_color ) );
		rendering::c_renderer::get ( )->draw_line ( static_cast< float >( X ), static_cast< float >( Y + H ), static_cast< float >( X + lineW ), static_cast< float >( Y + H ), color_to_d3dcolor ( inline_color ) );

		// bottom right
		rendering::c_renderer::get ( )->draw_line ( static_cast< float >( X + W - lineW ), static_cast< float >( Y + H ), static_cast< float >( X + W ), static_cast< float >( Y + H ), color_to_d3dcolor ( inline_color ) );
		rendering::c_renderer::get ( )->draw_line ( static_cast< float >( X + W ), static_cast< float >( Y + H - lineH ), static_cast< float >( X + W ), static_cast< float >( Y + H ), color_to_d3dcolor ( inline_color ) );
	}
}

template <typename T>
void ESP_stringset ( T& vec )
{
	Matrix4x4 m_mxViewMatrix = rust::get_view_matrix ( mist::esp.get ( ) );
	for ( const auto& entry : vec )
	{
		const auto entity = entry.first;
		if ( !entity )
			continue;

		Vector3 position = mist::esp->read<Vector3> ( rust::get_object_pos_component ( mist::esp.get ( ), entity ) + 0x90 );
		if ( position.empty ( ) )
			continue;

		int m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );
		if ( m_flDistance > g_Vars.esp.distance_limit )
			continue;

		Vector2 m_scrPosition;
		if ( !rust::world_to_screen ( position, m_scrPosition, m_mxViewMatrix ) )
			continue;

		char bfr [128];
		sprintf ( bfr, "% dM", m_flDistance );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, entry.second.c_str ( ) ) / 2, m_scrPosition.y, d3d9::small_fonts_9, Color ( 255, 255, 255, 220 ), 0, true, false, entry.second.c_str ( ) );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, bfr ) / 2, m_scrPosition.y + 20, d3d9::small_fonts_9, Color ( 255, 255, 255, 220 ), 0, true, false, bfr );
	}
}

template <typename T>
void ESP_raidset ( T& vec )
{
	Matrix4x4 m_mxViewMatrix = rust::get_view_matrix ( mist::esp.get ( ) );
	for ( const auto& entry : vec )
	{
		const auto entity = std::get<0> ( entry );
		auto time = std::get<3> ( entry );

		if ( time > std::chrono::steady_clock::now ( ) + minutes ( 10 ) )
			continue;

		auto position = std::get<2> ( entry );
		int m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );

		Vector2 m_scrPosition;
		if ( !rust::world_to_screen ( position, m_scrPosition, m_mxViewMatrix ) )
			continue;

		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, std::get<1> ( entry ).c_str ( ) ) / 2, m_scrPosition.y, d3d9::small_fonts_9, g_Vars.esp.raid_color.ToRegularColor ( ), 0, true, false, std::get<1> ( entry ).c_str ( ) );

		char raidbfr [128];
		sprintf ( raidbfr, "LAST ACTIVE: %lldS", floor<seconds> ( time - std::chrono::steady_clock::now ( ) ) );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, raidbfr ) / 2, m_scrPosition.y + 10, d3d9::small_fonts_9, Color ( 255, 255, 255, g_Vars.esp.raid_color.ToRegularColor ( ).a ( ) ), 0, true, false, raidbfr );

		char bfr [128];
		sprintf ( bfr, "% dM", m_flDistance );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, bfr ) / 2, m_scrPosition.y + 20, d3d9::small_fonts_9, Color ( 255, 255, 255, g_Vars.esp.raid_color.ToRegularColor ( ).a ( ) ), 0, true, false, bfr );
	}
}

template <typename T>
void ESP_staticstringset ( T& vec )
{
	Matrix4x4 m_mxViewMatrix = rust::get_view_matrix ( mist::esp.get ( ) );
	for ( const auto& entry : vec )
	{
		const auto entity = std::get<0> ( entry );
		if ( !entity )
			continue;

		Vector3 position = std::get<2> ( entry );

		int m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );

		if ( m_flDistance > 250.f )
			continue;

		if ( m_flDistance > g_Vars.esp.distance_limit )
			continue;

		Vector2 m_scrPosition;
		if ( !rust::world_to_screen ( position, m_scrPosition, m_mxViewMatrix ) )
			continue;

		auto radColor = D3DCOLOR_RGBA ( 255, 255, 255, 220 );
		auto name = std::get<1> ( entry );

		std::transform ( name.begin ( ), name.end ( ), name.begin ( ), ::toupper );

		char bfr [128];
		sprintf ( bfr, "% dM", m_flDistance );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, name.c_str ( ) ) / 2, m_scrPosition.y, d3d9::small_fonts_9, radColor, 0, true, false, name.c_str ( ) );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, bfr ) / 2, m_scrPosition.y + 10, d3d9::small_fonts_9, Color ( 255, 255, 255, radColor.a ( ) ), 0, true, false, bfr );
	}
}

template <typename T>
void ESP_staticstringset ( T& vec, D3DCOLOR clr )
{
	Matrix4x4 m_mxViewMatrix = rust::get_view_matrix ( mist::esp.get ( ) );
	for ( const auto& entry : vec )
	{
		const auto entity = std::get<0> ( entry );
		if ( !entity )
			continue;

		Vector3 position = std::get<2> ( entry );

		int m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );

		if ( m_flDistance > 250.f )
			continue;

		if ( m_flDistance > g_Vars.esp.distance_limit )
			continue;

		Vector2 m_scrPosition;
		if ( !rust::world_to_screen ( position, m_scrPosition, m_mxViewMatrix ) )
			continue;

		auto name = std::get<1> ( entry );

		std::transform ( name.begin ( ), name.end ( ), name.begin ( ), ::toupper );

		char bfr [128];
		sprintf ( bfr, "% dM", m_flDistance );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, name.c_str ( ) ) / 2, m_scrPosition.y, d3d9::small_fonts_9, clr, 0, true, false, name.c_str ( ) );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, bfr ) / 2, m_scrPosition.y + 10, d3d9::small_fonts_9, Color ( 255, 255, 255, clr.a ( ) ), 0, true, false, bfr );
	}
}

template <typename T>
void ESP_staticstringcolorset ( T& vec )
{
	Matrix4x4 m_mxViewMatrix = rust::get_view_matrix ( mist::esp.get ( ) );
	for ( const auto& entry : vec )
	{
		const auto entity = std::get<0> ( entry );
		if ( !entity )
			continue;

		Vector3 position = std::get<2> ( entry );

		int m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );

		if ( m_flDistance > g_Vars.esp.distance_limit )
			continue;

		Vector2 m_scrPosition;
		if ( !rust::world_to_screen ( position, m_scrPosition, m_mxViewMatrix ) )
			continue;

		auto name = std::get<1> ( entry );

		std::transform ( name.begin ( ), name.end ( ), name.begin ( ), ::toupper );

		FloatColor lol = std::get<3> ( entry );
		char bfr [128];
		sprintf ( bfr, "% dM", m_flDistance );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, name.c_str ( ) ) / 2, m_scrPosition.y, d3d9::small_fonts_9, lol.ToRegularColor ( ), 0, true, false, name.c_str ( ) );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, bfr ) / 2, m_scrPosition.y + 10, d3d9::small_fonts_9, Color ( 255, 255, 255, lol.ToRegularColor ( ).a ( ) ), 0, true, false, bfr );

		if ( m_flDistance < 50 && name == _ ( "corpse" ) )
			DrawCornerBox ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, bfr ) / 2 - 4, m_scrPosition.y - 23, 19, 20, lol.ToRegularColor ( ), Color ( 0, 0, 0, lol.ToRegularColor ( ).a ( ) ) );
	}
}

template <typename T>
void ESP_staticstringlistcolorset ( T& vec )
{
	Matrix4x4 m_mxViewMatrix = rust::get_view_matrix ( mist::esp.get ( ) );
	for ( const auto& entry : vec )
	{
		const auto entity = std::get<0> ( entry );
		if ( !entity )
			continue;

		Vector3 position = std::get<2> ( entry );

		int m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );

		if ( m_flDistance > g_Vars.esp.distance_limit )
			continue;

		Vector2 m_scrPosition;
		if ( !rust::world_to_screen ( position, m_scrPosition, m_mxViewMatrix ) )
			continue;

		auto names = std::get<1> ( entry );

		FloatColor lol = std::get<3> ( entry );

		char bfr1 [78];
		sprintf ( bfr1, "%s [%dM]", _ ( "CUPBOARD" ), m_flDistance );
		rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, bfr1 ) / 2, m_scrPosition.y, d3d9::small_fonts_9, lol.ToRegularColor ( ), 0, true, false, bfr1 );
		int i = 1;
		for ( auto& username : names )
		{
			std::transform ( username.begin ( ), username.end ( ), username.begin ( ), ::toupper );

			rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, username.c_str ( ) ) / 2, m_scrPosition.y + 13 * i, d3d9::small_fonts_9, lol.ToRegularColor ( ), 0, false, true, username.c_str ( ) );
			++i;
		}
	}
}

template <typename T>
void ESP_player ( T& vec )
{
	Matrix4x4 m_mxViewMatrix = rust::get_view_matrix ( mist::esp.get ( ) );
	for ( const auto& entry : vec )
	{
		const auto& entity = entry.first;
		if ( !entity )
			continue;

		if ( entry.second == entity_type::Scientist && !g_Vars.esp.npc )
			continue;

		if ( entry.second == entity_type::TunnelDweller && !g_Vars.esp.tunneldweller )
			continue;

		const Vector3 position = mist::esp->read<Vector3> ( rust::get_object_pos_component ( mist::esp.get ( ), entity ) + 0x90 );
		if ( position.empty ( ) )
			continue;

		float m_flDistance = game::m_rLocalInfo.m_vecLocal.distance ( position );

		if ( m_flDistance > g_Vars.esp.distance_limit )
			continue;

		BasePlayer* player = ( BasePlayer* ) rust::get_base_entity ( mist::esp.get ( ), entity );

		const ULONG uid = player->get_uid ( mist::esp.get ( ) );
		auto& cache = game::cache [uid];

		Vector2 right_foot, left_foot;
		Vector3 reduce { 0.f, 0.05f, 0.f };
		Vector2 head;

		if ( !rust::world_to_screen ( player->get_bone_position ( mist::esp.get ( ), BoneList::r_foot, cache.transform ) - reduce, right_foot, m_mxViewMatrix, true ) ||
			!rust::world_to_screen ( player->get_bone_position ( mist::esp.get ( ), BoneList::l_foot, cache.transform ) - reduce, left_foot, m_mxViewMatrix, true ) ||
			!rust::world_to_screen ( player->get_bone_position ( mist::esp.get ( ), BoneList::head, cache.transform ) + Vector3 ( xf ( 0.f ), xf ( 0.22f ), xf ( 0.f ) ), head, m_mxViewMatrix, true ) )
		{
			continue;
		}

		const Vector2 tempFeet = ( right_foot + left_foot ) / 2.f;
		const float _h = head.y - tempFeet.y;
		const float h = tempFeet.y - head.y;
		const float y = head.y;
		const float w = std::abs ( _h / 2.f );
		float x = tempFeet.x - _h / 4.f;

		auto name = cache.m_szName;

		if ( cache.m_bSleeper )
		{
			if ( g_Vars.esp.sleeper )
			{
				std::transform ( name.begin ( ), name.end ( ), name.begin ( ), ::toupper );

				char bfr [256];
				sprintf ( bfr, "%s [%.0fM]", name.c_str ( ), m_flDistance );
				rendering::c_renderer::get ( )->draw_string ( x + w / 2 - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, bfr ) / 2, y, d3d9::small_fonts_9, g_Vars.esp.sleeper_color.ToRegularColor ( ), 0, true, false, bfr );
			}
			continue;
		}

		float max_health = entry.second == entity_type::Scientist || entry.second == entity_type::TunnelDweller ? 150.f : 100.f;
		float health = min2 ( player->get_health ( mist::esp.get ( ) ), max_health );
		int health_size = rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, std::to_string ( ( int ) health ) );
		float x_offset = 9;
		float y_offset = health_size;

		if ( health <= 0.f )
			continue;

		x = x - w;

		FloatColor bx_col = g_Vars.esp.box_color;
		if ( entry.second == entity_type::Scientist )
			bx_col = { 5, 195, 221 };
		if ( entry.second == entity_type::TunnelDweller )
			bx_col = { 255, 127, 0 };
		if ( g_Vars.esp.teammate && cache.m_bFriend )
			bx_col = { 0, 100, 0 };
		else if ( g_Vars.esp.highlight_target && entity == data::aimbot::m_pTarget )
			bx_col = g_Vars.esp.highlight_target_color;

		// Fuck
		const auto max23 = [] ( const float& a, const float& b )
		{
			return ( a < b ) ? b : a;
		};

		float h_offset = max23 ( w / 8, 8 );

		if ( g_Vars.esp.box )
			DrawCornerBox ( x, y, w, h, bx_col.ToRegularColor ( ), Color ( 0, 0, 0, bx_col.ToRegularColor ( ).a ( ) ) );

		if ( g_Vars.esp.name )
			rendering::c_renderer::get ( )->draw_string ( x + w / 2 - rendering::c_renderer::get ( )->get_text_width ( d3d9::tahoma_11, cache.m_szName.c_str ( ) ) / 2, y - 13, d3d9::tahoma_11, g_Vars.esp.name_color.ToRegularColor ( ), 0, false, true, cache.m_szName.c_str ( ) );

		if ( g_Vars.esp.health )
		{
			const float percentage = health / max_health;
			const float green = percentage * xf ( 255.f );
			const float red = xf ( 255.f ) - green;

			rendering::c_renderer::get ( )->draw_rect ( x - h_offset, y, 2, h, 2, Color ( 0, 0, 0, g_Vars.esp.health_color.ToRegularColor ( ).a ( ) ).ToARGB ( ) );
			rendering::c_renderer::get ( )->draw_filled_rect ( x - h_offset, y, 2, h, Color ( 0, 0, 0, g_Vars.esp.health_color.ToRegularColor ( ).a ( ) ).ToARGB ( ) );
			rendering::c_renderer::get ( )->draw_filled_rect ( x - h_offset, y + h - ( h * percentage ), 2, ( h * percentage ), g_Vars.esp.health_override ? g_Vars.esp.health_color.ToRegularColor ( ).ToARGB ( ) : Color ( ( int ) red, ( int ) green, 0, g_Vars.esp.health_color.ToRegularColor ( ).a ( ) ).ToARGB ( ) );

			if ( health < max_health )
				rendering::c_renderer::get ( )->draw_string ( x - h_offset - 3, y + h - ( h * percentage ) - y_offset / 2, d3d9::small_fonts_9, Color ( 255, 255, 255, g_Vars.esp.health_color.ToRegularColor ( ).a ( ) ), 0, true, false, std::to_string ( ( int ) health ) );
		}

		if ( !cache.m_szWeaponName.empty ( ) && g_Vars.esp.ammo_bar )
		{
			//float scale;

			//// make the division of 2 ints produce a float instead of another int.
			//scale = cache.m_iMaxAmmo != -1 ? ( float ) cache.m_iAmmo / cache.m_iMaxAmmo : 1.f;

			//// relative to bar.
			//int bar = ( int ) std::round ( ( w - 2 ) * scale );

			//// draw.
			//rendering::c_renderer::get ( )->draw_filled_rect ( x + w / 2 + 1, y + h + 3, w, 4, Color ( 0, 0, 0, 180 ).ToARGB ( ) );

			//Color clr = g_Vars.esp.ammo_bar_color.ToRegularColor ( );

			//rendering::c_renderer::get ( )->draw_filled_rect ( x + w / 2 + 1, y + h + 3, bar, 2, clr.ToARGB ( ) );

			//// less then a 5th of the bullets left.
			//if ( cache.m_iAmmo < cache.m_iMaxAmmo )
			//{
			//	rendering::c_renderer::get ( )->draw_string ( x + w / 2 + bar, y + h, d3d9::small_fonts_9, Color ( 255, 255, 255, 180 ).ToARGB ( ), 0, true, false, std::to_string ( cache.m_iMaxAmmo ) );
			//}
		}

		int index = 5;

		if ( g_Vars.esp.ammo_bar )
			index = 10;

		if ( !cache.m_szWeaponName.empty ( ) && g_Vars.esp.weapon )
		{
			std::transform ( cache.m_szWeaponName.begin ( ), cache.m_szWeaponName.end ( ), cache.m_szWeaponName.begin ( ), ::toupper );

			if ( g_Vars.esp.weapon )
				rendering::c_renderer::get ( )->draw_string ( x + w / 2 - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, cache.m_szWeaponName.c_str ( ) ) / 2, y + h + index, d3d9::small_fonts_9, g_Vars.esp.weapon_color.ToRegularColor ( ), 0, true, false, cache.m_szWeaponName.c_str ( ) );
			if ( g_Vars.esp.distance )
				rendering::c_renderer::get ( )->draw_string ( x + w / 2 - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, std::to_string ( ( int ) m_flDistance ) + _ ( "M" ) ) / 2, y + h + 8 + index, d3d9::small_fonts_9, g_Vars.esp.distance_color.ToRegularColor ( ), 0, true, false, std::to_string ( ( int ) m_flDistance ) + _ ( "M" ) );
		}
		else if ( g_Vars.esp.distance )
			rendering::c_renderer::get ( )->draw_string ( x + w / 2 - rendering::c_renderer::get ( )->get_text_width ( d3d9::small_fonts_9, std::to_string ( ( int ) m_flDistance ) + _ ( "M" ) ) / 2, y + h + index, d3d9::small_fonts_9, g_Vars.esp.distance_color.ToRegularColor ( ), 0, true, false, std::to_string ( ( int ) m_flDistance ) + _ ( "M" ) );

		if ( g_Vars.esp.snaplines && entity == data::aimbot::m_pTarget )
		{
			D3DXVECTOR2 m_scrPosition1 = { monitor::width / 2.f, monitor::height };
			rendering::c_renderer::get ( )->draw_line ( m_scrPosition1.x, m_scrPosition1.y, tempFeet.x, tempFeet.y, g_Vars.esp.snaplines_color.ToRegularColor ( ).ToARGB ( ) );
		}

		if ( m_flDistance > 100.f || !g_Vars.esp.skeleton )
			continue;

		auto _neck = player->get_bone_position ( mist::esp.get ( ), BoneList::neck, cache.transform );
		auto spine = player->get_bone_position ( mist::esp.get ( ), BoneList::spine1, cache.transform );
		auto lupper = player->get_bone_position ( mist::esp.get ( ), BoneList::l_upperarm, cache.transform );
		auto rupper = player->get_bone_position ( mist::esp.get ( ), BoneList::r_upperarm, cache.transform );
		auto lfore = player->get_bone_position ( mist::esp.get ( ), BoneList::l_forearm, cache.transform );
		auto rfore = player->get_bone_position ( mist::esp.get ( ), BoneList::r_forearm, cache.transform );
		auto lhand = player->get_bone_position ( mist::esp.get ( ), BoneList::l_hand, cache.transform );
		auto rhand = player->get_bone_position ( mist::esp.get ( ), BoneList::r_hand, cache.transform );
		auto lknee = player->get_bone_position ( mist::esp.get ( ), BoneList::l_knee, cache.transform );
		auto rknee = player->get_bone_position ( mist::esp.get ( ), BoneList::r_knee, cache.transform );
		auto lhip = player->get_bone_position ( mist::esp.get ( ), BoneList::l_hip, cache.transform );
		auto rhip = player->get_bone_position ( mist::esp.get ( ), BoneList::r_hip, cache.transform );

		Vector3 _head = player->get_bone_position ( mist::esp.get ( ), BoneList::head, cache.transform );
		Vector2 chest, l_shoulder, r_shoulder, l_forearm, r_forearm, l_hand, r_hand, neck, l_arm, r_arm, l_knee, r_knee, l_hipbone, r_hipbone;

		if ( !rust::world_to_screen ( _head, head, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( _neck, neck, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( spine, chest, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( lupper, l_shoulder, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( rupper, r_shoulder, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( lfore, l_forearm, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( rfore, r_forearm, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( lhand, l_hand, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( rhand, r_hand, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( lknee, l_knee, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( rknee, r_knee, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( lhip, l_hipbone, m_mxViewMatrix ) )
			continue;
		if ( !rust::world_to_screen ( rhip, r_hipbone, m_mxViewMatrix ) )
			continue;

		rendering::c_renderer::get ( )->draw_line ( head.x, head.y, neck.x, neck.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( neck.x, neck.y, chest.x, chest.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );

		// left

		rendering::c_renderer::get ( )->draw_line ( neck.x, neck.y, l_shoulder.x, l_shoulder.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( l_shoulder.x, l_shoulder.y, l_forearm.x, l_forearm.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( l_forearm.x, l_forearm.y, l_hand.x, l_hand.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );

		rendering::c_renderer::get ( )->draw_line ( chest.x, chest.y, l_hipbone.x, l_hipbone.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( l_hipbone.x, l_hipbone.y, l_knee.x, l_knee.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( l_knee.x, l_knee.y, left_foot.x, left_foot.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );

		// right

		rendering::c_renderer::get ( )->draw_line ( neck.x, neck.y, r_shoulder.x, r_shoulder.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( r_shoulder.x, r_shoulder.y, r_forearm.x, r_forearm.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( r_forearm.x, r_forearm.y, r_hand.x, r_hand.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );

		rendering::c_renderer::get ( )->draw_line ( chest.x, chest.y, r_hipbone.x, r_hipbone.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( r_hipbone.x, r_hipbone.y, r_knee.x, r_knee.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
		rendering::c_renderer::get ( )->draw_line ( r_knee.x, r_knee.y, right_foot.x, right_foot.y, g_Vars.esp.skeleton_color.ToRegularColor ( ).ToARGB ( ) );
	}
}

struct savepos
{
	Vector3			pos;
};

std::vector<savepos> PosFinal;
savepos TempPos;

namespace overlay
{
	void render ( )
	{
		d3d9::dx9_device->Clear ( 0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB ( 0, 0, 0, 0 ), 1.0f, 0 );
		d3d9::dx9_device->BeginScene ( );
		DWORD old_fvf;
		d3d9::dx9_device->GetFVF ( &old_fvf );
		d3d9::dx9_device->SetFVF ( D3_DFVF_CUSTOMVERTEX );
		d3d9::dx9_device->SetTexture ( 0, nullptr );

		uintptr_t m_pPlayerInput = mist::esp->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::player_input );

		if ( g_Vars.esp.enabled )
		{
			game::draw_mutex.lock ( );
			auto stash = game::stashes;
			auto animals = game::animals;
			auto collectable = game::collectibles;
			auto ores = game::ores;
			auto items = game::items;
			auto radtown = game::barrel;
			auto vehicles = game::vehicles;
			auto special = game::special;
			auto traps = game::traps;
			auto raid = game::raid;
			auto players = game::players;
			auto cupboard = game::cupboard;
			auto bags = game::bag;
			game::draw_mutex.unlock ( );

			do
			{
				auto m_mxViewMatrix = rust::get_view_matrix ( mist::esp.get ( ) );

				if ( game::m_pLocalplayer == 0 )
					break;

				game::m_rLocalInfo.m_vecLocal = mist::esp->read<Vector3> ( rust::get_object_pos_component ( mist::esp.get ( ), game::m_rLocalInfo.m_pPlayer ) + 0x90 );

				if ( !mist::esp->read<uint8_t> ( m_pPlayerInput + 0x94 ) && g_Vars.esp.hide )
					break;

				if ( g_Vars.esp.stash )
					ESP_staticstringcolorset ( stash );

				if ( g_Vars.esp.animals )
					ESP_staticstringcolorset ( animals );

				if ( g_Vars.esp.collectable )
					ESP_staticstringcolorset ( collectable );

				if ( g_Vars.esp.ores )
					ESP_staticstringcolorset ( ores );

				if ( g_Vars.esp.item )
					ESP_staticstringcolorset ( items );

				if ( g_Vars.esp.radtown )
					ESP_staticstringcolorset ( radtown );

				if ( g_Vars.esp.vehicles )
				{
					if ( g_Vars.esp.special )
						ESP_staticstringcolorset ( special );

					ESP_staticstringcolorset ( vehicles );
				}

				if ( g_Vars.esp.traps )
					ESP_staticstringcolorset ( traps );

				if ( g_Vars.esp.raid )
					ESP_raidset ( raid );

				if ( g_Vars.esp.players )
					ESP_player ( players );

				if ( g_Vars.esp.toolcupboard )
					ESP_staticstringlistcolorset ( cupboard );

				if ( g_Vars.esp.sleepingbag )
					ESP_staticstringcolorset ( bags );

				if ( data::aimbot::m_bFound && g_Vars.esp.item_info )
				{
					static auto m_pCachedPlayer = 0;
					static char iterator = 0;
					static std::vector<PlayerItems> m_vecCachedItems;
					auto m_pPlayer = data::aimbot::m_pTarget;
					BasePlayer* m_pBasePlayer = ( BasePlayer* ) rust::get_base_entity ( mist::esp.get ( ), m_pPlayer );

					if ( m_pCachedPlayer != m_pPlayer || iterator % 1000 == 0 )
					{
						m_vecCachedItems = m_pBasePlayer->get_item_list ( mist::esp.get ( ) );
						m_pCachedPlayer = m_pPlayer;

						iterator = 0;
					}

					auto& cache = game::cache [m_pBasePlayer->get_uid ( mist::esp.get ( ) )];
					if ( m_vecCachedItems.size ( ) > 0 )
					{
						// Lol
						std::string headers = cache.m_szName.c_str ( );

						int bx = 30;
						int by = 176;
						int bby = 20;

						int m_vecSize = rendering::c_renderer::get ( )->get_text_width ( d3d9::verdana_13, headers );

						//rendering::c_renderer::get ( )->draw_filled_rect ( 25, 150, m_vecSize + 5, bby, D3DCOLOR_RGBA ( 15, 15, 15, 255 ) );
						rendering::c_renderer::get ( )->draw_string ( bx, 160, d3d9::verdana_13, Color ( 255, 255, 255, 220 ), 0, true, false, headers );

						for ( auto& m_piEntry : m_vecCachedItems )
						{
							if ( std::empty ( m_piEntry.m_szName ) || m_piEntry.m_iAmount == 0 || !m_piEntry.m_iAmount )
								continue;

							char bfr [128];
							sprintf ( bfr, "(%d) %s", m_piEntry.m_iAmount, m_piEntry.m_szName.c_str ( ) );
							bool m_bActive = m_piEntry.m_bActive;
							Color m_ColorActive = m_bActive ? g_Vars.menu.menu_color.ToRegularColor ( ) : Color ( 255, 255, 255, 255 );

							std::string items = bfr;

							rendering::c_renderer::get ( )->draw_string ( bx, by, d3d9::verdana_13, m_ColorActive, 0, true, false, items );
							by += 13;
							bby += 13;
						}
					}
					++iterator;
				}

				if ( g_Vars.misc.radar )
				{
					float x = monitor::width - 175, y = 200;

					rendering::c_renderer::get ( )->circle_filled ( x, y, 150, 0, 0, true, 32, color_to_d3dcolor ( Color ( 0, 0, 0, 120 ) ) );
					rendering::c_renderer::get ( )->draw_line ( x, y - 150, x, y + 150, color_to_d3dcolor ( Color ( 0, 0, 0, 255 ) ) );
					rendering::c_renderer::get ( )->draw_line ( x - 150, y, x + 150, y, color_to_d3dcolor ( Color ( 0, 0, 0, 255 ) ) );
					rendering::c_renderer::get ( )->draw_filled_rect ( x - 2, y - 2, 5, 5, D3DCOLOR_RGBA ( 0, 0, 0, 220 ) );
					rendering::c_renderer::get ( )->draw_filled_rect ( x - 1, y - 1, 3, 3, color_to_d3dcolor ( Color ( 255, 255, 255, 255 ) ) );

					Vector2 m_vecLocal { game::m_rLocalInfo.m_vecLocal.x, game::m_rLocalInfo.m_vecLocal.z };
					Vector2 m_angLocal = game::m_pLocalplayer->get_body_angles ( mist::esp.get ( ) );
					for ( const auto& entry : players )
					{
						auto entity = entry.first;
						if ( !entity )
							continue;

						if ( entry.second == entity_type::Scientist && !g_Vars.esp.npc )
							continue;

						if ( entry.second == entity_type::TunnelDweller && !g_Vars.esp.tunneldweller )
							continue;

						const Vector3 position = mist::esp->read<Vector3> ( rust::get_object_pos_component ( mist::esp.get ( ), entity ) + 0x90 );
						if ( position.empty ( ) )
							continue;

						BasePlayer* nn = ( BasePlayer* ) rust::get_base_entity ( mist::esp.get ( ), entity );

						if ( nn->is_sleeping ( mist::esp.get ( ) ) )
							continue;

						Vector2 m_vecEnemy { position.x, position.z };
						m_vecEnemy = m_vecLocal - m_vecEnemy;

						float m_flDistance = m_vecEnemy.length ( ) * ( float ) g_Vars.misc.scale / 100.f;
						m_vecEnemy = m_vecEnemy.normalized ( );
						m_flDistance = min2 ( 150.f, m_flDistance );
						m_vecEnemy *= m_flDistance;
						m_vecEnemy = rust::RotatePoint ( m_vecEnemy, { 0.f, 0.f }, 360.f - m_angLocal.y, false );
						{
							Color color = entry.second == entity_type::Scientist || entry.second == entity_type::TunnelDweller ? Color { 0, 255, 255, 200 } : Color { 255, 0, 0, 200 };
							if ( nn->is_wounded ( mist::esp.get ( ) ) )
								color = Color { 255, 255, 0, 120 };
							if ( game::cache [nn->get_uid ( mist::esp.get ( ) )].m_bFriend == true )
								color = Color { 0, 255, 0 };

							rendering::c_renderer::get ( )->draw_filled_rect ( x - m_vecEnemy.x - 2, y + m_vecEnemy.y - 2, 5, 5, D3DCOLOR_RGBA ( 0, 0, 0, 220 ) );
							rendering::c_renderer::get ( )->draw_filled_rect ( x - m_vecEnemy.x - 1, y + m_vecEnemy.y - 1, 3, 3, color_to_d3dcolor ( color ) );
						}
					}
				}

				if ( g_Vars.esp.offscren_enabled )
				{
					float x = monitor::width / 2, y = monitor::height / 2;

					Vector2 m_vecLocal { game::m_rLocalInfo.m_vecLocal.x, game::m_rLocalInfo.m_vecLocal.z };
					Vector2 m_angLocal = game::m_pLocalplayer->get_body_angles ( mist::esp.get ( ) );
					for ( const auto& entry : players )
					{
						auto entity = entry.first;
						if ( !entity )
							continue;

						if ( entry.second == entity_type::Scientist && !g_Vars.esp.npc )
							continue;

						if ( entry.second == entity_type::TunnelDweller && !g_Vars.esp.tunneldweller )
							continue;

						const Vector3 position = mist::esp->read<Vector3> ( rust::get_object_pos_component ( mist::esp.get ( ), entity ) + 0x90 );
						if ( position.empty ( ) )
							continue;

						Vector2 screen_pos;
						auto is_on_screen = rust::world_to_screen ( position, screen_pos, m_mxViewMatrix );

						if ( is_on_screen )
							continue;

						BasePlayer* nn = ( BasePlayer* ) rust::get_base_entity ( mist::esp.get ( ), entity );

						if ( nn->is_sleeping ( mist::esp.get ( ) ) )
							continue;

						Vector2 m_vecEnemy { position.x, position.z };
						m_vecEnemy = m_vecLocal - m_vecEnemy;

						float m_flDistance = m_vecEnemy.length ( ) * g_Vars.esp.offscren_distance / 100.f;
						m_vecEnemy = m_vecEnemy.normalized ( );
						//m_flDistance = min2 ( 150.f, m_flDistance );
						m_vecEnemy *= m_flDistance;
						m_vecEnemy = rust::RotatePoint ( m_vecEnemy, { 0.f, 0.f }, 360.f - m_angLocal.y, false );
						{
							Color color = entry.second == entity_type::Scientist || entry.second == entity_type::TunnelDweller ? Color { 0, 255, 255, 200 } : g_Vars.esp.offscreen_color.ToRegularColor ( );
							if ( nn->is_wounded ( mist::esp.get ( ) ) )
								color = Color { 255, 255, 0, 120 };
							if ( game::cache [nn->get_uid ( mist::esp.get ( ) )].m_bFriend == true )
								color = Color { 0, 255, 0 };

							rendering::c_renderer::get ( )->draw_filled_rect ( x - m_vecEnemy.x - 2, y + m_vecEnemy.y - 2, 5, 5, D3DCOLOR_RGBA ( 0, 0, 0, 220 ) );
							rendering::c_renderer::get ( )->draw_filled_rect ( x - m_vecEnemy.x - 1, y + m_vecEnemy.y - 1, 3, 3, color_to_d3dcolor ( color ) );
						}
					}
				}

				if ( g_Vars.misc.flyhack_indicator )
				{
					Vector3 m_vecNewLocal { };
					m_vecNewLocal = mist::esp->read<Vector3> ( rust::get_object_pos_component ( mist::esp.get ( ), game::m_rLocalInfo.m_pPlayer ) + 0x90 );

					savepos tempPos2 = {};
					tempPos2.pos = m_vecNewLocal;

					PosFinal.push_back ( tempPos2 );

					bool m_bInAir = GetAsyncKeyState ( g_Vars.misc.fly ); //|| ( !game::m_rLocalInfo.m_pModelState->HasFlag ( mist::misc.get ( ), ModelState::Flag::OnGround ) && !game::m_rLocalInfo.m_pModelState->get_swimming ( mist::esp.get ( ) ) );

					if ( m_bInAir )
					{
						savepos tempPos = {};
						tempPos.pos = m_vecNewLocal;

						PosFinal.push_back ( tempPos );
					}
					else
					{
						PosFinal = {};
					}

					Vector3 old_pos = PosFinal [0].pos;

					float m_flFlyhackDistanceVertical = 0.f;
					float m_flFlyhackDistanceHorizontal = 0.f;

					if ( game::m_pLocalplayer->get_health ( mist::esp.get ( ) ) == 0.0f )
					{
						m_flFlyhackDistanceVertical = 0.f;
						m_flFlyhackDistanceHorizontal = 0.f;
					}

					if ( g_Vars.misc.bfly )
					{
						Vector3 vector4 = m_vecNewLocal - old_pos;
						float num2 = std::abs ( vector4.y );
						float num3 = vector4.Magnitude ( );

						if ( vector4.y >= 0.f )
						{
							m_flFlyhackDistanceVertical += vector4.y;
						}
						if ( num2 < num3 )
						{
							m_flFlyhackDistanceHorizontal += num3;
						}
					}
					else
					{
						m_flFlyhackDistanceVertical = 0.f;
						m_flFlyhackDistanceHorizontal = 0.f;
					}

					//Vector2 m_scrPosition;
					//rust::world_to_screen ( old_pos, m_scrPosition, m_mxViewMatrix );

					//Vector2 m_scrPosition2;
					//rust::world_to_screen ( m_vecNewLocal, m_scrPosition2, m_mxViewMatrix );

					//rendering::c_renderer::get ( )->draw_string ( m_scrPosition.x, m_scrPosition.y, d3d9::small_fonts_9, D3DCOLOR_RGBA ( 0, 255, 0, 255 ), 0, true, false, "old pos" );
					//rendering::c_renderer::get ( )->draw_string ( m_scrPosition2.x, m_scrPosition2.y, d3d9::small_fonts_9, D3DCOLOR_RGBA ( 255, 0, 0, 255 ), 0, true, false, "new pos" );

					//uintptr_t movement = mist::misc->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::basemovement ); VASSERT ( movement );

					//if ( m_flFlyhackDistanceVertical > 1.7f )
					//{
					//	mist::misc->write<float> ( movement + 0x70, -3000.f );
					//	mist::misc->write<float> ( movement + 0x74, -3000.f );

					//	m_flFlyhackDistanceVertical = 0.f;
					//	m_flFlyhackDistanceHorizontal = 0.f;
					//}
					//else
					//{
					//	mist::misc->write<float> ( movement + 0x70, 1.1f );
					//	mist::misc->write<float> ( movement + 0x74, 0.550f );
					//}

					static auto center_x = d3d9::screen_width / 2.f;
					static auto center_y = d3d9::screen_height / 2.f;

					std::clamp ( m_flFlyhackDistanceVertical, 0.0f, 3.0f );
					{
						// GetJumpHeight + 1.5f (flyhack_forgiveness_vertical)
						const float percentage = m_flFlyhackDistanceVertical / 3.0f;
						const float red = percentage * xf ( 255.f );
						const float green = xf ( 255.f ) - red;

						rendering::c_renderer::get ( )->draw_filled_rect ( center_x - 200, center_y - 426, 401, 15, D3DCOLOR_RGBA ( 0, 0, 0, 255 ) );
						rendering::c_renderer::get ( )->draw_gradient ( center_x - 199, center_y - 425, 400 * percentage, 13, D3DCOLOR_RGBA ( ( int ) red, ( int ) green, 0, 255 ), D3DCOLOR_RGBA ( ( int ) red, ( int ) green, 0, 220 ), vertical );
					}

					std::clamp ( m_flFlyhackDistanceHorizontal, 0.0f, 6.5f );
					{
						// 5.f + 1.5f (flyhack_forgiveness_horizontal)
						const float percentage = m_flFlyhackDistanceHorizontal / 6.5f;
						const float red = percentage * xf ( 255.f );
						const float green = xf ( 255.f ) - red;

						rendering::c_renderer::get ( )->draw_filled_rect ( center_x - 200, center_y - 406, 401, 15, D3DCOLOR_RGBA ( 0, 0, 0, 255 ) );
						rendering::c_renderer::get ( )->draw_gradient ( center_x - 199, center_y - 405, 400 * percentage, 13, D3DCOLOR_RGBA ( ( int ) red, ( int ) green, 0, 255 ), D3DCOLOR_RGBA ( ( int ) red, ( int ) green, 0, 220 ), vertical );
					}
				}

				if ( data::aimbot::m_bFound && game::m_rLocalInfo.m_vInfo.m_bPrediction == true )
				{
					Vector2 predpos;
					if ( rust::world_to_screen ( data::aimbot::m_vecPosition, predpos, m_mxViewMatrix ) )
						rendering::c_renderer::get ( )->draw_string ( predpos.x - 1, predpos.y - 1, d3d9::small_fonts_9, Color ( 255, 255, 255, 255 ), 0, true, false, _ ( "X" ) );
				}
			} while ( 0 );
		}

		static float rainbow;
		rainbow += 0.005f;
		if ( rainbow > 1.f )
			rainbow = 0.f;

		{
			std::string text = _ ( "phantom.tech" );

			text += _ ( " [beta]" );

			if ( game::m_pLocalplayer != 0 )
			{
				text += _ ( " | " );

				text += game::m_rLocalInfo.m_szWeaponName;
			}

			Vector2 size = Render::DirectX::Fonts::menu.measure_size ( text );

			Vector2 screen = Render::GetScreenSize ( );

			// background.
			rendering::c_renderer::get ( )->draw_filled_rect ( screen.x - size.x - 20, 10, size.x + 15, size.y + 2, Color ( 39, 41, 54, 220 ).ToARGB ( ) );
			rendering::c_renderer::get ( )->draw_filled_rect ( screen.x - size.x - 20, 10, 2, size.y + 2, g_Vars.menu.menu_color.ToRegularColor ( ).ToARGB ( ) );

			// text.
			rendering::c_renderer::get ( )->draw_string ( screen.x - size.x - 15, 10, d3d9::tahoma_11, Color ( 220, 220, 220, 250 ), 0, false, true, text );
		}


		//if ( vars::weapon::draw_crosshair )
		{
			static auto center_x = d3d9::screen_width / 2.f;
			static auto center_y = d3d9::screen_height / 2.f;

			//rendering::c_renderer::get ( )->draw_filled_rect ( center_x - 2, center_y - 2, 5, 5, D3DCOLOR_RGBA ( 0, 0, 0, 220 ) );
			//rendering::c_renderer::get ( )->draw_filled_rect ( center_x - 1, center_y - 1, 3, 3, D3DCOLOR_RGBA ( 255, 255, 255, 220 ) );

			rendering::c_renderer::get ( )->draw_filled_rect ( center_x - 8 - 1, center_y - 1, 5 + 2, 1 + 2, D3DCOLOR_RGBA ( 0, 0, 0, 220 ) );//Left line
			rendering::c_renderer::get ( )->draw_filled_rect ( center_x + 4 - 1, center_y - 1, 5 + 2, 1 + 2, D3DCOLOR_RGBA ( 0, 0, 0, 220 ) );//Right line
			rendering::c_renderer::get ( )->draw_filled_rect ( center_x - 1, center_y - 8 - 1, 1 + 2, 5 + 2, D3DCOLOR_RGBA ( 0, 0, 0, 220 ) );//Top line
			rendering::c_renderer::get ( )->draw_filled_rect ( center_x - 1, center_y + 4 - 1, 1 + 2, 5 + 2, D3DCOLOR_RGBA ( 0, 0, 0, 220 ) );//Bottom line

			rendering::c_renderer::get ( )->draw_filled_rect ( center_x - 8, center_y, 5, 1, D3DCOLOR_RGBA ( 255, 255, 255, 220 ) );//Left line
			rendering::c_renderer::get ( )->draw_filled_rect ( center_x + 4, center_y, 5, 1, D3DCOLOR_RGBA ( 255, 255, 255, 220 ) );//Right line
			rendering::c_renderer::get ( )->draw_filled_rect ( center_x, center_y - 8, 1, 5, D3DCOLOR_RGBA ( 255, 255, 255, 220 ) );//Top line
			rendering::c_renderer::get ( )->draw_filled_rect ( center_x, center_y + 4, 1, 5, D3DCOLOR_RGBA ( 255, 255, 255, 220 ) );//Bottom line

			rendering::c_renderer::get ( )->draw_circle ( center_x, center_y, data::aimbot::m_flFOV, 0, 0, true, 32, D3DCOLOR_RGBA ( 255, 255, 255, 220 ) );
		}

		if ( Render::DirectX::initialized )
		{
			// gay idc
			InputHelper::Update ( );

			if ( InputHelper::Pressed ( VK_INSERT ) )
			{
				g_Vars.global.menuOpen = !g_Vars.global.menuOpen;
			}

			Render::DirectX::begin ( );
			{
				float m_flDesiredFov = GUI::ctx->animation + 1.0f;
				float m_flDesiredZoom = GUI::ctx->animation - 1.0f;
				float m_flCurrentFov = GUI::ctx->animation;

				static bool m_bShouldAnimate = true;
				if ( g_Vars.global.menuOpen )
				{
					if ( m_bShouldAnimate )
					{
						if ( m_flCurrentFov <= m_flDesiredZoom )
						{
							GUI::ctx->animation = m_flDesiredZoom;
							m_bShouldAnimate = false;
						}

						GUI::ctx->animation = m_flCurrentFov - 0.03f;
					}
				}
				else
				{
					if ( m_flCurrentFov >= m_flDesiredFov )
					{
						GUI::ctx->animation = m_flDesiredFov;
						m_bShouldAnimate = true;
					}

					GUI::ctx->animation = m_flCurrentFov + 0.03f;
				}

				//GUI::ctx->animation = g_Vars.global.menuOpen ? ( GUI::ctx->animation + ( 1.0f / 0.2f ) * Interfaces::m_pGlobalVars->frametime )
				//	: ( ( GUI::ctx->animation - ( 1.0f / 0.2f ) * Interfaces::m_pGlobalVars->frametime ) );

				//if ( !g_Vars.global.menuOpen )
				//	GUI::ctx->ColorPickerInfo.HashedID = 0;

				GUI::ctx->animation = std::clamp<float> ( GUI::ctx->animation, 0.f, 1.0f );

				Menu::Draw ( );
			}
			Render::DirectX::end ( );
		}

		d3d9::dx9_device->SetFVF ( old_fvf );
		d3d9::dx9_device->EndScene ( );
		d3d9::dx9_device->Present ( nullptr, nullptr, nullptr, nullptr );
	}

	void directx_init ( HWND hwnd )
	{
		using namespace d3d9;
		auto d3d_ = Direct3DCreate9 ( D3D_SDK_VERSION );
		D3DPRESENT_PARAMETERS d3dpp;

		ZeroMemory ( &d3dpp, sizeof ( d3dpp ) );
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hwnd;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferWidth = screen_width;
		d3dpp.BackBufferHeight = screen_height;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		d3d_->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dx9_device );

		D3DXCreateLine ( dx9_device, &dx9_line );
		D3DXCreateFontA ( dx9_device, 12, 0, 400, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, "Verdana", &verdana_13 );
		D3DXCreateFontA ( dx9_device, 13, 0, 700, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY, DEFAULT_PITCH, "Segoe UI", &tahoma_11 );
		D3DXCreateFontA ( dx9_device, 8, 0, 400, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY, DEFAULT_PITCH, "Small Fonts", &small_fonts_9 );

		Render::DirectX::init ( d3d9::dx9_device );
	}

	LRESULT CALLBACK wnd_proc ( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
	{
		switch ( Message )
		{
		case WM_PAINT:
			render ( );
			break;

		case WM_CREATE:
			DwmExtendFrameIntoClientArea ( hWnd, &d3d9::margin );
			break;

		case WM_DESTROY:
			PostQuitMessage ( 1 );
			return 0;

		default:
			return DefWindowProc ( hWnd, Message, wParam, lParam );
			break;
		}

		return 0;
	}

	void set_overlay_position ( )
	{
		using namespace d3d9;
		while ( true )
		{
			Sleep ( 5 );

			RECT rect;
			GetWindowRect ( game_window, &rect );
			SetWindowPos ( overlay_hwnd, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
			SetWindowPos ( overlay_hwnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER );

			if ( GetForegroundWindow ( ) == game_window )
			{
				ShowWindow ( overlay_hwnd, SW_RESTORE );
				SetWindowPos ( overlay_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
				SetWindowPos ( overlay_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );

				SetWindowPos ( game_window, overlay_hwnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
				SendMessage ( overlay_hwnd, WM_PAINT, 0, 0 );
			}
			else
			{
				ShowWindow ( overlay_hwnd, SW_HIDE );
			}
		}
	}

	void initialize ( )
	{
		using namespace d3d9;

		while ( !( game_window = LI_FN ( FindWindowW )( _ ( L"UnityWndClass" ), nullptr ) ) )
			sleep_for ( 1000ms );

		HWND hDesktop = LI_FN ( GetDesktopWindow ) ( );
		RECT resolution;
		LI_FN ( GetWindowRect ) ( hDesktop, &resolution );

		monitor::width = resolution.right;
		monitor::height = resolution.bottom;

		logging::print ( TYPE_LOG, _ ( "Found game!" ) );

		std::thread pos ( set_overlay_position );
		pos.detach ( );

		auto window_name = _ ( "BiGlH" );
		WNDCLASSEX wnd_class;
		ZeroMemory ( &wnd_class, sizeof ( WNDCLASSEX ) );
		wnd_class.cbSize = sizeof ( wnd_class );
		wnd_class.lpszClassName = window_name;
		wnd_class.lpfnWndProc = wnd_proc;
		RegisterClassEx ( &wnd_class );

		RECT client;
		GetClientRect ( game_window, &client );
		GetWindowRect ( game_window, &window_rect );
		screen_width = window_rect.right - window_rect.left;
		screen_height = window_rect.bottom - window_rect.top;
		overlay_hwnd = CreateWindowEx ( NULL,
			window_name,
			window_name,
			WS_POPUP | WS_VISIBLE,
			window_rect.left, window_rect.top, screen_width, screen_height,
			NULL,
			NULL,
			NULL,
			NULL );

		MARGINS margins = { -1 };
		DwmExtendFrameIntoClientArea ( overlay_hwnd, &margins );
		SetWindowLong ( overlay_hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED );

		directx_init ( overlay_hwnd );
		MSG msg;
		while ( true )
		{
			while ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage ( &msg );
				DispatchMessage ( &msg );
			}
			if ( msg.message == WM_QUIT )
				exit ( 0 );

			std::this_thread::sleep_for ( std::chrono::milliseconds ( 5 ) );
		}
	}
}