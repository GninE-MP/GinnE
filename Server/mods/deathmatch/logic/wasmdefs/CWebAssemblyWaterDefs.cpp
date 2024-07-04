/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyWaterDefs.cpp
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

#include "CWater.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyWaterDefs.h"

void CWebAssemblyWaterDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_water", "u****b");
    SetFunctionType("set_water_level", "bufbbbb");
    SetFunctionType("reset_water_level", "b");
    SetFunctionType("get_water_vertex_position", "bui*");
    SetFunctionType("set_water_vertex_position", "bui*");
    SetFunctionType("get_water_color", "b*");
    SetFunctionType("set_water_color", "b*");
    SetFunctionType("reset_water_color", "b");
}

void CWebAssemblyWaterDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_water", CreateWater },
        { "set_water_level", SetWaterLevel },
        { "reset_water_level", ResetWaterLevel },
        { "get_water_vertex_position", GetWaterVertexPosition },
        { "set_water_vertex_position", SetWaterVertexPosition },
        { "get_water_color", GetWaterColor },
        { "set_water_color", SetWaterColor },
        { "reset_water_color", ResetWaterColor }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyWaterDefs::CreateWater, env, args, results)
{
    CVector v1;
    CVector v2;
    CVector v3;
    CVector v4;
    bool    shallow;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(v1);
    argStream.ReadVector3D(v2);
    argStream.ReadVector3D(v3);
    argStream.ReadVector3D(v4);
    argStream.ReadBoolean(shallow, false);

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CWater* water = CStaticFunctionDefinitions::CreateWater(resource, &v1, &v2, &v3, &v4, shallow);

    if (!water)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(water);
    }

    return argStream.Return((CElement*)water);
}

WebAssemblyApi(CWebAssemblyWaterDefs::SetWaterLevel, env, args, results)
{
    CWater*   water;
    float32_t level;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(water);
    argStream.ReadFloat32(level);

    if (!water)
    {
        bool includeWorldNonSeaLevel;
        bool includeAllWaterElements;
        bool includeWorldSeaLevel;
        bool includeOutsideWorldLevel;

        argStream.ReadBoolean(includeWorldNonSeaLevel, true);
        argStream.ReadBoolean(includeAllWaterElements, true);
        argStream.ReadBoolean(includeWorldSeaLevel, true);
        argStream.ReadBoolean(includeOutsideWorldLevel, false);

        if (includeAllWaterElements)
        {
            CStaticFunctionDefinitions::SetAllElementWaterLevel(level);
        }

        return argStream.Return(CStaticFunctionDefinitions::SetWorldWaterLevel(level, includeWorldNonSeaLevel, includeWorldSeaLevel, includeOutsideWorldLevel));
    }

    return argStream.Return(CStaticFunctionDefinitions::SetElementWaterLevel(water, level));
}

WebAssemblyApi(CWebAssemblyWaterDefs::ResetWaterLevel, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.Return(CStaticFunctionDefinitions::ResetWorldWaterLevel());
}

WebAssemblyApi(CWebAssemblyWaterDefs::GetWaterVertexPosition, env, args, results)
{
    CWater*                          water;
    int32_t                          vertexIndex;
    CWebAssemblyMemoryPointerAddress outPosition;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(water);
    argStream.ReadInt32(vertexIndex);
    argStream.ReadPointerAddress(outPosition);

    if (!water || outPosition == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    CVector pos;
    if (!CStaticFunctionDefinitions::GetWaterVertexPosition(water, vertexIndex, pos))
    {
        return argStream.Return(false);
    }

    struct
    {
        float32_t x;
        float32_t y;
        float32_t z;
    } p;

    p.x = pos.fX;
    p.y = pos.fY;
    p.z = pos.fZ;

    argStream.WritePointer(outPosition, &p);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyWaterDefs::SetWaterVertexPosition, env, args, results)
{
    CWater* water;
    int32_t vertexIndex;
    CVector position;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(water);
    argStream.ReadInt32(vertexIndex);
    argStream.ReadVector3D(position);

    if (!water)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetWaterVertexPosition(water, vertexIndex, position));
}

WebAssemblyApi(CWebAssemblyWaterDefs::GetWaterColor, env, args, results)
{
    CWebAssemblyMemoryPointerAddress outColor;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointerAddress(outColor);

    if (outColor == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } c;

    if (!CStaticFunctionDefinitions::GetWaterColor(c.r, c.g, c.b, c.a))
    {
        return argStream.Return(false);
    }

    argStream.WritePointer(outColor, &c);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyWaterDefs::SetWaterColor, env, args, results)
{
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(color);

    return argStream.Return(CStaticFunctionDefinitions::SetWaterColor(color->r, color->g, color->b, color->a));
}

WebAssemblyApi(CWebAssemblyWaterDefs::ResetWaterColor, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.Return(CStaticFunctionDefinitions::ResetWaterColor());
}
