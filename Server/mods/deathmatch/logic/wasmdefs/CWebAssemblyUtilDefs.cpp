/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyUtilDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWebAssemblyDefs.h"
#include <SharedUtil.Memory.h>
#include "CDummy.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyUtilDefs.h"

#ifndef GninE_CLIENT
    #include "CRemoteCalls.h"
#endif

WebAssemblyApi(GRM, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    CElement* root = argStream.GetScript()->GetStoreContext()->GetResource()->GetResourceRootElement();

    return argStream.ReturnUserData(root);
}

void CWebAssemblyUtilDefs::RegisterFunctionTypes()
{
    SetFunctionType("print_data", "vs");
    SetFunctionType("get_tick_count", "l");
    SetFunctionType("get_real_time", "bilb");
    SetFunctionType("get_userdata_type", "i*si"); // int(bytes read) void*(pointer) char*(out_string) uint(max_size)
    SetFunctionType("get_color_from_string", "bs*"); // bool(succeed) const char*(color_code) Color*(out_color)
    
    SetFunctionType("get_distance_between_points_2D", "f**");
    SetFunctionType("get_distance_between_points_3D", "f**");
    SetFunctionType("get_easing_value", "ffsfff");
    SetFunctionType("interpolate_between", "b**fsfff*");

    SetFunctionType("preg_find", "bssi");
    SetFunctionType("preg_replace", "issssii");
    SetFunctionType("preg_match", "iss*iii");

    SetFunctionType("debug_sleep", "bi");

    SetFunctionType("to_color", "iiiii");
    SetFunctionType("get_process_memory_stats", "b*");

    SetFunctionType("get_root_element", "*");
}

void CWebAssemblyUtilDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "print_data", PrintData },
        { "get_tick_count", GetTickCount_ },
        { "get_real_time", GetRealTime },
        { "get_userdata_type", GetUserDataType },
        { "get_color_from_string", GetColorFromString },
        
        { "get_distance_between_points_2D", GetDistanceBetweenPoints2D },
        { "get_distance_between_points_3D", GetDistanceBetweenPoints3D },
        { "get_easing_value", GetEasingValue },
        { "interpolate_between", InterpolateBetween },

        { "preg_find", PregFind },
        { "preg_replace", PregReplace },
        { "preg_match", PregMatch },
        
        { "debug_sleep", DebugSleep },
        
        { "to_color", ToColor },
        { "get_process_memory_stats", GetProcessMemoryStats },

        { "get_root_element", GRM }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyUtilDefs::PrintData, env, args, results)
{
    SString str;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(str);

    CLogger::LogPrintf("INFO: %s\n", str.c_str());

    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetTickCount_, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.Return(GetTickCount64_());
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetRealTime, env, args, results)
{
    CWebAssemblyMemoryPointerAddress ptr;

    time_t timer;
    bool   localTime = true;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointerAddress(ptr);

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    argStream.ReadInt64(timer);
    argStream.ReadBoolean(localTime);

    if (timer < -1)
    {
        return argStream.Return(false, "seconds cannot be negative");
    }

    if (timer == -1)
    {
        time(&timer);
    }

    tm* timeData = localTime ? localtime(&timer) : gmtime(&timer);

    if (timeData == NULL)
    {
        return argStream.Return(false, "seconds is out of range");
    }

    struct
    {
        int32_t second;
        int32_t minute;
        int32_t hour;
        int32_t monthday;
        int32_t month;
        int32_t year;
        int32_t weekday;
        int32_t yearday;
        int32_t isdst;
        int64_t timestamp;
    } time;
    memset(&time, 0, sizeof(time));

    time.second = timeData->tm_sec;
    time.minute = timeData->tm_min;
    time.hour = timeData->tm_hour;
    time.monthday = timeData->tm_mday;
    time.month = timeData->tm_mon;
    time.year = timeData->tm_year;
    time.weekday = timeData->tm_wday;
    time.yearday = timeData->tm_yday;
    time.isdst = timeData->tm_isdst;
    time.timestamp = timer;

    argStream.WritePointer(ptr, &time);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetUserDataType, env, args, results)
{
    CElement* resRoot;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(resRoot);

    if (!resRoot)
    {
        CLogger::LogPrintf("couldn't get resroot!\n");
        return NULL;
    }

    CLogger::LogPrintf("resource root element type : [%s]\n", resRoot->GetTypeName().c_str());

    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetColorFromString, env, args, results)
{
    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetDistanceBetweenPoints2D, env, args, results)
{
    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetDistanceBetweenPoints3D, env, args, results)
{
    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetEasingValue, env, args, results)
{
    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::InterpolateBetween, env, args, results)
{
    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::PregFind, env, args, results)
{
    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::PregReplace, env, args, results)
{
    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::PregMatch, env, args, results)
{
    return NULL;
}

WebAssemblyApi(CWebAssemblyUtilDefs::DebugSleep, env, args, results)
{
    int ms;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadInt32(ms);

#ifdef GninE_CLIENT
    if (!g_pClientGame->GetDevelopmentMode())
#else
    if (!g_pGame->GetDevelopmentMode())
#endif
    {
        return argStream.Return(false, "This function can only be used in development mode");
    }

#ifdef GninE_CLIENT
    g_pClientGame->GetRemoteCalls()->ProcessQueuedFiles();
#else
    g_pGame->GetRemoteCalls()->ProcessQueuedFiles();
#endif

    std::this_thread::sleep_for(std::chrono::milliseconds(ms));

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyUtilDefs::ToColor, env, args, results)
{
    int red;
    int green;
    int blue;
    int alpha;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadInt32(red, 0);
    argStream.ReadInt32(green, 0);
    argStream.ReadInt32(blue, 0);
    argStream.ReadInt32(alpha, 255);

    unsigned long color = COLOR_RGBA(red, green, blue, alpha);

    return argStream.Return((int32_t)color);
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetProcessMemoryStats, env, args, results)
{
    CWebAssemblyMemoryPointerAddress ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointerAddress(ptr);

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    ProcessMemoryStats memoryStats = { 0 };

    if (TryGetProcessMemoryStats(memoryStats))
    {
        struct
        {
            uint32_t virtualMemorySize;
            uint32_t residentMemorySize;
            uint32_t sharedMemorySize;
            uint32_t privateMemorySize;
        } status = { 0 };
        memset(&status, 0, sizeof(status));

        status.virtualMemorySize = memoryStats.virtualMemorySize;
        status.residentMemorySize = memoryStats.residentMemorySize;
        status.sharedMemorySize = memoryStats.sharedMemorySize;
        status.privateMemorySize = memoryStats.privateMemorySize;

        argStream.WritePointer(ptr, &status);

        return argStream.Return(true);
    }

    return argStream.Return(false);
}
