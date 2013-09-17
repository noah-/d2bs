#include "JSScreenHook.h"
#include "D2BS.h"
#include "ScreenHook.h"
#include "Script.h"
#include "File.h"
using namespace std;

void hook_finalize(JSFreeOp *fop, JSObject *obj)
{
	Genhook* hook = (Genhook*)JS_GetPrivate(obj);

	if(hook)
	{
		JS_SetPrivate(obj, NULL);
		delete hook;
	}
}

JSAPI_FUNC(hook_remove)
{
	JSObject* obj = JS_THIS_OBJECT(cx, vp);
	Genhook::EnterGlobalSection();
	Genhook* hook = (Genhook*)JS_GetPrivate(cx, obj);
	if(hook)
	{
		//hook->SetIsVisible(false);
		delete hook;
	}

	JS_SetPrivate(cx, obj, NULL);
	//JS_ClearScope(cx, obj);
	JS_ValueToObject(cx, JSVAL_VOID, &obj);
	Genhook::LeaveGlobalSection();

	return JS_TRUE;
}

// Function to create a frame which gets called on a "new Frame ()"
// Parameters: x, y, xsize, ysize, alignment, automap, onClick, onHover
JSAPI_FUNC(frame_ctor)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	uint x = 0, y = 0, x2 = 0, y2 = 0;
	Align align = Left;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;

	if(argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		x = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	if(argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		y = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	if(argc > 2 && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
		x2 = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
	if(argc > 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
		y2 = JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);
	if(argc > 4 && JSVAL_IS_INT(JS_ARGV(cx, vp)[4]))
		align = (Align)JSVAL_TO_INT(JS_ARGV(cx, vp)[4]);
	if(argc > 5 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[5]))
		automap = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[5]);
	if(argc > 6 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[6]))
		click = JS_ARGV(cx, vp)[6];
	if(argc > 7 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[7]))
		hover = JS_ARGV(cx, vp)[7];

	JSObject* hook = BuildObject(cx, &frame_class, frame_methods, frame_props);
	if(!hook)
		THROW_ERROR(cx, "Failed to create frame object");

	// framehooks don't work out of game -- they just crash
	FrameHook* pFrameHook = new FrameHook(script, hook, x, y, x2, y2, automap, align, IG);

	if (!pFrameHook)
		THROW_ERROR(cx, "Failed to create framehook");

	JS_SetPrivate(cx, hook, pFrameHook);
	pFrameHook->SetClickHandler(click);
	pFrameHook->SetHoverHandler(hover);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(hook));

	return JS_TRUE;
}

JSAPI_PROP(frame_getProperty)
{
	FrameHook* pFramehook = (FrameHook*)JS_GetPrivate(cx, obj);
	if(!pFramehook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case FRAME_X:
			vp.setInt32(pFramehook->GetX());
			break;
		case FRAME_Y:
			vp.setInt32(pFramehook->GetY());
			break;
		case FRAME_XSIZE:
			vp.setInt32(pFramehook->GetXSize());
			break;
		case FRAME_YSIZE:
			vp.setInt32(pFramehook->GetYSize());
			break;
		case FRAME_ALIGN:
			vp.setInt32(pFramehook->GetAlign());
			break;
		case FRAME_VISIBLE:
			vp.setBoolean(pFramehook->GetIsVisible());
			break;
		case FRAME_ZORDER:
			vp.setInt32(pFramehook->GetZOrder());
			break;
		case FRAME_ONCLICK:
			vp.set(pFramehook->GetClickHandler());
			break;
		case FRAME_ONHOVER:
			vp.set(pFramehook->GetHoverHandler());
			break;
	}
	return JS_TRUE;
}

JSAPI_STRICT_PROP(frame_setProperty)
{
	FrameHook* pFramehook = (FrameHook*)JS_GetPrivate(cx, obj);
	if(!pFramehook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)){
		case FRAME_X:
			if(vp.isInt32())
				pFramehook->SetX(vp.toInt32());
			break;
		case FRAME_Y:
			if(vp.isInt32())
				pFramehook->SetY(vp.toInt32());
			break;
		case FRAME_XSIZE:
			if(vp.isInt32())
				pFramehook->SetXSize(vp.toInt32());
			break;
		case FRAME_YSIZE:
			if(vp.isInt32())
				pFramehook->SetYSize(vp.toInt32());
			break;
		case FRAME_ALIGN:
			if(vp.isInt32())
				pFramehook->SetAlign((Align)vp.toInt32());
			break;
		case FRAME_VISIBLE:
			if(vp.isBoolean())
				pFramehook->SetIsVisible(!!vp.toBoolean());
			break;
		case FRAME_ZORDER:
			if(vp.isInt32())
				pFramehook->SetZOrder((ushort)vp.toInt32());
			break;
		case FRAME_ONCLICK:
			pFramehook->SetClickHandler(vp.get());
			break;
		case FRAME_ONHOVER:
			pFramehook->SetHoverHandler(vp.get());
			break;
	}
	return JS_TRUE;
}

//Box functions

//Parameters: x, y, xsize, ysize, color, opacity, alignment, automap, onClick, onHover
JSAPI_FUNC(box_ctor)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	ScreenhookState state = (script->GetState () == OutOfGame) ? OOG : IG;
	uint x = 0, y = 0, x2 = 0, y2 = 0;
	ushort color = 0, opacity = 0;
	Align align = Left;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;

	if(argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		x = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	if(argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		y = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	if(argc > 2 && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
		x2 = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
	if(argc > 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
		y2 = JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);
	if(argc > 4 && JSVAL_IS_INT(JS_ARGV(cx, vp)[4]))
		color = (ushort)JSVAL_TO_INT(JS_ARGV(cx, vp)[4]);
	if(argc > 5 && JSVAL_IS_INT(JS_ARGV(cx, vp)[5]))
		opacity = (ushort)JSVAL_TO_INT(JS_ARGV(cx, vp)[5]);
	if(argc > 6 && JSVAL_IS_INT(JS_ARGV(cx, vp)[6]))
		align = (Align)JSVAL_TO_INT(JS_ARGV(cx, vp)[6]);
	if(argc > 7 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[7]))
		automap = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[7]);
	if(argc > 8 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[8]))
		click = JS_ARGV(cx, vp)[8];
	if(argc > 9 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[9]))
		hover = JS_ARGV(cx, vp)[9];

	JSObject* hook = BuildObject(cx, &box_class, box_methods, box_props);
	if(!hook)
		THROW_ERROR(cx, "Failed to create box object");

	BoxHook* pBoxHook = new BoxHook(script, hook, x, y, x2, y2, color, opacity, automap, align, state);

	if (!pBoxHook)
		THROW_ERROR(cx, "Unable to initalize a box class.");

	JS_SetPrivate(cx, hook, pBoxHook);
	pBoxHook->SetClickHandler(click);
	pBoxHook->SetHoverHandler(hover);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(hook));

	return JS_TRUE;
}
JSAPI_PROP(box_getProperty)
{
	BoxHook* pBoxHook = (BoxHook*)JS_GetPrivate(cx, obj);
	if(!pBoxHook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case BOX_X:
			vp.setInt32(pBoxHook->GetX());
			break;
		case BOX_Y:
			vp.setInt32(pBoxHook->GetY());
			break;
		case BOX_XSIZE:
			vp.setInt32(pBoxHook->GetXSize());
			break;
		case BOX_YSIZE:
			vp.setInt32(pBoxHook->GetYSize());
			break;
		case BOX_ALIGN:
			vp.setInt32(pBoxHook->GetAlign());
			break;
		case BOX_COLOR:
			vp.setInt32(pBoxHook->GetColor());
			break;
		case BOX_OPACITY:
			vp.setInt32(pBoxHook->GetOpacity());
			break;
		case BOX_VISIBLE:
			vp.setBoolean(pBoxHook->GetIsVisible());
			break;
		case BOX_ZORDER:
			vp.setInt32(pBoxHook->GetZOrder());
			break;
		case BOX_ONCLICK:
			vp.set(pBoxHook->GetClickHandler());
			break;
		case BOX_ONHOVER:
			vp.set(pBoxHook->GetHoverHandler());
			break;
	}
	return JS_TRUE;
}

JSAPI_STRICT_PROP(box_setProperty)
{
	BoxHook* pBoxHook = (BoxHook*)JS_GetPrivate(cx, obj);
	if(!pBoxHook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case BOX_X:
			if(vp.isInt32())
				pBoxHook->SetX(vp.toInt32());
			break;
		case BOX_Y:
			if(vp.isInt32())
				pBoxHook->SetY(vp.toInt32());
			break;
		case BOX_XSIZE:
			if(vp.isInt32())
				pBoxHook->SetXSize(vp.toInt32());
			break;
		case BOX_YSIZE:
			if(vp.isInt32())
				pBoxHook->SetYSize(vp.toInt32());
			break;
		case BOX_OPACITY:
			if(vp.isInt32())
				pBoxHook->SetOpacity((ushort)vp.toInt32());
			break;
		case BOX_COLOR:
			if(vp.isInt32())
				pBoxHook->SetColor((ushort)vp.toInt32());
			break;
		case BOX_ALIGN:
			if(vp.isInt32())
				pBoxHook->SetAlign((Align)vp.toInt32());
			break;
		case BOX_VISIBLE:
			if(vp.isBoolean())
				pBoxHook->SetIsVisible(!!vp.toBoolean());
			break;
		case BOX_ZORDER:
			if(vp.isInt32())
				pBoxHook->SetZOrder((ushort)vp.toInt32());
			break;
		case BOX_ONCLICK:
			pBoxHook->SetClickHandler(vp.get());
			break;
		case BOX_ONHOVER:
			pBoxHook->SetHoverHandler(vp.get());
			break;
	}
	return JS_TRUE;
}


//Line functions

// Parameters: x, y, x2, y2, color, automap, click, hover
JSAPI_FUNC(line_ctor)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	ScreenhookState state = (script->GetState () == OutOfGame) ? OOG : IG;
	int x = 0, y = 0, x2 = 0, y2 = 0;
	ushort color = 0;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;

	if(argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		x = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	if(argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		y = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	if(argc > 2 && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
		x2 = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
	if(argc > 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
		y2 = JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);
	if(argc > 4 && JSVAL_IS_INT(JS_ARGV(cx, vp)[4]))
		color = (ushort)JSVAL_TO_INT(JS_ARGV(cx, vp)[4]);
	if(argc > 5 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[5]))
		automap = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[5]);
	if(argc > 6 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[6]))
		click = JS_ARGV(cx, vp)[6];
	if(argc > 7 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[7]))
		hover = JS_ARGV(cx, vp)[7];

	JSObject* hook = BuildObject(cx, &line_class, line_methods, line_props);
	if(!hook)
		THROW_ERROR(cx, "Failed to create line object");

	LineHook* pLineHook = new LineHook(script, hook, x, y, x2, y2, color, automap, Left, state);

	if (!pLineHook)
		THROW_ERROR(cx, "Unable to initalize a line class.");

	JS_SetPrivate(cx, hook, pLineHook);
	pLineHook->SetClickHandler(click);
	pLineHook->SetHoverHandler(hover);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(hook));

	return JS_TRUE;
}


JSAPI_PROP(line_getProperty)
{
	LineHook* pLineHook = (LineHook*)JS_GetPrivate(cx, obj);
	if(!pLineHook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case LINE_X:
			vp.setInt32(pLineHook->GetX());
			break;
		case LINE_Y:
			vp.setInt32(pLineHook->GetY());
			break;
		case LINE_XSIZE:
			vp.setInt32(pLineHook->GetX2());
			break;
		case LINE_YSIZE:
			vp.setInt32(pLineHook->GetY2());
			break;
		case LINE_COLOR:
			vp.setInt32(pLineHook->GetColor());
			break;
		case LINE_VISIBLE:
			vp.setBoolean(pLineHook->GetIsVisible());
			break;
		case LINE_ZORDER:
			vp.setInt32(pLineHook->GetZOrder());
			break;
		case LINE_ONCLICK:
			vp.set(pLineHook->GetClickHandler());
			break;
		case LINE_ONHOVER:
			vp.set(pLineHook->GetHoverHandler());
			break;
	}
	return JS_TRUE;
}

JSAPI_STRICT_PROP(line_setProperty)
{
	LineHook* pLineHook = (LineHook*)JS_GetPrivate(cx, obj);
	if(!pLineHook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case LINE_X:
			if (vp.isInt32())
				pLineHook->SetX(vp.toInt32());
			break;
		case LINE_Y:
			if (vp.isInt32())
				pLineHook->SetY(vp.toInt32());
			break;
		case LINE_XSIZE:
			if (vp.isInt32())
				pLineHook->SetX2(vp.toInt32());
			break;
		case LINE_YSIZE:
			if (vp.isInt32())
				pLineHook->SetY2(vp.toInt32());
			break;
		case LINE_COLOR:
			if (vp.isInt32())
				pLineHook->SetColor((ushort)vp.toInt32());
			break;
		case LINE_VISIBLE:
			if (vp.isBoolean())
				pLineHook->SetIsVisible(!!vp.toBoolean());
			break;
		case LINE_ZORDER:
			if(vp.isInt32())
				pLineHook->SetZOrder((ushort)vp.toInt32());
			break;
		case LINE_ONCLICK:
			pLineHook->SetClickHandler(vp.get());
			break;
		case LINE_ONHOVER:
			pLineHook->SetHoverHandler(vp.get());
			break;
	}
	return JS_TRUE;
}


// Function to create a text which gets called on a "new text ()"

// Parameters: text, x, y, color, font, align, automap, onHover, onText
JSAPI_FUNC(text_ctor)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	ScreenhookState state = (script->GetState () == OutOfGame) ? OOG : IG;
	uint x = 0, y = 0;
	ushort color = 0, font = 0;
	Align align = Left;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;
	char* szText = "";

	if(argc > 0 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		szText = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
		if(!szText)
			return JS_TRUE;
	if(argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		x = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	if(argc > 2 && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
		y = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
	if(argc > 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
		color = (ushort)JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);
	if(argc > 4 && JSVAL_IS_INT(JS_ARGV(cx, vp)[4]))
		font = (ushort)JSVAL_TO_INT(JS_ARGV(cx, vp)[4]);
	if(argc > 5 && JSVAL_IS_INT(JS_ARGV(cx, vp)[5]))
		align = (Align)JSVAL_TO_INT(JS_ARGV(cx, vp)[5]);
	if(argc > 6 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[6]))
		automap = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[6]);
	if(argc > 7 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[7]))
		click = JS_ARGV(cx, vp)[7];
	if(argc > 8 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[8]))
		hover = JS_ARGV(cx, vp)[8];

	JSObject* hook = BuildObject(cx, &text_class, text_methods, text_props);
	if(!hook)
		THROW_ERROR(cx, "Failed to create text object");

	TextHook* pTextHook = new TextHook(script, hook, szText, x, y, font, color, automap, align, state);

	if(!pTextHook)
		THROW_ERROR(cx, "Failed to create texthook");

	JS_SetPrivate(cx, hook, pTextHook);
	pTextHook->SetClickHandler(click);
	pTextHook->SetHoverHandler(hover);

	JS_SET_RVAL(cx, vp,  OBJECT_TO_JSVAL(hook));

	return JS_TRUE;
}

JSAPI_PROP(text_getProperty)
{
	TextHook* pTextHook = (TextHook*)JS_GetPrivate(cx, obj);
	if(!pTextHook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case TEXT_X:
			vp.setInt32(pTextHook->GetX());
			break;
		case TEXT_Y:
			vp.setInt32(pTextHook->GetY());
			break;
		case TEXT_COLOR:
			vp.setInt32(pTextHook->GetColor());
			break;
		case TEXT_FONT:
			vp.setInt32(pTextHook->GetFont());
			break;
		case TEXT_TEXT:
			vp.setString(JS_NewStringCopyZ(cx, pTextHook->GetText()));
			break;
		case TEXT_ALIGN:
			vp.setInt32(pTextHook->GetAlign());
			break;
		case TEXT_VISIBLE:
			vp.setBoolean(pTextHook->GetIsVisible());
			break;
		case TEXT_ZORDER:
			vp.setInt32(pTextHook->GetZOrder());
			break;
		case TEXT_ONCLICK:
			vp.set(pTextHook->GetClickHandler());
			break;
		case TEXT_ONHOVER:
			vp.set(pTextHook->GetHoverHandler());
			break;
	}
	return JS_TRUE;
}

JSAPI_STRICT_PROP(text_setProperty)
{
	TextHook* pTextHook = (TextHook*)JS_GetPrivate(cx, obj);
	if(!pTextHook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case TEXT_X:
			if(vp.isInt32())
				pTextHook->SetX(vp.toInt32());
			break;
		case TEXT_Y:
			if(vp.isInt32())
				pTextHook->SetY(vp.toInt32());
			break;
		case TEXT_COLOR:
			if(vp.isInt32())
				pTextHook->SetColor((ushort)vp.toInt32());
			break;
		case TEXT_FONT:
			if(vp.isInt32())
				pTextHook->SetFont((ushort)vp.toInt32());
			break;
		case TEXT_TEXT:
			if(vp.isString())
			{
				char* pText = JS_EncodeString(cx,vp.toString());
				if(!pText)
					return JS_TRUE;
				pTextHook->SetText(pText);
				JS_free(cx, pText);
			}
			break;
		case TEXT_ALIGN:
			if(vp.isInt32())
				pTextHook->SetAlign((Align)vp.toInt32());
			break;
		case TEXT_VISIBLE:
			if(vp.isBoolean())
				pTextHook->SetIsVisible(!!vp.toBoolean());
			break;
		case TEXT_ZORDER:
			if(vp.isInt32())
				pTextHook->SetZOrder((ushort)vp.toInt32());
			break;
		case TEXT_ONCLICK:
			pTextHook->SetClickHandler(vp.get());
			break;
		case TEXT_ONHOVER:
			pTextHook->SetHoverHandler(vp.get());
			break;
	}
	return JS_TRUE;
}


// Function to create a image which gets called on a "new Image ()"

// Parameters: image, x, y, color, align, automap, onHover, onimage
JSAPI_FUNC(image_ctor)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	ScreenhookState state = (script->GetState () == OutOfGame) ? OOG : IG;
	uint x = 0, y = 0;
	ushort color = 0;
	Align align = Left;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;
	char* szText = "";
	char path[_MAX_FNAME+_MAX_PATH];

	if(argc > 0 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		szText	= JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
		if(!szText)
			return JS_TRUE;
	if(argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		x = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	if(argc > 2 && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
		y = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
	if(argc > 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
		color = (ushort)JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);
	if(argc > 4 && JSVAL_IS_INT(JS_ARGV(cx, vp)[4]))
		align = (Align)JSVAL_TO_INT(JS_ARGV(cx, vp)[4]);
	if(argc > 5 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[5]))
		automap = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[5]);
	if(argc > 6 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[6]))
		click = JS_ARGV(cx, vp)[6];
	if(argc > 7 && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[7]))
		hover = JS_ARGV(cx, vp)[7];

	if(!isValidPath(path))
		THROW_ERROR(cx, "Invalid image file path");

	JSObject* hook = BuildObject(cx, &image_class, image_methods, image_props);
	if(!hook)
		THROW_ERROR(cx, "Failed to create image object");

	sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, szText);
	ImageHook* pImageHook = new ImageHook(script, hook, path, x, y, color, automap, align, state);

	if(!pImageHook)
		THROW_ERROR(cx, "Failed to create ImageHook");

	JS_SetPrivate(cx, hook, pImageHook);
	pImageHook->SetClickHandler(click);
	pImageHook->SetHoverHandler(hover);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(hook));

	return JS_TRUE;
}

JSAPI_PROP(image_getProperty)
{
	ImageHook* pImageHook = (ImageHook*)JS_GetPrivate(cx, obj);
	if(!pImageHook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case IMAGE_X:
			vp.setInt32(pImageHook->GetX());
			break;
		case IMAGE_Y:
			vp.setInt32(pImageHook->GetY());
			break;
		case IMAGE_LOCATION:
			vp.setString(JS_NewStringCopyZ(cx, pImageHook->GetImage()));
			break;
		case IMAGE_ALIGN:
			vp.setInt32(pImageHook->GetAlign());
			break;
		case IMAGE_VISIBLE:
			vp.setBoolean(pImageHook->GetIsVisible());
			break;
		case IMAGE_ZORDER:
			vp.setInt32(pImageHook->GetZOrder());
			break;
		case IMAGE_ONCLICK:
			vp.set(pImageHook->GetClickHandler());
			break;
		case IMAGE_ONHOVER:
			vp.set(pImageHook->GetHoverHandler());
			break;
	}
	return JS_TRUE;
}

JSAPI_STRICT_PROP(image_setProperty)
{
	ImageHook* pImageHook = (ImageHook*)JS_GetPrivate(cx, obj);
	if(!pImageHook)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case IMAGE_X:
			if(vp.isInt32())
				pImageHook->SetX(vp.toInt32());
			break;
		case IMAGE_Y:
			if(vp.isInt32())
				pImageHook->SetY(vp.toInt32());
			break;
		case IMAGE_LOCATION:
			if(vp.isString())
			{
				char* pimage = JS_EncodeString(cx,vp.toString());
				if(!pimage)
					return JS_TRUE;
				pImageHook->SetImage(pimage);
			}
			break;
		case IMAGE_ALIGN:
			if(vp.isInt32())
				pImageHook->SetAlign((Align)vp.toInt32());
			break;
		case IMAGE_VISIBLE:
			if(vp.isBoolean())
				pImageHook->SetIsVisible(!!vp.toBoolean());
			break;
		case IMAGE_ZORDER:
			if(vp.isInt32())
				pImageHook->SetZOrder((ushort)vp.toInt32());
			break;
		case IMAGE_ONCLICK:
			pImageHook->SetClickHandler(vp.get());
			break;
		case IMAGE_ONHOVER:
			pImageHook->SetHoverHandler(vp.get());
			break;
	}
	return JS_TRUE;
}

JSAPI_FUNC(screenToAutomap)
{
	if(argc == 1)
	{
		// the arg must be an object with an x and a y that we can convert
		if(JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]))
		{
			// get the params
			JSObject* arg = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
			jsval x, y;
			if(JS_GetProperty(cx, arg, "x", &x) == JS_FALSE || JS_GetProperty(cx, arg, "y", &y) == JS_FALSE)
				THROW_ERROR(cx, "Failed to get x and/or y values");
			if(!JSVAL_IS_INT(x) || !JSVAL_IS_INT(y))
				THROW_ERROR(cx, "Input has an x or y, but they aren't the correct type!");
			int32 ix, iy;
			if(JS_ValueToInt32(cx, x, &ix) == JS_FALSE || JS_ValueToInt32(cx, y, &iy))
				THROW_ERROR(cx, "Failed to convert x and/or y values");
			// convert the values
			POINT result = ScreenToAutomap(ix, iy);
			x = INT_TO_JSVAL(result.x);
			y = INT_TO_JSVAL(result.y);
			JSObject* res = JS_NewObject(cx, NULL, NULL, NULL);
			jsval* argv = JS_ARGV(cx, vp);
			if(JS_SetProperty(cx, res, "x", &argv[0]) == JS_FALSE || JS_SetProperty(cx, res, "y", &argv[1]) == JS_FALSE)
				THROW_ERROR(cx, "Failed to set x and/or y values");
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res));
		}
		else
			THROW_ERROR(cx, "Invalid object specified to screenToAutomap");
	}
	else if(argc == 2)
	{
		// the args must be ints
		if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		{
			int32 ix, iy;
			jsval* argv = JS_ARGV(cx, vp);
			if(JS_ValueToInt32(cx, argv[0], &ix) == JS_FALSE || JS_ValueToInt32(cx, argv[1], &iy) == JS_FALSE)
				THROW_ERROR(cx, "Failed to convert x and/or y values");
			// convert the values
			POINT result = ScreenToAutomap(ix, iy);
			argv[0] = INT_TO_JSVAL(result.x);
			argv[1] = INT_TO_JSVAL(result.y);
			JSObject* res = JS_NewObject(cx, NULL, NULL, NULL);
			if(JS_SetProperty(cx, res, "x", &argv[0]) == JS_FALSE || JS_SetProperty(cx, res, "y", &argv[1]) == JS_FALSE)
				THROW_ERROR(cx, "Failed to set x and/or y values");
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res));
		}
		else
			THROW_ERROR(cx, "screenToAutomap expects two arguments to be two integers");
	}
	else
		THROW_ERROR(cx, "Invalid arguments specified for screenToAutomap");
	return JS_TRUE;
}

JSAPI_FUNC(automapToScreen)
{
	jsval* argv = JS_ARGV(cx, vp);
	if(argc == 1)
	{
		// the arg must be an object with an x and a y that we can convert
		if(JSVAL_IS_OBJECT(argv[0]))
		{
			// get the params
			JSObject* arg = JSVAL_TO_OBJECT(argv[0]);
			jsval x, y;
			if(JS_GetProperty(cx, arg, "x", &x) == JS_FALSE || JS_GetProperty(cx, arg, "y", &y) == JS_FALSE)
				THROW_ERROR(cx, "Failed to get x and/or y values");
			if(!JSVAL_IS_INT(x) || !JSVAL_IS_INT(y))
				THROW_ERROR(cx, "Input has an x or y, but they aren't the correct type!");
			int32 ix, iy;
			// 22 JS_BeginRequest(cx);
			if(JS_ValueToInt32(cx, x, &ix) == JS_FALSE || JS_ValueToInt32(cx, y, &iy))
			{
				/* 22 JS_EndRequest(cx);*/
				THROW_ERROR(cx, "Failed to convert x and/or y values");
			}
			/* 22 JS_EndRequest(cx);*/
			// convert the values
			POINT result = {ix,iy};
			AutomapToScreen(&result);
			x = INT_TO_JSVAL(ix);
			y = INT_TO_JSVAL(iy);
			// 22 JS_BeginRequest(cx);
			if(JS_SetProperty(cx, arg, "x", &x) == JS_FALSE || JS_SetProperty(cx, arg, "y", &y) == JS_FALSE)
			{
				/* 22 JS_EndRequest(cx);*/
				THROW_ERROR(cx, "Failed to set x and/or y values");
			}
			/* 22 JS_EndRequest(cx);*/
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(arg));
		}
		else
			THROW_ERROR(cx, "Invalid object specified to automapToScreen");
	}
	else if(argc == 2)
	{
		// the args must be ints
		if(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]))
		{
			int32 ix, iy;
			// 22 JS_BeginRequest(cx);
			if(JS_ValueToInt32(cx, argv[0], &ix) == JS_FALSE || JS_ValueToInt32(cx, argv[1], &iy) == JS_FALSE)
			{
				/* 22 JS_EndRequest(cx);*/
				THROW_ERROR(cx, "Failed to convert x and/or y values");
			}
			/* 22 JS_EndRequest(cx);*/
			// convert the values
			POINT result = {ix,iy};
			AutomapToScreen(&result);
			argv[0] = INT_TO_JSVAL(result.x);
			argv[1] = INT_TO_JSVAL(result.y);
			JSObject* res = JS_NewObject(cx, NULL, NULL, NULL);
			if(JS_SetProperty(cx, res, "x", &argv[0]) == JS_FALSE || JS_SetProperty(cx, res, "y", &argv[1]) == JS_FALSE)
				THROW_ERROR(cx, "Failed to set x and/or y values");
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res));
		}
		else
			THROW_ERROR(cx, "automapToScreen expects two arguments to be two integers");
	}
	else
		THROW_ERROR(cx, "Invalid arguments specified for automapToScreen");
	return JS_TRUE;
}

