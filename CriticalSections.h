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
		EnterCriticalSection(&Vars.cGameLoopSection);
		InterlockedIncrement(&Vars.SectionCount);
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

class CriticalMisc
{
private:
	bool bEnteredCriticalSection;

public:
	CriticalMisc() : bEnteredCriticalSection(false) {}
	~CriticalMisc()  { LeaveSection(); }

	void EnterSection() {
		EnterCriticalSection(&Vars.cGameLoopSection);
		InterlockedIncrement( &Vars.SectionCount );
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			LeaveCriticalSection(&Vars.cGameLoopSection);
			InterlockedDecrement( &Vars.SectionCount );
			bEnteredCriticalSection = false;
		}
	}
};
