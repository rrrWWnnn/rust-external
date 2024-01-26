#include "communication.hpp"

namespace mist
{
	std::unique_ptr< CSharedMemComms > esp;
	std::unique_ptr< CSharedMemComms > aim;
	std::unique_ptr< CSharedMemComms > obj;
	std::unique_ptr< CSharedMemComms > misc;
	std::unique_ptr< CSharedMemComms > other;
	std::unique_ptr< CSharedMemComms > wep;
	std::unique_ptr< CSharedMemComms > omnisprint;
};