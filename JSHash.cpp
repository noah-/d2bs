#include "JSHash.h"
#include "Hash.h"
#include "File.h"
#include "D2BS.h"
#include "Helpers.h"

JSAPI_FUNC(my_md5) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    char* input = JS_EncodeStringToUTF8(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    char* result = md5(input);
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
    delete[] result;
    JS_free(cx, input);
    return JS_TRUE;
}

JSAPI_FUNC(my_sha1) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    char* input = JS_EncodeStringToUTF8(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    char* result = sha1(input);
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
    delete[] result;
    JS_free(cx, input);
    return JS_TRUE;
}

JSAPI_FUNC(my_sha256) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    char* input = JS_EncodeStringToUTF8(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    char* result = sha256(input);
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
    delete[] result;
    JS_free(cx, input);
    return JS_TRUE;
}

JSAPI_FUNC(my_sha384) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    char* input = JS_EncodeStringToUTF8(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    char* result = sha384(input);
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
    delete[] result;
    JS_free(cx, input);
    return JS_TRUE;
}

JSAPI_FUNC(my_sha512) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    char* input = JS_EncodeStringToUTF8(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    char* result = sha512(input);
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, result)));
    delete[] result;
    JS_free(cx, input);
    return JS_TRUE;
}

JSAPI_FUNC(my_md5_file) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    const wchar_t* file = JS_GetStringCharsZ(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    if (!(file && file[0] && isValidPath(file)))
        THROW_ERROR(cx, "Invalid file path!");

    wchar_t path[_MAX_FNAME + _MAX_PATH];
    wprintf_s(path, _MAX_FNAME + _MAX_PATH, L"%s\\%s", Vars.szScriptPath, file);

    char* p = UnicodeToAnsi(path);
    char* result = md5_file(p);
    delete[] p;
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
    delete[] result;
    return JS_TRUE;
}

JSAPI_FUNC(my_sha1_file) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    const wchar_t* file = JS_GetStringCharsZ(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    if (!(file && file[0] && isValidPath(file)))
        THROW_ERROR(cx, "Invalid file path!");

    wchar_t path[_MAX_FNAME + _MAX_PATH];
    wprintf_s(path, _MAX_FNAME + _MAX_PATH, L"%s\\%s", Vars.szScriptPath, file);

    char* p = UnicodeToAnsi(path);
    char* result = sha1_file(p);
    delete[] p;
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
    delete[] result;
    return JS_TRUE;
}

JSAPI_FUNC(my_sha256_file) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    const wchar_t* file = JS_GetStringCharsZ(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    if (!(file && file[0] && isValidPath(file)))
        THROW_ERROR(cx, "Invalid file path!");

    wchar_t path[_MAX_FNAME + _MAX_PATH];
    wprintf_s(path, _MAX_FNAME + _MAX_PATH, L"%s\\%s", Vars.szScriptPath, file);

    char* p = UnicodeToAnsi(path);
    char* result = sha256_file(p);
    delete[] p;
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
    delete[] result;
    return JS_TRUE;
}

JSAPI_FUNC(my_sha384_file) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    const wchar_t* file = JS_GetStringCharsZ(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    if (!(file && file[0] && isValidPath(file)))
        THROW_ERROR(cx, "Invalid file path!");

    wchar_t path[_MAX_FNAME + _MAX_PATH];
    wprintf_s(path, _MAX_FNAME + _MAX_PATH, L"%s\\%s", Vars.szScriptPath, file);

    char* p = UnicodeToAnsi(path);
    char* result = sha384_file(p);
    delete[] p;
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
    delete[] result;
    return JS_TRUE;
}

JSAPI_FUNC(my_sha512_file) {
    if (argc != 1)
        THROW_ERROR(cx, "Invalid arguments");

    const wchar_t* file = JS_GetStringCharsZ(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    if (!(file && file[0] && isValidPath(file)))
        THROW_ERROR(cx, "Invalid file path!");

    wchar_t path[_MAX_FNAME + _MAX_PATH];
    wprintf_s(path, _MAX_FNAME + _MAX_PATH, L"%s\\%s", Vars.szScriptPath, file);

    char* p = UnicodeToAnsi(path);
    char* result = sha512_file(p);
    delete[] p;
    if (result && result[0])
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result)));
    delete[] result;
    return JS_TRUE;
}
