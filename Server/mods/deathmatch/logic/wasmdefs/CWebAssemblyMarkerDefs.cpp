/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyMarkerDefs.cpp
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

#include "CMarker.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyMarkerDefs.h"

void CWebAssemblyMarkerDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_marker", "u*sf*u");

    SetFunctionType("get_marker_count", "i");
    SetFunctionType("get_marker_type", "xusx");
    SetFunctionType("get_marker_size", "fu");
    SetFunctionType("get_marker_color", "bu*");
    SetFunctionType("get_marker_target", "bu*");
    SetFunctionType("get_marker_icon", "xusx");

    SetFunctionType("set_marker_type", "bus");
    SetFunctionType("set_marker_size", "buf");
    SetFunctionType("set_marker_color", "bu*");
    SetFunctionType("set_marker_target", "bu*");
    SetFunctionType("set_marker_icon", "bus");
}

void CWebAssemblyMarkerDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_marker", CreateMarker },

        { "get_marker_count", GetMarkerCount },
        { "get_marker_type", GetMarkerType },
        { "get_marker_size", GetMarkerSize },
        { "get_marker_color", GetMarkerColor },
        { "get_marker_target", GetMarkerTarget },
        { "get_marker_icon", GetMarkerIcon },

        { "set_marker_type", SetMarkerType },
        { "set_marker_size", SetMarkerSize },
        { "set_marker_color", SetMarkerColor },
        { "set_marker_target", SetMarkerTarget },
        { "set_marker_icon", SetMarkerIcon }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyMarkerDefs::CreateMarker, env, args, results)
{
    CVector   position;
    SString   type;
    float32_t size;
    struct C
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }*        color;
    CElement* visibleTo;

    C defaultColor = { 255, 0, 0, 255 };

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(position);
    argStream.ReadString(type, "default");
    argStream.ReadFloat32(size, 4.0f);
    argStream.ReadPointer(color, &defaultColor);
    argStream.ReadUserData(visibleTo, CStaticFunctionDefinitions::GetRootElement());

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CMarker* marker = CStaticFunctionDefinitions::CreateMarker(resource, position, type.c_str(), size, SColorRGBA(color->r, color->g, color->b, color->a), visibleTo);

    if (!marker)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(marker);
    }

    return argStream.Return((CElement*)marker);
}

WebAssemblyApi(CWebAssemblyMarkerDefs::GetMarkerCount, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    uint32_t count = 0;

    if (!CStaticFunctionDefinitions::GetMarkerCount(count))
    {
        return argStream.Return((int32_t)0);
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyMarkerDefs::GetMarkerType, env, args, results)
{
    CMarker*                         marker;
    CWebAssemblyMemoryPointerAddress outType;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadPointerAddress(outType);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!marker || outType == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    char markerType[64];
    memset(markerType, 0, sizeof(markerType));

    if (!CStaticFunctionDefinitions::GetMarkerType(marker, markerType))
    {
        return argStream.Return(0);
    }

    SString type = markerType;

    uint32_t size = std::min(maxSize, (uint32_t)type.size());

    argStream.WritePointer(outType, type.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyMarkerDefs::GetMarkerSize, env, args, results)
{
    CMarker* marker;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);

    if (!marker)
    {
        return argStream.Return(0.0f);
    }

    float size;

    if (!CStaticFunctionDefinitions::GetMarkerSize(marker, size))
    {
        return argStream.Return(0.0f);
    }

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyMarkerDefs::GetMarkerColor, env, args, results)
{
    CMarker*                         marker;
    CWebAssemblyMemoryPointerAddress outColor;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadPointerAddress(outColor);

    if (!marker || outColor == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    SColor color;

    if (!CStaticFunctionDefinitions::GetMarkerColor(marker, color))
    {
        return argStream.Return(false);
    }

    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } c = { color.B, color.G, color.R, color.A };

    argStream.WritePointer(outColor, &c);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyMarkerDefs::GetMarkerTarget, env, args, results)
{
    CMarker*                         marker;
    CWebAssemblyMemoryPointerAddress outTarget;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadPointerAddress(outTarget);

    if (!marker || outTarget == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    CVector target;

    if (!CStaticFunctionDefinitions::GetMarkerTarget(marker, target))
    {
        return argStream.Return(false);
    }

    struct
    {
        float32_t x;
        float32_t y;
        float32_t z;
    } t = { target.fX, target.fY, target.fZ };

    argStream.WritePointer(outTarget, &t);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyMarkerDefs::GetMarkerIcon, env, args, results)
{
    CMarker*                         marker;
    CWebAssemblyMemoryPointerAddress outIcon;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadPointerAddress(outIcon);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!marker || outIcon == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    char markerIcon[64];
    memset(markerIcon, 0, sizeof(markerIcon));

    if (!CStaticFunctionDefinitions::GetMarkerIcon(marker, markerIcon))
    {
        return argStream.Return(0);
    }

    SString icon = markerIcon;

    uint32_t size = std::min(maxSize, (uint32_t)icon.size());

    argStream.WritePointer(outIcon, icon.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyMarkerDefs::SetMarkerType, env, args, results)
{
    CElement* marker;
    SString   type;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadString(type);

    if (!marker)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetMarkerType(marker, type.c_str()));
}

WebAssemblyApi(CWebAssemblyMarkerDefs::SetMarkerSize, env, args, results)
{
    CElement* marker;
    float32_t size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadFloat32(size);

    if (!marker)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetMarkerSize(marker, size));
}

WebAssemblyApi(CWebAssemblyMarkerDefs::SetMarkerColor, env, args, results)
{
    CElement* marker;
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadPointer(color);

    if (!marker || !color)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetMarkerColor(marker, SColorRGBA(color->r, color->g, color->b, color->a)));
}

WebAssemblyApi(CWebAssemblyMarkerDefs::SetMarkerTarget, env, args, results)
{
    CElement* marker;
    CVector   target;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadVector3D(target);

    if (!marker)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetMarkerTarget(marker, &target));
}

WebAssemblyApi(CWebAssemblyMarkerDefs::SetMarkerIcon, env, args, results)
{
    CElement* marker;
    SString   icon;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(marker);
    argStream.ReadString(icon);

    if (!marker)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetMarkerIcon(marker, icon.c_str()));
}
