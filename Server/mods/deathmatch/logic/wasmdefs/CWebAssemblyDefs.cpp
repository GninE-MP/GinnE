/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWebAssemblyDefs.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"

#include "CWebAssemblyUtilDefs.h"
#include "CWebAssemblyElementDefs.h"
#include "CWebAssemblyPlayerDefs.h"
#include "CWebAssemblyTeamDefs.h"
#include "CWebAssemblyThreadDefs.h"
#include "CWebAssemblyTimerDefs.h"
#include "CWebAssemblyCameraDefs.h"
#include "CWebAssemblyNetworkDefs.h"
#include "CWebAssemblyVoiceDefs.h"
#include "CWebAssemblyWaterDefs.h"
#include "CWebAssemblyRadarAreaDefs.h"
#include "CWebAssemblyPickupDefs.h"
#include "CWebAssemblyMarkerDefs.h"
#include "CWebAssemblyColShapeDefs.h"
#include "CWebAssemblyBlipDefs.h"
#include "CWebAssemblyDatabaseDefs.h"
#include "CWebAssemblyFileDefs.h"

CFastHashMap<SString, CWebAssemblyFunctionType> FunctionTypes;

void CWebAssemblyDefs::RegisterApiFunctionTypes()
{
    CWebAssemblyUtilDefs::RegisterFunctionTypes();
    CWebAssemblyElementDefs::RegisterFunctionTypes();
    CWebAssemblyPlayerDefs::RegisterFunctionTypes();
    CWebAssemblyTeamDefs::RegisterFunctionTypes();
    CWebAssemblyThreadDefs::RegisterFunctionTypes();
    CWebAssemblyTimerDefs::RegisterFunctionTypes();
    CWebAssemblyCameraDefs::RegisterFunctionTypes();
    CWebAssemblyNetworkDefs::RegisterFunctionTypes();
    CWebAssemblyVoiceDefs::RegisterFunctionTypes();
    CWebAssemblyWaterDefs::RegisterFunctionTypes();
    CWebAssemblyRadarAreaDefs::RegisterFunctionTypes();
    CWebAssemblyPickupDefs::RegisterFunctionTypes();
    CWebAssemblyMarkerDefs::RegisterFunctionTypes();
    CWebAssemblyColShapeDefs::RegisterFunctionTypes();
    CWebAssemblyBlipDefs::RegisterFunctionTypes();
    CWebAssemblyDatabaseDefs::RegisterFunctionTypes();
    CWebAssemblyFileDefs::RegisterFunctionTypes();
}

void CWebAssemblyDefs::RegisterApi(CWebAssemblyScript* script)
{
    CWebAssemblyUtilDefs::RegisterFunctions(script);
    CWebAssemblyElementDefs::RegisterFunctions(script);
    CWebAssemblyPlayerDefs::RegisterFunctions(script);
    CWebAssemblyTeamDefs::RegisterFunctions(script);
    CWebAssemblyThreadDefs::RegisterFunctions(script);
    CWebAssemblyTimerDefs::RegisterFunctions(script);
    CWebAssemblyCameraDefs::RegisterFunctions(script);
    CWebAssemblyNetworkDefs::RegisterFunctions(script);
    CWebAssemblyVoiceDefs::RegisterFunctions(script);
    CWebAssemblyWaterDefs::RegisterFunctions(script);
    CWebAssemblyRadarAreaDefs::RegisterFunctions(script);
    CWebAssemblyPickupDefs::RegisterFunctions(script);
    CWebAssemblyMarkerDefs::RegisterFunctions(script);
    CWebAssemblyColShapeDefs::RegisterFunctions(script);
    CWebAssemblyBlipDefs::RegisterFunctions(script);
    CWebAssemblyDatabaseDefs::RegisterFunctions(script);
    CWebAssemblyFileDefs::RegisterFunctions(script);
}

void CWebAssemblyDefs::SetFunctionType(const SString& functionName, CWebAssemblyFunctionType functionType)
{
    if (functionName.empty())
    {
        return;
    }

    FunctionTypes[GetApiName(functionName)] = functionType;
}

void CWebAssemblyDefs::SetFunctionType(const SString& functionName, const SString& structure)
{
    if (functionName.empty())
    {
        return;
    }

    size_t length = structure.length();

    CWebAssemblyVariables args, results;

    if (length > 0)
    {
        PushType(&results, structure[0]);
    }

    for (size_t i = 1; i < length; i++)
    {
        PushType(&args, structure[i]);
    }

    FunctionTypes[GetApiName(functionName)] = CWebAssemblyFunctionType(args, results);
}

CWebAssemblyFunctionType CWebAssemblyDefs::GetFunctionType(const SString& functionName)
{
    return FunctionTypes[GetApiName(functionName)];
}

bool CWebAssemblyDefs::ExistsFunctionType(const SString& functionName)
{
    return FunctionTypes.find(GetApiName(functionName)) != FunctionTypes.end();
}

SString CWebAssemblyDefs::GetApiName(const SString& functionName)
{
    SString name = "gnine_";
    name += functionName;

    return name;
}

void CWebAssemblyDefs::PushType(CWebAssemblyVariables* vars, const char& flag)
{
    if (!vars)
    {
        return;
    }

    if (!flag)
    {
        return;
    }
    
    switch (flag)
    {
        case 'v': // void
            break;
        case 'i': // int32 - int
            vars->PushInt32();
            break;
        case 'l': // int64 - long long
            vars->PushInt64();
            break;
        case 'f': // float32 - float
            vars->PushFloat32();
            break;
        case 'd': // float64 - double
            vars->PushFloat64();
            break;
        case 's': // int32 - string
            vars->PushString();
            break;
        case 'c': // int32 - callback
            vars->PushInt32();
            break;
        case '*': // int32 - pointer
            vars->PushPointer();
            break;
        case 'b': // int32 - bool
            vars->PushInt32();
            break;
        case 'x': // int32 - pointer size
            vars->PushInt32();
            break;
        case 'z': // intptr - app architecture size [x86, x64]
            #if IS_APP_ON_64_BIT_VERSION
                vars->PushInt64();
            #else
                vars->PushInt32();
            #endif

            break;
        case 'u': // intptr - userdata
            #if IS_APP_ON_64_BIT_VERSION
                vars->PushInt64();
            #else
                vars->PushInt32();
            #endif
            
            break;
        case 'e': // element - userdata
            #if IS_APP_ON_64_BIT_VERSION
                vars->PushInt64();
            #else
                vars->PushInt32();
            #endif

            break;
        default:
            break;
    }
}
