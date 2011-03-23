// klasa do przechowywania obiektow w plecaku bohatera

#include "CItem.h"

CItem::CItem() {};

CItem::~CItem() {};

const std::string & CItem::GetAbility() {
    return mAbility;
}

void CItem::SetAbility(const std::string & ability) {
    mAbility = ability;
}

