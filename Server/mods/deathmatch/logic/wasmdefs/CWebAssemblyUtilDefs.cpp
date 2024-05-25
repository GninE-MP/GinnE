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
    // function signiture tutorial:
    // "[return]...args";

    SetFunctionType("print_data", "vs");

    SetFunctionType("get_shared_pointer_address", "z*");
    SetFunctionType("get_pointer_from_shared_pointer", "*z");
    SetFunctionType("does_shared_pointer_address_belong_to_this_module", "bz");
    SetFunctionType("does_shared_pointer_address_belong_to_this_resource", "bz");
    SetFunctionType("read_shared_pointer_address", "xz*x");
    SetFunctionType("write_shared_pointer_address", "xz*x");
    SetFunctionType("get_max_shared_pointer_size", "xz");
    SetFunctionType("get_memory_free_space", "x");

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

    SetFunctionType("call_callable", "*u**x");
    SetFunctionType("is_valid_callable", "bu");
    SetFunctionType("get_callable_declaration", "ius*i");
    SetFunctionType("free_callable_result", "v*");
    SetFunctionType("c_function_to_callable", "b**");
    SetFunctionType("get_callable_resource", "u*");
    SetFunctionType("get_callable_reference", "u*");
    SetFunctionType("is_callable_wasm_function", "b*");
}

void CWebAssemblyUtilDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "print_data", PrintData },

        { "get_shared_pointer_address", GetSharedPointerAddress },
        { "get_pointer_from_shared_pointer", GetPointerFromSharedPointer },
        { "does_shared_pointer_address_belong_to_this_module", DoesSharedPointerAddressBelongToThisModule },
        { "does_shared_pointer_address_belong_to_this_resource", DoesSharedPointerAddressBelongToThisResource },
        { "read_shared_pointer_address", ReadSharedPointerAddress },
        { "write_shared_pointer_address", WriteSharedPointerAddress },
        { "get_max_shared_pointer_size", GetMaxSharedPointerSize },
        { "get_memory_free_space", GetMemoryFreeSpace },

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
        { "get_process_memory_stats", GetProcessMemoryStats },

        { "call_callable", CallCallable },
        { "is_valid_callable", IsValidCallable },
        { "get_callable_declaration", GetCallableDeclaration },
        { "free_callable_result", FreeCallableResult },
        { "c_function_to_callable", CFunctionToCallable },
        { "get_callable_resource", GetCallableResource },
        { "get_callable_reference", GetCallableReference },
        { "is_callable_wasm_function", IsCallableWasmFunction },
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

WebAssemblyApi(CWebAssemblyUtilDefs::GetSharedPointerAddress, env, args, results)
{
    CWebAssemblyMemoryPointerAddress ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointerAddress(ptr);

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.ReturnNull();
    }

    CWebAssemblyMemory* memory = argStream.GetScript()->GetMemory();

    void* pPtr = memory->GetMemoryPhysicalPointer(ptr);

    if (!memory->DoesPointerBelongToMemory(pPtr))
    {
        return argStream.ReturnNull();
    }

    return argStream.ReturnSystemPointer(pPtr);
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetPointerFromSharedPointer, env, args, results)
{
    void* ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(ptr);

    if (!ptr)
    {
        return argStream.ReturnNull();
    }

    CWebAssemblyMemory* memory = argStream.GetScript()->GetMemory();

    if (!memory->DoesPointerBelongToMemory(ptr))
    {
        return argStream.ReturnNull();
    }

    return argStream.Return((CWebAssemblyUserData)((CWebAssemblyMemoryPointerAddress)(((intptr_t)ptr) - ((intptr_t)memory->GetBase()))));
}

WebAssemblyApi(CWebAssemblyUtilDefs::DoesSharedPointerAddressBelongToThisModule, env, args, results)
{
    void* ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(ptr);

    return argStream.Return(argStream.GetScript()->GetMemory()->DoesPointerBelongToMemory(ptr));
}

WebAssemblyApi(CWebAssemblyUtilDefs::DoesSharedPointerAddressBelongToThisResource, env, args, results)
{
    void* ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(ptr);

    return argStream.Return(argStream.GetScript()->GetStoreContext()->DoesPointerBelongToContext(ptr));
}

WebAssemblyApi(CWebAssemblyUtilDefs::ReadSharedPointerAddress, env, args, results)
{
    void*                            ptr;
    CWebAssemblyMemoryPointerAddress dataPtr;
    int32_t                          size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(ptr);
    argStream.ReadPointerAddress(dataPtr);
    argStream.ReadInt32(size);

    if (size < 1)
    {
        return argStream.Return(0);
    }

    CWebAssemblyScript* script = argStream.GetScript()->GetStoreContext()->FindPointerScript(ptr);

    if (!script)
    {
        return argStream.Return(0);
    }

    CWebAssemblyMemory* memory = script->GetMemory();

    intptr_t ptrAddress = (intptr_t)ptr;
    intptr_t memBaseAddress = (intptr_t)memory->GetBase();
    intptr_t memEndAddress = ((intptr_t)memory->GetBase()) + memory->GetSize();
    intptr_t maxReadSize = memEndAddress - ptrAddress;

    size = std::min(size, (int32_t)maxReadSize);

    if (size < 1)
    {
        return argStream.Return(0);
    }

    argStream.WritePointer(dataPtr, ptr, size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyUtilDefs::WriteSharedPointerAddress, env, args, results)
{
    void*                            ptr;
    CWebAssemblyMemoryPointerAddress dataPtr;
    int32_t                          size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(ptr);
    argStream.ReadPointerAddress(dataPtr);
    argStream.ReadInt32(size);

    if (size < 1)
    {
        return argStream.Return(0);
    }

    CWebAssemblyScript* script = argStream.GetScript()->GetStoreContext()->FindPointerScript(ptr);

    if (!script)
    {
        return argStream.Return(0);
    }

    CWebAssemblyMemory* memory = script->GetMemory();

    intptr_t ptrAddress = (intptr_t)ptr;
    intptr_t memBaseAddress = (intptr_t)memory->GetBase();
    intptr_t memEndAddress = ((intptr_t)memory->GetBase()) + memory->GetSize();
    intptr_t maxReadSize = memEndAddress - ptrAddress;

    size = std::min(size, (int32_t)maxReadSize);

    if (size < 1)
    {
        return argStream.Return(0);
    }

    memcpy(ptr, memory->GetMemoryPhysicalPointer(dataPtr), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetMaxSharedPointerSize, env, args, results)
{
    void* ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(ptr);

    CWebAssemblyScript* script = argStream.GetScript()->GetStoreContext()->FindPointerScript(ptr);

    if (!script)
    {
        return argStream.Return(0);
    }

    intptr_t            address = (intptr_t)ptr;
    CWebAssemblyMemory* memory = script->GetMemory();

    intptr_t memoryEndPointer = ((intptr_t)memory->GetBase()) + memory->GetSize();

    if (address > memoryEndPointer)
    {
        return argStream.Return(0);
    }

    return argStream.Return(memoryEndPointer - address);
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetMemoryFreeSpace, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.Return((int32_t)argStream.GetScript()->GetMemory()->GetFreeSpaceSize());
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

WebAssemblyApi(CWebAssemblyUtilDefs::CallCallable, env, args, results)
{
    uint8_t*                         functionHash;
    uint8_t*                         argsData;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(functionHash);
    argStream.ReadPointer(argsData);
    argStream.ReadPointerAddress(outError);
    argStream.ReadUInt32(maxErrorSize, 0xffffffff);

    CCallable callable;

    if (!callable.ReadHash(functionHash))
    {
        return argStream.ReturnNull();
    }

    CWebAssemblyMemory* mem = argStream.GetScript()->GetMemory();

    SString errorMessage;

    CLuaArguments cArgs, cResults;
    
    if (!callable.Call(&cArgs, &cResults, &errorMessage))
    {
        size_t errorLength = errorMessage.size();

        if (errorLength > 0)
        {
            errorLength = std::min(errorLength, maxErrorSize);

            argStream.WritePointer(outError, errorMessage.data(), errorLength);
        }

        return argStream.ReturnNull();
    }

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::IsValidCallable, env, args, results)
{
    uint8_t* functionHash;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(functionHash);

    CCallable callable;

    return argStream.Return(callable.ReadHash(functionHash));
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetCallableDeclaration, env, args, results)
{
    uint8_t*                         functionHash;
    SString                          functionName;
    CWebAssemblyMemoryPointerAddress outString;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(functionHash);
    argStream.ReadString(functionName);
    argStream.ReadPointerAddress(outString);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (outString == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    CCallable callable;

    if (!callable.ReadHash(functionHash))
    {
        return argStream.Return(0);
    }

    SString declaration;

    if (callable.IsWasmFunction())
    {
        CWebAssemblyFunction* wasmFunction = callable.GetWasmFunction();

        if (wasmFunction)
        {
            declaration = wasmFunction->GetFunctionType().GenerateFunctionStructureText(functionName);
        }
    }
    else
    {
        declaration += "any ";
        declaration += functionName;
        declaration += "(...any)";
    }

    if (declaration.empty())
    {
        return argStream.Return(0);
    }

    size_t length = std::min(declaration.size(), maxSize);

    argStream.WritePointer(outString, declaration.data(), length);

    return argStream.Return(length);
}

WebAssemblyApi(CWebAssemblyUtilDefs::FreeCallableResult, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::CFunctionToCallable, env, args, results)
{
    uintptr_t                        functionHash;
    CWebAssemblyMemoryPointerAddress callableRefPtr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadInternalFunctionHash(functionHash);
    argStream.ReadPointerAddress(callableRefPtr);

    if (callableRefPtr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    CWebAssemblyFunction* function = argStream.GetScript()->GetInternalFunctionByHash(functionHash);

    if (!function)
    {
        return argStream.Return(false);
    }

    CCallable callable(function);
    callable.SetLuaResource(GetWebAssemblyEnvResource(env));
    callable.SetIsWasmFunctionState(true);

    uint8_t callableHash[C_CALLABLE_HASH_SIZE];
    callable.WriteHash(callableHash);

    argStream.WritePointer(callableRefPtr, callableHash, C_CALLABLE_HASH_SIZE);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetCallableResource, env, args, results)
{
    uint8_t* callableHash;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(callableHash);

    CCallable callable;

    if (!callable.ReadHash(callableHash))
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(RESOURCE_TO_WASM_USERDATA(callable.GetLuaResource()));
}

WebAssemblyApi(CWebAssemblyUtilDefs::GetCallableReference, env, args, results)
{
    uint8_t* callableHash;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(callableHash);

    CCallable callable;

    if (!callable.ReadHash(callableHash))
    {
        return argStream.ReturnNull();
    }

    if (callable.IsWasmFunction())
    {
        return argStream.Return((CWebAssemblyUserData)callable.GetWasmFunction());
    }

    return argStream.Return((CWebAssemblyUserData)callable.GetLuaFunctionRef());
}

WebAssemblyApi(CWebAssemblyUtilDefs::IsCallableWasmFunction, env, args, results)
{
    uint8_t* callableHash;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(callableHash);

    CCallable callable;

    if (!callable.ReadHash(callableHash))
    {
        return argStream.Return(false);
    }

    return argStream.Return(callable.IsWasmFunction());
}
