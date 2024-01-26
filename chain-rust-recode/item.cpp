#include "item.hpp"
#include "math.hpp"

int Item::get_amount ( CSharedMemComms* drv )
{
	return drv->read<int> ( reinterpret_cast< uintptr_t >( this ) + 0x30 );
}

int Item::get_uid ( CSharedMemComms* drv )
{
	return drv->read<int> ( reinterpret_cast< uintptr_t >( this ) + 0x28 );
}

int Item::get_iid ( CSharedMemComms* drv )
{
	uintptr_t itemInfo = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + 0x20 );
	return drv->read<int> ( itemInfo + 0x18 );
}

bool Item::is_weapon ( CSharedMemComms* drv )
{
	const auto item_definition = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + 0x20 );
	if ( !item_definition )
		return false;

	return drv->read<uint32_t> ( item_definition + 0x40 ) == 0;
}

uintptr_t Item::get_base_projectile ( CSharedMemComms* drv )
{
	return drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::base_projectile );
}

std::string Item::get_name ( CSharedMemComms* drv )
{
	uintptr_t itemInfo = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + 0x20 ); // ItemDefinition info
	uintptr_t shortname = drv->read<uintptr_t> ( itemInfo + offsets::shortname ); // shortname
	std::string result = drv->read_unicode ( shortname + 0x14, 32 );

	if ( !result.empty ( ) )
		return result;

	return _ ( "none" );
}

std::string Item::get_nice_name ( CSharedMemComms* drv )
{
	uintptr_t m_pItemDefinition = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + 0x20 );
	uintptr_t m_pTranslation = drv->read<uintptr_t> ( m_pItemDefinition + 0x28 );
	uintptr_t m_pName = drv->read<uintptr_t> ( m_pTranslation + 0x18 );
	std::string m_szTranslation = drv->read_unicode ( m_pName + 0x14, 32 );

	if ( !m_szTranslation.empty ( ) )
		return m_szTranslation;

	return _ ( "" );
}

std::string Item::get_bullet_name ( CSharedMemComms* drv )
{
	uintptr_t base_projectile = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::base_projectile );
	uintptr_t primary_magazine = drv->read<uintptr_t> ( base_projectile + offsets::magazine );
	uintptr_t ammo_type = drv->read<uintptr_t> ( primary_magazine + offsets::ammotype );
	uintptr_t shortname = drv->read<uintptr_t> ( ammo_type + offsets::shortname ); // shortname

	std::string ammo_name = drv->read_unicode ( shortname + 0x14, 32 );
	return ammo_name;
}

int Item::get_bullet_amount ( CSharedMemComms* drv )
{
	uintptr_t base_projectile = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::base_projectile );
	uintptr_t primary_magazine = drv->read<uintptr_t> ( base_projectile + offsets::magazine );
	return drv->read<int> ( primary_magazine + 0x18 );
}

int Item::get_bullet_max ( CSharedMemComms* drv )
{
	uintptr_t base_projectile = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + offsets::base_projectile );
	uintptr_t primary_magazine = drv->read<uintptr_t> ( base_projectile + offsets::magazine );
	return drv->read<int> ( primary_magazine + 0x1C );
}

Item* ItemContainer::get_item ( CSharedMemComms* drv, int id )
{
	uintptr_t itemList = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + 0x38 ); // List<Item> itemList
	uintptr_t items = drv->read<uintptr_t> ( itemList + 0x10 );

	uintptr_t requestedItem = drv->read<uintptr_t> ( items + 0x20 + id * 0x08 );
	return ( Item* ) requestedItem;
}

ItemContainer* Inventory::get_belt ( CSharedMemComms* drv )
{
	uintptr_t belt = drv->read<uintptr_t> ( reinterpret_cast< uintptr_t >( this ) + 0x28 );
	return ( ItemContainer* ) belt;
}
