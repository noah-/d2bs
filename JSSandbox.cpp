#include "JSSandbox.h"
#include "D2BS.h"
#include "ScriptEngine.h"

JSAPI_FUNC(sandbox_ctor)
{
	sandbox* box = new sandbox; // leaked?
	box->context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
	if(!box->context)
	{
		delete box;
		return JS_TRUE;
	}
	box->innerObj = JS_NewObject(box->context, &global_obj, NULL, NULL);
	if(!box->innerObj)
	{
		JS_DestroyContext(box->context);
		delete box;
		return JS_TRUE;
	}

	if(JS_InitStandardClasses(box->context, box->innerObj) == JS_FALSE)
	{
		JS_DestroyContext(box->context);
		delete box;
		return JS_TRUE;
	}

	// TODO: add a default include function for sandboxed scripts
	// how do I do that individually though? :/

	JSObject* res = JS_NewObject(cx, &sandbox_class, NULL, NULL);
	if(JS_AddRoot(&res) == JS_FALSE)
	{
		JS_DestroyContext(box->context);
		delete box;
		return JS_TRUE;
	}
	if(!res || !JS_DefineFunctions(cx, res, sandbox_methods))
	{
		JS_RemoveRoot(&box->innerObj);
		JS_DestroyContext(box->context);
		delete box;
		return JS_TRUE;
	}
	JS_SetPrivate(cx, res, box);
	JS_RemoveRoot(&res);
	*rval = OBJECT_TO_JSVAL(res);

	return JS_TRUE;
}

JSAPI_PROP(sandbox_addProperty)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	JSContext* cxptr = (!box ? cx : box->context);
	JSObject* ptr = (!box ? obj : box->innerObj);

	if(JSVAL_IS_INT(id))
	{
		int32 i;
		if(JS_ValueToInt32(cx, id, &i) == JS_FALSE)
			return JS_TRUE;
		char name[32];
		_itoa_s(i, name, 32, 10);
		JSBool found;
		if(JS_HasProperty(cxptr, ptr, name, &found) == JS_FALSE)
			return JS_TRUE;
		if(found)
			return JS_TRUE;

		return JS_DefineProperty(cxptr, ptr, name, *vp, NULL, NULL, JSPROP_ENUMERATE);
	}
	else if(JSVAL_IS_STRING(id))
	{
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(id));
		JSBool found;
		if(JS_HasProperty(cxptr, ptr, name, &found) == JS_FALSE)
			return JS_TRUE;
		if(found)
			return JS_TRUE;

		return JS_DefineProperty(cxptr, ptr, name, *vp, NULL, NULL, JSPROP_ENUMERATE);
	}
	return JS_FALSE;
}

JSAPI_PROP(sandbox_delProperty)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);

	if(JSVAL_IS_INT(id))
	{
		int32 i;
		if(JS_ValueToInt32(cx, id, &i) == JS_FALSE)
			return JS_TRUE;
		char name[32];
		_itoa_s(i, name, 32, 10);
		if(box && JS_DeleteProperty(box->context, box->innerObj, name))
			return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(id))
	{
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(id));
		if(box && JS_DeleteProperty(box->context, box->innerObj, name))
			return JS_TRUE;
	}
	return JS_FALSE;
}

JSAPI_PROP(sandbox_getProperty)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);

	if(JSVAL_IS_INT(id))
	{
		int32 i;
		if(JS_ValueToInt32(cx, id, &i) == JS_FALSE)
			return JS_TRUE;
		char name[32];
		_itoa_s(i, name, 32, 10);
		*vp = JSVAL_VOID;
		if(box && JS_LookupProperty(box->context, box->innerObj, name, vp))
			return JS_TRUE;
		if(JSVAL_IS_VOID(*vp) && JS_LookupProperty(cx, obj, name, vp))
			return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(id))
	{
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(id));
		*vp = JSVAL_VOID;
		if(box && (JS_LookupProperty(box->context, box->innerObj, name, vp)))
			return JS_TRUE;
		if(JSVAL_IS_VOID(*vp) && JS_LookupProperty(cx, obj, name, vp))
			return JS_TRUE;
	}
	return JS_FALSE;
}

JSAPI_PROP(sandbox_setProperty)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);

	if(JSVAL_IS_INT(id))
	{
		int32 i;
		if(JS_ValueToInt32(cx, id, &i) == JS_FALSE)
			return JS_TRUE;
		char name[32];
		_itoa_s(i, name, 32, 10);
		if(box)
			if(JS_SetProperty(box->context, box->innerObj, name, vp))
				return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(id))
	{
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(id));
		if(box)
			if(JS_SetProperty(box->context, box->innerObj, name, vp))
				return JS_TRUE;
	}
	return JS_FALSE;
}

void sandbox_finalize(JSContext *cx, JSObject *obj)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	if(box) {
		JS_SetContextThread(box->context);
		JS_DestroyContext(box->context);
		delete box;
	}
}

JSAPI_FUNC(sandbox_eval)
{
	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
		if(!box)
			THROW_ERROR(cx, "Invalid execution object!");
		char* code = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		jsval result;
		if(JS_BufferIsCompilableUnit(box->context, box->innerObj, code, strlen(code)) &&
			JS_EvaluateScript(box->context, box->innerObj, code, strlen(code), "sandbox", 0, &result))
				*rval = result;
	} else THROW_ERROR(cx, "Invalid parameter, string expected");
	return JS_TRUE;
}

JSAPI_FUNC(sandbox_include)
{
	*rval = JSVAL_FALSE;
	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
		char* file = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		if(file && strlen(file) <= _MAX_FNAME && box)
		{
			char buf[_MAX_PATH+_MAX_FNAME];
			sprintf_s(buf, sizeof(buf), "%s\\libs\\%s", Vars.szScriptPath, file);
			if(box->list.count(std::string(file)) == -1)
			{
				JSScript* tmp = JS_CompileFile(box->context, box->innerObj, buf);
				if(tmp)
				{
					jsval result;
					if(JS_ExecuteScript(box->context, box->innerObj, tmp, &result))
					{
						box->list[file] = true;
						*rval = result;
					}
					JS_DestroyScript(cx, tmp);
				}
			}
		}
	}
	else
		THROW_ERROR(cx, "Invalid parameter, file expected");

	return JS_TRUE;
}

JSAPI_FUNC(sandbox_isIncluded)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	if(argc > 0 && JSVAL_IS_STRING(argv[0]) && box)
	{
		char* file = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		char buf[_MAX_PATH+_MAX_FNAME];
		sprintf_s(buf, sizeof(buf), "%s\\libs\\%s", Vars.szScriptPath, file);
		*rval = BOOLEAN_TO_JSVAL(!!box->list.count(std::string(buf)));
	} else THROW_ERROR(cx, "Invalid parameter, file expected");
	return JS_TRUE;
}

JSAPI_FUNC(sandbox_clear)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	if(box)
		JS_ClearScope(cx, box->innerObj);
	return JS_TRUE;
}

