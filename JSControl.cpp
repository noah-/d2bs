//#include "Control.h"
#include "JSControl.h"
#include "Helpers.h"
#include "D2Helpers.h"

EMPTY_CTOR(control)

void control_finalize(JSContext *cx, JSObject *obj)
{
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(pData)
	{
		JS_SetPrivate(cx, obj, NULL);
		delete pData;
	}
}

JSAPI_PROP(control_getProperty)
{
	if(ClientState() != ClientStateMenu)
		return JS_FALSE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));
	if(!pData)
		return JS_FALSE;

	Control* ctrl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!ctrl)
		return JS_FALSE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	JS_BeginRequest(cx);
	switch(JSVAL_TO_INT(ID))
	{
		case CONTROL_TEXT:
			if(ctrl->dwIsCloaked != 33)
			{
				char* tmp = UnicodeToAnsi((ctrl->dwType == 6 ? ctrl->wText2 : ctrl->wText));
				*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
				delete[] tmp;
			}
			break;
		case CONTROL_X:
			JS_NewNumberValue(cx, ctrl->dwPosX, vp);
			break;
		case CONTROL_Y:
			JS_NewNumberValue(cx, ctrl->dwPosY, vp);
			break;
		case CONTROL_XSIZE:
			JS_NewNumberValue(cx, ctrl->dwSizeX, vp);
			break;
		case CONTROL_YSIZE:
			JS_NewNumberValue(cx, ctrl->dwSizeY, vp);
			break;
		case CONTROL_STATE:
			JS_NewNumberValue(cx, (ctrl->dwDisabled - 2), vp);
			break;
		case CONTROL_MAXLENGTH:
			//JS_NewNumberValue(cx, ctrl->dwMaxLength, vp);
			break;
		case CONTROL_TYPE:
			JS_NewNumberValue(cx, ctrl->dwType, vp);
			break;
		case CONTROL_VISIBLE:
			// nothing to do yet because we don't know what to do
			break;
		case CONTROL_CURSORPOS:
			JS_NewNumberValue(cx, ctrl->dwCursorPos, vp);
			break;
		case CONTROL_SELECTSTART:
			JS_NewNumberValue(cx, ctrl->dwSelectStart, vp);
			break;
		case CONTROL_SELECTEND:
			JS_NewNumberValue(cx, ctrl->dwSelectEnd, vp);
			break;
		case CONTROL_PASSWORD:
			*vp = BOOLEAN_TO_JSVAL(!!(ctrl->dwIsCloaked == 33));
			break;
		case CONTROL_DISABLED:
			JS_NewNumberValue(cx, ctrl->dwDisabled, vp);
			break;
	}
	JS_EndRequest(cx);
	return JS_TRUE;
}

JSAPI_STRICT_PROP(control_setProperty)
{
	if(ClientState() != ClientStateMenu)
		return JS_FALSE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp)));
	if(!pData)
		return JS_FALSE;

	Control* ctrl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!ctrl)
		return JS_FALSE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID))
	{
		case CONTROL_TEXT:
			if(ctrl->dwType == 1 && JSVAL_IS_STRING(*vp))
			{
				char* pText = JS_EncodeString(cx,JS_ValueToString(cx, *vp));
				if(!pText)
					return JS_TRUE;
				wchar_t* szwText = AnsiToUnicode(pText);
				D2WIN_SetControlText(ctrl, szwText);
				delete[] szwText;
				JS_free(cx, pText);
			}
			break;
		case CONTROL_STATE:
			if(JSVAL_IS_INT(*vp))
			{
				int32 nState;
				JS_BeginRequest(cx);
				if(!JS_ValueToECMAInt32(cx, *vp, &nState) || nState < 0 || nState > 3)
				{
					JS_EndRequest(cx);
					THROW_ERROR(cx, "Invalid state value");
				}
				memset((void*)&ctrl->dwDisabled, (nState + 2), sizeof(DWORD));
				JS_EndRequest(cx);
			}
			break;
		case CONTROL_CURSORPOS:
			if(JSVAL_IS_INT(*vp))
			{
				JS_BeginRequest(cx);
				uint32 dwPos;
				if(!JS_ValueToECMAUint32(cx, *vp, &dwPos))
				{
					JS_EndRequest(cx);
					THROW_ERROR(cx, "Invalid cursor position value");
				}
				JS_EndRequest(cx);
				memset((void*)&ctrl->dwCursorPos, dwPos, sizeof(DWORD));
			}
			break;
		case CONTROL_DISABLED:
			if(JSVAL_IS_INT(*vp))
			{
				memset((void*)&ctrl->dwDisabled, JSVAL_TO_INT(*vp), sizeof(DWORD));
			}
			break;
	}

	return JS_TRUE;
}

JSAPI_FUNC(control_getNext)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp)));
	if(!pData)
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(pControl && pControl->pNext)
		pControl = pControl->pNext;
	else
		pControl = NULL;

	if(pControl)
	{
		pData->pControl = pControl;
		pData->dwType = pData->pControl->dwType;
		pData->dwX = pData->pControl->dwPosX;
		pData->dwY = pData->pControl->dwPosY;
		pData->dwSizeX = pData->pControl->dwSizeX;
		pData->dwSizeY = pData->pControl->dwSizeY;
		JS_SetPrivate(cx, JS_THIS_OBJECT(cx, vp), pData);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_THIS_OBJECT(cx, vp)));
	}
	else
	{
		JSObject* obj = JS_THIS_OBJECT(cx, vp);
		JS_ClearScope(cx, obj);
		if(JS_ValueToObject(cx, JSVAL_NULL, &obj) == JS_FALSE)
			return JS_TRUE;
		JS_SET_RVAL(cx, vp, JSVAL_FALSE); 
	}
	
	return JS_TRUE;
}

JSAPI_FUNC(control_click)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp)));
	if(!pData)
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!pControl)
	{
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(0)); 
		return JS_TRUE;
	}

	uint32 x = (uint32)-1, y = (uint32)-1;

	if(argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
	{
		JS_BeginRequest(cx);
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], &x);
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], &y);
		JS_EndRequest(cx);
	}

	clickControl(pControl, x, y);

	return JS_TRUE;
}

JSAPI_FUNC(control_setText)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp)));
	if(!pData)
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!pControl)
	{
		JS_SET_RVAL(cx, vp,INT_TO_JSVAL(0));
		return JS_TRUE;
	}
	
	if(argc < 0 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	char* pText = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	if(!pText)
		return JS_TRUE;
	wchar_t* szwText = AnsiToUnicode(pText);
	JS_free(cx, pText);
	D2WIN_SetControlText(pControl, szwText);

	delete[] szwText;
	return JS_TRUE;
}

JSAPI_FUNC(control_getText)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp)));
	if(!pData)
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!pControl)
	{
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(0));
		return JS_TRUE;
	}

	if(pControl->dwType != 4 || !pControl->pFirstText)
		return JS_TRUE;
	JS_BeginRequest(cx);
	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pReturnArray));
	
	int nArrayCount = 0;
	
	for(ControlText* pText = pControl->pFirstText; pText; pText = pText->pNext)
	{
		if(!pText->wText)
			continue;

		char* tmp = UnicodeToAnsi(pText->wText);
		jsval aString = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, tmp));
		delete[] tmp;
		JS_SetElement(cx, pReturnArray, nArrayCount, &aString); 

		nArrayCount++;
	}
	JS_EndRequest(cx);
	return JS_TRUE;
}


JSAPI_FUNC(my_getControl)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	int32 nType = -1, nX = -1, nY = -1, nXSize = -1, nYSize = -1;
	int32 *args[] = {&nType, &nX, &nY, &nXSize, &nYSize};
	JS_BeginRequest(cx);
	for(uintN i = 0; i < argc; i++)
		if(JSVAL_IS_INT(JS_ARGV(cx, vp)[i]))
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[i], args[i]);
	JS_EndRequest(cx);

	Control* pControl = findControl(nType, (char*)NULL, -1, nX, nY, nXSize, nYSize);
	if(!pControl)
		return JS_TRUE;

	ControlData* data = new ControlData;
	data->dwType = pControl->dwType;
	data->dwX = pControl->dwPosX;
	data->dwY = pControl->dwPosY;
	data->dwSizeX = pControl->dwSizeX;
	data->dwSizeY = pControl->dwSizeY;
	data->pControl =pControl;
	
	JSObject* control = BuildObject(cx, &control_class, control_funcs, control_props, data);
	if(!control)
		THROW_ERROR(cx, "Failed to build control!");

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(control));
	return JS_TRUE;
}
