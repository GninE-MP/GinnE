/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyBlipDefs.cpp
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

#include "CBlip.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyBlipDefs.h"

void CWebAssemblyBlipDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_blip", "e*ii*iie");
    SetFunctionType("create_blip_attached_to", "eeii*iie");

    SetFunctionType("get_blip_icon", "ie");
    SetFunctionType("get_blip_size", "ie");
    SetFunctionType("get_blip_color", "be*");
    SetFunctionType("get_blip_ordering", "ie");
    SetFunctionType("get_blip_visible_distance", "ie");

    SetFunctionType("set_blip_icon", "bei");
    SetFunctionType("set_blip_size", "bei");
    SetFunctionType("set_blip_color", "be*");
    SetFunctionType("set_blip_ordering", "bei");
    SetFunctionType("set_blip_visible_distance", "bei");
}

void CWebAssemblyBlipDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_blip", CreateBlip },
        { "create_blip_attached_to", CreateBlipAttachedTo },

        { "get_blip_icon", GetBlipIcon },
        { "get_blip_size", GetBlipSize },
        { "get_blip_color", GetBlipColor },
        { "get_blip_ordering", GetBlipOrdering },
        { "get_blip_visible_distance", GetBlipVisibleDistance },

        { "set_blip_icon", SetBlipIcon },
        { "set_blip_size", SetBlipSize },
        { "set_blip_color", SetBlipColor },
        { "set_blip_ordering", SetBlipOrdering },
        { "set_blip_visible_distance", SetBlipVisibleDistance }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyBlipDefs::CreateBlip, env, args, results)
{
    CVector   position;
    int32_t   icon;
    int32_t   size;
    struct C
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }*        color;
    int32_t   ordering;
    int32_t   visibleDistance;
    CElement* visibleTo;

    C defaultColor = { 0, 0, 255, 255 };

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(position);
    argStream.ReadInt32(icon, 0);
    argStream.ReadInt32(size, 2);
    argStream.ReadPointer(color, &defaultColor);
    argStream.ReadInt32(ordering, 0);
    argStream.ReadInt32(visibleDistance, 16383);
    argStream.ReadUserData(visibleTo, CStaticFunctionDefinitions::GetRootElement());

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    icon = Clamp(0, icon, 255);
    size = Clamp(0, size, 25);
    ordering = Clamp(-32768, ordering, 32767);
    visibleDistance = Clamp(0, visibleDistance, 65535);

    CBlip* blip = CStaticFunctionDefinitions::CreateBlip(resource, position, (unsigned char)icon, (unsigned char)size, SColorRGBA(color->r, color->g, color->b, color->a), (short)ordering, (unsigned short)visibleDistance, visibleTo);

    if (!blip)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(blip);
    }

    return argStream.Return((CElement*)blip);
}

WebAssemblyApi(CWebAssemblyBlipDefs::CreateBlipAttachedTo, env, args, results)
{
    CElement* element;
    int32_t   icon;
    int32_t   size;
    struct C
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }*        color;
    int32_t   ordering;
    int32_t   visibleDistance;
    CElement* visibleTo;

    C defaultColor = { 0, 0, 255, 255 };

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadInt32(icon, 0);
    argStream.ReadInt32(size, 2);
    argStream.ReadPointer(color, &defaultColor);
    argStream.ReadInt32(ordering, 0);
    argStream.ReadInt32(visibleDistance, 16383);
    argStream.ReadUserData(visibleTo, CStaticFunctionDefinitions::GetRootElement());

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    icon = Clamp(0, icon, 255);
    size = Clamp(0, size, 25);
    ordering = Clamp(-32768, ordering, 32767);
    visibleDistance = Clamp(0, visibleDistance, 65535);

    CBlip* blip = CStaticFunctionDefinitions::CreateBlipAttachedTo(resource, element, (unsigned char)icon, (unsigned char)size, SColorRGBA(color->r, color->g, color->b, color->a), (short)ordering, (unsigned short)visibleDistance, visibleTo);

    if (!blip)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(blip);
    }

    return argStream.Return((CElement*)blip);
}

WebAssemblyApi(CWebAssemblyBlipDefs::GetBlipIcon, env, args, results)
{
    CBlip* blip;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);

    if (!blip)
    {
        return argStream.Return(0);
    }

    unsigned char icon;

    if (!CStaticFunctionDefinitions::GetBlipIcon(blip, icon))
    {
        return argStream.Return(0);
    }

    return argStream.Return((int32_t)icon);
}

WebAssemblyApi(CWebAssemblyBlipDefs::GetBlipSize, env, args, results)
{
    CBlip* blip;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);

    if (!blip)
    {
        return argStream.Return(0);
    }

    unsigned char size;

    if (!CStaticFunctionDefinitions::GetBlipSize(blip, size))
    {
        return argStream.Return(0);
    }

    return argStream.Return((int32_t)size);
}

WebAssemblyApi(CWebAssemblyBlipDefs::GetBlipColor, env, args, results)
{
    CBlip* blip;
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);
    argStream.ReadPointer(color);

    if (!blip || !color)
    {
        return argStream.Return(false);
    }

    SColor bColor;

    if (!CStaticFunctionDefinitions::GetBlipColor(blip, bColor))
    {
        return argStream.Return(false);
    }

    color->r = bColor.R;
    color->g = bColor.G;
    color->b = bColor.B;
    color->a = bColor.A;

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyBlipDefs::GetBlipOrdering, env, args, results)
{
    CBlip* blip;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);

    if (!blip)
    {
        return argStream.Return(0);
    }

    short ordering;

    if (!CStaticFunctionDefinitions::GetBlipOrdering(blip, ordering))
    {
        return argStream.Return(0);
    }

    return argStream.Return((int32_t)ordering);
}

WebAssemblyApi(CWebAssemblyBlipDefs::GetBlipVisibleDistance, env, args, results)
{
    CBlip* blip;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);

    if (!blip)
    {
        return argStream.Return(0);
    }

    unsigned short visibleDistance;

    if (!CStaticFunctionDefinitions::GetBlipVisibleDistance(blip, visibleDistance))
    {
        return argStream.Return(0);
    }

    return argStream.Return((int32_t)visibleDistance);
}

WebAssemblyApi(CWebAssemblyBlipDefs::SetBlipIcon, env, args, results)
{
    CElement* blip;
    int32_t   icon;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);
    argStream.ReadInt32(icon);

    if (!blip)
    {
        return argStream.Return(false);
    }

    icon = Clamp(0, icon, 255);

    return argStream.Return(CStaticFunctionDefinitions::SetBlipIcon(blip, (unsigned char)icon));
}

WebAssemblyApi(CWebAssemblyBlipDefs::SetBlipSize, env, args, results)
{
    CElement* blip;
    int32_t   size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);
    argStream.ReadInt32(size);

    if (!blip)
    {
        return argStream.Return(false);
    }

    size = Clamp(0, size, 25);

    return argStream.Return(CStaticFunctionDefinitions::SetBlipSize(blip, (unsigned char)size));
}

WebAssemblyApi(CWebAssemblyBlipDefs::SetBlipColor, env, args, results)
{
    CElement* blip;
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);
    argStream.ReadPointer(color);

    if (!blip || !color)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetBlipColor(blip, SColorRGBA(color->r, color->g, color->b, color->a)));
}

WebAssemblyApi(CWebAssemblyBlipDefs::SetBlipOrdering, env, args, results)
{
    CElement* blip;
    int32_t   ordering;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);
    argStream.ReadInt32(ordering);

    if (!blip)
    {
        return argStream.Return(false);
    }

    ordering = Clamp(-32768, ordering, 32767);

    return argStream.Return(CStaticFunctionDefinitions::SetBlipOrdering(blip, (short)ordering));
}

WebAssemblyApi(CWebAssemblyBlipDefs::SetBlipVisibleDistance, env, args, results)
{
    CElement* blip;
    int32_t   visibleDistance;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(blip);
    argStream.ReadInt32(visibleDistance);

    if (!blip)
    {
        return argStream.Return(false);
    }

    visibleDistance = Clamp(0, visibleDistance, 65535);

    return argStream.Return(CStaticFunctionDefinitions::SetBlipVisibleDistance(blip, (unsigned short)visibleDistance));
}
