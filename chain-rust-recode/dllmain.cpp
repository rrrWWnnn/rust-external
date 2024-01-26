#include "stdafx.hpp"
#include "overlay.hpp"
#include "security.hpp"
#include "bytes.hpp"

static auto model_state = ( ModelState* ) mist::omnisprint->read<uintptr_t> ( reinterpret_cast< uintptr_t >( game::m_pLocalplayer ) + offsets::modelState );
static auto flags = mist::omnisprint->read<int> ( reinterpret_cast< uintptr_t >( game::m_pLocalplayer ) + offsets::flags );
static uintptr_t movement = mist::omnisprint->read<uintptr_t> ( ( uintptr_t ) game::m_pLocalplayer + offsets::basemovement );
void omnisprint ( )
{
	//if ( !g_Vars.misc.omnisprint )
	//	return;

	while ( true )
	{
		//mist::omnisprint->write<int> ( reinterpret_cast< uintptr_t >( model_state ) + offsets::flags, flags |= ( int ) 16 );
		mist::omnisprint->write<float> ( movement + 0x40, 1.f );
	}
}

void omnisprint2 ( )
{
	//if ( !g_Vars.misc.omnisprint )
	//	return;

	while ( true )
	{
		game::m_pLocalplayer->omni_sprint ( mist::omnisprint.get ( ) );
	}
}

#ifdef _WINDLL
DWORD WINAPI MainThread ( PVOID )
#else
int main ( )
#endif
{
	Beep ( 315, 200 );
	SetPriorityClass ( GetCurrentProcess ( ), REALTIME_PRIORITY_CLASS );

	//if ( !g_Protection.initial_safety_check ( ) )
	//{
	//	LI_FN ( exit )( 69 );
	//}

	//g_Protection.Run ( );

	//logging::print ( TYPE_LOG, _ ( "Press enter to continue..." ) );

	//std::cin.get ( );

	//logging::print ( TYPE_LOG, _ ( "Initializing communications." ) );

	mist::other->initialize ( _ ( "RustClient.exe" ) );
	mist::aim->initialize ( _ ( "RustClient.exe" ) );
	mist::esp->initialize ( _ ( "RustClient.exe" ) );
	mist::misc->initialize ( _ ( "RustClient.exe" ) );
	mist::obj->initialize ( _ ( "RustClient.exe" ) );
	mist::wep->initialize ( _ ( "RustClient.exe" ) );
	mist::omnisprint->initialize ( _ ( "RustClient.exe" ) );

	//logging::print ( TYPE_DEBUG, _ ( "Cheat loaded! Waiting for game.." ) );

	std::thread mousewheel ( InputHelper::GetScrollDelta );
	mousewheel.detach ( );

	std::thread overlay ( overlay::initialize );
	overlay.detach ( );

	object::setup ( );

	std::thread objectloop ( object::main );
	objectloop.detach ( );

	std::thread hit ( aimbot::hit_sound );
	hit.detach ( );

	//logging::print ( TYPE_DEBUG, _ ( "Waiting for player to enter game.." ) );
	while ( game::m_rLocalInfo.m_pPlayer == 0 )
		sleep_for ( milliseconds ( 200 ) );

	//logging::print ( TYPE_DEBUG, _ ( "Player entered game!" ) );
	std::thread aimbot ( aimbot::thread );
	aimbot.detach ( );

	std::thread misc ( misc::thread );
	misc.detach ( );

	//std::thread omni1 ( omnisprint );
	//omni1.detach ( );

	//std::thread omni22 ( omnisprint2 );
	//omni22.detach ( );

	//logging::print ( TYPE_DEBUG, "Have fun!" );

	while ( true )
	{
		if ( !LI_FN ( FindWindowW )( _ ( L"UnityWndClass" ), nullptr ) )
		{
			LI_FN ( exit )( 0 );
		}

		sleep_for ( 400ms );
	}
}

#ifdef _WINDLL
BOOL WINAPI DllMain ( HMODULE hDll, DWORD dwReason, PVOID )
{
	if ( dwReason == DLL_PROCESS_ATTACH )
		CreateThread ( 0, NULL, MainThread, 0, NULL, NULL );

	return true;
}
#endif