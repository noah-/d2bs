#include "AutoRoot.h"
#include "ScriptEngine.h"

AutoRoot::AutoRoot(jsval nvar) : var(nvar), count(0) { Take(); }
AutoRoot::~AutoRoot() {
	if(count < 0)
	{
		fprintf(stderr, "AutoRoot failed: Count is still %i, but the root is being destroyed", count);
		DebugBreak();
		exit(3);
	}
	JS_RemoveRoot(&var);
}

void AutoRoot::Take() 
{ 
	count++;
	JS_AddNamedRootRT(ScriptEngine::GetRuntime(), &var, "AutoRoot");
}

void AutoRoot::Release()
{
	count--;
	if(count < 0)
	{
		fprintf(stderr, "Improper AutoRoot usage: Count is less than 0");
		DebugBreak();
		exit(3);
	}
}
