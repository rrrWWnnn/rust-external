#pragma once
#include "../../stdafx.hpp"
#include "../../math.hpp"

namespace InputHelper {
	inline bool KeyState[ 256 ];
	inline bool PrevKeyState[ 256 ];

	inline int Scroll;

	inline Vector2 MousePos;
	inline Vector2 MouseDelta;

	void Update( );
	bool IsHoveringOverPoint ( Vector2 Position, Vector2 Area );
	Vector2 GetMousePos ( );
	bool Pressed( int key );
	bool Down( int key );
	bool Released( int key );
	void GetScrollDelta ( );
}