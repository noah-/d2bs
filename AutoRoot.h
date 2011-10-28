#pragma once

#ifndef __AUTOROOT_H__
#define __AUTOROOT_H__

#include "js32.h"

class AutoRoot
{
private:
	jsval var;
	int count;

	AutoRoot(const AutoRoot&);
	AutoRoot& operator=(const AutoRoot&);
public:
	AutoRoot() : var(JSVAL_NULL), count(0) {}
	AutoRoot(jsval var);
	~AutoRoot();
	void Take();
	void Release();
	jsval* value() { return &var; }
	jsval operator* () { return *value(); }
	bool operator==(AutoRoot& other) { return value() == other.value(); }
};

#endif
