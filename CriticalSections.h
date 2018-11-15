#pragma once

#include "D2BS.h"

class CriticalRoom
{
private:
	bool bEnteredCriticalSection;

public:
	CriticalRoom() : bEnteredCriticalSection(false) {}
	~CriticalRoom() {
		LeaveSection(); 
	}

	void EnterSection() {
		InterlockedIncrement(&Vars.SectionCount); 
		EnterCriticalSection(&Vars.cGameLoopSection);		
		bEnteredCriticalSection = true;
		InterlockedDecrement(&Vars.SectionCount);	
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			bEnteredCriticalSection = false;
			LeaveCriticalSection(&Vars.cGameLoopSection);
					
		}
	}
};

class AutoCriticalRoom
{
private:
	bool bEnteredCriticalSection;
	void  EnterSection() {
		InterlockedIncrement(&Vars.SectionCount); 
		bEnteredCriticalSection = true;
		EnterCriticalSection(&Vars.cGameLoopSection);		
		InterlockedDecrement(&Vars.SectionCount);	
	}
	
	void LeaveSection() {
		if(bEnteredCriticalSection) {
			bEnteredCriticalSection = false;
			LeaveCriticalSection(&Vars.cGameLoopSection);
					
		}
	}

public:
	AutoCriticalRoom() : bEnteredCriticalSection(false) {EnterSection();}
	~AutoCriticalRoom() {LeaveSection(); }

	

	
};