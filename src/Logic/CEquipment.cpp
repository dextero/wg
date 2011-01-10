#include "CEquipment.h"

template<> CEquipment* CSingleton< CEquipment >::msSingleton = 0;

CEquipment::CEquipment()
{
}

CEquipment::~CEquipment()
{
}

void CEquipment::AddItem(const std::wstring &id, const std::wstring &name)
{
	SQuestItem item;
	item.id = id;
	item.name = name;
	mItems.push_back( item );
}

void CEquipment::RemoveItem(const std::wstring &id)
{
	for ( unsigned i = 0; i < mItems.size(); i++ )
	{
		if ( mItems[i].id == id )
		{
			mItems.erase( mItems.begin() + i );
			return;
		}
	}
}

bool CEquipment::ContainsItem(const std::wstring &id)
{
	for ( unsigned i = 0; i < mItems.size(); i++ )
		if ( mItems[i].id == id )
		return true;

	return false;
}

