#pragma once
#include "xorstr.hpp"
#include <Windows.h>
#include <Winternl.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <string>

enum protection_status_t
{
	STATUS_SAFE,
	STATUS_UNSAFE_SYSTEM,

	STATUS_WINAPI_DEBUGGER,
	STATUS_WINAPI_REMOTE_DEBUGGER,
	STATUS_PEB_DEBUGGER,
	STATUS_THREAD_DEBUGGER,
	STATUS_THREAD_REGISTER_DEBUGGER,
	STATUS_KUSER_KERNEL_DEBUGGER,

	STATUS_PROGRAM_NOSNAPSHOT,
	STATUS_PROGRAM_EBUG,
	STATUS_PROGRAM_DBG,

	STATUS_DRIVER_NOENUM,
	STATUS_DRIVER_SANDBOXIE,
	STATUS_DRIVER_WINPCAP,
	STATUS_DRIVER_PROCESS_HACKER,
	STATUS_DRIVER_CHEAT_ENGINE,
};

static constexpr uintptr_t KUSER_SHARED_DATA = 0x7FFE0000;

#include "lazy_importer.hpp"

class CProtection
{
	__forceinline bool is_debugger_running ( )
	{
		// [junk-start]
		if ( LI_FN ( IsDebuggerPresent )( ) )
		{
			error_string = _ ( "STATUS_WINAPI_DEBUGGER" );
			return true;
		}

		BOOL bIsDbgPresent = false;
		LI_FN ( CheckRemoteDebuggerPresent )( LI_FN ( GetCurrentProcess )( ), &bIsDbgPresent );

		if ( bIsDbgPresent )
		{
			error_string = _ ( "STATUS_WINAPI_REMOTE_DEBUGGER" );
			return true;
		}

		//PBOOLEAN BeingDebugged = ( PBOOLEAN ) __readgsqword ( 0x60 ) + 2;
		//if ( BeingDebugged )
		//{
		//	error_string = _ ( "STATUS_PEB_DEBUGGER" );
		//	return true;
		//}

		static PCONTEXT ctx = PCONTEXT ( LI_FN ( VirtualAlloc )( nullptr, sizeof ( ctx ), MEM_COMMIT, PAGE_READWRITE ) );
		SecureZeroMemory ( ctx, sizeof ( CONTEXT ) );
		ctx->ContextFlags = CONTEXT_DEBUG_REGISTERS;

		if ( LI_FN ( GetThreadContext )( LI_FN ( GetCurrentThread )( ), ctx ) == 0 )
		{
			error_string = _ ( "STATUS_THREAD_DEBUGGER" );
			return true;
		}

		if ( ctx->Dr0 != 0 || ctx->Dr1 != 0 || ctx->Dr2 != 0 || ctx->Dr3 != 0 )
		{
			error_string = _ ( "STATUS_THREAD_REGISTER_DEBUGGER" );
			return true;
		}

		if ( *( BOOLEAN* ) ( KUSER_SHARED_DATA + 0x02d4 ) )
		{
			error_string = _ ( "STATUS_KUSER_KERNEL_DEBUGGER" );
			return true;
		}

		return false;
		// [junk-end]
	}

	__forceinline bool is_running_forbidden_program ( )
	{
		// [junk-start]
		std::string szProcesses[] = {
			_ ( "ebug" ),
			_ ( "dbg" ),
			_ ( "idaq" ),
			_ ( "regmon" ),
			_ ( "nalyzer" ),
			_ ( "ireshark" ),
			_ ( "HookExplorer" ),
			_ ( "PETools" ),
			_ ( "LordPE" ),
			_ ( "SysInspector" ),
			_ ( "sniff_" ),
			_ ( "procmon" ),
			_ ( "dump" ),
			_ ( "procexp" ),
			_ ( "olly" ),
			_ ( "tcpview" ),
			_ ( "autoruns" ),
			_ ( "ImportREC" ),
			_ ( "joebox" ),
			_ ( "acker" ),
			_ ( "filemon" ),
			_ ( "hunter" ), //pc hunter
			_ ( "atengi" ), //cheat engine
			_ ( "parsecd" ),
		};

		WORD iLength = sizeof ( szProcesses ) / sizeof ( szProcesses [0] );

		PROCESSENTRY32 pe32;
		SecureZeroMemory ( &pe32, sizeof ( PROCESSENTRY32 ) );

		// We want a snapshot of processes
		HANDLE hSnapshot = LI_FN ( CreateToolhelp32Snapshot )( TH32CS_SNAPPROCESS, 0 );

		// Check for a valid handle, in this case we need to check for
		// INVALID_HANDLE_VALUE instead of NULL
		if ( hSnapshot == INVALID_HANDLE_VALUE )
		{
			error_string = _ ( "PROGRAM_NOSNAPSHOT" );
			return true;
		}

		// Now we can enumerate the running process, also 
		// we can't forget to set the PROCESSENTRY32.dwSize member
		// otherwise the following functions will fail
		pe32.dwSize = sizeof ( PROCESSENTRY32 );

		if ( !LI_FN ( Process32First )( hSnapshot, &pe32 ) )
		{
			LI_FN ( CloseHandle )( hSnapshot );
			error_string = _ ( "PROGRAM_NOSNAPSHOT" );
			return true;
		}

		do
		{
			for ( int i = 0; i < iLength; i++ )
			{
				if ( strstr ( pe32.szExeFile, _ ( "tracker" ) ) || strstr ( pe32.szExeFile, _ ( "Tracker" ) ) )
					continue;

				if ( strstr ( pe32.szExeFile, szProcesses [i].c_str ( ) ) )
				{
					LI_FN ( CloseHandle )( hSnapshot );

					error_string = _ ( "PROGRAM_" );
					error_string += pe32.szExeFile;
					return true;
				}
			}
		} while ( LI_FN ( Process32Next )( hSnapshot, &pe32 ) );

		LI_FN ( CloseHandle )( hSnapshot );
		return false;
		// [junk-end]
	}

	__forceinline bool is_running_forbidden_driver ( )
	{
		// [junk-start]
		std::string szDrivers[] = {
			_ ( "Sbie" ), //sandboxie
			_ ( "acker" ), //process hacker
			_ ( "CEDRI" ), //cheatengine
		};

		WORD iLength = sizeof ( szDrivers ) / sizeof ( szDrivers [0] );

		LPVOID drivers [1024];
		DWORD cbNeeded;

		using GetDeviceDriverBaseNameA_t = int ( __stdcall* )( LPVOID, LPSTR, DWORD );
		static auto GetDeviceDriverBaseNameA_fn = ( GetDeviceDriverBaseNameA_t ) LI_FN ( GetProcAddress )( GetModuleHandleA ( _ ( "kernel32.dll" ) ), "K32GetDeviceDriverBaseNameA" );

		if ( LI_FN ( EnumDeviceDrivers )( drivers, sizeof ( drivers ), &cbNeeded ) && cbNeeded < sizeof ( drivers ) )
		{
			TCHAR szDriver [1024];
			int cDrivers = cbNeeded / sizeof ( drivers [0] );
			for ( int i = 0; i < cDrivers; i++ )
			{
				if ( GetDeviceDriverBaseNameA_fn ( drivers [i], szDriver, sizeof ( szDriver ) / sizeof ( szDriver [0] ) ) )
				{
					if ( strstr ( szDriver, _ ( "tracker" ) ) || strstr ( szDriver, _ ( "Tracker" ) ) )
						continue;

					for ( int j = 0; j < iLength; j++ )
					{
						if ( strstr ( szDriver, szDrivers [j].c_str ( ) ) )
						{
							error_string = _ ( "DRIVER_" );
							error_string += szDriver;
							return true;
						}
					}
				}
			}
		}
		else
		{
			error_string = _ ( "DRIVER_NOENUM" );
			return true;
		}

		return false;
		// [junk-end]
	}
public:
	std::string error_string;
	char* random_data;

	__declspec( noinline ) bool initial_safety_check ( )
	{
		// [junk-start]
		//we ~~dont~~ do ban for these, instead we warn user and exit

		//ida
		{
			PROCESSENTRY32 pe32;
			SecureZeroMemory ( &pe32, sizeof ( PROCESSENTRY32 ) );

			// We want a snapshot of processes
			HANDLE hSnapshot = LI_FN ( CreateToolhelp32Snapshot )( TH32CS_SNAPPROCESS, 0 );

			if ( hSnapshot == INVALID_HANDLE_VALUE )
			{
				error_string = _ ( "UNSPECIFIED SYSTEM ERROR 0x1337" );
				return false;
			}

			pe32.dwSize = sizeof ( PROCESSENTRY32 );

			if ( !LI_FN ( Process32First )( hSnapshot, &pe32 ) )
			{
				LI_FN ( CloseHandle )( hSnapshot );
				error_string = _ ( "UNSPECIFIED SYSTEM ERROR 0x420" );
				return false;
			}

			do
			{
				if ( strstr ( pe32.szExeFile, _ ( "idaq" ) ) )
				{
					LI_FN ( CloseHandle )( hSnapshot );
					error_string = _ ( "PLEASE CLOSE IDA" );
					return false;
				}
			} while ( LI_FN ( Process32Next )( hSnapshot, &pe32 ) );

			LI_FN ( CloseHandle )( hSnapshot );
		}

		return true;
		// [junk-end]
	}

	__forceinline bool safety_check ( )
	{
		// [junk-start]
		if ( is_debugger_running ( ) )
			return false;

		if ( is_running_forbidden_program ( ) )
			return false;

		// bad idea almost everyone has these installed
		//if ( is_running_forbidden_driver ( ) )
		//	return false;

		return true;
		// [junk-end]
	}

	void Run ( );

};

extern CProtection g_Protection;