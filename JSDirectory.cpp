/*
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// TODO: Rewrite this crap :(

#define _USE_32BIT_TIME_T

#include <direct.h>
#include <io.h>
#include <cerrno>

#include "JSDirectory.h"
#include "D2BS.h"
#include "File.h"
//#include "js32.h"

////////////////////////////////////////////////////////////////////////////////
// Directory stuff
////////////////////////////////////////////////////////////////////////////////

EMPTY_CTOR(dir)

JSAPI_FUNC(my_openDir)
{
	if(argc != 1)
		return JS_TRUE;

	char path[_MAX_PATH];
	char* name = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));

	if(!isValidPath(name))
		return JS_TRUE;

	sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, name);

	if((_mkdir(path) == -1) && (errno == ENOENT)) {
		JS_ReportError(cx, "Couldn't get directory %s, path '%s' not found", name, path);
		return JS_FALSE;
	} 
	else {
		DirData* d = new DirData(name);
		JSObject *jsdir = BuildObject(cx, &folder_class, dir_methods, dir_props, d);
		*rval = OBJECT_TO_JSVAL(jsdir);
	}

	return JS_TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// dir_getFiles
// searches a directory for files with a specified extension(*.* assumed)
//
// Added by lord2800
////////////////////////////////////////////////////////////////////////////////

JSAPI_FUNC(dir_getFiles)
{
	if(argc > 1)
		return JS_TRUE;
	if(argc < 1)
		argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "*.*"));

	DirData* d = (DirData*)JS_GetPrivate(cx, obj);
	char* search = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!search)
		THROW_ERROR(cx, "Failed to get search string");

	long hFile;
	char path[_MAX_PATH], oldpath[_MAX_PATH];
	sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, d->name);

	_getcwd(oldpath, _MAX_PATH);
	_chdir(path);

	_finddata_t found;
	JSObject* jsarray = JS_NewArrayObject(cx, 0, NULL);
	*rval = OBJECT_TO_JSVAL(jsarray);

	if((hFile = _findfirst(search, &found)) != -1L)
	{
		jsint element = 0;
		do
		{
			if((found.attrib & _A_SUBDIR))
				continue;
			jsval file = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, found.name));
			JS_SetElement(cx, jsarray, element++, &file);
		} while(_findnext(hFile, &found) == 0);
	}

	_chdir(oldpath);

	return JS_TRUE;
}

JSAPI_FUNC(dir_getFolders)
{
	if(argc > 1)
		return JS_TRUE;
	if(argc < 1)
		argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "*.*"));

	DirData* d = (DirData*)JS_GetPrivate(cx, obj);
	char* search = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!search)
		THROW_ERROR(cx, "Failed to get search string");

	long hFile;
	char path[_MAX_PATH], oldpath[_MAX_PATH];
	sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, d->name);

	_getcwd(oldpath, _MAX_PATH);
	_chdir(path);

	_finddata_t found;
	JSObject* jsarray = JS_NewArrayObject(cx, 0, NULL);
	*rval = OBJECT_TO_JSVAL(jsarray);

	if((hFile = _findfirst(search, &found)) != -1L)
	{
		jsint element = 0;
		do
		{
			if(!strcmp(found.name, "..") || !strcmp(found.name, ".") || !(found.attrib & _A_SUBDIR))
				continue;
			jsval file = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, found.name));
			JS_SetElement(cx, jsarray, element++, &file);
		} while(_findnext(hFile, &found) == 0);
	}

	_chdir(oldpath);

	return JS_TRUE;
}

JSAPI_FUNC(dir_create)
{
	DirData* d = (DirData*)JS_GetPrivate(cx, obj);
	char path[_MAX_PATH];
	if (!JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, "No path passed to dir.create()");
	char* name = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));

	if(!isValidPath(name))
		return JS_TRUE;

	sprintf_s(path, sizeof(path), "%s\\%s\\%s", Vars.szScriptPath, d->name, name);
	if(_mkdir(path) == -1 && (errno == ENOENT)) {
		JS_ReportError(cx, "Couldn't create directory %s, path %s not found", name, path);
		return JS_FALSE;
	} 
	else {
		DirData* d = new DirData(name);
		JSObject* jsdir = BuildObject(cx, &folder_class, dir_methods, dir_props, d);
		*rval = OBJECT_TO_JSVAL(jsdir);
	}

	return JS_TRUE;
}

JSAPI_FUNC(dir_delete)
{
	DirData* d = (DirData*)JS_GetPrivate(cx, obj);

	char path[_MAX_PATH];
	sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, d->name);

	if(_rmdir(path) == -1)
	{
		// TODO: Make an optional param that specifies recursive delete
		if(errno == ENOTEMPTY)
			THROW_ERROR(cx, "Tried to delete directory, but it is not empty or is the current working directory");
		if(errno == ENOENT)
			THROW_ERROR(cx, "Path not found");
	}
	*rval = JSVAL_TRUE;

	return JS_TRUE;
}

JSAPI_PROP(dir_getProperty)
{
	DirData* d = (DirData*)JS_GetPrivate(cx, obj);

	if(!d) return JS_FALSE;

	switch(JSVAL_TO_INT(id))
	{
		case DIR_NAME:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, d->name));
			break;
	}
	return JS_TRUE;
}

void dir_finalize(JSContext *cx, JSObject *obj)
{
	DirData* d = (DirData*)JS_GetPrivate(cx, obj);
	delete d;
}

