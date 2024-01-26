#pragma once
#include <string>
#include <sstream>

#include "../stdafx.hpp"
#include "../math.hpp"
#include "../color.hpp"

#include "Wrapper/font/font.h"
#include "Wrapper/sprite/sprite.h"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

using TextureID = void*;
using ColorU32 = uint32_t;
using Rect2D = Vector4;
using FontHandle = std::size_t;

struct IDirect3DDevice9;

enum text_flags : int {
	CENTER_X = ( 1 << 0 ),
	CENTER_Y = ( 1 << 1 ),
	ALIGN_RIGHT = ( 1 << 2 ),
	ALIGN_BOTTOM = ( 1 << 3 ),
	DROP_SHADOW = ( 1 << 4 ),
	OUTLINED = ( 1 << 5 ),
};

enum : uint32_t {
	FONT_VERDANA = 0,
	FONT_MENU_BOLD = FONT_VERDANA,
	FONT_MENU = FONT_VERDANA,
	FONT_CSGO_ICONS,
	FONT_VERDANA_30_BOLD,
	FONT_VERDANA_25_REGULAR,
	FONT_VISITOR,
	FONT_PORTER,
	FONT_CSGO_ICONS2,
	FONT_VERDANA_40_BOLD,
};

class c_sprite;
namespace Render {
	namespace DirectX {
		extern IDirect3DDevice9* device;
		extern IDirect3DStateBlock9* state_block;
		extern IDirect3DVertexDeclaration9* vert_dec;
		extern IDirect3DVertexShader9* vert_shader;
		extern IDirect3DVertexBuffer9* vert_buf;
		extern std::vector<font*> font_arr;

		namespace Fonts {
			inline font menu( _ ( "Verdana" ), 12, 500, true );
			inline font menu_small( _ ( "Small Fonts" ), 9, 500 );
			inline font menu_pixel( _ ( "Small Fonts" ), 9, 500 );
			inline font menu_bold( _ ( "Verdana" ), 12, 700, true );
			inline font menu_icon( _ ( "fonteditor" ), 50, 500, true );
		}

		namespace Textures {
			inline c_sprite* background = new c_sprite( );
			inline c_sprite* logo = new c_sprite( );
		}

		extern bool initialized;

		void set_render_states( );

		void init( IDirect3DDevice9* p_device );

		void invalidate( );

		void restore( IDirect3DDevice9* p_device );

		void set_viewport( D3DVIEWPORT9 vp );
		D3DVIEWPORT9 get_viewport( );

		void set_scissor_rect( D3DVIEWPORT9 vp );
		D3DVIEWPORT9 get_scissor_rect( );
		void reset_scissor_rect( );

		// drawing functions

		void rect( Vector2 pos, Vector2 size, Color c );
		void rect_fill( Vector2 pos, Vector2 size, Color c );;

		void triangle( Vector2 pos1, Vector2 pos2, Vector2 pos3, Color c );
		void triangle_filled( Vector2 pos1, Vector2 pos2, Vector2 pos3, Color c );

		void polygon_gradient( Vector2 pos1, Vector2 pos2, Vector2 pos3, Vector2 pos4, Color c_a, Color c_b );

		void gradient_v( Vector2 pos, Vector2 size, Color c_a, Color c_b );
		void gradient_h( Vector2 pos, Vector2 size, Color c_a, Color c_b );

		void gradient_multi_color_filled( Vector2 pos, Vector2 size, Color c_a, Color c_b, Color c_c, Color c_d );
		void gradient_multi_color( Vector2 pos, Vector2 size, Color c_a, Color c_b, Color c_c, Color c_d );

		void line( Vector2 a, Vector2 b, Color c );

		void circle_filled( Vector2 center, float radius, int rotate, int type, bool smoothing, int resolution, Color c );
		void arc( Vector2 center, float radius, float multiplier, Color c, bool antialias = true );
		void circle_gradient( Vector2 center, float radius, int rotate, int type, int resolution, Color c, Color c1 );

		void begin( );
		void end( );
	}

	Vector2 GetScreenSize( );
};

