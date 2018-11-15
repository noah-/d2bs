#include "JSExits.h"

EMPTY_CTOR(exit)

void exit_finalize(JSFreeOp *fop, JSObject *obj)
{
	myExit* pExit = (myExit*)JS_GetPrivate(obj);
	delete pExit;
}

JSAPI_PROP(exit_getProperty)
{
	myExit* pExit = (myExit*)JS_GetPrivate(cx, obj);

	vp.set(JSVAL_VOID);

	if(!pExit)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx, id, &ID);
	switch(JSVAL_TO_INT(ID))
	{
		case EXIT_X:
			vp.setInt32(pExit->x);
			break;
		case EXIT_Y:
			vp.setInt32(pExit->y);
			break;
		case EXIT_TARGET:
			vp.setInt32(pExit->id);
			break;
		case EXIT_TYPE:
			vp.setInt32(pExit->type);
			break;
		case EXIT_TILEID:
			vp.setInt32(pExit->tileid);
			break;
		case EXIT_LEVELID:
			vp.setInt32(pExit->level);
			break;
		default:
			break;
	}

	return JS_TRUE;
}

