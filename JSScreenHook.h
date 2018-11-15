#ifndef __JSSCREENHOOK_H__
#define __JSSCREENHOOK_H__

#include "js32.h"

JSAPI_FUNC(hook_remove);
void hook_finalize(JSFreeOp *fop, JSObject *obj);

/*********************************************************
					Frame Header
**********************************************************/
JSAPI_FUNC(frame_ctor);
JSAPI_PROP(frame_getProperty);
JSAPI_STRICT_PROP(frame_setProperty);

enum frame_tinyid {
	FRAME_X,
	FRAME_Y,
	FRAME_XSIZE,
	FRAME_YSIZE,
	FRAME_VISIBLE,
	FRAME_ALIGN,
	FRAME_ONCLICK,
	FRAME_ONHOVER,
	FRAME_ZORDER
};

static JSPropertySpec frame_props[] = {
	{"x",		FRAME_X,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{"y",		FRAME_Y,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{"xsize",	FRAME_XSIZE,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{"ysize",	FRAME_YSIZE,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{"visible",	FRAME_VISIBLE,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{"align",	FRAME_ALIGN,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{"zorder",	FRAME_ZORDER,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{"click",	FRAME_ONCLICK,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{"hover",	FRAME_ONHOVER,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(frame_getProperty),	JSOP_WRAPPER(frame_setProperty)},
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

static JSFunctionSpec frame_methods[] = {
	JS_FS("remove",			hook_remove,			0, FUNCTION_FLAGS),
	JS_FS_END
};

/*********************************************************
					box Header
**********************************************************/
JSAPI_FUNC(box_ctor);
JSAPI_PROP(box_getProperty);
JSAPI_STRICT_PROP(box_setProperty);

enum box_tinyid {
	BOX_X,
	BOX_Y,
	BOX_XSIZE,
	BOX_YSIZE,
	BOX_COLOR,
	BOX_OPACITY,
	BOX_VISIBLE,
	BOX_ALIGN,
	BOX_ONCLICK,
	BOX_ONHOVER,
	BOX_ZORDER
};

static JSPropertySpec box_props[] = {
	{"x",		BOX_X,			JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"y",		BOX_Y,			JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"xsize",	BOX_XSIZE,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"ysize",	BOX_YSIZE,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"visible",	BOX_VISIBLE,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"color",	BOX_COLOR,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"opacity",	BOX_OPACITY,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"align",	BOX_ALIGN,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"zorder",	BOX_ZORDER,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"click",	BOX_ONCLICK,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{"hover",	BOX_ONHOVER,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(box_getProperty),	JSOP_WRAPPER(box_setProperty)},
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

static JSFunctionSpec box_methods[] = {
	JS_FS("remove",			hook_remove,			0, FUNCTION_FLAGS),
	JS_FS_END
};


/*********************************************************
					Line Header
**********************************************************/
JSAPI_FUNC(line_ctor);
JSAPI_PROP(line_getProperty);
JSAPI_STRICT_PROP(line_setProperty);

enum line_tinyid {
	LINE_X,
	LINE_Y,
	LINE_XSIZE,
	LINE_YSIZE,
	LINE_COLOR,
	LINE_VISIBLE,
	LINE_ONCLICK,
	LINE_ONHOVER,
	LINE_ZORDER
};

static JSPropertySpec line_props[] = {
	{"x",		LINE_X,			JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{"y",		LINE_Y,			JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{"x2",		LINE_XSIZE,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{"y2",		LINE_YSIZE,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{"visible",	LINE_VISIBLE,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{"color",	LINE_COLOR,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{"zorder",	LINE_ZORDER,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{"click",	LINE_ONCLICK,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{"hover",	LINE_ONHOVER,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(line_getProperty),	JSOP_WRAPPER(line_setProperty)},
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

static JSFunctionSpec line_methods[] = {
	JS_FS("remove",			hook_remove,			0, FUNCTION_FLAGS),
	JS_FS_END
};

/*********************************************************
					Text Header
**********************************************************/
JSAPI_FUNC(text_ctor);
JSAPI_PROP(text_getProperty);
JSAPI_STRICT_PROP(text_setProperty);

enum text_tinyid {
	TEXT_X,
	TEXT_Y,
	TEXT_COLOR,
	TEXT_FONT,
	TEXT_TEXT,
	TEXT_ALIGN,
	TEXT_VISIBLE,
	TEXT_ONCLICK,
	TEXT_ONHOVER,
	TEXT_ZORDER
};

static JSPropertySpec text_props[] = {
	{"x",		TEXT_X,			JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"y",		TEXT_Y,			JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"color",	TEXT_COLOR,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"font",	TEXT_FONT,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"visible",	TEXT_VISIBLE,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"text",	TEXT_TEXT,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"align",	TEXT_ALIGN,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"zorder",	TEXT_ZORDER,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"click",	TEXT_ONCLICK,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{"hover",	TEXT_ONHOVER,	JSPROP_STATIC_VAR,	JSOP_WRAPPER(text_getProperty),	JSOP_WRAPPER(text_setProperty)},
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

static JSFunctionSpec text_methods[] = {
	JS_FS("remove",			hook_remove,			0, FUNCTION_FLAGS),
	JS_FS_END
};

/*********************************************************
					Image Header
**********************************************************/
JSAPI_FUNC(image_ctor);
JSAPI_PROP(image_getProperty);
JSAPI_STRICT_PROP(image_setProperty);

enum image_tinyid {
	IMAGE_X,
	IMAGE_Y,
	IMAGE_LOCATION,
	IMAGE_ALIGN,
	IMAGE_VISIBLE,
	IMAGE_ONCLICK,
	IMAGE_ONHOVER,
	IMAGE_ZORDER
};

static JSPropertySpec image_props[] = {
	{"x",		IMAGE_X,			JSPROP_STATIC_VAR,	JSOP_WRAPPER(image_getProperty),	JSOP_WRAPPER(image_setProperty)},
	{"y",		IMAGE_Y,			JSPROP_STATIC_VAR,	JSOP_WRAPPER(image_getProperty),	JSOP_WRAPPER(image_setProperty)},
	{"visible",	IMAGE_VISIBLE,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(image_getProperty),	JSOP_WRAPPER(image_setProperty)},
	{"location",IMAGE_LOCATION,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(image_getProperty),	JSOP_WRAPPER(image_setProperty)},
	{"align",	IMAGE_ALIGN,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(image_getProperty),	JSOP_WRAPPER(image_setProperty)},
	{"zorder",	IMAGE_ZORDER,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(image_getProperty),	JSOP_WRAPPER(image_setProperty)},
	{"click",	IMAGE_ONCLICK,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(image_getProperty),	JSOP_WRAPPER(image_setProperty)},
	{"hover",	IMAGE_ONHOVER,		JSPROP_STATIC_VAR,	JSOP_WRAPPER(image_getProperty),	JSOP_WRAPPER(image_setProperty)},
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

static JSFunctionSpec image_methods[] = {
	JS_FS("remove",			hook_remove,			0, FUNCTION_FLAGS),
	JS_FS_END
};

JSAPI_FUNC(screenToAutomap);
JSAPI_FUNC(automapToScreen);

#endif
