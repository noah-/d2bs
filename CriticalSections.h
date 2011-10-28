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
			InterlockedDecrement(&Vars.SectionCount);
			LeaveCriticalSection(&Vars.cGameLoopSection);
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
		InterlockedIncrement( &Vars.SectionCount );
		EnterCriticalSection(&Vars.cGameLoopSection);
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			InterlockedDecrement( &Vars.SectionCount );
			LeaveCriticalSection(&Vars.cGameLoopSection);
			bEnteredCriticalSection = false;
		}
	}
};
