#pragma once

#include "D2BS.h"

class CriticalRoom
{
private:
	bool bEnteredCriticalSection;

public:
	CriticalRoom() : bEnteredCriticalSection(false) {}
	~CriticalRoom() { LeaveSection(); }

	void EnterSection() {
		InterlockedIncrement(&Vars.SectionCount); 
		EnterCriticalSection(&Vars.cGameLoopSection);		
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			bEnteredCriticalSection = false;
			LeaveCriticalSection(&Vars.cGameLoopSection);
			InterlockedDecrement(&Vars.SectionCount);			
		}
	}
};

class CriticalMisc
{
private:
	bool bEnteredCriticalSection;

public:
	CriticalMisc() : bEnteredCriticalSection(false) {}
	~CriticalMisc()  { LeaveSection(); }

	void EnterSection() {
		InterlockedIncrement(&Vars.SectionCount);
		EnterCriticalSection(&Vars.cGameLoopSection);		
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			LeaveCriticalSection(&Vars.cGameLoopSection);
			InterlockedDecrement(&Vars.SectionCount);
			bEnteredCriticalSection = false;
		}
	}
};
