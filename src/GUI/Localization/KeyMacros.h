#ifndef _KEYMACROS_H_
#define _KEYMACROS_H_

#include "../../Utils/StringUtils.h"
typedef unsigned long long hash_t;

#define KEY(x) const hash_t x = StringUtils::GetStringHash(#x);

#endif// _KEYMACROS_H_

