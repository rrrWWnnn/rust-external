#include "rendering.hpp"
#include <array>

namespace rendering
{
	D3DCOLOR color_to_d3dcolor2 ( Color color )
	{
		return D3DCOLOR_RGBA ( color.r ( ), color.g ( ), color.b ( ), color.a ( ) );
	}

	void c_renderer::draw_string ( float x, float y, LPD3DXFONT pFont, Color color, int flags, bool outline, bool dropshadow, std::string text, ... )
	{
		va_list va_alist;
		char buf [1024];
		va_start ( va_alist, text );
		_vsnprintf ( buf, sizeof ( buf ), text.c_str ( ), va_alist );
		va_end ( va_alist );

		RECT rect;
		if ( outline )
		{
			const auto outline_color { Color ( 0, 0, 0, color.a ( ) ) };

			////SetRect ( &rect, x, y, x, y );
			////draw_filled_rect ( rect.left, rect.top, get_text_width ( pFont, buf ), 13, color_to_d3dcolor2 ( outline_color ) );
			//// 
			SetRect ( &rect, x, y - 1, x, y - 1 );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );
			SetRect ( &rect, x, y + 1, x, y + 1 );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );

			SetRect ( &rect, x - 1, y, x - 1, y );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );
			SetRect ( &rect, x + 1, y, x + 1, y );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );

			SetRect ( &rect, x + 1, y + 1, x + 1, y + 1 );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );
			SetRect ( &rect, x - 1, y + 1, x - 1, y + 1 );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );

			SetRect ( &rect, x + 1, y - 1, x + 1, y - 1 );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );
			SetRect ( &rect, x - 1, y - 1, x - 1, y - 1 );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );
		}

		if ( dropshadow )
		{
			const auto outline_color { Color ( 0, 0, 0, color.a ( ) ) };

			SetRect ( &rect, x + 1, y + 1, x + 1, y + 1 );
			pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( outline_color ) );
		}

		SetRect ( &rect, x, y, x, y );
		pFont->DrawTextA ( NULL, buf, -1, &rect, flags | DT_NOCLIP, color_to_d3dcolor2 ( color ) );
	}

	int c_renderer::get_text_width ( LPD3DXFONT pFont, std::string text, ... )
	{
		va_list va_alist;
		char buf [1024];
		va_start ( va_alist, text );
		_vsnprintf ( buf, sizeof ( buf ), text.c_str ( ), va_alist );
		va_end ( va_alist );

		RECT rect = { 0,0,0,0 };

		if ( pFont )
		{
			pFont->DrawTextA ( NULL, buf, strlen ( buf ), &rect, DT_CALCRECT,
				D3DCOLOR_XRGB ( 0, 0, 0 ) );
		}

		return rect.right - rect.left;
	}

	void c_renderer::draw_filled_rect ( float x, float y, float w, float h, D3DCOLOR col )
	{
		D3DXVECTOR2 vLine [2];
		d3d9::dx9_line->SetWidth ( w );
		vLine [0].x = x + w / 2;
		vLine [0].y = y;
		vLine [1].x = x + w / 2;
		vLine [1].y = y + h;

		d3d9::dx9_line->Begin ( );
		d3d9::dx9_line->Draw ( vLine, 2, col );
		d3d9::dx9_line->End ( );
	}

	void c_renderer::draw_line ( float x, float y, float xx, float yy, D3DCOLOR col )
	{
		D3DXVECTOR2 line [2];
		d3d9::dx9_line->SetWidth ( 1 );
		line [0] = D3DXVECTOR2 ( x, y );
		line [1] = D3DXVECTOR2 ( xx, yy );
		d3d9::dx9_line->Draw ( line, 2, col );
	}

	void c_renderer::draw_rect ( float x, float y, float width, float height, D3DCOLOR col )
	{
		D3DXVECTOR2 points [5];
		points [0] = D3DXVECTOR2 ( x, y );
		points [1] = D3DXVECTOR2 ( x + width, y );
		points [2] = D3DXVECTOR2 ( x + width, y + height );
		points [3] = D3DXVECTOR2 ( x, y + height );
		points [4] = D3DXVECTOR2 ( x, y );
		d3d9::dx9_line->SetWidth ( 1 );
		d3d9::dx9_line->Draw ( points, 5, col );
	}

	void c_renderer::draw_rect ( float x, float y, float width, float height, int thickness, D3DCOLOR col )
	{
		D3DXVECTOR2 points [5];
		points [0] = D3DXVECTOR2 ( x, y );
		points [1] = D3DXVECTOR2 ( x + width, y );
		points [2] = D3DXVECTOR2 ( x + width, y + height );
		points [3] = D3DXVECTOR2 ( x, y + height );
		points [4] = D3DXVECTOR2 ( x, y );
		d3d9::dx9_line->SetWidth ( thickness );
		d3d9::dx9_line->Draw ( points, 5, col );
	}

	void c_renderer::draw_circle ( float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, D3DCOLOR col )
	{
		std::vector<vertex> circle ( resolution + 2 );
		float angle = rotate * D3DX_PI / 180;
		float pi;

		//if ( type == full ) pi = D3DX_PI;        // Full circle
		//if ( type == half ) pi = D3DX_PI / 2;      // 1/2 circle
		//if ( type == quarter ) pi = D3DX_PI / 4;   // 1/4 circle

		pi = D3DX_PI;

		for ( int i = 0; i < resolution + 2; i++ )
		{
			circle [i].x = ( float ) ( x - radius * cos ( i * ( 2 * pi / resolution ) ) );
			circle [i].y = ( float ) ( y - radius * sin ( i * ( 2 * pi / resolution ) ) );
			circle [i].z = 0;
			circle [i].rhw = 1;
			circle [i].color = col;
		}

		// Rotate matrix
		int _res = resolution + 2;
		for ( int i = 0; i < _res; i++ )
		{
			circle [i].x = x + cos ( angle ) * ( circle [i].x - x ) - sin ( angle ) * ( circle [i].y - y );
			circle [i].y = y + sin ( angle ) * ( circle [i].x - x ) + cos ( angle ) * ( circle [i].y - y );
		}

		LPDIRECT3DVERTEXBUFFER9 g_pVB;

		d3d9::dx9_device->CreateVertexBuffer ( ( resolution + 2 ) * sizeof ( vertex ), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL );

		VOID* pVertices;
		g_pVB->Lock ( 0, ( resolution + 2 ) * sizeof ( vertex ), ( void** ) &pVertices, 0 );
		memcpy ( pVertices, &circle [0], ( resolution + 2 ) * sizeof ( vertex ) );
		g_pVB->Unlock ( );


		d3d9::dx9_device->SetTexture ( 0, NULL );
		d3d9::dx9_device->SetPixelShader ( NULL );
		if ( smoothing )
		{
			d3d9::dx9_device->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
			d3d9::dx9_device->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, TRUE );
		}
		d3d9::dx9_device->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		d3d9::dx9_device->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3d9::dx9_device->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		d3d9::dx9_device->SetStreamSource ( 0, g_pVB, 0, sizeof ( vertex ) );
		d3d9::dx9_device->SetFVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

		d3d9::dx9_device->DrawPrimitive ( D3DPT_LINESTRIP, 0, resolution );
		if ( g_pVB != NULL ) g_pVB->Release ( );
	}

	void c_renderer::circle_filled ( float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, D3DCOLOR col )
	{
		std::vector<vertex> circle ( resolution + 2 );
		float angle = rotate * D3DX_PI / 180;
		float pi;

		//if ( type == full ) pi = D3DX_PI;        // Full circle
		//if ( type == half ) pi = D3DX_PI / 2;      // 1/2 circle
		//if ( type == quarter ) pi = D3DX_PI / 4;   // 1/4 circle

		pi = D3DX_PI;

		for ( int i = 0; i < resolution + 2; i++ )
		{
			circle [i].x = ( float ) ( x - radius * cos ( i * ( 2 * pi / resolution ) ) );
			circle [i].y = ( float ) ( y - radius * sin ( i * ( 2 * pi / resolution ) ) );
			circle [i].z = 0;
			circle [i].rhw = 1;
			circle [i].color = col;
		}

		// Rotate matrix
		int _res = resolution + 2;
		for ( int i = 0; i < _res; i++ )
		{
			circle [i].x = x + cos ( angle ) * ( circle [i].x - x ) - sin ( angle ) * ( circle [i].y - y );
			circle [i].y = y + sin ( angle ) * ( circle [i].x - x ) + cos ( angle ) * ( circle [i].y - y );
		}

		LPDIRECT3DVERTEXBUFFER9 g_pVB;

		d3d9::dx9_device->CreateVertexBuffer ( ( resolution + 2 ) * sizeof ( vertex ), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL );

		VOID* pVertices;
		g_pVB->Lock ( 0, ( resolution + 2 ) * sizeof ( vertex ), ( void** ) &pVertices, 0 );
		memcpy ( pVertices, &circle [0], ( resolution + 2 ) * sizeof ( vertex ) );
		g_pVB->Unlock ( );


		d3d9::dx9_device->SetTexture ( 0, NULL );
		d3d9::dx9_device->SetPixelShader ( NULL );
		if ( smoothing )
		{
			d3d9::dx9_device->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
			d3d9::dx9_device->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, TRUE );
		}
		d3d9::dx9_device->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		d3d9::dx9_device->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3d9::dx9_device->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		d3d9::dx9_device->SetStreamSource ( 0, g_pVB, 0, sizeof ( vertex ) );
		d3d9::dx9_device->SetFVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

		d3d9::dx9_device->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0, resolution );
		if ( g_pVB != NULL ) g_pVB->Release ( );
	}

	void c_renderer::draw_gradient ( float x, float y, float width, float height, D3DCOLOR col, D3DCOLOR col2, gr_orientation orientation )
	{
		static struct D3DVERTEX { float x, y, z, rhw; DWORD color; } vertices [4] = { {0,0,0,1.0f,0},{0,0,0,1.0f,0},{0,0,0,1.0f,0},{0,0,0,1.0f,0} };
		vertices [0].x = x;
		vertices [0].y = y;
		vertices [0].color = col;

		vertices [1].x = x + width;
		vertices [1].y = y;
		vertices [1].color = orientation == horizontal ? col2 : col;

		vertices [2].x = x;
		vertices [2].y = y + height;
		vertices [2].color = orientation == horizontal ? col : col2;

		vertices [3].x = x + width;
		vertices [3].y = y + height;
		vertices [3].color = col2;


		static LPDIRECT3DVERTEXBUFFER9 pVertexObject = NULL;
		static void* pVertexBuffer = NULL;

		if ( !pVertexObject )
		{
			if ( FAILED ( d3d9::dx9_device->CreateVertexBuffer ( sizeof ( vertices ), 0,
				D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &pVertexObject, NULL ) ) )
				return;
		}
		if ( FAILED ( pVertexObject->Lock ( 0, sizeof ( vertices ), &pVertexBuffer, 0 ) ) )
			return;

		memcpy ( pVertexBuffer, vertices, sizeof ( vertices ) );
		pVertexObject->Unlock ( );

		d3d9::dx9_device->SetStreamSource ( 0, pVertexObject, 0, sizeof ( D3DVERTEX ) );
		d3d9::dx9_device->SetFVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
		d3d9::dx9_device->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, 2 );
	}

	Vector3 c_renderer::rotate_point ( Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck )
	{
		float r_1, r_2;
		float x_1, y_1;

		r_1 = -( EntityPos.y - LocalPlayerPos.y );
		r_2 = EntityPos.x - LocalPlayerPos.x;
		float Yaw = angle - 90.0f;

		float yawToRadian = Yaw * ( float ) ( M_PI / 180.0F );
		x_1 = ( float ) ( r_2 * ( float ) cos ( ( double ) ( yawToRadian ) ) - r_1 * sin ( ( double ) ( yawToRadian ) ) ) / 20;
		y_1 = ( float ) ( r_2 * ( float ) sin ( ( double ) ( yawToRadian ) ) + r_1 * cos ( ( double ) ( yawToRadian ) ) ) / 20;

		*viewCheck = y_1 < 0;

		x_1 *= zoom;
		y_1 *= zoom;

		int sizX = sizeX / 2;
		int sizY = sizeY / 2;

		x_1 += sizX;
		y_1 += sizY;

		if ( x_1 < 5 )
			x_1 = 5;

		if ( x_1 > sizeX - 5 )
			x_1 = sizeX - 5;

		if ( y_1 < 5 )
			y_1 = 5;

		if ( y_1 > sizeY - 5 )
			y_1 = sizeY - 5;

		x_1 += posX;
		y_1 += posY;

		return Vector3 ( x_1, y_1, 0 );
	}

	void c_renderer::vector_angles_radar ( Vector3 forward, Vector3& angles )
	{
		if ( forward.x == 0.f && forward.y == 0.f )
		{
			angles.x = forward.z > 0.f ? -90.f : 90.f;
			angles.y = 0.f;
		}
		else
		{
			D3DXVECTOR3 lol = { forward.x, forward.x, forward.z };

			angles.x = RAD2DEG ( atan2 ( -forward.z, D3DXVec3Length ( &lol ) ) );
			angles.y = RAD2DEG ( atan2 ( forward.y, forward.x ) );
		}

		angles.z = 0.f;
	}

	void c_renderer::rotate_triangle ( std::array<Vector3, 3>& points, float rotation )
	{
		const auto points_center = ( points.at ( 0 ) + points.at ( 1 ) + points.at ( 2 ) ) / 3;
		for ( auto& point : points )
		{
			point = point - points_center;

			const auto temp_x = point.x;
			const auto temp_y = point.y;

			const auto theta = DEG2RAD ( rotation );
			const auto c = cosf ( theta );
			const auto s = sinf ( theta );

			point.x = temp_x * c - temp_y * s;
			point.y = temp_x * s + temp_y * c;

			point = point + points_center;
		}
	}

	void c_renderer::triangle ( Vector2 pos1, Vector2 pos2, Vector2 pos3, Color c )
	{
		vertex verts [4] = {
			{ int ( pos1.x ), int ( pos1.y ), 0.01f, 0.01f, color_to_d3dcolor2 ( c ) },
			{ int ( pos2.x ), int ( pos2.y ), 0.01f, 0.01f, color_to_d3dcolor2 ( c ) },
			{ int ( pos3.x ), int ( pos3.y ), 0.01f, 0.01f, color_to_d3dcolor2 ( c ) },
			{ int ( pos1.x ), int ( pos1.y ), 0.01f, 0.01f, color_to_d3dcolor2 ( c ) },
		};

		d3d9::dx9_device->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
		d3d9::dx9_device->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, TRUE );
		d3d9::dx9_device->SetTexture ( 0, nullptr );
		d3d9::dx9_device->DrawPrimitiveUP ( D3DPT_LINESTRIP, 3, &verts, 20 );
		d3d9::dx9_device->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
		d3d9::dx9_device->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, FALSE );
	}

	void c_renderer::triangle_filled ( Vector3 pos1, Vector3 pos2, Vector3 pos3, Color c )
	{
		vertex verts [3] = {
			{ int ( pos1.x ), int ( pos1.y ), 0.01f, 0.01f, color_to_d3dcolor2 ( c ) },
			{ int ( pos2.x ), int ( pos2.y ), 0.01f, 0.01f, color_to_d3dcolor2 ( c ) },
			{ int ( pos3.x ), int ( pos3.y ), 0.01f, 0.01f, color_to_d3dcolor2 ( c ) },
		};

		d3d9::dx9_device->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
		d3d9::dx9_device->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, TRUE );
		d3d9::dx9_device->SetTexture ( 0, nullptr );
		d3d9::dx9_device->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 1, &verts, 20 );
		d3d9::dx9_device->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
		d3d9::dx9_device->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, FALSE );
	}

	void c_renderer::arc ( Vector2 center, float radius, float multiplier, Color c, bool antialias )
	{
		const auto d3d_col = color_to_d3dcolor2 ( c );
		const float pi = D3DX_PI * multiplier;
		const auto step_angle = pi / 30;
		const auto segments = 64;
		const auto verts = ( vertex* ) _malloca ( sizeof ( vertex ) * ( segments + 2 ) );

		if ( verts == nullptr )
			return;

		for ( auto i = 0; i < segments + 2; i++ )
		{
			verts [i].x = ( center.x - radius * std::cos ( i * ( 2 * pi / segments ) ) );
			verts [i].y = ( center.y - radius * std::sin ( i * ( 2 * pi / segments ) ) );
			verts [i].z = 0;
			verts [i].rhw = 1;
			verts [i].color = d3d_col;
		}

		for ( auto i = 0; i < segments + 2; i++ )
		{
			verts [i].x = center.x + std::cos ( step_angle ) * ( verts [i].x - center.x ) - std::sin ( step_angle ) * ( verts [i].y - center.y );
			verts [i].y = center.y + std::sin ( step_angle ) * ( verts [i].x - center.x ) + std::cos ( step_angle ) * ( verts [i].y - center.y );
		}

		d3d9::dx9_device->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, antialias );
		d3d9::dx9_device->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, antialias );

		d3d9::dx9_device->SetTexture ( 0, nullptr );
		d3d9::dx9_device->DrawPrimitiveUP ( D3DPT_LINESTRIP, segments, verts, 20 );

		d3d9::dx9_device->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
		d3d9::dx9_device->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, FALSE );
	}
}