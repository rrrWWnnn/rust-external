#pragma once

struct mono_string
{
	char buffer [128];
};

namespace unity
{
	Vector3 get_position_injected ( CSharedMemComms* drv, const uintptr_t transform );
	uintptr_t get_component ( CSharedMemComms* drv, uintptr_t gameObject, const char* componentNameStr );
};