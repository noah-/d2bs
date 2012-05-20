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

#include <set>

#include "JSSQLite.h"
#include "D2BS.h"
#include "File.h"

using namespace std;

struct SqliteDB;
struct DBStmt;
typedef std::set<DBStmt*> StmtList;

struct SqliteDB {
	sqlite3* db;
	bool open;
	char* path;
	StmtList stmts;
};

struct DBStmt {
	sqlite3_stmt* stmt;
	bool open, canGet;
	SqliteDB* assoc_db;
	JSObject *current_row;
};

void close_db_stmt(DBStmt* stmt);
bool clean_sqlite_db(SqliteDB* db);

bool clean_sqlite_db(SqliteDB* db) {
	if(db && db->open) {
		for(StmtList::iterator it = db->stmts.begin(); it != db->stmts.end(); it++) {
			close_db_stmt(*it);
		}
		db->stmts.clear();
		if(SQLITE_OK != sqlite3_close(db->db)) return false;
	}
	return true;
}

void close_db_stmt(DBStmt* stmt) {
	if(stmt->stmt && stmt->open) {
		sqlite3_finalize(stmt->stmt);
		stmt->stmt = NULL;
		stmt->open = false;
	}
}

JSAPI_FUNC(my_sqlite_version)
{
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, sqlite3_version)));
	return JS_TRUE;
}

JSAPI_FUNC(my_sqlite_memusage)
{
	jsval* rval;
	JS_NewNumberValue(cx, (jsdouble)sqlite3_memory_used(), rval);
	JS_SET_RVAL(cx, vp, rval);
	return JS_TRUE;
}

JSAPI_EMPTY_CTOR(sqlite_stmt)

JSAPI_FUNC(sqlite_ctor)
{
	if(argc > 0 && !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "Invalid parameters in SQLite constructor");
	char* path = ":memory:";
	if(argc > 0)
		path = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

	// if the path is not a special placeholder (:memory:, et. al.), sandbox it
	if(path[0] != ':') {
		if(!isValidPath(path))
			THROW_ERROR(cx, "Invalid characters in database name");

		char* tmp = new char[_MAX_PATH+_MAX_FNAME];
		sprintf_s(tmp, _MAX_PATH+_MAX_FNAME, "%s\\%s", Vars.szScriptPath, path);
		path = _strdup(tmp);
		delete[] tmp;
	}

	bool autoOpen = true;
	if(JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[1]))
		autoOpen = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]);

	sqlite3* db = NULL;
	if(autoOpen) {
		if(SQLITE_OK != sqlite3_open(path, &db)) {
			char msg[1024];
			sprintf_s(msg, sizeof(msg), "Could not open database: %s", sqlite3_errmsg(db));
			THROW_ERROR(cx, msg);
		}
	}


	SqliteDB* dbobj = new SqliteDB; // leaked?
	dbobj->db = db;
	dbobj->open = autoOpen;
	dbobj->path = _strdup(path);

	// if the path is not a special placeholder, it needs to be freed
	if(path[0] != ':')
		free(path);

	JSObject* jsdb = BuildObject(cx, &sqlite_db, sqlite_methods, sqlite_props, dbobj);
	if(!jsdb) {
		sqlite3_close(db);
		free(dbobj->path);
		delete dbobj;
		THROW_ERROR(cx, "Could not create the sqlite object");
	}
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsdb));

	return JS_TRUE;
}

JSAPI_FUNC(sqlite_execute)
{
	SqliteDB* dbobj = (SqliteDB*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_db, NULL);
	if(dbobj->open != true)
		THROW_ERROR(cx, "Database must first be opened!");
	if(argc < 1 || argc > 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "Invalid parameters in SQLite.execute");

	char* sql = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0])), *err = NULL;
	if(SQLITE_OK != sqlite3_exec(dbobj->db, sql, NULL, NULL, &err)) {
		char msg[2048];
		strcpy_s(msg, sizeof(msg), err);
		sqlite3_free(err);
		THROW_ERROR(cx, msg);
	}
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_query)
{
	SqliteDB* dbobj = (SqliteDB*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_db, NULL);
	if(dbobj->open != true)
		THROW_ERROR(cx, "Database must first be opened!");
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "Invalid parameters to SQLite.query");

	char* sql = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
	sqlite3_stmt* stmt;
	if(SQLITE_OK != sqlite3_prepare_v2(dbobj->db, sql, strlen(sql), &stmt, NULL)) {
		THROW_ERROR(cx, sqlite3_errmsg(dbobj->db));
	}
	if(stmt == NULL) {
		THROW_ERROR(cx, "Statement has no effect");
	}

	for(uintN i = 1; i < argc; i++) {
		switch(JS_TypeOfValue(cx, JS_ARGV(cx, vp)[i])) {
			case JSTYPE_VOID: 
				sqlite3_bind_null(stmt, i);
				break;
			case JSTYPE_STRING:
				sqlite3_bind_text(stmt, i, JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[i])), -1, SQLITE_STATIC);
				break;
			case JSTYPE_NUMBER:
				if(JSVAL_IS_DOUBLE(JS_ARGV(cx, vp)[i]))
					sqlite3_bind_double(stmt, i, *(jsdouble)JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[i]));
				else if(JSVAL_IS_INT(JS_ARGV(cx, vp)[i]))
					sqlite3_bind_int(stmt, i, JSVAL_TO_INT(JS_ARGV(cx, vp)[i]));
				break;
			case JSTYPE_BOOLEAN:
				sqlite3_bind_text(stmt, i, JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[i]) ? "true" : "false", -1, SQLITE_STATIC);
				break;
			default:
				sqlite3_finalize(stmt);
				char msg[1024];
				sprintf_s(msg, sizeof(msg), "Invalid bound parameter %i", i);
				THROW_ERROR(cx, msg);
				break;
		}
	}

	DBStmt* dbstmt = new DBStmt;
	dbstmt->stmt = stmt;
	dbstmt->open = true;
	dbstmt->canGet = false;
	dbstmt->assoc_db = dbobj;
	dbstmt->current_row = NULL;
	dbobj->stmts.insert(dbstmt);

	JSObject* row = BuildObject(cx, &sqlite_stmt, sqlite_stmt_methods, sqlite_stmt_props, dbstmt);
	if(!row) {
		sqlite3_finalize(stmt);
		delete dbstmt;
		THROW_ERROR(cx, "Could not create the sqlite row object");
	}
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(row));
	return JS_TRUE;
}


JSAPI_FUNC(sqlite_close)
{
	SqliteDB* dbobj = (SqliteDB*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_db, NULL);
	if(!clean_sqlite_db(dbobj)) {
		char msg[1024];
		sprintf_s(msg, sizeof(msg), "Could not close database: %s", sqlite3_errmsg(dbobj->db));
		THROW_ERROR(cx, msg);
	}
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_open)
{
	SqliteDB* dbobj = (SqliteDB*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_db, NULL);
	if(!dbobj->open) {
		if(SQLITE_OK != sqlite3_open(dbobj->path, &dbobj->db)) {
			char msg[1024];
			sprintf_s(msg, sizeof(msg), "Could not open database: %s", sqlite3_errmsg(dbobj->db));
			THROW_ERROR(cx, msg);
		}
	}
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	return JS_TRUE;
}

JSAPI_PROP(sqlite_getProperty)
{
	SqliteDB* dbobj = (SqliteDB*)JS_GetInstancePrivate(cx, obj, &sqlite_db, NULL);

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) {
		case SQLITE_PATH:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, dbobj->path));
			break;
		case SQLITE_OPEN:
			*vp = BOOLEAN_TO_JSVAL(dbobj->open);
			break;
		case SQLITE_LASTROWID:
			*vp = INT_TO_JSVAL(sqlite3_last_insert_rowid(dbobj->db));
			break;
		case SQLITE_STMTS: {
				JSObject *stmts = JS_NewArrayObject(cx, dbobj->stmts.size(), NULL);
				*vp = OBJECT_TO_JSVAL(stmts);
				int i = 0;
				for(StmtList::iterator it = dbobj->stmts.begin(); it != dbobj->stmts.end(); it++, i++) {
					if((*it)->open) {
						JSObject* stmt = BuildObject(cx, &sqlite_stmt, sqlite_stmt_methods, sqlite_stmt_props, *it);
						jsval tmp = OBJECT_TO_JSVAL(stmt);
						JS_SetElement(cx, stmts, i, &tmp);
					}
				}
			}
			break;
		case SQLITE_CHANGES:
			JS_NewNumberValue(cx, (jsdouble)sqlite3_changes(dbobj->db), vp);
			break;
	}
	return JS_TRUE;
}

void sqlite_finalize(JSContext *cx, JSObject *obj)
{
	SqliteDB* dbobj = (SqliteDB*)JS_GetInstancePrivate(cx, obj, &sqlite_db, NULL);
	JS_SetPrivate(cx, obj, NULL);
	if(dbobj) {
		clean_sqlite_db(dbobj);
		delete[] dbobj->path;
		delete dbobj;
	}
}

JSBool sqlite_equal(JSContext *cx, JSObject *obj, jsval v, JSBool *bp)
{
	SqliteDB* dbobj = (SqliteDB*)JS_GetInstancePrivate(cx, obj, &sqlite_db, NULL);
	if(!JSVAL_IS_OBJECT(v))
		return JS_TRUE;
	JSObject *obj2 = JSVAL_TO_OBJECT(v);
	if(!obj2 || JS_GET_CLASS(cx, obj2) != JS_GET_CLASS(cx, obj))
		return JS_TRUE;

	SqliteDB* dbobj2 = (SqliteDB*)JS_GetPrivate(cx, obj2);
	if(dbobj2->db != dbobj->db)
		return JS_TRUE;

	*bp = JS_TRUE;
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_getobject)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);
	sqlite3_stmt *stmt = stmtobj->stmt;

	if(!stmtobj->canGet) {
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
		return JS_TRUE;
	}
	if(stmtobj->current_row) {
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(stmtobj->current_row));
		return JS_TRUE;
	}

	int cols = sqlite3_column_count(stmt);
	if(cols == 0) {
		JS_SET_RVAL(cx, vp, JSVAL_TRUE);
		return JS_TRUE;
	}

	JSObject *obj2 = JS_ConstructObject(cx, NULL, NULL, NULL);
	jsval val;
	if(!obj2)
		THROW_ERROR(cx, "Failed to create row result object");
	for(int i = 0; i < cols; i++) {
		const char* colnam = sqlite3_column_name(stmt, i);
		switch(sqlite3_column_type(stmt, i)) {
			case SQLITE_INTEGER:
				// jsdouble == double, so this conversion is no problem
				JS_NewNumberValue(cx, (jsdouble)sqlite3_column_int64(stmt, i), &val);
				if(!JS_SetProperty(cx, obj2, colnam, &val))
					THROW_ERROR(cx, "Failed to add column to row results");
				break;
			case SQLITE_FLOAT:
				JS_NewNumberValue(cx, sqlite3_column_double(stmt, i), &val);
				if(!JS_SetProperty(cx, obj2, colnam, &val))
					THROW_ERROR(cx, "Failed to add column to row results");
				break;
			case SQLITE_TEXT:
				val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, reinterpret_cast<const char*>(sqlite3_column_text(stmt, i))));
				if(!JS_SetProperty(cx, obj2, colnam, &val))
					THROW_ERROR(cx, "Failed to add column to row results");
				break;
			case SQLITE_BLOB:
				// currently not supported
				THROW_ERROR(cx, "Blob type not supported (yet)");
				break;
			case SQLITE_NULL:
				if(!JS_SetProperty(cx, obj2, colnam, (jsval*) JSVAL_NULL))
					THROW_ERROR(cx, "Failed to add column to row results");
				break;
		}
	}
	stmtobj->current_row = obj2;
	if(JS_AddRoot(&stmtobj->current_row) == JS_FALSE)
		return JS_TRUE;
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj2));
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_colcount)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);
	sqlite3_stmt *stmt = stmtobj->stmt;

	if(!stmtobj->canGet)
		THROW_ERROR(cx, "Statement is not ready");

	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_column_count(stmt)));
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_colval)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);
	sqlite3_stmt *stmt = stmtobj->stmt;

	if(!stmtobj->canGet)
		THROW_ERROR(cx, "Statement is not ready");

	if(argc < 1 || argc > 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "Invalid parameter for SQLiteStatement.getColumnValue");

	int i = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	switch(sqlite3_column_type(stmt, i)) {
		case SQLITE_INTEGER:
			// jsdouble == double, so this conversion is no problem
			JS_NewNumberValue(cx, (jsdouble)sqlite3_column_int64(stmt, i), rval);
			break;
		case SQLITE_FLOAT:
			JS_NewNumberValue(cx, sqlite3_column_double(stmt, i), rval);
			break;
		case SQLITE_TEXT:
			JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)))));
			break;
		case SQLITE_BLOB:
			// currently not supported
			THROW_ERROR(cx, "Blob type not supported (yet)");
			break;
		case SQLITE_NULL:
			JS_SET_RVAL(cx, vp, JSVAL_NULL);
			break;
	}
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_colname)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);
	sqlite3_stmt *stmt = stmtobj->stmt;

	if(!stmtobj->canGet)
		THROW_ERROR(cx, "Statement is not ready");

	if(argc < 1 || argc > 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "Invalid parameter for SQLiteStatement.getColumnValue");

	int i = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	JS_SET_RVAL(cx, vp,  STRING_TO_JSVAL(JS_NewStringCopyZ(cx, sqlite3_column_name(stmt, i))));
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_execute)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);

	int res = sqlite3_step(stmtobj->stmt);

	if(SQLITE_ROW != res && SQLITE_DONE != res)
		THROW_ERROR(cx, sqlite3_errmsg(stmtobj->assoc_db->db));
	close_db_stmt(stmtobj);
	JS_SET_RVAL(cx, vp,  BOOLEAN_TO_JSVAL((SQLITE_DONE == res)));
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_bind)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);
	sqlite3_stmt* stmt = stmtobj->stmt;
	if(argc < 2 || argc > 2 || !(JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]) || JSVAL_IS_INT(JS_ARGV(cx, vp)[0])))
		THROW_ERROR(cx, "Invalid parameters for SQLiteStatement.bind");

	int colnum = -1;
	if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		colnum = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	else
		colnum = sqlite3_bind_parameter_index(stmt, JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0])));

	if(colnum == 0)
		THROW_ERROR(cx, "Invalid parameter number, parameters start at 1");

	switch(JS_TypeOfValue(cx, JS_ARGV(cx, vp)[1])) {
		case JSTYPE_VOID: 
			sqlite3_bind_null(stmt, colnum);
			break;
		case JSTYPE_STRING:
			sqlite3_bind_text(stmt, colnum, JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[1])), -1, SQLITE_STATIC);
			break;
		case JSTYPE_NUMBER:
			if(JSVAL_IS_DOUBLE(JS_ARGV(cx, vp)[1]))
				sqlite3_bind_double(stmt, colnum, *(double*)JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]));
			else if(JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
				sqlite3_bind_int(stmt, colnum, JSVAL_TO_INT(JS_ARGV(cx, vp)[1]));
			break;
		case JSTYPE_BOOLEAN:
			sqlite3_bind_text(stmt, colnum, JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]) ? "true" : "false", -1, SQLITE_STATIC);
			break;
		default:
			THROW_ERROR(cx, "Invalid bound parameter");
			break;
	}

	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_next)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);

	int res = sqlite3_step(stmtobj->stmt);

	if(SQLITE_ROW != res && SQLITE_DONE != res)
		THROW_ERROR(cx, sqlite3_errmsg(stmtobj->assoc_db->db));

	stmtobj->canGet = !!(SQLITE_ROW == res);
	if(stmtobj->current_row) {
		JS_RemoveRoot(&stmtobj->current_row);
		stmtobj->current_row = NULL;
	}
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL((SQLITE_ROW == res)));
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_skip)
{
	JS_SET_RVAL(cx, vp, JS_ARGV(cx, vp)[0]);
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		THROW_ERROR(cx, "Invalid parameter to SQLiteStatement.skip");
	for(int i = JSVAL_TO_INT(JS_ARGV(cx, vp)[0])-1; i >= 0; i++) {
		int res = sqlite3_step(stmtobj->stmt);
		if(res != SQLITE_ROW) {
			if(res == SQLITE_DONE) {
				JS_SET_RVAL(cx, vp, INT_TO_JSVAL((JSVAL_TO_INT(JS_ARGV(cx, vp)[0]-1)-i)));
				stmtobj->canGet = false;
				i = 0;
				continue;
			}
			THROW_ERROR(cx, sqlite3_errmsg(stmtobj->assoc_db->db));
		}
		stmtobj->canGet = true;
	}
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_reset)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &sqlite_stmt, NULL);
	if(SQLITE_OK != sqlite3_reset(stmtobj->stmt))
		THROW_ERROR(cx, sqlite3_errmsg(stmtobj->assoc_db->db));
	stmtobj->canGet = false;
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	return JS_TRUE;
}

JSAPI_FUNC(sqlite_stmt_close)
{
	JSObject* obj = JS_THIS_OBJECT(cx, vp);
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, obj, &sqlite_stmt, NULL);
	if(stmtobj->current_row)
		JS_RemoveRoot(&stmtobj->current_row);
	close_db_stmt(stmtobj);
	delete stmtobj;
	
	JS_SetPrivate(cx, obj, NULL);
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	JS_ClearScope(cx, obj);
	if(JS_ValueToObject(cx, JSVAL_NULL, &obj) == JS_FALSE)
		return JS_TRUE;

	return JS_TRUE;
}

JSAPI_PROP(sqlite_stmt_getProperty)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, obj, &sqlite_stmt, NULL);

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)){
		case SQLITE_STMT_SQL:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, sqlite3_sql(stmtobj->stmt)));
			break;
		case SQLITE_STMT_READY:
			*vp = BOOLEAN_TO_JSVAL(stmtobj->canGet);
			break;
	}
	return JS_TRUE;
}

void sqlite_stmt_finalize(JSContext *cx, JSObject *obj)
{
	DBStmt* stmtobj = (DBStmt*)JS_GetInstancePrivate(cx, obj, &sqlite_stmt, NULL);
	JS_SetPrivate(cx, obj, NULL);
	if(stmtobj) {
		if(stmtobj->stmt && stmtobj->open) {
			stmtobj->assoc_db->stmts.erase(stmtobj);
			if(stmtobj->current_row)
				JS_RemoveRoot(&stmtobj->current_row);
			close_db_stmt(stmtobj);
		}
		delete stmtobj;
	}
}

