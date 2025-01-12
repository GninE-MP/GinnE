/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyVoiceDefs.cpp
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

#include "CMainConfig.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyObjectDefs.h"

void CWebAssemblyObjectDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_object", "ei**b");

    SetFunctionType("get_object_rotation", "be*");
    SetFunctionType("get_object_scale", "be*");
    SetFunctionType("is_object_breakable", "be");

    SetFunctionType("set_object_rotation", "be*");
    SetFunctionType("set_object_scale", "be*");
    SetFunctionType("set_object_breakable", "beb");
    SetFunctionType("move_object", "bei**sfff");
    SetFunctionType("stop_object", "be");
}

void CWebAssemblyObjectDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_object", CreateObject },

        { "get_object_rotation", GetObjectRotation },
        { "get_object_scale", GetObjectScale },
        { "is_object_breakable", IsObjectBreakable },
        
        { "set_object_rotation", SetObjectRotation },
        { "set_object_scale", SetObjectScale },
        { "set_object_breakable", SetObjectBreakable },
        { "move_object", MoveObject },
        { "stop_object", StopObject }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyObjectDefs::CreateObject, env, args, results)
{
    int32_t  modelIdInt;
    CVector  position;
    CVector  rotation;
    bool     isLowLOD;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadInt32(modelIdInt);
    argStream.ReadVector3D(position);
    argStream.ReadVector3D(rotation);
    argStream.ReadBoolean(isLowLOD, false);

    ushort modelId = (ushort)modelIdInt;

    if (CObjectManager::IsValidModel(modelId))
    {
        CLuaMain* pLuaMain = GetWebAssemblyLuaMain(env);

        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();

            if (pResource)
            {
                CObject* pObject = CStaticFunctionDefinitions::CreateObject(pResource, modelId, position, rotation, isLowLOD);

                if (pObject)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();

                    if (pGroup)
                    {
                        pGroup->Add(pObject);
                    }

                    return argStream.Return((CElement*)pObject);
                }
            }
        }
    }

    return argStream.Return(NULL);
}

WebAssemblyApi(CWebAssemblyObjectDefs::GetObjectRotation, env, args, results)
{
    CObject* object;
    struct
    {
        float32_t x;
        float32_t y;
        float32_t z;
    }*       outVector;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadUserData(object);
    argStream.ReadPointer(outVector);

    if (!object || !outVector)
    {
        return argStream.Return(false);
    }

    CVector rotation;

    if (!CStaticFunctionDefinitions::GetObjectRotation(object, rotation))
    {
        argStream.Return(false);
    }

    outVector->x = rotation.fX;
    outVector->y = rotation.fY;
    outVector->z = rotation.fZ;

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyObjectDefs::GetObjectScale, env, args, results)
{
    CObject* object;
    struct
    {
        float32_t x;
        float32_t y;
        float32_t z;
    }*       outScale;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadUserData(object);
    argStream.ReadPointer(outScale);

    if (!object || !outScale)
    {
        return argStream.Return(false);
    }

    const CVector& scale = object->GetScale();

    outScale->x = scale.fX;
    outScale->y = scale.fY;
    outScale->z = scale.fZ;

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyObjectDefs::IsObjectBreakable, env, args, results)
{
    CObject* object;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadUserData(object);

    if (!object)
    {
        return argStream.Return(false);
    }

    return argStream.Return(object->IsBreakable());
}

WebAssemblyApi(CWebAssemblyObjectDefs::SetObjectRotation, env, args, results)
{
    CObject* object;
    CVector  rotation;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadUserData(object);
    argStream.ReadVector3D(rotation);

    if (!object)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetObjectRotation(object, rotation));
}

WebAssemblyApi(CWebAssemblyObjectDefs::SetObjectScale, env, args, results)
{
    CObject* object;
    CVector  scale;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadUserData(object);
    argStream.ReadVector3D(scale);

    if (!object)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetObjectScale(object, scale));
}

WebAssemblyApi(CWebAssemblyObjectDefs::SetObjectBreakable, env, args, results)
{
    CObject* object;
    bool     breakable;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadUserData(object);
    argStream.ReadBoolean(breakable);

    if (!object)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetObjectBreakable(object, breakable));
}

WebAssemblyApi(CWebAssemblyObjectDefs::MoveObject, env, args, results)
{
    CElement*  object;
    int32_t    time;
    CVector    targetPosition;
    CVector    targetRotation;
    SString    easingTypeString;
    float32_t  easingPeriod;
    float32_t  easingAmplitude;
    float32_t  easingOvershoot;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadUserData(object);
    argStream.ReadInt32(time);
    argStream.ReadVector3D(targetPosition);
    argStream.ReadVector3D(targetRotation);
    argStream.ReadString(easingTypeString);
    argStream.ReadFloat32(easingPeriod, 0.3f);
    argStream.ReadFloat32(easingAmplitude, 1.0f);
    argStream.ReadFloat32(easingOvershoot, 1.70158f);

    if (!object)
    {
        return argStream.Return(false);
    }

    CEasingCurve::eType easingType = CEasingCurve::Linear;

    if (!easingTypeString.empty()) StringToEnum(easingTypeString, easingType);

    
    CLuaMain* pLuaMain = GetWebAssemblyLuaMain(env);

    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();

        if (pResource)
        {
            return argStream.Return(CStaticFunctionDefinitions::MoveObject(pResource, object, time, targetPosition, targetRotation, easingType, easingPeriod, easingAmplitude, easingOvershoot));
        }
    }

    return argStream.Return(false);
}

WebAssemblyApi(CWebAssemblyObjectDefs::StopObject, env, args, results)
{
    CElement* object;

    CWebAssemblyArgReader argStream(env, args, results);
    argStream.ReadUserData(object);

    if (!object)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::StopObject(object));
}
