/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyCameraDefs.cpp
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

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyCameraDefs.h"

struct DummyVector
{
    float32_t x;
    float32_t y;
    float32_t z;
};

struct CameraMatrix
{
    DummyVector position;
    DummyVector lookAt;
    float32_t   roll;
    float32_t   fov;
};

void CWebAssemblyCameraDefs::RegisterFunctionTypes()
{
    SetFunctionType("get_camera_matrix", "bu*");
    SetFunctionType("get_camera_target", "uu");
    SetFunctionType("get_camera_interior", "iu");

    SetFunctionType("set_camera_matrix", "bu*");
    SetFunctionType("set_camera_target", "buu");
    SetFunctionType("set_camera_interior", "bui");
    SetFunctionType("fade_camera", "bubf*");
}

void CWebAssemblyCameraDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "get_camera_matrix", GetCameraMatrix },
        { "get_camera_target", GetCameraTarget },
        { "get_camera_interior", GetCameraInterior },

        { "set_camera_matrix", SetCameraMatrix },
        { "set_camera_target", SetCameraTarget },
        { "set_camera_interior", SetCameraInterior },
        { "fade_camera", FadeCamera }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyCameraDefs::GetCameraMatrix, env, args, results)
{
    CPlayer*                         player;
    CWebAssemblyMemoryPointerAddress outCameraMatrix;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outCameraMatrix);

    if (!player || outCameraMatrix == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    CVector   position, lookAt;
    float32_t roll, fov;

    if (!CStaticFunctionDefinitions::GetCameraMatrix(player, position, lookAt, roll, fov))
    {
        return argStream.Return(false);
    }

    CameraMatrix matrix;
    matrix.position.x = position.fX;
    matrix.position.y = position.fY;
    matrix.position.z = position.fZ;
    matrix.lookAt.x = lookAt.fX;
    matrix.lookAt.y = lookAt.fY;
    matrix.lookAt.z = lookAt.fZ;
    matrix.roll = roll;
    matrix.fov = fov;

    argStream.WritePointer(outCameraMatrix, &matrix);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyCameraDefs::GetCameraTarget, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.ReturnNull();
    }

    CElement* target = CStaticFunctionDefinitions::GetCameraTarget(player);

    if (!target)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(target);
}

WebAssemblyApi(CWebAssemblyCameraDefs::GetCameraInterior, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(0);
    }

    uint8_t interior;

    if (!CStaticFunctionDefinitions::GetCameraInterior(player, interior))
    {
        return argStream.Return(0);
    }

    return argStream.Return((int32_t)interior);
}

WebAssemblyApi(CWebAssemblyCameraDefs::SetCameraMatrix, env, args, results)
{
    CPlayer*      player;
    CameraMatrix* matrix;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointer(matrix);

    if (!player || !matrix)
    {
        return argStream.Return(false);
    }

    CVector position, lookAt;

    position.fX = matrix->position.x;
    position.fY = matrix->position.y;
    position.fZ = matrix->position.z;

    lookAt.fX = matrix->lookAt.x;
    lookAt.fY = matrix->lookAt.y;
    lookAt.fZ = matrix->lookAt.z;

    float fov = matrix->fov;

    if (fov <= 0.0f || fov >= 180.0f)
    {
        fov = 70.0f;
    }

    return argStream.Return(CStaticFunctionDefinitions::SetCameraMatrix(player, position, &lookAt, matrix->roll, fov));
}

WebAssemblyApi(CWebAssemblyCameraDefs::SetCameraTarget, env, args, results)
{
    CPlayer*  player;
    CElement* target;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUserData(target, (CElement*)NULL);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetCameraTarget(player, target));
}

WebAssemblyApi(CWebAssemblyCameraDefs::SetCameraInterior, env, args, results)
{
    CPlayer* player;
    int32_t  interior;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadInt32(interior);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetCameraInterior(player, (uint8_t)interior));
}

WebAssemblyApi(CWebAssemblyCameraDefs::FadeCamera, env, args, results)
{
    CPlayer* player;
    bool     fadeIn;
    float    timeToFade;
    struct C
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;

    C defaultColor = { 0 };
    defaultColor.a = 0xff;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadBoolean(fadeIn);
    argStream.ReadFloat32(timeToFade, 1.0f);
    argStream.ReadPointer(color, &defaultColor);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::FadeCamera(player, fadeIn, timeToFade, color->r, color->g, color->b));
}
