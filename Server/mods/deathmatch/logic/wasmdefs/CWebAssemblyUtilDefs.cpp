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

typedef struct
{
    size_t argsCount;
    size_t dataSize;
    void*  data;
} cargs;

cargs*               args_create();
void                 args_delete(cargs* args);
size_t               args_calculate_arg_heap_size(cargs* args, size_t size, char type);
void                 args_push_null(cargs* args);
void                 args_push_boolean(cargs* args, bool boolean);
void                 args_push_number(cargs* args, double number);
void                 args_push_string(cargs* args, const char* string, size_t size);
void                 args_push_callable(cargs* args, uint8_t* string);
void                 args_push_userdata(cargs* args, CWebAssemblyUserData userdata);
void                 args_push_list(cargs* args, cargs* list);
void                 args_pop(cargs* args, size_t count);
long long            args_find_arg_start_position(cargs* args, size_t index, size_t from);
char                 args_get_arg_type(cargs* args, size_t index);
bool                 args_get_arg_as_boolean(cargs* args, size_t index);
double               args_get_arg_as_number(cargs* args, size_t index);
char*                args_get_arg_as_string(cargs* args, size_t index, size_t* bsize);
uint8_t*             args_get_arg_as_callable(cargs* args, size_t index);
CWebAssemblyUserData args_get_arg_as_userdata(cargs* args, size_t index);
cargs*               args_get_arg_as_list(cargs* args, size_t index);
void                 args_read_clua_argument(cargs* args, CLuaArgument* arg);

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
    SetFunctionType("c_function_to_callable", "b**");
    SetFunctionType("get_callable_resource", "u*");
    SetFunctionType("get_callable_reference", "u*");
    SetFunctionType("is_callable_wasm_function", "b*");

    SetFunctionType("args_create", "u");
    SetFunctionType("args_delete", "vu");
    SetFunctionType("args_push_null", "vu");
    SetFunctionType("args_push_boolean", "vub");
    SetFunctionType("args_push_number", "vud");
    SetFunctionType("args_push_string", "vus");
    SetFunctionType("args_push_callable", "vu*"),
    SetFunctionType("args_push_userdata", "vuu");
    SetFunctionType("args_push_list", "vuu");
    SetFunctionType("args_pop", "vui");
    SetFunctionType("args_get_arg_type", "iui");
    SetFunctionType("args_get_arg_as_boolean", "bui");
    SetFunctionType("args_get_arg_as_number", "dui");
    SetFunctionType("args_get_arg_as_string", "xui*x");
    SetFunctionType("args_get_arg_as_callable", "bui*");
    SetFunctionType("args_get_arg_as_userdata", "uui");
    SetFunctionType("args_get_arg_as_list", "uui");
    SetFunctionType("args_get_arg_count", "iu");
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
        { "c_function_to_callable", CFunctionToCallable },
        { "get_callable_resource", GetCallableResource },
        { "get_callable_reference", GetCallableReference },
        { "is_callable_wasm_function", IsCallableWasmFunction },

        { "args_create", ArgsCreate },
        { "args_delete", ArgsDelete },
        { "args_push_null", ArgsPushNull },
        { "args_push_boolean", ArgsPushBoolean },
        { "args_push_number", ArgsPushNumber },
        { "args_push_string", ArgsPushString },
        { "args_push_callable", ArgsPushCallable },
        { "args_push_userdata", ArgsPushUserdata },
        { "args_push_list", ArgsPushList },
        { "args_pop", ArgsPop },
        { "args_get_arg_type", ArgsGetArgType },
        { "args_get_arg_as_boolean", ArgsGetArgAsBoolean },
        { "args_get_arg_as_number", ArgsGetArgAsNumber },
        { "args_get_arg_as_string", ArgsGetArgAsString },
        { "args_get_arg_as_callable", ArgsGetArgAsCallable },
        { "args_get_arg_as_userdata", ArgsGetArgAsUserdata },
        { "args_get_arg_as_list", ArgsGetArgAsList },
        { "args_get_arg_count", ArgsGetArgCount }
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
    cargs*                           argsL;
    CWebAssemblyMemoryPointerAddress outError;
    uint32_t                         maxErrorSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(functionHash);
    argStream.ReadSystemPointer(argsL);
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

    if (argsL && argsL->data && argsL->dataSize > 0)
    {
        cArgs.UnpackCompiledArguments((uint8_t*)argsL->data, argsL->dataSize);
    }
    
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

    cargs* callResults = args_create();
    
    std::vector<CLuaArgument*>::const_iterator iter = cResults.IterBegin();

    for (; iter != cResults.IterEnd(); iter++)
    {
        CLuaArgument* result = *iter;

        args_read_clua_argument(callResults, result);
    }

    return argStream.ReturnSystemPointer(callResults);
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

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsCreate, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    cargs* argsL = args_create();

    return argStream.ReturnSystemPointer(argsL);
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsDelete, env, args, results)
{
    cargs* argsL;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);

    args_delete(argsL);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsPushNull, env, args, results)
{
    cargs* argsL;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);

    args_push_null(argsL);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsPushBoolean, env, args, results)
{
    cargs* argsL;
    bool   arg;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadBoolean(arg);

    args_push_boolean(argsL, arg);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsPushNumber, env, args, results)
{
    cargs*    argsL;
    float64_t arg;
    
    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadFloat64(arg);

    args_push_number(argsL, arg);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsPushString, env, args, results)
{
    cargs*  argsL;
    SString arg;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadString(arg);

    args_push_string(argsL, arg.data(), arg.size());

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsPushCallable, env, args, results)
{
    cargs*   argsL;
    uint8_t* arg;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadPointer(arg);

    if (!arg)
    {
        return argStream.ReturnNull();
    }

    args_push_callable(argsL, arg);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsPushUserdata, env, args, results)
{
    cargs*               argsL;
    CWebAssemblyUserData arg;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadIntPtr(arg);

    args_push_userdata(argsL, arg);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsPushList, env, args, results)
{
    cargs* argsL;
    cargs* arg;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadSystemPointer(arg);

    args_push_list(argsL, arg);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsPop, env, args, results)
{
    cargs*  argsL;
    int32_t count;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadInt32(count);

    args_pop(argsL, count);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsGetArgType, env, args, results)
{
    cargs*  argsL;
    int32_t index;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadInt32(index);

    return argStream.Return((int32_t)args_get_arg_type(argsL, index));
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsGetArgAsBoolean, env, args, results)
{
    cargs*  argsL;
    int32_t index;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadInt32(index);

    return argStream.Return(args_get_arg_as_boolean(argsL, index));
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsGetArgAsNumber, env, args, results)
{
    cargs*  argsL;
    int32_t index;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadInt32(index);

    return argStream.Return(args_get_arg_as_number(argsL, index));
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsGetArgAsString, env, args, results)
{
    cargs*                           argsL;
    int32_t                          index;
    CWebAssemblyMemoryPointerAddress outString;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadInt32(index);
    argStream.ReadPointerAddress(outString);
    argStream.ReadUInt32(maxSize);

    if (outString == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    size_t size = 0;
    char* argData = args_get_arg_as_string(argsL, index, &size);

    if (!argData || size < 1)
    {
        return argStream.Return(0);
    }

    size_t length = std::min(size, maxSize);

    if (length < 1)
    {
        free((void*)argData);
        return argStream.Return(0);
    }

    argStream.WritePointer(outString, argData, length);

    free((void*)argData);

    return argStream.Return(length);
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsGetArgAsCallable, env, args, results)
{
    cargs*                           argsL;
    int32_t                          index;
    CWebAssemblyMemoryPointerAddress outCallable;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadInt32(index);
    argStream.ReadPointerAddress(outCallable);

    if (outCallable == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    uint8_t* argData = args_get_arg_as_callable(argsL, index);

    if (!argData)
    {
        return argStream.Return(false);
    }

    argStream.WritePointer(outCallable, argData, C_CALLABLE_HASH_SIZE);

    free((void*)argData);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsGetArgAsUserdata, env, args, results)
{
    cargs*  argsL;
    int32_t index;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadInt32(index);

    return argStream.Return(args_get_arg_as_userdata(argsL, index));
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsGetArgAsList, env, args, results)
{
    cargs*  argsL;
    int32_t index;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);
    argStream.ReadInt32(index);

    return argStream.ReturnSystemPointer(args_get_arg_as_list(argsL, index));
}

WebAssemblyApi(CWebAssemblyUtilDefs::ArgsGetArgCount, env, args, results)
{
    cargs* argsL;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(argsL);

    if (!argsL)
    {
        return 0;
    }

    return argStream.Return((uint32_t)argsL->argsCount);
}

cargs* args_create() {
    cargs* args = (cargs*)malloc(sizeof(cargs));

    if (!args) {
        return NULL;
    }

    memset((void*)args, 0, sizeof(cargs));

    args->argsCount = 0;
    args->dataSize = 1;
    args->data = malloc(args->dataSize);
    memset(args->data, 0, args->dataSize);

    return args;
}

void args_delete(cargs* args) {
    if (!args) {
        return;
    }

    if (args->data) {
        free(args->data);
    }

    memset((void*)args, 0, sizeof(cargs));
    
    free((void*)args);
}

size_t args_calculate_arg_heap_size(cargs* args, size_t size, char type) {
    if (!args) {
        return 0;
    }

    size_t syntaxSize = 0;

    switch (type) {
        case C_ARGS_TYPE_ID_CALLABLE:
        case C_ARGS_TYPE_ID_STRING:
            syntaxSize += C_ARGS_STRING_SYNTAX_LENGTH * 2;
            break;
        case C_ARGS_TYPE_ID_LIST:
            syntaxSize += (
                C_ARGS_LIST_OPEN_SYNTAX_LENGTH +
                C_ARGS_LIST_CLOSE_SYNTAX_LENGTH +
                C_ARGS_LIST_ARGC_OPEN_SYNTAX_LENGTH +
                C_ARGS_LIST_ARGC_CLOSE_SYNTAX_LENGTH
            );
            break;
        default:
            break;
    }

    return args->dataSize + size + (args->dataSize == 1 ? -1 : 0) + syntaxSize + C_ARGS_SEPRATOR_LENGTH + C_ARGS_TYPE_IDENTIFIER_LENGTH;
}

void args_push_null(cargs* args) {
    if (!args || !args->data) {
        return;
    }

    char type = C_ARGS_TYPE_ID_NULL;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    size_t size = 0;

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_boolean(cargs* args, bool boolean) {
    if (!args || !args->data) {
        return;
    }

    char type = C_ARGS_TYPE_ID_BOOLEAN;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    size_t size = sizeof(bool);

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    *data = boolean ? '1' : '0';
    data += sizeof(char);

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_number(cargs* args, double number) {
    if (!args || !args->data) {
        return;
    }

    char type = C_ARGS_TYPE_ID_NUMBER;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    char numberBuffer[100];
    size_t size = sprintf(numberBuffer, "%lf", number);

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;
    
    *data = type;
    data += sizeof(type);

    memcpy((void*)data, (void*)numberBuffer, size);
    data += size;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_string(cargs* args, const char* string, size_t size) {
    if (!args || !args->data) {
        return;
    }

    if (!string) {
        size = 0;
    }

    char* cpy = (char*)malloc(size);

    if (size > 0) {
        size_t stringPassCount = 0;

        char* pos = (char*)string;
        
        for (long long i = 0; i < size; i++, pos++) {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                size += C_ARGS_STRING_PASS_SYNTAX_LENGTH;

                cpy = (char*)realloc((void*)cpy, size);
                memcpy((void*)(cpy + i), C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH);

                i += C_ARGS_STRING_PASS_SYNTAX_LENGTH;
            }
            
            cpy[i] = *pos;
        }
    }

    char type = C_ARGS_TYPE_ID_STRING;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH);
    data += C_ARGS_STRING_SYNTAX_LENGTH;

    if (cpy && size > 0) {
        memcpy((void*)data, cpy, size);
        data += size;
    
        free((void*)cpy);
    }

    memcpy((void*)data, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH);
    data += C_ARGS_STRING_SYNTAX_LENGTH;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_callable(cargs* args, uint8_t* string) {
    if (!args || !args->data) {
        return;
    }

    size_t size = C_CALLABLE_HASH_SIZE;

    if (!string) {
        size = 0;
    }

    char* cpy = (char*)malloc(size);

    if (size > 0) {
        size_t stringPassCount = 0;

        char* pos = (char*)string;
        
        for (long long i = 0; i < size; i++, pos++) {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                size += C_ARGS_STRING_PASS_SYNTAX_LENGTH;

                cpy = (char*)realloc((void*)cpy, size);
                memcpy((void*)(cpy + i), C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH);

                i += C_ARGS_STRING_PASS_SYNTAX_LENGTH;
            }
            
            cpy[i] = *pos;
        }
    }

    char type = C_ARGS_TYPE_ID_CALLABLE;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH);
    data += C_ARGS_STRING_SYNTAX_LENGTH;

    if (cpy && size > 0) {
        memcpy((void*)data, cpy, size);
        data += size;
    
        free((void*)cpy);
    }

    memcpy((void*)data, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH);
    data += C_ARGS_STRING_SYNTAX_LENGTH;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_userdata(cargs* args, CWebAssemblyUserData userdata) {
    if (!args || !args->data) {
        return;
    }

    char type = C_ARGS_TYPE_ID_USERDATA;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    char userdataBuffer[100];
    size_t size = sprintf(userdataBuffer, "%d", (CWebAssemblyUserData)userdata);

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, (void*)userdataBuffer, size);
    data += size;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_list(cargs* args, cargs* list) {
    if (!args || !args->data) {
        return;
    }

    size_t size = 0;
    
    if (list && list->data) {
        size = list->dataSize == 1 ? 0 : list->dataSize;
    }

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    char type = C_ARGS_TYPE_ID_LIST;

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);

    char argsCountString[12];
    int argsCountStringLength = snprintf(argsCountString, sizeof(argsCountString), "%zu", list->argsCount);
    
    args->dataSize += argsCountStringLength;

    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, C_ARGS_LIST_ARGC_OPEN_SYNTAX, C_ARGS_LIST_ARGC_OPEN_SYNTAX_LENGTH);
    data += C_ARGS_LIST_ARGC_OPEN_SYNTAX_LENGTH;

    memcpy((void*)data, argsCountString, argsCountStringLength);
    data += argsCountStringLength;

    memcpy((void*)data, C_ARGS_LIST_ARGC_CLOSE_SYNTAX, C_ARGS_LIST_ARGC_CLOSE_SYNTAX_LENGTH);
    data += C_ARGS_LIST_ARGC_CLOSE_SYNTAX_LENGTH;

    memcpy((void*)data, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH);
    data += C_ARGS_LIST_OPEN_SYNTAX_LENGTH;

    if (list && list->data && size > 0) {
        memcpy((void*)data, list->data, size);
        data += size;
    }

    memcpy((void*)data, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH);
    data += C_ARGS_LIST_CLOSE_SYNTAX_LENGTH;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_pop(cargs* args, size_t count) {
    if (!args || !args->data) {
        return;
    }

    if (count > args->argsCount) {
        count = args->argsCount;
    }

    if (count < 1) {
        return;
    }

    if (args->dataSize <= C_ARGS_SEPRATOR_LENGTH) {
        return;
    }

    char* data = (char*)args->data;

    int  openedList = 0;
    bool isInsideString = false;

    for (long long i = args->dataSize - 1 - C_ARGS_SEPRATOR_LENGTH; i >= 0 && count > 0; i--) {
        char* pos = data + i;

        if (isInsideString) {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                if (strncmp(pos - 1, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
                    continue;
                }

                isInsideString = false;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
            isInsideString = true;
            continue;
        }

        if (openedList > 0) {
            if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0) {
                openedList++;
            }else if (strncmp(pos, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH) == 0) {
                openedList--;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0) {
            openedList++;
        }

        if (strncmp(pos, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH) == 0) {
            size_t popSize = args->dataSize - i - C_ARGS_SEPRATOR_LENGTH;

            args->argsCount--;
            args->dataSize -= popSize;
            
            if (args->dataSize < 1) {
                args->dataSize = 1;
            }

            memset((void*)(data + args->dataSize), 0, popSize);

            args->data = realloc(args->data, args->dataSize);

            count--;
        }
    }

    if (count > 0) {
        args->argsCount = 0;
        args->dataSize = 1;
        args->data = realloc(args->data, args->dataSize);
        memset(args->data, 0, args->dataSize);
    }
}

long long args_find_arg_start_position(cargs* args, size_t index, size_t from) {
    if (!args || !args->data || args->argsCount == 0) {
        return C_ARGS_INVALID_POSITION;
    }

    if (index > args->argsCount || index < 1) {
        return C_ARGS_INVALID_POSITION;
    }

    if (args->dataSize < C_ARGS_SEPRATOR_LENGTH) {
        return C_ARGS_INVALID_POSITION;
    }

    bool isFirst = index == 1;

    index = args->argsCount - index + 1;

    char* data = (char*)args->data;

    int  openedList = 0;
    bool isInsideString = false;

    long long position = 0;

    for (long long i = args->dataSize - 1 - C_ARGS_SEPRATOR_LENGTH; i >= from && index > 0; i--) {
        char* pos = data + i;

        if (isInsideString) {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                if (strncmp(pos - 1, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
                    continue;
                }

                isInsideString = false;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
            isInsideString = true;
            continue;
        }

        if (openedList > 0) {
            if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0) {
                openedList++;
            }else if (strncmp(pos, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH) == 0) {
                openedList--;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0) {
            openedList++;
        }

        if (strncmp(pos, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH) == 0) {
            position = i + C_ARGS_SEPRATOR_LENGTH;

            index--;
        }
    }

    if (index > 0) {
        if (isFirst) {
            return 0;
        }

        return C_ARGS_INVALID_POSITION;
    }

    return position;
}

char args_get_arg_type(cargs* args, size_t index) {
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return C_ARGS_TYPE_ID_UNKNOWN;
    }

    return *((char*)args->data + pos);
}

bool args_get_arg_as_boolean(cargs* args, size_t index) {
    bool result = false;

    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return result;
    }

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_BOOLEAN) {
        return result;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH;

    result = *data == '1' ? true : false;

    return result;
}

double args_get_arg_as_number(cargs* args, size_t index) {
    double number = 0;

    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return number;
    }

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_NUMBER) {
        return number;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH;

    number = strtod(data, NULL);

    return number;
}

char* args_get_arg_as_string(cargs* args, size_t index, size_t* bsize) {
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return NULL;
    }

    long long end = args_find_arg_start_position(args, index + 1, pos);

    if (end == C_ARGS_INVALID_POSITION) {
        end = args->dataSize;
    }

    end -= C_ARGS_SEPRATOR_LENGTH;

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_STRING) {
        return NULL;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH + C_ARGS_STRING_SYNTAX_LENGTH;

    size_t size = end - pos - C_ARGS_SEPRATOR_LENGTH - C_ARGS_STRING_SYNTAX_LENGTH;

    char* str = (char*)malloc(size + 1);
    memset((void*)str, 0, size + 1);

    size_t sSize = size;
    for (size_t i = 0, ci = 0; i < sSize; i++) {
        char* chunk = data + i;

        if (strncmp(chunk, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
            if (strncmp(chunk + C_ARGS_STRING_PASS_SYNTAX_LENGTH, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                size -= C_ARGS_STRING_PASS_SYNTAX_LENGTH;
                str = (char*)realloc((void*)str, size);

                continue;
            }
        }
        
        str[ci] = *chunk;
        ci++;
    }

    if (bsize) {
        *bsize = size;
    }

    return str;
}

uint8_t* args_get_arg_as_callable(cargs* args, size_t index)
{
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return NULL;
    }

    long long end = args_find_arg_start_position(args, index + 1, pos);

    if (end == C_ARGS_INVALID_POSITION) {
        end = args->dataSize;
    }

    end -= C_ARGS_SEPRATOR_LENGTH;

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_CALLABLE) {
        return NULL;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH + C_ARGS_STRING_SYNTAX_LENGTH;

    size_t size = end - pos - C_ARGS_SEPRATOR_LENGTH - C_ARGS_STRING_SYNTAX_LENGTH;

    char* str = (char*)malloc(size + 1);
    memset((void*)str, 0, size + 1);

    size_t sSize = size;
    for (size_t i = 0, ci = 0; i < sSize; i++) {
        char* chunk = data + i;

        if (strncmp(chunk, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
            if (strncmp(chunk + C_ARGS_STRING_PASS_SYNTAX_LENGTH, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                size -= C_ARGS_STRING_PASS_SYNTAX_LENGTH;
                str = (char*)realloc((void*)str, size);

                continue;
            }
        }
        
        str[ci] = *chunk;
        ci++;
    }

    return (uint8_t*)str;
}

CWebAssemblyUserData args_get_arg_as_userdata(cargs* args, size_t index) {
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return NULL;
    }

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_USERDATA) {
        return NULL;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH;

    if (sizeof(CWebAssemblyUserData) == 8) {
        return (CWebAssemblyUserData)strtoll(data, NULL, 10);
    }

    return (CWebAssemblyUserData)atoi(data);
}

cargs* args_get_arg_as_list(cargs* args, size_t index) {
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return NULL;
    }

    long long end = args_find_arg_start_position(args, index + 1, pos);

    if (end == C_ARGS_INVALID_POSITION) {
        end = args->dataSize;
    }

    end -= C_ARGS_SEPRATOR_LENGTH;

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_LIST) {
        return NULL;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH + C_ARGS_LIST_ARGC_OPEN_SYNTAX_LENGTH;

    uintptr_t lastPlace = (uintptr_t)data;
    size_t argsCount = (size_t)strtol(data, &data, 10);
    uintptr_t argsCountCharLength = (uintptr_t)data - lastPlace;

    data += C_ARGS_LIST_ARGC_CLOSE_SYNTAX_LENGTH + C_ARGS_LIST_OPEN_SYNTAX_LENGTH;

    size_t size = end - pos - C_ARGS_SEPRATOR_LENGTH*2 - C_ARGS_LIST_CLOSE_SYNTAX_LENGTH - argsCountCharLength;

    cargs* list = args_create();

    list->argsCount = argsCount;
    list->dataSize = size < 1 ? 1 : size;
    list->data = malloc(list->dataSize);

    if (size > 0) {
        memcpy(list->data, (void*)data, size);
    }

    return list;
}

void args_read_clua_argument(cargs* args, CLuaArgument* arg)
{
    if (!args || !arg)
    {
        return;
    }

    switch (arg->GetType())
    {
        case LUA_TNIL:
            args_push_null(args);
            break;
        case LUA_TBOOLEAN:
            args_push_boolean(args, arg->GetBoolean());
            break;
        case LUA_TNUMBER:
            args_push_number(args, arg->GetNumber());
            break;
        case LUA_TSTRING:
            args_push_string(args, arg->GetString().c_str(), arg->GetString().size());
            break;
        case LUA_TTABLE:
            {
                cargs* table = args_create();

                CLuaArguments* tableData = arg->GetTableData();

                if (tableData)
                {
                    std::vector<CLuaArgument*>::const_iterator iter = tableData->IterBegin();

                    for (; iter != tableData->IterEnd(); iter++)
                    {
                        args_read_clua_argument(table, *iter);
                    }
                }

                args_push_list(args, table);

                args_delete(table);
            }

            break;
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
            args_push_userdata(args, (CWebAssemblyUserData)arg->GetUserData());
            break;
        case LUA_TFUNCTION:
            {
                CCallable callable = arg->GetCallable();

                uint8_t hash[C_CALLABLE_HASH_SIZE];

                callable.WriteHash(hash);

                args_push_callable(args, hash);
            }
            break;
        default:
            break;
    }
}
