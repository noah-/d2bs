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
#include <cstring>
#include <cstdlib>

#include <string>

#include <Windows.h>

#include "File.h"
#include "D2BS.h"

using namespace std;

const char* readLine(FILE* fptr, bool locking)
{
	if(feof(fptr))
		return NULL;
	string buffer;
	char c = 0;
	// grab all the characters in this line
	do {
		if(locking)
			c = (char)fgetc(fptr);
		else
			c = (char)_fgetc_nolock(fptr);
		// append the new character unless it's a carriage return
		if(c != '\r' && c != '\n' && !feof(fptr))
			buffer.append(1, c);
	} while(!feof(fptr) && c != '\n');
	return _strdup(buffer.c_str());
}

bool writeValue(FILE* fptr, JSContext* cx, jsval value, bool isBinary, bool locking)
{
	char* str;
	int len = 0, result;
	int32 ival = 0;
	jsdouble dval = 0;
	bool bval;

	switch(JS_TypeOfValue(cx, value))
	{
		case JSTYPE_VOID: case JSTYPE_NULL:
			if(locking)
				result = fwrite(&ival, sizeof(int), 1, fptr);
			else
				result = _fwrite_nolock(&ival, sizeof(int), 1, fptr);
			if(result == 1)
				return true;
			break;
		case JSTYPE_STRING:
			str = JS_EncodeString(cx,JSVAL_TO_STRING(value));
			if(locking)
				result = fwrite(str, sizeof(char), strlen(str), fptr);
			else
				result = _fwrite_nolock(str, sizeof(char), strlen(str), fptr);
			return (int)strlen(str) == result;
			JS_free(cx, str);
			break;
		case JSTYPE_NUMBER:
			if(isBinary)
			{
				if(JSVAL_IS_DOUBLE(value))
				{
					if(JS_ValueToNumber(cx, value, &dval))
					{
						if(locking)
							result = fwrite(&dval, sizeof(jsdouble), 1, fptr);
						else
							result = _fwrite_nolock(&dval, sizeof(jsdouble), 1, fptr);
						return result == 1;
					}
					else
						return false;
				}
				else if(JSVAL_IS_INT(value))
				{
					if(JS_ValueToInt32(cx, value, &ival))
					{
						if(locking)
							result = fwrite(&ival, sizeof(int32), 1, fptr);
						else
							result = _fwrite_nolock(&dval, sizeof(int32), 1, fptr);
						return result == 1;
					}
					else
						return false;
				}
			}
			else
			{
				if(JSVAL_IS_DOUBLE(value))
				{
					if(JS_ValueToNumber(cx, value, &dval) == JS_FALSE)
						return false;
					// jsdouble will never be a 64-char string, but I'd rather be safe than sorry
					str = new char[64];
					sprintf_s(str, 64, "%.16f", dval);
					len = strlen(str);
					if(locking)
						result = fwrite(str, sizeof(char), len, fptr);
					else
						result = _fwrite_nolock(str, sizeof(char), len, fptr);
					delete[] str;
					if(result == len)
						return true;
				}
				else if(JSVAL_IS_INT(value))
				{
					if(JS_ValueToInt32(cx, value, &ival) == JS_FALSE)
						return false;
					str = new char[16];
					_itoa_s(ival, str, 16, 10);
					len = strlen(str);
					if(locking)
						result = fwrite(str, sizeof(char), len, fptr);
					else
						result = _fwrite_nolock(str, sizeof(char), len, fptr);
					delete[] str;
					if(result == len)
						return true;
				}
			}
			break;
		case JSTYPE_BOOLEAN:
			if(!isBinary)
			{
				bval = !!JSVAL_TO_BOOLEAN(value);
				str = bval ? "true" : "false";
				if(locking)
					result = fwrite(str, sizeof(char), strlen(str), fptr);
				else
					result = _fwrite_nolock(str, sizeof(char), strlen(str), fptr);
				return (int)strlen(str) == result;
			}
			else
			{
				bval = !!JSVAL_TO_BOOLEAN(value);
				if(locking)
					result = fwrite(&bval, sizeof(bool), 1, fptr);
				else
					result = _fwrite_nolock(&bval, sizeof(bool), 1, fptr);
				return result == 1;
			}
			break;
/*		case JSTYPE_OBJECT:
			JSObject *arr = JSVAL_TO_OBJECT(value);
			if(JS_IsArrayObject(cx, arr)) {
				JS_GetArrayLength(cx, arr, &uival);
				for(jsuint i = 0; i < uival; i++)
				{
					jsval val;
					JS_GetElement(cx, arr, i, &val);
					if(!writeValue(fptr, cx, val, isBinary))
						return false;
				}
				return true;
			}
			else
			{
				JSString* jsstr = JS_ValueToString(cx, value);
				str = JS_EncodeString(cx,jsstr);
				if(locking)
					result = fwrite(str, sizeof(char), strlen(str), fptr);
				else
					result = _fwrite_nolock(str, sizeof(char), strlen(str), fptr);
				return strlen(str) == result;
			}
			break;
*/
	}
	return false;
}

/** Safely open a file relative to the script dir.
 *
 * In theory this is the only function used to open files that is exposed to
 * javascript.
 *
 * \param filename Name of the file to open relative to the script folder
 *
 * \param mode Mode to open in. See fopen_s.
 *
 * \param cx JSContext that is running. Used to throw errors.
 *
 * \return The file pointer.
 */
FILE* fileOpenRelScript(const char* filename, const char* mode, JSContext* cx)
{
	FILE* f;
	char fullPath[_MAX_PATH+_MAX_FNAME];

	// Get the relative path
	if(getPathRelScript(filename, _MAX_PATH+_MAX_FNAME, fullPath) == NULL)
	{
		JS_ReportError(cx, "Invalid file name");
		return NULL;
	}

	// Open the file
	if(fopen_s(&f, fullPath, mode) != 0 || f == NULL)
	{
		JS_ReportError(cx, "Couldn't open file %s: %s", filename, _strerror(NULL));
		return NULL;
	}

	return f;
}

/** Get the full path relative to the script path. Does the validation check.
 *
 * \param filename Name of the file to open relative to the script folder.
 *
 * \param bufLen Length of the output buffer.
 *
 * \param fullPath Buffer where the full path will be stored. Empty string if
 *    location is invalid.
 *
 * \return fullPath on success or NULL on failure.
 */
char* getPathRelScript(const char* filename, int bufLen, char* fullPath)
{
	char fullScriptPath[_MAX_PATH+_MAX_FNAME];
	char* relPath;
	int strLenScript;
	DWORD scrPathLen;

	strLenScript = strlen(Vars.szScriptPath);

	// Make the filename relative to the script path
	relPath = (char*)_alloca(strLenScript+strlen(filename)+2);
	strcpy_s(relPath, strLenScript+strlen(filename)+2, Vars.szScriptPath);
	relPath[strLenScript] = '\\';
	strcpy_s(relPath+strLenScript+1, strlen(filename)+1, filename);

	// Transform to the full pathname
	GetFullPathName(relPath, bufLen, fullPath, NULL);

	// Get the full path of the script path, check it is the prefix of fullPath
	scrPathLen = GetFullPathName(Vars.szScriptPath, sizeof(fullScriptPath),
		fullScriptPath, NULL);

	// Check that fullScriptPath is the prefix of fullPath
	// As GetFullPathName seems to not add a trailing \, if there is not a
	// trailing \ in fullScriptPath check for it in fullPath
	if(strncmp(fullPath, fullScriptPath, scrPathLen) != 0 ||
	    (fullScriptPath[scrPathLen-1] != '\\' &&
	     fullPath[scrPathLen] != '\\'))
	{
		fullPath[0] = '\0';
		return NULL;
	}

	return fullPath;
}

/** Check that the full path of the script path is the prefix of the fullpath
 * of name. Also checks that there is no ..\ or ../ sequences or ":?*<>| chars.
 *
 * \param name The file/path to validate.
 *
 * \return true if path is valid, false otherwise.
 */
bool isValidPath(const char* name)
{
	char fullPath[_MAX_PATH+_MAX_FNAME];

	// Use getPathRelScript to validate based on full paths
	if(getPathRelScript(name, _MAX_PATH+_MAX_FNAME, fullPath) == NULL)
		return false;

	return (!strstr(name, "..\\") && !strstr(name, "../") && (strcspn(name, "\":?*<>|") == strlen(name)));
}
