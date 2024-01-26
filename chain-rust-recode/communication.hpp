#pragma once

#include <windows.h>
#include <winternl.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include <codecvt>
#include <TlHelp32.h>
#include <locale>
#include <vector>

#include "xorstr.hpp"

#define _USERMODE 1

typedef struct _NULL_MEMORY
{
	void* buffer_address;
	UINT_PTR address;
	ULONGLONG size;
	ULONG pid;
	BOOLEAN write;
	BOOLEAN read;
	BOOLEAN req_base;
	void* output;
	const char* module_name;
	ULONG64 base_address;
}NULL_MEMORY;

template<typename ... Arg>
uint64_t call_hook ( const Arg ... args )
{
	void* hooked_func = GetProcAddress ( LoadLibrary ( ( LPCSTR ) "win32u.dll" ), "NtGdiDdDDINetDispStopMiracastDisplayDevice" );
	auto func = static_cast< uint64_t ( _stdcall* )( Arg... ) >( hooked_func );

	return func ( args ... );
}

inline std::string utf16_to_utf8 ( std::u16string utf16_string )
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast< const int16_t* >( utf16_string.data ( ) );
	return convert.to_bytes ( p, p + utf16_string.size ( ) );
}

struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()( HANDLE handle ) const
	{
		if ( handle != NULL || handle != INVALID_HANDLE_VALUE )
		{
			CloseHandle ( handle );
		}
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

class CSharedMemComms
{
public:
	inline static std::uint32_t proc = 0;
	inline static HANDLE handle = 0;
public:
	static std::uint32_t get_process_id ( std::string_view process_name )
	{
		PROCESSENTRY32 processentry;
		const unique_handle snapshot_handle ( CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, NULL ) );

		if ( snapshot_handle.get ( ) == INVALID_HANDLE_VALUE )
			return NULL;

		processentry.dwSize = sizeof ( MODULEENTRY32 );

		while ( Process32Next ( snapshot_handle.get ( ), &processentry ) == TRUE )
		{
			if ( process_name.compare ( ( LPSTR ) ( processentry.szExeFile ) ) == NULL )
			{
				return processentry.th32ProcessID;
			}
		}
		return NULL;
	}

	static void initialize ( std::string_view process_name )
	{
		proc = get_process_id ( process_name );

#if _USERMODE == 1
		HANDLE _handle = OpenProcess ( PROCESS_ALL_ACCESS, false, proc );

		if ( _handle == INVALID_HANDLE_VALUE )
			return;

		handle = _handle;
#endif
	}

	static ULONG64 get_module_base_address ( const char* module_name )
	{
#if _USERMODE == 1

		// Get a snapshot of the target process.
		HANDLE snapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ( DWORD ) proc );

		// Check if our snapshot is valid.
		if ( snapshot == INVALID_HANDLE_VALUE )
		{
			return 0;
		}

		// Create our empty moduleentry buffer.
		MODULEENTRY32 mod_entry;
		mod_entry.dwSize = sizeof ( MODULEENTRY32 );

		// Check the first item in our list is valid
		if ( Module32First ( snapshot, &mod_entry ) == FALSE )
		{
			return 0;
		}

		// Iterate through the list until we find our module.
		while ( Module32Next ( snapshot, &mod_entry ) == TRUE )
		{
			if ( strcmp ( mod_entry.szModule, module_name ) == 0 )
			{
				return ( uint64_t ) mod_entry.modBaseAddr;
			}
		}

		// Return 0 if we reach here.
		return 0;

#endif

		NULL_MEMORY instructions = { 0 };
		instructions.pid = proc;
		instructions.req_base = TRUE;
		instructions.read = FALSE;
		instructions.write = FALSE;
		instructions.module_name = module_name;
		call_hook ( &instructions );

		ULONG64 base = NULL;
		base = instructions.base_address;
		return base;
	}

	template <class T>
	static T read ( uintptr_t read_address )
	{
#if _USERMODE == 1

		T buffer;
		read ( read_address, &buffer, sizeof ( buffer ) );
		return buffer;
#endif

		T response {};
		NULL_MEMORY instructions;
		instructions.pid = proc;
		instructions.size = sizeof ( T );
		instructions.address = read_address;
		instructions.read = TRUE;
		instructions.write = FALSE;
		instructions.req_base = FALSE;
		instructions.output = &response;
		call_hook ( &instructions );

		return response;
	}

	static bool write ( UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size )
	{
#if _USERMODE == 1
		return WriteProcessMemory ( handle, ( LPVOID ) write_address, ( LPVOID ) source_address, write_size, 0 );
#endif

		NULL_MEMORY instructions;
		instructions.address = write_address;
		instructions.pid = proc;
		instructions.write = TRUE;
		instructions.read = FALSE;
		instructions.req_base = FALSE;
		instructions.buffer_address = ( void* ) source_address;
		instructions.size = write_size;

		call_hook ( &instructions );

		return true;
	}

	template <typename t>
	static bool read ( UINT_PTR address, t buffer, SIZE_T size )
	{
#if _USERMODE == 1

		// will store the number of bytes read from the target address
		SIZE_T read;

		// perform the read
		if ( ReadProcessMemory ( handle, reinterpret_cast< void* >( address ), buffer, size, &read ) == 0 )
		{
			// read failed, return false
			return false;
		}

		// check read size matches intended read size
		if ( read < size ) return false;

		// read succeeded, all bytes read.
		return true;

#endif

		NULL_MEMORY instructions;
		instructions.read = TRUE;
		instructions.write = FALSE;
		instructions.req_base = FALSE;

		instructions.pid = proc;
		instructions.size = size;
		instructions.address = address;
		instructions.output = buffer;

		call_hook ( &instructions );

		return true;
	}

	template<typename S>
	static bool write ( UINT_PTR write_address, const S& value )
	{
		return write ( write_address, ( UINT_PTR ) &value, sizeof ( S ) );
	}

	static std::string read_ascii ( const uintptr_t address, const size_t size )
	{
		std::unique_ptr<char[]> buffer ( new char [size] );
		if ( !read<char*> ( address, buffer.get ( ), size ) )
			return std::string ( );
		return std::string ( buffer.get ( ) );
	}

	static std::string read_unicode ( const uintptr_t address, const int stringLength )
	{
		char16_t wcharTemp [128] = { '\0' };

		if ( !read<char16_t*> ( address, wcharTemp, stringLength * 2 ) )
			return std::string ( "" );

		std::string u8_conv = utf16_to_utf8 ( wcharTemp );

		return u8_conv;
	}

	static std::wstring wread_unicode ( const uintptr_t address, const int stringLength )
	{
		char16_t wcharTemp [128] = { '\0' };

		if ( !read<char16_t*> ( address, wcharTemp, stringLength * 2 ) )
			return std::wstring ( L"" );

		return std::wstring ( reinterpret_cast< wchar_t* >( wcharTemp ) );
	}

	template<typename Type>
	static Type readchain ( uint64_t address, std::vector<uint64_t> chain )
	{
		uint64_t current = address;
		for ( int i = 0; i < chain.size ( ) - 1; i++ )
		{
			current = read<uint64_t> ( current + chain [i] );
		}
		return read<Type> ( current + chain [chain.size ( ) - 1] );
	}
};

namespace mist
{
	extern std::unique_ptr< CSharedMemComms > esp;
	extern std::unique_ptr< CSharedMemComms > aim;
	extern std::unique_ptr< CSharedMemComms > obj;
	extern std::unique_ptr< CSharedMemComms > misc;
	extern std::unique_ptr< CSharedMemComms > other;
	extern std::unique_ptr< CSharedMemComms > wep;
	extern std::unique_ptr< CSharedMemComms > omnisprint;
};