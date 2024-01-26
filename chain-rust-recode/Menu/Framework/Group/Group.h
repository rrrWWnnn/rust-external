#pragma once

namespace GUI::Group {
	void BeginGroup( const std::string& name, const Vector2& size );
	void EndGroup( );

	void SetNextSize( const Vector2& size );
	void PopLastSize( );
}
