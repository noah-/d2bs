#include "js32.h"
#include "JSFile.h"
#include "JSSQLite.h"
#include "JSSandbox.h"
#include "JSUnit.h"
#include "JSScreenHook.h"
#include "JSPresetUnit.h"
#include "JSDirectory.h"
#include "JSFileTools.h"
#include "JSArea.h"
#include "JSControl.h"
#include "JSParty.h"
#include "JSExits.h"
#include "JSRoom.h"
#include "JSScript.h"

JSClass global_obj = {
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

JSClass sqlite_db = {
	"SQLite", JSCLASS_HAS_PRIVATE | JSCLASS_IS_EXTENDED,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sqlite_finalize,
	NULL, NULL, NULL, sqlite_ctor, NULL, NULL, NULL, NULL
};

JSClass sqlite_stmt = {
	"DBStatement", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sqlite_stmt_finalize,
	NULL, NULL, NULL, sqlite_stmt_ctor
};

JSClass script_class = {
    "D2BSScript", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    NULL, NULL, NULL, script_ctor
};

JSClass frame_class = {
    "Frame", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize,
	NULL, NULL, NULL, frame_ctor, NULL, NULL, NULL, NULL
};

JSClass box_class = {
    "Box", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize,
	NULL, NULL, NULL, box_ctor, NULL, NULL, NULL, NULL
};

JSClass line_class = {
    "Line", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize,
	NULL, NULL, NULL, line_ctor, NULL, NULL, NULL, NULL
};

JSClass text_class = {
    "Text", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize,
	NULL, NULL, NULL, text_ctor, NULL, NULL, NULL, NULL
};

JSClass image_class = {
    "Image", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize,
	NULL, NULL, NULL, image_ctor, NULL, NULL, NULL, NULL
};

JSClass sandbox_class = {
	"Sandbox", JSCLASS_HAS_PRIVATE,
	sandbox_addProperty, sandbox_delProperty, sandbox_getProperty, sandbox_setProperty,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sandbox_finalize,
	NULL, NULL, NULL, sandbox_ctor, NULL, NULL, NULL, NULL
};

JSClass room_class = {
    "Room", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    NULL, NULL, NULL, room_ctor
};

JSClass presetunit_class = {
    "PresetUnit", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, presetunit_finalize,
    NULL, NULL, NULL, presetunit_ctor
};

JSClass party_class = {
    "Party", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    NULL, NULL, NULL, party_ctor
};

JSClass filetools_class = {
	"FileTools", NULL,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL, filetools_ctor, NULL, NULL, NULL, NULL
};

JSClass file_class = {
	"File", JSCLASS_HAS_PRIVATE | JSCLASS_IS_EXTENDED,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, file_finalize,
	NULL, NULL, NULL, file_ctor, NULL, NULL, NULL, NULL
};

JSClass exit_class = {
    "Exit", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, exit_finalize,
    NULL, NULL, NULL, exit_ctor
};

JSClass folder_class = {
	"Folder", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, dir_finalize,
    NULL, NULL, NULL, dir_ctor
};

JSClass control_class = {
    "Control", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, control_finalize,
    NULL, NULL, NULL, control_ctor
};

JSClass area_class = {
    "Area", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, area_finalize,
    NULL, NULL, NULL, area_ctor
};

JSClass unit_class = {
    "Unit",	JSCLASS_HAS_PRIVATE | JSCLASS_IS_EXTENDED,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, unit_finalize,
    NULL, NULL, NULL, unit_ctor
};


JSExtendedClass unit_class_ex = {
	unit_class,
	unit_equal,
	NULL, NULL, NULL, NULL
};

JSExtendedClass file_class_ex = {
	file_class,
	file_equality,
	NULL, NULL, NULL, NULL
};

JSExtendedClass sqlite_db_ex = {
	sqlite_db,
	sqlite_equal,
	NULL, NULL, NULL, NULL
};

JSClassSpec global_classes[] = {
	/*JSClass*				properties			functions				static props	static funcs */
	// game objects
	{&unit_class_ex.base,	unit_props,			unit_methods,			NULL,			NULL},
	{&presetunit_class,		presetunit_props,	NULL,					NULL,			NULL},
	{&area_class,			area_props,			NULL,					NULL,			NULL},
	{&control_class,		control_props,		control_funcs,			NULL,			NULL},
	{&folder_class,			dir_props,			dir_methods,			NULL,			NULL},
	{&exit_class,			exit_props,			NULL,					NULL,			NULL},
	{&party_class,			party_props,		party_methods,			NULL,			NULL},
	{&room_class,			room_props,			room_methods,			NULL,			NULL},

	// utility objects
	{&file_class_ex.base,	file_props,			file_methods,			NULL,			file_s_methods},
	{&sqlite_db_ex.base,	sqlite_props,		sqlite_methods,			NULL,			NULL},
	{&sqlite_stmt,			sqlite_stmt_props,	sqlite_stmt_methods,	NULL,			NULL},
	{&filetools_class,		NULL,				NULL,					NULL,			filetools_s_methods},
	{&sandbox_class,		NULL,				sandbox_methods,		NULL,			NULL},
	{&script_class,			script_props,		script_methods,			NULL,			NULL},

	// screenhook objects
	{&frame_class,			frame_props,		frame_methods,			NULL,			NULL},
	{&box_class,			box_props,			box_methods,			NULL,			NULL},
	{&line_class,			line_props,			line_methods,			NULL,			NULL},
	{&text_class,			text_props,			text_methods,			NULL,			NULL},
	{&image_class,			image_props,		image_methods,			NULL,			NULL},
	{0}
};
