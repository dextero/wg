#ifndef __CSAFEPTR_H__
#define __CSAFEPTR_H__

#include <cstdio>

/**
 * Liosan, 22.05.09
 * Inteligentny, bezpieczny wskaznik, ktory przyjmuje wartosc NULL po zniszczeniu obiektu
 */

namespace Memory{

// pieprzony C++, po cholere to? :/ no, ale inaczej sie nie kompiluje... :/
template<class T> class CSafePtrable;

template<class T>
class CSafePtr{
private:
	friend class CSafePtrable<T>;
    T *obj;
    mutable CSafePtr<T> *next;
	mutable CSafePtr<T> *prev;
	explicit CSafePtr(){
	}
    explicit CSafePtr(T *obj): obj(obj){
		next = this;
		prev = this;
    }
	void Attach(const CSafePtr<T> &other){
        obj = other.obj;
		next = other.next;
		prev = ((CSafePtr<T>*)(&other));
		prev->next = this;
		next->prev = this;
    }
	inline void Detach(){
		next->prev = prev;
		prev->next = next;
		prev = this;
		next = this;
	}
	inline void NullAll(){
		obj = NULL;
		CSafePtr<T> *n = next;
		Detach();
		if (n != this)
			n->NullAll();
	}
public:
    CSafePtr(const CSafePtr<T> &other){
        Attach(other);
    }
    ~CSafePtr(){
		Detach();
    }

    inline T& operator*() const{
        return *obj;
    }

    inline T *operator->() const{
        return obj;
    }

    inline bool operator==(const CSafePtr<T> &other) const {
        return obj == other.obj;
    }

	inline bool operator==(T* other) const{
		return obj == other;
	}
    
    inline bool operator!=(const CSafePtr<T> &other) const{
        return obj != other.obj;
    }

	inline bool operator!=(T* other) const {
		return obj != other;
	}
	
	inline CSafePtr<T> &operator=(const CSafePtr<T> &other){
		Detach();
		Attach(other);
		return *this;
    }

	inline void DeleteObject(){
		delete obj;
	}

	inline static CSafePtr<T> NullPtr(){
		return CSafePtr((T*)NULL);
	}
};

}

#endif //__CSAFEPTR_H__

