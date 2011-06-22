#include "CCallbackDispatcher.h"

CCallbackDispatcher::CCallbackDispatcher(CPhysical *attached):
mAttached(attached){}

void CCallbackDispatcher::RegisterEventDeath(EventCallbackFunc func, void *callee, void *data){
	SRegisteredFunc rf;
	rf.callee = callee;
	rf.func = func;
	rf.data = data;
	mCallbacks.push_back(rf);
}

void CCallbackDispatcher::UnregisterCallee(void *callee){
	std::vector<SRegisteredFunc>::iterator tmp = mCallbacks.end();
	for (std::vector<SRegisteredFunc>::iterator it = mCallbacks.begin(); it!=mCallbacks.end(); it++){
		if (tmp != mCallbacks.end()){
			mCallbacks.erase(tmp);
			tmp = mCallbacks.end();
		}
		if ((*it).callee == callee)	tmp = it;
	}
	if (tmp != mCallbacks.end()){
		mCallbacks.erase(tmp);
		tmp = mCallbacks.end();
	}
}

void CCallbackDispatcher::ExecuteCallback(SRegisteredFunc &func){
    //fprintf(stderr,"Calling %p\n",func.callee);
	func.func(mAttached,func.callee,func.data);
}

void CCallbackDispatcher::Dispatch(){
	for (std::vector<SRegisteredFunc>::iterator it = mCallbacks.begin(); it!=mCallbacks.end(); it++)
		ExecuteCallback(*it);
}
