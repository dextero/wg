#ifndef _EQUIPMENT_H_
#define _EQUIPMENT_H_

#include <string>
#include <vector>
#include "../Utils/CSingleton.h"

#define gEquipment CEquipment::GetSingleton()

class CEquipment : public CSingleton< CEquipment >
{
public:
	struct SQuestItem
	{
		std::wstring id;
		std::wstring name;
	};

	CEquipment();
	~CEquipment();

	void AddItem( const std::wstring& id, const std::wstring& name );
	void RemoveItem( const std::wstring& id );
	bool ContainsItem( const std::wstring& id );

private:
	std::vector<SQuestItem> mItems;
};

#endif

