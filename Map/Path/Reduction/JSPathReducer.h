#pragma once

#include "PathReducer.h"
#include "../Path.h"
#include "../../Diablo_II/LevelMap.h"
#include "../../../js32.h"

namespace Mapping
{
namespace Pathing
{
namespace Reducing
{

#pragma warning ( disable: 4512 )

class JSPathReducer : public PathReducer
{
private:
	JSContext* cx;
	JSObject* obj;
	jsval reject, reduce, mutate;

public:
	JSPathReducer(const JSPathReducer&);
	JSPathReducer& operator=(const JSPathReducer&);
	JSPathReducer(LevelMap* m, JSContext* cx, JSObject* obj, jsval _reject, jsval _reduce, jsval _mutate) :
		reject(_reject), reduce(_reduce), mutate(_mutate) { JS_AddRoot(&reject); JS_AddRoot(&reduce); JS_AddRoot(&mutate); }
	~JSPathReducer(void) { JS_RemoveRoot(&reject); JS_RemoveRoot(&reduce); JS_RemoveRoot(&mutate); }

	void Reduce(PointList const & in, PointList& out, bool abs)
	{
		// create the initial array to pass to the js function
		int count = in.size();

		JS_EnterLocalRootScope(cx);

		jsval* vec = new jsval[count];
		for(int i = 0; i < count; i++)
		{
			jsval x = INT_TO_JSVAL(in[i].first),
				  y = INT_TO_JSVAL(in[i].second);

			JSObject* pt = BuildObject(cx);
			JS_SetProperty(cx, pt, "x", &x);
			JS_SetProperty(cx, pt, "y", &y);

			vec[i] = OBJECT_TO_JSVAL(pt);
		}
		JSObject* arr = JS_NewArrayObject(cx, count, vec);

		jsval argv[] = { JSVAL_NULL, JSVAL_ZERO, OBJECT_TO_JSVAL(arr) };
		for(int i = 0; i < count; i++)
		{
			jsval rval = JSVAL_FALSE;
			argv[0] = vec[i];
			argv[1] = INT_TO_JSVAL(i);
			JS_CallFunctionValue(cx, obj, reduce, 3, argv, &rval);
			if(!!JSVAL_TO_BOOLEAN(rval))
				out.push_back(in[i]);
		}

		JS_LeaveLocalRootScope(cx);
		delete[] vec;
	}
	bool Reject(Point const & pt, bool abs)
	{
		jsval rval = JSVAL_FALSE;
		jsval argv[] = { INT_TO_JSVAL(pt.first), INT_TO_JSVAL(pt.second) };
		JS_CallFunctionValue(cx, obj, reject, 2, argv, &rval);
		return !!JSVAL_TO_BOOLEAN(rval);
	}
	void MutatePoint(Point & pt, bool abs)
	{
		jsval rval = JSVAL_FALSE;
		jsval argv[] = { INT_TO_JSVAL(pt.first), INT_TO_JSVAL(pt.second) };
		JS_CallFunctionValue(cx, obj, mutate, 2, argv, &rval);
		if(JSVAL_IS_OBJECT(rval))
		{
			JS_GetElement(cx, JSVAL_TO_OBJECT(rval), 0, &argv[0]);
			JS_GetElement(cx, JSVAL_TO_OBJECT(rval), 1, &argv[1]);
			pt.first = JSVAL_TO_INT(argv[0]);
			pt.second = JSVAL_TO_INT(argv[1]);
		}
	}
};

#pragma warning ( default: 4512 )

}
}
}
