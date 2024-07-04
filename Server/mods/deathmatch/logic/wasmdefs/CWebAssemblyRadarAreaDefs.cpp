/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyRadarAreaDefs.cpp
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

#include "CRadarArea.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyRadarAreaDefs.h"

void CWebAssemblyRadarAreaDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_radar_area", "u***u");

    SetFunctionType("get_radar_area_size", "bu*");
    SetFunctionType("get_radar_area_color", "bu*");
    SetFunctionType("is_radar_area_flashing", "bu");
    SetFunctionType("is_inside_radar_area", "bu*");

    SetFunctionType("set_radar_area_size", "bu*");
    SetFunctionType("set_radar_area_color", "bu*");
    SetFunctionType("set_radar_area_flashing", "bub");
}

void CWebAssemblyRadarAreaDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_radar_area", CreateRadarArea },

        { "get_radar_area_size", GetRadarAreaSize },
        { "get_radar_area_color", GetRadarAreaColor },
        { "is_radar_area_flashing", IsRadarAreaFlashing },
        { "is_inside_radar_area", IsInsideRadarArea },

        { "set_radar_area_size", SetRadarAreaSize },
        { "set_radar_area_color", SetRadarAreaColor },
        { "set_radar_area_flashing", SetRadarAreaFlashing }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyRadarAreaDefs::CreateRadarArea, env, args, results)
{
    CVector2D position;
    CVector2D size;
    struct C
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;
    CElement* visibleTo;

    C defaultColor;
    defaultColor.r = 255;
    defaultColor.g = 0;
    defaultColor.b = 0;
    defaultColor.a = 255;

    CWebAssemblyArgReader argStream(env, args, results);

    CElement* rootElement = CStaticFunctionDefinitions::GetRootElement();

    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadPointer(color, &defaultColor);
    argStream.ReadUserData(visibleTo, rootElement);

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    SColorRGBA c(color->r, color->g, color->b, color->a);

    CRadarArea* area = CStaticFunctionDefinitions::CreateRadarArea(resource, position, size, c, visibleTo);

    if (!area)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(area);
    }

    return argStream.Return((CElement*)area);
}

WebAssemblyApi(CWebAssemblyRadarAreaDefs::GetRadarAreaSize, env, args, results)
{
    CRadarArea*                      area;
    CWebAssemblyMemoryPointerAddress outSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(area);
    argStream.ReadPointerAddress(outSize);

    if (!area || outSize == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    CVector2D size;

    if (!CStaticFunctionDefinitions::GetRadarAreaSize(area, size))
    {
        return argStream.Return(false);
    }

    struct
    {
        float32_t x;
        float32_t y;
    } oSize;

    oSize.x = size.fX;
    oSize.y = size.fY;

    argStream.WritePointer(outSize, &oSize);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyRadarAreaDefs::GetRadarAreaColor, env, args, results)
{
    CRadarArea*                      area;
    CWebAssemblyMemoryPointerAddress outColor;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(area);
    argStream.ReadPointerAddress(outColor);

    if (!area || outColor == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    SColor color;

    if (!CStaticFunctionDefinitions::GetRadarAreaColor(area, color))
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

    c.r = color.R;
    c.g = color.G;
    c.b = color.B;
    c.a = color.A;

    argStream.WritePointer(outColor, &c);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyRadarAreaDefs::IsRadarAreaFlashing, env, args, results)
{
    CRadarArea* area;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(area);

    if (!area)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::IsRadarAreaFlashing(area));
}

WebAssemblyApi(CWebAssemblyRadarAreaDefs::IsInsideRadarArea, env, args, results)
{
    CRadarArea* area;
    CVector2D   position;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(area);
    argStream.ReadVector2D(position);

    if (!area)
    {
        return argStream.Return(false);
    }

    bool inside = false;

    if (!CStaticFunctionDefinitions::IsInsideRadarArea(area, position, inside))
    {
        return argStream.Return(false);
    }

    return argStream.Return(inside);
}

WebAssemblyApi(CWebAssemblyRadarAreaDefs::SetRadarAreaSize, env, args, results)
{
    CElement* area;
    CVector2D size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(area);
    argStream.ReadVector2D(size);

    if (!area)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetRadarAreaSize(area, size));
}

WebAssemblyApi(CWebAssemblyRadarAreaDefs::SetRadarAreaColor, env, args, results)
{
    CElement* area;
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(area);
    argStream.ReadPointer(color);

    if (!area)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetRadarAreaColor(area, SColorRGBA(color->r, color->g, color->b, color->a)));
}

WebAssemblyApi(CWebAssemblyRadarAreaDefs::SetRadarAreaFlashing, env, args, results)
{
    CElement* area;
    bool      flash;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(area);
    argStream.ReadBoolean(flash);

    if (!area)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetRadarAreaFlashing(area, flash));
}
