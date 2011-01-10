#ifndef __CSAFEPTRABLE_H__
#define __CSAFEPTRABLE_H__

/**
 * Liosan, 22.05.10
 * Klasa wspierajaca bycie celem safe-pointera, ktory sie NULLuje jak obiekt zostanie zniszczony
 */

#include "CSafePtr.h"

namespace Memory{

template<class T>
class CSafePtrable{
private:
	CSafePtr<T> any;
public:
	CSafePtrable():any((T*)this){
	}
	virtual ~CSafePtrable(){
		any.NullAll();
	}

	// to MA byc kopia - bez &
	inline CSafePtr<T> GetSafePointer() const {
		return any;
	}
};
}

#endif // __CSAFEPTRABLE_H__

