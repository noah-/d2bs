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
	if(JS_AddRoot(cx, &res) == JS_FALSE)
	{
		JS_DestroyContext(box->context);
		delete box;
		return JS_TRUE;
	}
	if(!res || !JS_DefineFunctions(cx, res, sandbox_methods))
	{
		JS_RemoveRoot(cx, &box->innerObj);
		JS_DestroyContext(box->context);
		delete box;
		return JS_TRUE;
	}
	JS_SetPrivate(cx, res, box);
	JS_RemoveRoot(cx, &res);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res));

	return JS_TRUE;
}

JSAPI_PROP(sandbox_addProperty)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	JSContext* cxptr = (!box ? cx : box->context);
	JSObject* ptr = (!box ? obj : box->innerObj);
	jsval ID;
	JS_IdToValue(cx,id,&ID);


	if(JSVAL_IS_INT(ID))
	{
		int32 i;
		if(JS_ValueToInt32(cx, ID, &i) == JS_FALSE)
			return JS_TRUE;
		char name[32];
		_itoa_s(i, name, 32, 10);
		JSBool found;
		if(JS_HasProperty(cxptr, ptr, name, &found) == JS_FALSE)
			return JS_TRUE;
		if(found)
			return JS_TRUE;

		return JS_DefineProperty(cxptr, ptr, name, vp.get(), NULL, NULL, JSPROP_ENUMERATE);
	}
	else if(JSVAL_IS_STRING(ID))
	{
		char* name = JS_EncodeString(cx,JSVAL_TO_STRING(ID));
		JSBool found;
		if(JS_HasProperty(cxptr, ptr, name, &found) == JS_FALSE)
			return JS_TRUE;
		if(found)
			return JS_TRUE;

		return JS_DefineProperty(cxptr, ptr, name, vp.get(), NULL, NULL, JSPROP_ENUMERATE);
	}
	return JS_FALSE;
}

JSAPI_PROP(sandbox_delProperty)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	jsval ID;
	JS_IdToValue(cx,id,&ID);
	
	if(JSVAL_IS_INT(ID))
	{
		int32 i;
		if(JS_ValueToInt32(cx, ID, &i) == JS_FALSE)
			return JS_TRUE;
		char name[32];
		_itoa_s(i, name, 32, 10);
		if(box && JS_DeleteProperty(box->context, box->innerObj, name))
			return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(ID))
	{
		char* name = JS_EncodeString(cx,JSVAL_TO_STRING(ID));
		if(box && JS_DeleteProperty(box->context, box->innerObj, name))
			return JS_TRUE;
	}
	return JS_FALSE;
}

JSAPI_PROP(sandbox_getProperty)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	jsval ID;
	JS_IdToValue(cx,id,&ID);

	if(JSVAL_IS_INT(ID))
	{
		int32 i;
		if(JS_ValueToInt32(cx, ID, &i) == JS_FALSE)
			return JS_TRUE;
		char name[32];
		_itoa_s(i, name, 32, 10);
		vp.set(JSVAL_VOID);
		if(box && JS_LookupProperty(box->context, box->innerObj, name, &vp.get()))
			return JS_TRUE;
		if(JSVAL_IS_VOID(vp.get()) && JS_LookupProperty(cx, obj, name, &vp.get()))
			return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(ID))
	{
		char* name = JS_EncodeString(cx,JSVAL_TO_STRING(ID));
		vp.set(JSVAL_VOID);
		if(box && (JS_LookupProperty(box->context, box->innerObj, name, &vp.get())))
			return JS_TRUE;
		if(JSVAL_IS_VOID(vp.get()) && JS_LookupProperty(cx, obj, name, &vp.get()))
			return JS_TRUE;
	}
	return JS_FALSE;
}

JSAPI_STRICT_PROP(sandbox_setProperty)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	jsval ID;
	JS_IdToValue(cx,id,&ID);
	
	if(JSVAL_IS_INT(ID))
	{
		int32 i;
		if(JS_ValueToInt32(cx, ID, &i) == JS_FALSE)
			return JS_TRUE;
		char name[32];
		_itoa_s(i, name, 32, 10);
		if(box)
			if(JS_SetProperty(box->context, box->innerObj, name, &vp.get()))
				return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(ID))
	{
		char* name = JS_EncodeString(cx,JSVAL_TO_STRING(ID));
		if(box)
			if(JS_SetProperty(box->context, box->innerObj, name, &vp.get()))
				return JS_TRUE;
	}
	return JS_FALSE;
}

void sandbox_finalize(JSFreeOp *fop, JSObject *obj)
{
	sandbox* box = (sandbox*)JS_GetPrivate(obj);
	if(box) {
//bob1.8.8		JS_SetContextThread(box->context);
		JS_DestroyContext(box->context);
		delete box;
	}
}

JSAPI_FUNC(sandbox_eval)
{
	if(argc > 0 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
	{
		sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sandbox_class, NULL);
		if(!box)
			THROW_ERROR(cx, "Invalid execution object!");
		char* code = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
		jsval result;
		if(JS_BufferIsCompilableUnit(box->context, box->innerObj, code, strlen(code)) &&
			JS_EvaluateScript(box->context, box->innerObj, code, strlen(code), "sandbox", 0, &result))
				JS_SET_RVAL(cx, vp, result);
	} else THROW_ERROR(cx, "Invalid parameter, string expected");
	return JS_TRUE;
}

JSAPI_FUNC(sandbox_include)
{
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	if(argc > 0 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
	{
		sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sandbox_class, NULL);
		char* file = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
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
						JS_SET_RVAL(cx, vp, result);
					}
					//nolonger needed?
					//JS_DestroyScript(cx, tmp);
				}
			//JS_RemoveScriptRoot(cx, &tmp);
			}
		}
	}
	else
		THROW_ERROR(cx, "Invalid parameter, file expected");

	return JS_TRUE;
}

JSAPI_FUNC(sandbox_isIncluded)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sandbox_class, NULL);
	if(argc > 0 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]) && box)
	{
		char* file = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
		char buf[_MAX_PATH+_MAX_FNAME];
		sprintf_s(buf, sizeof(buf), "%s\\libs\\%s", Vars.szScriptPath, file);
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!!box->list.count(std::string(buf))));
	} else THROW_ERROR(cx, "Invalid parameter, file expected");
	return JS_TRUE;
}

JSAPI_FUNC(sandbox_clear)
{
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sandbox_class, NULL);
	//if(box)
	//	JS_ClearScope(cx, box->innerObj);
	return JS_TRUE;
}

