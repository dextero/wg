// klasa do przechowywania obiektow w plecaku bohatera

#include "CItem.h"

CItem::CItem(): mLevel(1) {};

CItem::~CItem() {};

const std::string & CItem::GetAbility() {
    return mAbility;
}

void CItem::SetAbility(const std::string & ability) {
    mAbility = ability;
}

