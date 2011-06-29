#ifndef __SAFE_FUNCTIONS_H__
#define __SAFE_FUNCTIONS_H__
#include <cstddef>
#include <vector>

template <class T>
void SafeVectorRelease(std::vector<T> * vect){
	if(vect ==NULL) return;
    typename std::vector<T>::iterator vit = vect->begin();
	while(vit!=vect->end()){
		T el = *vit;
		if(el!=NULL){
			delete el;
			el = NULL;
			vit++;
		}
	}

	vect->clear();
	delete vect;
}

#endif

