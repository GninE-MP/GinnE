/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyDatabaseDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWebAssemblyDefs.h"
#include <SharedUtil.Memory.h>
#include "CDummy.h"
#include "CStaticFunctionDefinitions.h"
#include "Utils.h"
#include "CScriptDebugging.h"

#include "lua/CLuaCallback.h"
#include "../luadefs/CLuaDatabaseDefs.h"
#include "CPerfStatManager.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyDatabaseDefs.h"

void CWebAssemblyDatabaseDefs::RegisterFunctionTypes()
{
    SetFunctionType("db_connect", "esssss");
    SetFunctionType("db_exec", "besusx");
    SetFunctionType("db_query", "uesucusx");
    SetFunctionType("db_free", "bu");
    SetFunctionType("db_poll", "uuisx");
    SetFunctionType("db_prepare_string", "xesusxsx");

    SetFunctionType("execute_sql_create_table", "bss");
    SetFunctionType("execute_sql_drop_table", "bs");
    SetFunctionType("execute_sql_delete", "bsssx");
    SetFunctionType("execute_sql_insert", "bssssx");
    SetFunctionType("execute_sql_select", "usssisx");
    SetFunctionType("execute_sql_update", "bssssx");
    SetFunctionType("execute_sql_query", "ususx");
}

void CWebAssemblyDatabaseDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "db_connect", DbConnect },
        { "db_exec", DbExec },
        { "db_query", DbQuery },
        { "db_free", DbFree },
        { "db_poll", DbPoll },
        { "db_prepare_string", DbPrepareString },

        { "execute_sql_create_table", ExecuteSQLCreateTable },
        { "execute_sql_drop_table", ExecuteSQLDropTable },
        { "execute_sql_delete", ExecuteSQLDelete },
        { "execute_sql_insert", ExecuteSQLInsert },
        { "execute_sql_select", ExecuteSQLSelect },
        { "execute_sql_update", ExecuteSQLUpdate },
        { "execute_sql_query", ExecuteSQLQuery },
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::DbConnect, env, args, results)
{
    SString type;
    SString host;
    SString username;
    SString password;
    SString options;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(type);
    argStream.ReadString(host);
    argStream.ReadString(username, "");
    argStream.ReadString(password, "");
    argStream.ReadString(options, "");

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    if (type == "sqlite" && !host.empty())
    {
        if (host.BeginsWith(":/"))
        {
            host = host.SubStr(1);

            if (!IsValidFilePath(host.c_str()))
            {
                return argStream.ReturnNull();
            }

            host = PathJoin(g_pGame->GetConfig()->GetGlobalDatabasesPath(), host);
        }
        else
        {
            std::string absPath;

            CResource* pathResource = resource;

            if (!CResourceManager::ParseResourcePathInput(host, pathResource, &absPath))
            {
                return argStream.ReturnNull();
            }

            host = absPath;
        }
    }

    if (type == "mysql")
    {
        resource->SetUsingDbConnectMysql(true);
    }

    bool    loggingEnabled;
    SString logTag;
    SString queueName;

    GetOption<CDbOptionsMap>(options, "log", loggingEnabled, 1);
    GetOption<CDbOptionsMap>(options, "tag", logTag, "script");
    GetOption<CDbOptionsMap>(options, "queue", queueName, (type == "mysql") ? host : DB_SQLITE_QUEUE_NAME_DEFAULT);
    SetOption<CDbOptionsMap>(options, "log", loggingEnabled);
    SetOption<CDbOptionsMap>(options, "tag", logTag);
    SetOption<CDbOptionsMap>(options, "queue", queueName);

    SConnectionHandle connection = g_pGame->GetDatabaseManager()->Connect(type, host, username, password, options);

    if (connection == INVALID_DB_HANDLE)
    {
        return argStream.ReturnNull();
    }

    
    CDatabaseConnectionElement* db = new CDatabaseConnectionElement(NULL, connection);

    if (!db)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(db);
    }

    return argStream.Return((CElement*)db);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::DbExec, env, args, results)
{
    CDatabaseConnectionElement*      db;
    SString                          query;
    CLuaArguments                    arguments;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(db);
    argStream.ReadString(query);
    argStream.ReadLuaArguments(arguments, CLuaArguments());
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    if (!db)
    {
        return argStream.Return(false);
    }

    CDbJobData* jobData = g_pGame->GetDatabaseManager()->Exec(db->GetConnectionHandle(), query, &arguments);

    if (!jobData)
    {
        if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed() && outError != WEB_ASSEMBLY_NULL_PTR)
        {
            SString errorMessage = g_pGame->GetDatabaseManager()->GetLastErrorMessage();

            uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

            argStream.WritePointer(outError, errorMessage.data(), size);
        }

        return argStream.Return(false);
    }

    jobData->SetCallback(CLuaDatabaseDefs::DbExecCallback, NULL);
    jobData->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(GetWebAssemblyLuaVM(env)));

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::DbQuery, env, args, results)
{
    CDatabaseConnectionElement*      db;
    SString                          query;
    CLuaArguments                    arguments;
    CLuaFunctionRef                  callback;
    CLuaArguments                    callbackArguments;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(db);
    argStream.ReadString(query);
    argStream.ReadLuaArguments(arguments, CLuaArguments());
    argStream.ReadLuaFunctionRef(callback, CLuaFunctionRef());
    argStream.ReadLuaArguments(callbackArguments, CLuaArguments());
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    if (!db)
    {
        return argStream.ReturnNull();
    }

    CDbJobData* jobData = g_pGame->GetDatabaseManager()->QueryStart(db->GetConnectionHandle(), query, &arguments);

    if (!jobData)
    {
        if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed() && outError != WEB_ASSEMBLY_NULL_PTR)
        {
            SString errorMessage = g_pGame->GetDatabaseManager()->GetLastErrorMessage();

            uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

            argStream.WritePointer(outError, errorMessage.data(), size);
        }

        return argStream.ReturnNull();
    }

    if (VERIFY_FUNCTION(callback))
    {
        CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

        if (luaMain)
        {
            CLuaArguments Arguments;

            Arguments.PushDbQuery(jobData);
            Arguments.PushArguments(callbackArguments);

            jobData->SetCallback(CLuaDatabaseDefs::DbQueryCallback, g_pGame->GetLuaCallbackManager()->CreateCallback(luaMain, callback, Arguments));
        }
    }

    jobData->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(GetWebAssemblyLuaVM(env)));

    return argStream.Return(jobData);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::DbFree, env, args, results)
{
    CDbJobData* jobData;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(jobData);

    if (!jobData)
    {
        return argStream.Return(false);
    }

    jobData->SetCallback(CLuaDatabaseDefs::DbFreeCallback, NULL);
    jobData->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(GetWebAssemblyLuaVM(env)));

    return argStream.Return(g_pGame->GetDatabaseManager()->QueryFree(jobData));
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::DbPoll, env, args, results)
{
    CDbJobData*                      jobData;
    int32_t                          timeout;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(jobData);
    argStream.ReadInt32(timeout);
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    if (!jobData)
    {
        return argStream.ReturnNull();
    }

    if (jobData->stage > EJobStage::RESULT)
    {
        SString errorMessage = "Previous dbPoll already returned result";

        uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

        argStream.WritePointer(outError, errorMessage.data(), size);

        argStream.ReturnNull();
    }

    if (jobData->result.bIgnoreResult)
    {
        SString errorMessage = "Cannot call dbPoll after dbFree";

        uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

        argStream.WritePointer(outError, errorMessage.data(), size);

        return argStream.ReturnNull();
    }

    if (!g_pGame->GetDatabaseManager()->QueryPoll(jobData, timeout))
    {
        return argStream.ReturnNull();
    }

    if (jobData->result.status == EJobResult::FAIL)
    {
        if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed() && outError != WEB_ASSEMBLY_NULL_PTR)
        {
            SString errorMessage = g_pGame->GetDatabaseManager()->GetLastErrorMessage();

            uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

            argStream.WritePointer(outError, errorMessage.data(), size);
        }

        return argStream.ReturnNull();
    }

    const CRegistryResultData* Result = jobData->result.registryResult->GetThis();

    cargs* result = args_create();

    for (CRegistryResultIterator iter = Result->begin(); iter != Result->end(); iter++)
    {
        const CRegistryResultRow& row = *iter;

        cargs* lRow = args_create();

        for (int i = 0; i < Result->nColumns; i++)
        {
            const CRegistryResultCell& cell = row[i];
            SString                    column = Result->ColNames[i];

            args_push_string(lRow, column.c_str(), column.size());

            switch (cell.nType)
            {
                case SQLITE_INTEGER:
                    args_push_number(lRow, static_cast<double>(cell.nVal));
                    break;
                case SQLITE_FLOAT:
                    args_push_number(lRow, cell.fVal);
                    break;
                case SQLITE_BLOB:
                    args_push_string(lRow, (const char*)cell.pVal, cell.nLength);
                    break;
                case SQLITE_TEXT:
                    args_push_string(lRow, (const char*)cell.pVal, cell.nLength - 1);
                    break;
                default:
                    args_push_boolean(lRow, false);
            }
        }

        args_push_list(result, lRow);

        args_delete(lRow);
    }

    return argStream.ReturnSystemPointer(result);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::DbPrepareString, env, args, results)
{
    CDatabaseConnectionElement*      db;
    SString                          query;
    CLuaArguments                    arguments;
    CWebAssemblyMemoryPointerAddress outString;
    uint32_t                         maxStringSize;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(db);
    argStream.ReadString(query);
    argStream.ReadLuaArguments(arguments, CLuaArguments());
    argStream.ReadPointerAddress(outString, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxStringSize, 0xffffffff);
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    if (!db || outString == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    SString result = g_pGame->GetDatabaseManager()->PrepareString(db->GetConnectionHandle(), query, &arguments);
    SString errorMessage = g_pGame->GetDatabaseManager()->GetLastErrorMessage();

    if (!result.empty() || errorMessage.empty())
    {
        uint32_t size = std::min(maxStringSize, (uint32_t)result.size());

        argStream.WritePointer(outString, result.data(), size);

        return argStream.Return(size);
    }

    if (!g_pGame->GetDatabaseManager()->IsLastErrorSuppressed() && outError != WEB_ASSEMBLY_NULL_PTR)
    {
        uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

        argStream.WritePointer(outError, errorMessage.data(), size);
    }

    return argStream.Return(0);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::ExecuteSQLCreateTable, env, args, results)
{
    SString table;
    SString definition;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(table);
    argStream.ReadString(definition);

    CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(GetWebAssemblyLuaVM(env));
    CStaticFunctionDefinitions::ExecuteSQLCreateTable(table, definition);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::ExecuteSQLDropTable, env, args, results)
{
    SString table;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(table);
    
    CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(GetWebAssemblyLuaVM(env));
    CStaticFunctionDefinitions::ExecuteSQLDropTable(table);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::ExecuteSQLDelete, env, args, results)
{
    SString                          table;
    SString                          definition;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(table);
    argStream.ReadString(definition);
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(GetWebAssemblyLuaVM(env));

    if (!CStaticFunctionDefinitions::ExecuteSQLDelete(table, definition))
    {
        SString errorMessage = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();

        if (outError != WEB_ASSEMBLY_NULL_PTR)
        {
            uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

            argStream.WritePointer(outError, errorMessage.data(), size);
        }

        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::ExecuteSQLInsert, env, args, results)
{
    SString                          table;
    SString                          definition;
    SString                          columns;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(table);
    argStream.ReadString(definition);
    argStream.ReadString(columns);
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(GetWebAssemblyLuaVM(env));

    if (!CStaticFunctionDefinitions::ExecuteSQLInsert(table, definition, columns))
    {
        SString errorMessage = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();

        if (outError != WEB_ASSEMBLY_NULL_PTR)
        {
            uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

            argStream.WritePointer(outError, errorMessage.data(), size);
        }

        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::ExecuteSQLSelect, env, args, results)
{
    SString                          table;
    SString                          columns;
    SString                          whereS;
    uint32_t                         limit;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(table);
    argStream.ReadString(columns);
    argStream.ReadString(whereS, "");
    argStream.ReadUInt32(limit, 0);
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    CRegistryResult Result;

    CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(GetWebAssemblyLuaVM(env));

    if (!CStaticFunctionDefinitions::ExecuteSQLSelect(table, columns, whereS, limit, &Result))
    {
        SString errorMessage = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();

        if (outError != WEB_ASSEMBLY_NULL_PTR)
        {
            uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

            argStream.WritePointer(outError, errorMessage.data(), size);
        }

        return argStream.ReturnNull();
    }

    cargs* result = args_create();
    
    for (CRegistryResultIterator iter = Result->begin(); iter != Result->end(); iter++)
    {
        const CRegistryResultRow& row = *iter;

        cargs* lRow = args_create();

        for (int j = 0; j < Result->nColumns; j++)
        {
            const CRegistryResultCell& cell = row[j];
            SString                    column = Result->ColNames[j];

            if (cell.nType == SQLITE_NULL)
            {
                continue;
            }

            args_push_string(lRow, column.c_str(), column.size());

            switch (cell.nType)
            {
                case SQLITE_INTEGER:
                    args_push_number(lRow, static_cast<double>(cell.nVal));
                    break;
                case SQLITE_FLOAT:
                    args_push_number(lRow, cell.fVal);
                    break;
                case SQLITE_BLOB:
                    args_push_string(lRow, (const char*)cell.pVal, cell.nLength);
                    break;
                case SQLITE_TEXT:
                    args_push_string(lRow, (const char*)cell.pVal, cell.nLength - 1);
                    break;
                default:
                    args_push_null(lRow);
            }
        }

        args_push_list(result, lRow);

        args_delete(lRow);
    }

    return argStream.ReturnSystemPointer(result);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::ExecuteSQLUpdate, env, args, results)
{
    SString                          table;
    SString                          set;
    SString                          whereS;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(table);
    argStream.ReadString(set);
    argStream.ReadString(whereS, "");
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(GetWebAssemblyLuaVM(env));

    if (!CStaticFunctionDefinitions::ExecuteSQLUpdate(table, set, whereS))
    {
        SString errorMessage = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();

        uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

        argStream.WritePointer(outError, errorMessage.data(), size);

        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyDatabaseDefs::ExecuteSQLQuery, env, args, results)
{
    SString                          query;
    CLuaArguments                    arguments;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(query);
    argStream.ReadLuaArguments(arguments, CLuaArguments());
    argStream.ReadPointerAddress(outError, WEB_ASSEMBLY_NULL_PTR);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    CRegistryResult Result;

    CPerfStatSqliteTiming::GetSingleton()->SetCurrentResource(GetWebAssemblyLuaVM(env));

    if (CStaticFunctionDefinitions::ExecuteSQLQuery(query, &arguments, &Result))
    {
        SString errorMessage = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError();

        if (outError != WEB_ASSEMBLY_NULL_PTR)
        {
            uint32_t size = std::min(maxErrorSize, (uint32_t)errorMessage.size());

            argStream.WritePointer(outError, errorMessage.data(), size);
        }

        return argStream.ReturnNull();
    }

    cargs* result = args_create();
    
    for (CRegistryResultIterator iter = Result->begin(); iter != Result->end(); iter++)
    {
        const CRegistryResultRow& row = *iter;

        cargs* lRow = args_create();

        for (int j = 0; j < Result->nColumns; j++)
        {
            const CRegistryResultCell& cell = row[j];
            SString                    column = Result->ColNames[j];

            if (cell.nType == SQLITE_NULL)
            {
                continue;
            }

            args_push_string(lRow, column.c_str(), column.size());

            switch (cell.nType)
            {
                case SQLITE_INTEGER:
                    args_push_number(lRow, static_cast<double>(cell.nVal));
                    break;
                case SQLITE_FLOAT:
                    args_push_number(lRow, cell.fVal);
                    break;
                case SQLITE_BLOB:
                    args_push_string(lRow, (const char*)cell.pVal, cell.nLength);
                    break;
                case SQLITE_TEXT:
                    args_push_string(lRow, (const char*)cell.pVal, cell.nLength - 1);
                    break;
                default:
                    args_push_null(lRow);
            }
        }

        args_push_list(result, lRow);

        args_delete(lRow);
    }

    return argStream.ReturnSystemPointer(result);
}
