/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyPickupDefs.cpp
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

#include "CPickup.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyPickupDefs.h"

void CWebAssemblyPickupDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_pickup", "u*iiii");

    SetFunctionType("get_pickup_type", "iu");
    SetFunctionType("get_pickup_weapon", "iu");
    SetFunctionType("get_pickup_amount", "iu");
    SetFunctionType("get_pickup_ammo", "iu");
    SetFunctionType("get_pickup_respawn_interval", "iu");
    SetFunctionType("is_pickup_spawned", "bu");

    SetFunctionType("set_pickup_type", "buiii");
    SetFunctionType("set_pickup_respawn_interval", "bui");
    SetFunctionType("use_pickup", "buu");
}

void CWebAssemblyPickupDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_pickup", CreatePickup },

        { "get_pickup_type", GetPickupType },
        { "get_pickup_weapon", GetPickupWeapon },
        { "get_pickup_amount", GetPickupAmount },
        { "get_pickup_ammo", GetPickupAmmo },
        { "get_pickup_respawn_interval", GetPickupRespawnInterval },
        { "is_pickup_spawned", IsPickupSpawned },

        { "set_pickup_type", SetPickupType },
        { "set_pickup_respawn_interval", SetPickupRespawnInterval },
        { "use_pickup", UsePickup }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyPickupDefs::CreatePickup, env, args, results)
{
    CVector  position;
    int32_t  type;
    int32_t  value;
    uint32_t respawnTime;
    int32_t  ammo;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(position);
    argStream.ReadInt32(type);
    argStream.ReadInt32(value);
    argStream.ReadUInt32(respawnTime, 30000);
    argStream.ReadInt32(ammo, 50);

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CPickup* pickup = CStaticFunctionDefinitions::CreatePickup(resource, position, (unsigned char)type, (double)value, (unsigned long)respawnTime, (double)ammo);

    if (!pickup)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(pickup);
    }

    return argStream.Return((CElement*)pickup);
}

WebAssemblyApi(CWebAssemblyPickupDefs::GetPickupType, env, args, results)
{
    CPickup* pickup;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);

    if (!pickup)
    {
        return argStream.Return(-1);
    }

    unsigned char type;

    if (!CStaticFunctionDefinitions::GetPickupType(pickup, type))
    {
        return argStream.Return(-1);
    }

    return argStream.Return((int32_t)type);
}

WebAssemblyApi(CWebAssemblyPickupDefs::GetPickupWeapon, env, args, results)
{
    CPickup* pickup;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);

    if (!pickup)
    {
        return argStream.Return(0);
    }

    unsigned char weapon;

    if (!CStaticFunctionDefinitions::GetPickupWeapon(pickup, weapon))
    {
        return argStream.Return(0);
    }

    return argStream.Return((int32_t)weapon);
}

WebAssemblyApi(CWebAssemblyPickupDefs::GetPickupAmount, env, args, results)
{
    CPickup* pickup;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);

    if (!pickup)
    {
        return argStream.Return(0);
    }

    float amount;

    if (!CStaticFunctionDefinitions::GetPickupAmount(pickup, amount))
    {
        return argStream.Return(0);
    }

    return argStream.Return(amount);
}

WebAssemblyApi(CWebAssemblyPickupDefs::GetPickupAmmo, env, args, results)
{
    CPickup* pickup;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);

    if (!pickup)
    {
        return argStream.Return(0);
    }

    unsigned short ammo;

    if (!CStaticFunctionDefinitions::GetPickupAmmo(pickup, ammo))
    {
        return argStream.Return(0);
    }

    return argStream.Return((int32_t)ammo);
}

WebAssemblyApi(CWebAssemblyPickupDefs::GetPickupRespawnInterval, env, args, results)
{
    CPickup* pickup;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);

    if (!pickup)
    {
        return argStream.Return(0);
    }

    unsigned long interval;

    if (!CStaticFunctionDefinitions::GetPickupRespawnInterval(pickup, interval))
    {
        return argStream.Return(0);
    }

    return argStream.Return((int32_t)interval);
}

WebAssemblyApi(CWebAssemblyPickupDefs::IsPickupSpawned, env, args, results)
{
    CPickup* pickup;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);

    if (!pickup)
    {
        return argStream.Return(false);
    }

    bool spawned;

    if (!CStaticFunctionDefinitions::IsPickupSpawned(pickup, spawned))
    {
        return argStream.Return(false);
    }

    return argStream.Return(spawned);
}

WebAssemblyApi(CWebAssemblyPickupDefs::SetPickupType, env, args, results)
{
    CElement* pickup;
    int32_t   type;
    int32_t   value;
    int32_t   ammo;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);
    argStream.ReadInt32(type);
    argStream.ReadInt32(value);
    argStream.ReadInt32(ammo);

    if (!pickup)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPickupType(pickup, (unsigned char)type, (double)value, (double)ammo));
}

WebAssemblyApi(CWebAssemblyPickupDefs::SetPickupRespawnInterval, env, args, results)
{
    CElement* pickup;
    uint32_t  interval;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);
    argStream.ReadUInt32(interval);

    if (!pickup)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPickupRespawnInterval(pickup, (unsigned long)interval));
}

WebAssemblyApi(CWebAssemblyPickupDefs::UsePickup, env, args, results)
{
    CElement* pickup;
    CPlayer*  player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(pickup);
    argStream.ReadUserData(player);

    if (!pickup || !player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::UsePickup(pickup, player));
}
