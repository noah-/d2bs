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

#include <cstdio>
#include <io.h>
#include <errno.h>
#include <windows.h>
#include <Shlwapi.h>

#include <assert.h>

#include "JSFileTools.h"
#include "D2BS.h"
#include "File.h"
#include "Helpers.h"

using namespace std;

EMPTY_CTOR(filetools)

JSAPI_FUNC(filetools_remove)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply a file name");

	char* file = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	char fullpath[_MAX_PATH+_MAX_FNAME];

	if(getPathRelScript(file, _MAX_PATH+_MAX_FNAME, fullpath) == NULL)
		THROW_ERROR(cx, "Invalid file name");

	remove(fullpath);
	JS_free(cx, file);
	return JS_TRUE;
}

JSAPI_FUNC(filetools_rename)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply an original file name");
	if(argc < 2 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[1]))
		THROW_ERROR(cx, "You must supply a new file name");

	char* orig = JS_EncodeString(cx ,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	char* newName = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));

	char porig[_MAX_PATH+_MAX_FNAME];
	char pnewName[_MAX_PATH+_MAX_FNAME];

	if(getPathRelScript(orig, _MAX_PATH+_MAX_FNAME, porig) == NULL)
		THROW_ERROR(cx, "Invalid original file name");

	if(getPathRelScript(newName, _MAX_PATH+_MAX_FNAME, pnewName) == NULL)
		THROW_ERROR(cx, "Invalid new file name");

	rename(porig, pnewName);
	JS_free(cx, orig);
    JS_free(cx, newName);
	return JS_TRUE;
}

JSAPI_FUNC(filetools_copy)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply an original file name");
	if(argc < 2 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[1]))
		THROW_ERROR(cx, "You must supply a new file name");

	char* orig = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	char* newName = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
	char pnewName[_MAX_PATH+_MAX_FNAME];
	bool overwrite = false;

	if(argc > 2 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[2]))
		overwrite = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[2]);

	if(getPathRelScript(newName, _MAX_PATH+_MAX_FNAME, pnewName) == NULL)
		THROW_ERROR(cx, "Invalid new file name");

	if(overwrite && _access(pnewName, 0) == 0)
		return JS_TRUE;

	FILE* fptr1 = fileOpenRelScript(orig, "r", cx);
	FILE* fptr2 = fileOpenRelScript(newName, "w", cx);

	// If fileOpenRelScript failed, it already reported the error
	if(fptr1 == NULL || fptr2 == NULL)
		return JS_FALSE;

	while(!feof(fptr1)) 
	{
		int ch = fgetc(fptr1);
		if(ferror(fptr1)) 
		{
			THROW_ERROR(cx, _strerror("Read Error"));
			break;
		} 
		else 
		{
			if(!feof(fptr1)) 
				fputc(ch, fptr2);
			if(ferror(fptr2)) 
			{
				THROW_ERROR(cx, _strerror("Write Error"));
				break;
			}
		}
	} 
	if(ferror(fptr1) || ferror(fptr2))
	{
		clearerr(fptr1);
		clearerr(fptr2);
		fflush(fptr2);
		fclose(fptr2);
		fclose(fptr1);
		remove(pnewName); // delete the partial file so it doesnt look like we succeeded
		THROW_ERROR(cx, _strerror("File copy failed"));
	}

	fflush(fptr2);
	fclose(fptr2);
	fclose(fptr1);
	JS_free(cx, orig);
    JS_free(cx, newName);
	return JS_TRUE;
}

JSAPI_FUNC(filetools_exists)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "Invalid file name");
	char* file = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	char fullpath[_MAX_PATH+_MAX_FNAME];

	if(getPathRelScript(file, _MAX_PATH+_MAX_FNAME, fullpath) == NULL)
		THROW_ERROR(cx, "Invalid file name");

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!(_access(fullpath, 0) != 0 && errno == ENOENT)));

	return JS_TRUE;
}


JSAPI_FUNC(filetools_readText)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply a file name");
	char* fname = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	FILE* fptr = fileOpenRelScript(fname, "r", cx);

	// If fileOpenRelScript failed, it already reported the error
	if(fptr == NULL)
		return JS_FALSE;

	uint size, readCount;
	char* contents;

	// Determine file size
	fseek(fptr, 0, SEEK_END);
	size = ftell(fptr);
	rewind(fptr);

	// Allocate and read the string. Need to set last char to \0 since fread
	// doesn't.
	contents = new char[size+1];
	readCount = fread(contents, sizeof(char), size, fptr);
	assert(readCount <= size);	// Avoid SEGFAULT
	contents[readCount] = 0;
	fclose(fptr);

	// Check to see if we had an error
	if(ferror(fptr))
	{
		delete[] contents;
		THROW_ERROR(cx, _strerror("Read failed"));
	}

	// Convert to JSVAL cleanup and return
	JS_BeginRequest(cx);
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyN(cx, contents, strlen(contents))));
	JS_EndRequest(cx);
	delete[] contents;
	return JS_TRUE;
}

JSAPI_FUNC(filetools_writeText)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply a file name");

	char* fname = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	FILE* fptr = fileOpenRelScript(fname, "w", cx);
	bool result = true;

	// If fileOpenRelScript failed, it already reported the error
	if(fptr == NULL)
		return JS_FALSE;

	for(uintN i = 1; i < argc; i++)
		if(!writeValue(fptr, cx, JS_ARGV(cx, vp)[i], false, true))
			result = false;

	fflush(fptr);
	fclose(fptr);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(result));
	JS_free(cx, fname);
	return JS_TRUE;
}

JSAPI_FUNC(filetools_appendText)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply a file name");

	char* fname = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	FILE* fptr = fileOpenRelScript(fname, "a+", cx);
	bool result = true;

	// If fileOpenRelScript failed, it already reported the error
	if(fptr == NULL)
		return JS_FALSE;

	for(uintN i = 1; i < argc; i++)
		if(!writeValue(fptr, cx,JS_ARGV(cx, vp)[i], false, true))
			result = false;

	fflush(fptr);
	fclose(fptr);

	 JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(result));

	return JS_TRUE;
}

