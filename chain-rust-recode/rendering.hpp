#pragma once
#include "stdafx.hpp"
#include "color.hpp"
#include "math.hpp"

enum gr_orientation
{
	horizontal,
	vertical
};

struct vertex
{
	float x, y, z, rhw;
	D3DCOLOR color;
};

struct Vertex_t
{
	Vertex_t ( ) { }
	Vertex_t ( const Vector2& pos, const Vector2& coord = Vector2 ( 0, 0 ) )
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init ( const Vector2& pos, const Vector2& coord = Vector2 ( 0, 0 ) )
	{
		m_Position = pos;
		m_TexCoord = coord;
	}

	Vector2 m_Position;
	Vector2 m_TexCoord;
};

namespace rendering
{
	class c_renderer
	{
	public:
		static c_renderer* get ( )
		{
			static c_renderer* instance = new c_renderer;
			return instance;
		}

		void draw_string ( float x, float y, LPD3DXFONT pFont, Color color, int flags, bool outline, bool dropshadow, std::string text, ... );

		int get_text_width ( LPD3DXFONT pFont, std::string text, ... );

		void draw_filled_rect ( float x, float y, float w, float h, D3DCOLOR col );

		void draw_line ( float x, float y, float xx, float yy, D3DCOLOR col );

		void draw_rect ( float x, float y, float width, float height, D3DCOLOR col );

		void draw_rect ( float x, float y, float width, float height, int thickness, D3DCOLOR col );

		void draw_circle ( float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, D3DCOLOR col );

		void circle_filled ( float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, D3DCOLOR col );

		void draw_gradient ( float x, float y, float width, float height, D3DCOLOR col, D3DCOLOR col2, gr_orientation orientation );

		Vector3 rotate_point ( Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck );

		void vector_angles_radar ( Vector3 forward, Vector3& angles );

		void rotate_triangle ( std::array<Vector3, 3>& points, float rotation );

		void triangle ( Vector2 pos1, Vector2 pos2, Vector2 pos3, Color c );

		void triangle_filled ( Vector3 pos1, Vector3 pos2, Vector3 pos3, Color c );

		void arc ( Vector2 center, float radius, float multiplier, Color c, bool antialias );
	};
}