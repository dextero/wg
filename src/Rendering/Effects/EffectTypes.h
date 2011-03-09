#ifndef __CEFFECTTYPES_H__
#define __CEFFECTTYPES_H__

#include "../../Utils/StringUtils.h"

typedef boost::uint64_t EffectHandle;
#define CreateEffectHandle( name ) StringUtils::GetStringHash( name  )

#endif //__CEFFECTTYPES_H__//
