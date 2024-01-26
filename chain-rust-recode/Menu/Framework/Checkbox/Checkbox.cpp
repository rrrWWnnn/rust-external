#include "../gui.h"
#include "../../../stdafx.hpp"

std::unordered_map<size_t, float> uAnimation;

bool GUI::Controls::Checkbox( const std::string& name, bool* value, bool unsafe ) {
	Vector2 CursorPos = PopCursorPos( );
	Vector2 DrawPos = ctx->pos + CursorPos;
	Vector2 DrawSize = Vector2( 8, 8 );

	Render::DirectX::rect( DrawPos, DrawSize - 1, Color( 10, 10, 10 ) );

	Render::DirectX::Fonts::menu.draw_text( GUI::SplitStr( name, '#' )[ 0 ].data( ), DrawPos + Vector2( 20, -3 ), unsafe ? Color( 180, 180, 100 ) : Color( 200, 200, 200 ), 0 );

	bool hovered = InputHelper::IsHoveringOverPoint( DrawPos, DrawSize - 1 ) && InputHelper::IsHoveringOverPoint( ctx->ParentPos, ctx->ParentSize );
	bool hovered_text = InputHelper::IsHoveringOverPoint( DrawPos + Vector2( 0, -3 ), Render::DirectX::Fonts::menu.measure_size( GUI::SplitStr( name, '#' )[ 0 ].data( ) ) + Vector2( 20, 0 ) ) && InputHelper::IsHoveringOverPoint( ctx->ParentPos, ctx->ParentSize );

	if( !( *value ) ) {
		if( hovered || hovered_text )
			Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 2, Color( 83, 83, 83 ), Color( 58, 58, 58 ) );
		else
			Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 2, Color( 75, 75, 75 ), Color( 51, 51, 51 ) );
	}
	else {
		Render::DirectX::gradient_v ( DrawPos + 1, DrawSize - 2, g_Vars.menu.menu_color.ToRegularColor( ), g_Vars.menu.menu_color.ToRegularColor ( ) * 0.75f );
	}

	if( ctx->FocusedID == 0 ) {
		if( ( hovered || hovered_text ) && InputHelper::Pressed( VK_LBUTTON ) )
			ctx->FocusedID = GUI::Hash( name );
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( !InputHelper::Down( VK_LBUTTON ) ) {
			ctx->FocusedID = 0;

			if( ( hovered || hovered_text ) )
				*value ^= 1;
		}
	}

	GUI::PushCursorPos( CursorPos + Vector2( 0, DrawSize.y + GUI::ObjectPadding( ) ) );
	return *value || ctx->setup;
}
