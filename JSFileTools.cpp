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
	char* file = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	if(!isValidPath(file))
		THROW_ERROR(cx, "Invalid file name");
	char path[_MAX_PATH+_MAX_FNAME];
	sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, file);

	remove(path);

	return JS_TRUE;
}

JSAPI_FUNC(filetools_rename)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply an original file name");
	char* orig = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf_s(porig, sizeof(porig), "%s\\%s", Vars.szScriptPath, orig);

	if(argc < 2 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[1]))
		THROW_ERROR(cx, "You must supply a new file name");
	char* newName = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
	if(!isValidPath(newName))
		THROW_ERROR(cx, "Invalid file name");
	char pnewName[_MAX_PATH+_MAX_FNAME];
	sprintf_s(pnewName, sizeof(pnewName), "%s\\%s", Vars.szScriptPath, newName);

	rename(porig, pnewName);

	return JS_TRUE;
}

JSAPI_FUNC(filetools_copy)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply an original file name");
	char* orig = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf_s(porig, sizeof(porig), "%s\\%s", Vars.szScriptPath, orig);

	if(argc < 2 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[1]))
		THROW_ERROR(cx, "You must supply a new file name");
	char* newName = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
	if(!isValidPath(newName))
		THROW_ERROR(cx, "Invalid file name");
	char pnewName[_MAX_PATH+_MAX_FNAME];
	sprintf_s(pnewName, sizeof(pnewName), "%s\\%s", Vars.szScriptPath, newName);

	bool overwrite = false;
	if(argc > 2 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[2]))
		overwrite = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[2]);

	if(overwrite && _access(pnewName, 0) == 0)
		return JS_TRUE;

	FILE* fptr1 = NULL;
	fopen_s(&fptr1, porig, "r");
	FILE* fptr2 = NULL;
	fopen_s(&fptr2, pnewName, "w");

	//Sanity check to make sure the file opened for reading!
	if(!fptr1)
		THROW_ERROR(cx, _strerror("Read file open failed"));
	// Same for file opened for writing
	if(!fptr2)
		THROW_ERROR(cx, _strerror("Write file open failed"));

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

	return JS_TRUE;
}

JSAPI_FUNC(filetools_exists)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "Invalid file name");
	char* file = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	if(!isValidPath(file))
		THROW_ERROR(cx, "Invalid file name");
	char path[_MAX_PATH+_MAX_FNAME];
	sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, file);

	JS_SET_RVAL(cx, vp,  BOOLEAN_TO_JSVAL(!(_access(path, 0) != 0 && errno == ENOENT)));

	return JS_TRUE;
}

JSAPI_FUNC(filetools_readText)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply an original file name");
	char* orig = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf_s(porig, sizeof(porig), "%s\\%s", Vars.szScriptPath, orig);

	if((_access(porig, 0) != 0 && errno == ENOENT))
		THROW_ERROR(cx, "File not found");

	FILE* fptr = NULL;
	uint size, readCount;
	fopen_s(&fptr, porig, "r");
	fseek(fptr, 0, SEEK_END);
	size = ftell(fptr);
	rewind(fptr);
	char* contents = new char[size+1];
	memset(contents, 0, size+1);
	readCount = fread(contents, sizeof(char), size, fptr);
	if( readCount != size && ferror(fptr))
	{
		delete[] contents;
		THROW_ERROR(cx, _strerror("Read failed"));
	}
	fclose(fptr);

	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyN(cx, contents, strlen(contents))));
	delete[] contents;
	return JS_TRUE;
}

JSAPI_FUNC(filetools_writeText)
{
        if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
                THROW_ERROR(cx, "You must supply an original file name");

        char* orig = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        if(!isValidPath(orig))
                THROW_ERROR(cx, "Invalid file name");

        char porig[_MAX_PATH+_MAX_FNAME];
        sprintf_s(porig, sizeof(porig), "%s\\%s", Vars.szScriptPath, orig);

        StringReplace(porig, '/', '\\', strlen(porig));

        PathRemoveFileSpec(porig); 
        if(!PathFileExists(porig))
                THROW_ERROR(cx, "File path directory invalid");

        sprintf_s(porig, sizeof(porig), "%s\\%s", Vars.szScriptPath, orig);

        bool result = true;
        FILE* fptr = NULL;
        fopen_s(&fptr, porig, "w");
        
        for(uintN i = 1; i < argc; i++)
                if(!writeValue(fptr, cx, JS_ARGV(cx, vp)[i], false, true))
        fflush(fptr);
        fclose(fptr);
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(result));
        return JS_TRUE;
}

JSAPI_FUNC(filetools_appendText)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "You must supply an original file name");
	char* orig = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf_s(porig, sizeof(porig), "%s\\%s", Vars.szScriptPath, orig);

	bool result = true;
	FILE* fptr = NULL;
	if(fopen_s(&fptr, porig, "a+") != 0)
		THROW_ERROR(cx, _strerror("Failed to open file"));
	for(uintN i = 1; i < argc; i++)
		if(!writeValue(fptr, cx, JS_ARGV(cx, vp)[i], false, true))
			result = false;
	fflush(fptr);
	fclose(fptr);
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(result));

	return JS_TRUE;
}

