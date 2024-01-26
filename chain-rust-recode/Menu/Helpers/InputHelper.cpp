#include "InputHelper.h"
#include "../../stdafx.hpp"

#include <Windows.h>

void InputHelper::Update( ) {
	if( GetForegroundWindow( ) == d3d9::game_window ) {
		for( int i = 0; i < 256; i++ ) {
			PrevKeyState[ i ] = KeyState[ i ];
			KeyState[ i ] = GetAsyncKeyState( i );
		}
	}

	static HWND window;
	if( !window )
		window = FindWindowA( _ ( "UnityWndClass" ), 0 );

	POINT mouse;
	GetCursorPos( &mouse );
	ScreenToClient( window, &mouse );

	MouseDelta = MousePos - Vector2( mouse.x, mouse.y );

	MousePos = Vector2( mouse.x, mouse.y );
}

bool InputHelper::IsHoveringOverPoint ( Vector2 Position, Vector2 Area )
{
	return MousePos.x > Position.x
		&& MousePos.y > Position.y
		&& MousePos.x < Position.x + Area.x
		&& MousePos.y < Position.y + Area.y;
}

Vector2 InputHelper::GetMousePos ( )
{
	return MousePos;
}

bool InputHelper::Down( int key ) {
	return KeyState[ key ] && PrevKeyState[ key ];
}

bool InputHelper::Pressed( int key ) {
	return KeyState[ key ] && !PrevKeyState[ key ];
}

bool InputHelper::Released( int key ) {
	return !KeyState[ key ] && PrevKeyState[ key ];
}

HHOOK mh;
int delta = 0;

LRESULT CALLBACK Mouse ( int nCode, WPARAM wParam, LPARAM lParam )
{
	if ( nCode < 0 )
	{
		return CallNextHookEx ( mh, nCode, wParam, lParam );
	}

	MSLLHOOKSTRUCT* pMouseStruct = ( MSLLHOOKSTRUCT* ) lParam;

	if ( pMouseStruct != NULL )
	{
		if ( wParam == WM_MOUSEWHEEL )
		{
			if ( HIWORD ( pMouseStruct->mouseData ) == 120 )
			{
				delta = 1;
			}
			else
			{
				delta = -1;
			}
		}
		else
		{
			delta = 0;
		}
	}

	return CallNextHookEx ( mh, nCode, wParam, lParam );
}

void InputHelper::GetScrollDelta ( )
{
	while ( true )
	{
		delta = 0;
		if ( !( mh = SetWindowsHookExA ( WH_MOUSE_LL, Mouse, NULL, 0 ) ) )
		{
			delta = -404;
		}

		MSG message;
		bool peek = true;
		long tm = time ( 0 );

		while ( peek )
		{
			PeekMessage ( &message, NULL, 0, 0, PM_REMOVE );
			if ( !delta == 0 || tm < time ( 0 ) )
			{
				peek = false;
			}
		}

		UnhookWindowsHookEx ( mh );

		g_Vars.global.scrollbar = delta;

		sleep_for ( 1ms );
	}
}