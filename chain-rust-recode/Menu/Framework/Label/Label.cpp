#include "../gui.h"

bool GUI::Controls::Label( const std::string& name, bool unsafe ) {
	Vector2 CursorPos = PopCursorPos( );
	Vector2 DrawPos = ctx->pos + CursorPos;

	DrawPos.x += 20;
	Render::DirectX::Fonts::menu.draw_text( GUI::SplitStr( name, '#' )[ 0 ].data( ), DrawPos - Vector2( 0, 5 ), unsafe ? Color( 180, 180, 100 ) : Color( 200, 200, 200 ), 4 );

	GUI::PushCursorPos( CursorPos + Vector2( 0, 8 + GUI::ObjectPadding( ) ) );
	return ( name.size( ) > 0 );
}
