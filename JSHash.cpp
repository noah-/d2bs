#include "JSHash.h"
#include "Hash.h"
#include "File.h"
#include "D2BS.h"

JSAPI_FUNC(my_md5)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* result = md5(JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_sha1)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* result = sha1(JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_sha256)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* result = sha256(JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_sha384)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* result = sha384(JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_sha512)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* result = sha512(JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_md5_file)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* file = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	if(!(file && file[0] && isValidPath(file)))
		THROW_ERROR(cx, "Invalid file path!");

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\%s", Vars.szScriptPath, file);

	char* result = md5_file(path);
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_sha1_file)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* file = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	if(!(file && file[0] && isValidPath(file)))
		THROW_ERROR(cx, "Invalid file path!");

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\%s", Vars.szScriptPath, file);

	char* result = sha1_file(path);
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_sha256_file)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* file = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	if(!(file && file[0] && isValidPath(file)))
		THROW_ERROR(cx, "Invalid file path!");

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\%s", Vars.szScriptPath, file);

	char* result = sha256_file(path);
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_sha384_file)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* file = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	if(!(file && file[0] && isValidPath(file)))
		THROW_ERROR(cx, "Invalid file path!");

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\%s", Vars.szScriptPath, file);

	char* result = sha384_file(path);
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
	delete result;
	return JS_TRUE;
}

JSAPI_FUNC(my_sha512_file)
{
	if(argc != 1)
		THROW_ERROR(cx, "Invalid arguments");

	char* file = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	if(!(file && file[0] && isValidPath(file)))
		THROW_ERROR(cx, "Invalid file path!");

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\%s", Vars.szScriptPath, file);

	char* result = sha512_file(path);
	if(result && result[0])
		JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
	delete result;
	return JS_TRUE;
}

