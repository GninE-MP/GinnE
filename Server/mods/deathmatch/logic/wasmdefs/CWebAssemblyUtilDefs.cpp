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
#include "Utils.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyUtilDefs.h"

#ifndef GninE_CLIENT
    #include "CRemoteCalls.h"
#endif

void CWebAssemblyUtilDefs::RegisterFunctionTypes()
{
    SetFunctionType("print_data", "vs");
    SetFunctionType("get_tick_count", "l");
    SetFunctionType("get_real_time", "bilb");
    SetFunctionType("get_userdata_type", "i*si");
    SetFunctionType("get_color_from_string", "bs*");
    
    SetFunctionType("get_distance_between_points_2D", "f**");
    SetFunctionType("get_distance_between_points_3D", "f**");
    SetFunctionType("get_easing_value", "ffsfff");
    SetFunctionType("interpolate_between", "b***fsfff");

    SetFunctionType("debug_sleep", "bi");

    SetFunctionType("to_color", "iiiii");
    SetFunctionType("get_process_memory_stats", "b*");
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

        { "debug_sleep", DebugSleep },
        
        { "to_color", ToColor },
        { "get_process_memory_stats", GetProcessMemoryStats }
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
    void*                            userdata;
    CWebAssemblyMemoryPointerAddress typeNamePtr;
    uint32_t                         maxBytes;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(userdata);
    argStream.ReadPointerAddress(typeNamePtr);
    argStream.ReadUInt32(maxBytes, 0xffffffff);

    if (typeNamePtr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    if (!userdata)
    {
        return argStream.Return(0);
    }

    lua_State* luaVM = argStream.GetScript()->GetStoreContext()->GetResource()->GetVirtualMachine()->GetVM();
    SString typeName = GetUserDataClassName(userdata, luaVM, false);

    typeName = typeName.empty() ? "userdata" : typeName;

    size_t bytesCount = typeName.size();
    size_t totalSize = std::min(bytesCount, maxBytes);

    argStream.WritePointer(typeNamePtr, typeName.data(), totalSize);

    return argStream.Return(totalSize);
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetColorFromString, env, args, results)
{
    SString                          colorCode;
    CWebAssemblyMemoryPointerAddress ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(colorCode);
    argStream.ReadPointerAddress(ptr);

    if (colorCode.empty())
    {
        return argStream.Return(false);
    }

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } colorData;
    memset((void*)&colorData, 0, sizeof(colorData));

    if (!XMLColorToInt(colorCode, colorData.r, colorData.g, colorData.b, colorData.a))
    {
        return argStream.Return(false);
    }

    argStream.WritePointer(ptr, &colorData);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetDistanceBetweenPoints2D, env, args, results)
{
    CVector2D p1;
    CVector2D p2;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector2D(p1);
    argStream.ReadVector2D(p2);

    return argStream.Return(DistanceBetweenPoints2D(p1, p2));
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetDistanceBetweenPoints3D, env, args, results)
{
    CVector p1;
    CVector p2;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(p1);
    argStream.ReadVector3D(p2);

    return argStream.Return(DistanceBetweenPoints3D(p1, p2));
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetEasingValue, env, args, results)
{
    float32_t progress;
    SString   easingTypeString;
    float32_t easingPeriod;
    float32_t easingAmplitude;
    float32_t easingOvershoot;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadFloat32(progress);
    argStream.ReadString(easingTypeString);
    argStream.ReadFloat32(easingPeriod);
    argStream.ReadFloat32(easingAmplitude);
    argStream.ReadFloat32(easingOvershoot);

    CEasingCurve::eType easingType;

    StringToEnum(easingTypeString, easingType);

    CEasingCurve easingCurve(easingType);
    easingCurve.SetParams(easingPeriod, easingAmplitude, easingOvershoot);
    float32_t curve = easingCurve.ValueForProgress(progress);

    return argStream.Return(curve);
}

WebAssemblyApi(CWebAssemblyUtilDefs::InterpolateBetween, env, args, results)
{
    CVector                          p1;
    CVector                          p2;
    CWebAssemblyMemoryPointerAddress outPoint;
    float32_t                        progress;
    SString                          easingTypeString;
    float32_t                        easingPeriod;
    float32_t                        easingAmplitude;
    float32_t                        easingOvershoot;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(p1);
    argStream.ReadVector3D(p2);
    argStream.ReadPointerAddress(outPoint);
    argStream.ReadFloat32(progress);
    argStream.ReadString(easingTypeString);
    argStream.ReadFloat32(easingPeriod);
    argStream.ReadFloat32(easingAmplitude);
    argStream.ReadFloat32(easingOvershoot);

    CEasingCurve::eType easingType;
    StringToEnum(easingTypeString, easingType);

    CVector vecResult = TInterpolation<CVector>::Interpolate(p1, p2, progress, easingType, easingPeriod, easingAmplitude, easingOvershoot);

    struct 
    {
        float32_t x;
        float32_t y;
        float32_t z;
    } out;
    memset((void*)&out, 0, sizeof(out));

    argStream.WritePointer(outPoint, &out);

    return argStream.Return(true);
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
