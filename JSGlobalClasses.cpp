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
#include "JSProfile.h"

JSClass global_obj = {
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL,
    JSCLASS_NO_OPTIONAL_MEMBERS
};
//JSAPI_EMPTY_CTOR(global)

//JSClass global_obj = { "global", JSCLASS_GLOBAL_FLAGS, JSCLASS_DEFAULT_WITH_CTOR(global) };


//JSClass pipe_class = { "Pipe", JSCLASS_HAS_PRIVATE, 
//	JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
//				 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, pipe_finalize, Pipe)};

//JSClassSpec pipe_spec = JS_CS(&pipe_class, &stream_class, Pipe, 0, pipe_methods, pipe_props, pipe_static_methods, nullptr);


JSClass sqlite_db = {
	"SQLite", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, (JSPropertyOp) JS_PropertyStub, (JSStrictPropertyOp) JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sqlite_finalize, sqlite_ctor)};

JSClass sqlite_stmt = {
	"DBStatement", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sqlite_stmt_finalize, sqlite_stmt_ctor)};

JSClass script_class = {
	"D2BSScript", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL, script_ctor)};

JSClass frame_class = {
	"Frame", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize, frame_ctor)};

JSClass box_class = {
	"Box", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize, box_ctor)};

JSClass line_class = {
	"Line", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize, line_ctor)};

JSClass text_class = {
	"Text", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize, text_ctor)};

JSClass image_class = {
	"Image", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, hook_finalize, image_ctor)};

JSClass sandbox_class = {
	"Sandbox", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(sandbox_addProperty, sandbox_delProperty, sandbox_getProperty, sandbox_setProperty,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sandbox_finalize, sandbox_ctor)};

JSClass room_class = {
	"Room", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL, room_ctor)};

JSClass presetunit_class = {
	"PresetUnit", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, presetunit_finalize, presetunit_ctor)};

JSClass party_class = {
	"Party", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL, party_ctor)};

JSClass filetools_class = {
	"FileTools", NULL ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL, filetools_ctor)};

JSClass file_class = {
	"File", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, file_finalize, file_ctor)};

JSClass exit_class = {
	"Exit", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, exit_finalize, exit_ctor)};

JSClass folder_class = {
	"Folder", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, dir_finalize, dir_ctor)};

JSClass control_class = {
	"Control", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, control_finalize, control_ctor)};

JSClass area_class = {
	"Area", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, area_finalize, area_ctor)};

JSClass unit_class = {
	"Unit", JSCLASS_HAS_PRIVATE ,
    JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
			 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, unit_finalize, unit_ctor)};

JSClass profile_class = {
    "Profile", JSCLASS_HAS_PRIVATE,
  JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, profile_finalize, profile_ctor)};

JSClass profileType_class = {
    "ProfileType", JSCLASS_HAS_PRIVATE,
   JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
   JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL, profileType_ctor)};

	JSClass dialogLine_class = {
    "DailogLine", JSCLASS_HAS_PRIVATE | JSCLASS_HAS_RESERVED_SLOTS(1),
   JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
   JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL, NULL)};

//JSExtendedClass unit_class_ex = {
//	unit_class,
//	unit_equal,
//	NULL, NULL, NULL, NULL
//};
//
//JSExtendedClass file_class_ex = {
//	file_class,
//	file_equality,
//	NULL, NULL, NULL, NULL
//};
//
//JSExtendedClass sqlite_db_ex = {
//	sqlite_db,
//	sqlite_equal,
//	NULL, NULL, NULL, NULL
//};
//JSClassSpec pipe_spec = JS_CS(&pipe_class, &stream_class, Pipe, 0, pipe_methods, pipe_props, pipe_static_methods, nullptr);
//JSClassSpec file_spec = JS_CS(&file_class, &stream_class, File, 0, file_methods, file_props, file_static_methods, nullptr);
JSClassSpec global_classes[] = {
	//JS_CS(&event_class, nullptr, ::Event, 0, nullptr, nullptr, event_methods, nullptr),
	{&unit_class,			0,	unit_ctor,			0,	unit_methods,	unit_props,				NULL,	NULL},
	{&presetunit_class,		0,	presetunit_ctor,	0,	NULL,			presetunit_props,		NULL,	NULL},
	{&area_class,			0,	area_ctor,			0,	NULL,			area_props,				NULL,	NULL},
	{&control_class,		0,	control_ctor,		0,	control_funcs,	control_props,			NULL,	NULL},
	{&folder_class,			0,	dir_ctor,			0,	dir_methods,	dir_props,				NULL,	NULL},
	{&exit_class,			0,	exit_ctor,			0,	NULL,			exit_props,				NULL,	NULL},
	{&party_class,			0,	party_ctor,			0,	party_methods,	party_props,			NULL,	NULL},
	{&room_class,			0,	room_ctor,			0,	room_methods,	room_props,				NULL,	NULL},

	{&file_class,			0,	file_ctor,			0,	file_methods,	file_props,				file_s_methods,	NULL},
	{&filetools_class,		0,	filetools_ctor,		0,	NULL,			NULL,					filetools_s_methods,	NULL},
	{&sqlite_db,			0,	sqlite_ctor,		0,	sqlite_methods,	sqlite_props,			NULL,	NULL},
	{&sqlite_stmt,			0,	sqlite_stmt_ctor,	0,	sqlite_stmt_methods,	sqlite_stmt_props,			NULL,	NULL},
	{&sandbox_class,		0,	sandbox_ctor,		0,	sandbox_methods,	NULL,				NULL,	NULL},
	{&script_class,			0,	script_ctor,		0,	script_methods,	script_props,			NULL,	NULL},
	
	{&frame_class,			0,	frame_ctor,			0,	frame_methods,	frame_props,			NULL,	NULL},
	{&box_class,			0,	box_ctor,			0,	box_methods,	box_props,				NULL,	NULL},
	{&line_class,			0,	line_ctor,			0,	line_methods,	line_props,				NULL,	NULL},
	{&text_class,			0,	text_ctor,			0,	text_methods,	text_props,				NULL,	NULL},
	{&image_class,			0,	image_ctor,			0,	image_methods,	image_props,			NULL,	NULL},
	{0}
	
};

//JSClassSpec global_classes[] = {
	/*JSClass*				properties			functions				static props	static funcs */
	// game objects
	//{&unit_class_ex.base,	unit_props,			unit_methods,			NULL,			NULL},
	//{&presetunit_class,		presetunit_props,	NULL,					NULL,			NULL},
	//{&area_class,			area_props,			NULL,					NULL,			NULL},
	//{&control_class,		control_props,		control_funcs,			NULL,			NULL},
	//{&folder_class,			dir_props,			dir_methods,			NULL,			NULL},
	//{&exit_class,			exit_props,			NULL,					NULL,			NULL},
	//{&party_class,			party_props,		party_methods,			NULL,			NULL},
	//{&room_class,			room_props,			room_methods,			NULL,			NULL},
	//{&profile_class,		profile_props,		profile_methods,		NULL,			NULL},
	//{&profileType_class,	NULL,				NULL,					profileType_props,
	//																					NULL},

	// utility objects
	//{&file_class,	file_props,			file_methods,			NULL,			file_s_methods},
	//{&sqlite_db,	sqlite_props,		sqlite_methods,			NULL,			NULL},
	//{&sqlite_stmt,			sqlite_stmt_props,	sqlite_stmt_methods,	NULL,			NULL},
	//{&filetools_class,		NULL,				NULL,					NULL,			filetools_s_methods},
	//{&sandbox_class,		NULL,				sandbox_methods,		NULL,			NULL},
	//{&script_class,			script_props,		script_methods,			NULL,			NULL},

	// screenhook objects
	//{&frame_class,			frame_props,		frame_methods,			NULL,			NULL},
//	{&box_class,			box_props,			box_methods,			NULL,			NULL},
//	{&line_class,			line_props,			line_methods,			NULL,			NULL},
//	{&text_class,			text_props,			text_methods,			NULL,			NULL},
//	{&image_class,			image_props,		image_methods,			NULL,			NULL},
//	{0}
//};
