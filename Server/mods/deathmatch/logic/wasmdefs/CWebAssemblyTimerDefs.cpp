/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyTimerDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWebAssemblyDefs.h"
#include <SharedUtil.Memory.h>
#include "CStaticFunctionDefinitions.h"
#include "CDummy.h"
#include "Utils.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyTimerDefs.h"

#include <pthread.h>

void CWebAssemblyTimerDefs::RegisterFunctionTypes()
{
    SetFunctionType("set_timer", "u*iiu");
    SetFunctionType("kill_timer", "bu");
    SetFunctionType("reset_timer", "bu");
    SetFunctionType("get_timers", "id*i");
    SetFunctionType("is_timer", "bu");
    SetFunctionType("get_timer_details", "bu*");
}

void CWebAssemblyTimerDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "set_timer", SetTimer },
        { "kill_timer", KillTimer },
        { "reset_timer", ResetTimer },
        { "get_timers", GetTimers },
        { "is_timer", IsTimer },
        { "get_timer_details", GetTimerDetails }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyTimerDefs::SetTimer, env, args, results)
{
    CLuaFunctionRef function;
    uint32_t        timeInterval;
    uint32_t        timeToExecute;
    CLuaArguments   arguments;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadLuaFunctionRef(function);
    argStream.ReadUInt32(timeInterval);
    argStream.ReadUInt32(timeToExecute);
    argStream.ReadLuaArguments(arguments);

    if (!function.IsCallable())
    {
        return argStream.ReturnNull();
    }

    CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

    if (!luaMain)
    {
        return argStream.ReturnNull();
    }

    CLuaTimer* timer = luaMain->GetTimerManager()->AddTimer(function, CTickCount((double)timeInterval), timeToExecute, arguments);

    if (!timer)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(timer);
}

WebAssemblyApi(CWebAssemblyTimerDefs::KillTimer, env, args, results)
{
    CLuaTimer* timer;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(timer);

    if (!timer)
    {
        return argStream.Return(false);
    }

    CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

    if (!luaMain)
    {
        return argStream.Return(false);
    }

    luaMain->GetTimerManager()->RemoveTimer(timer);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyTimerDefs::ResetTimer, env, args, results)
{
    CLuaTimer* timer;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(timer);

    if (!timer)
    {
        return argStream.Return(false);
    }

    CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

    if (!luaMain)
    {
        return argStream.Return(false);
    }

    luaMain->GetTimerManager()->ResetTimer(timer);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyTimerDefs::GetTimers, env, args, results)
{
    float64_t                        time;
    CWebAssemblyMemoryPointerAddress outTimers;
    uint32_t                         maxItemCount;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadFloat64(time);
    argStream.ReadPointerAddress(outTimers);
    argStream.ReadUInt32(maxItemCount, 0xffffffff);

    CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

    if (!luaMain)
    {
        return argStream.Return(0);
    }

    CLuaTimerManager* timerManager = luaMain->GetTimerManager();
    CTickCount        currentTime = CTickCount::Now();

    uint32_t count = 0;

    CFastList<CLuaTimer*>::const_iterator iter = timerManager->IterBegin();

    for (; iter != timerManager->IterEnd() && maxItemCount > 0; iter++)
    {
        CLuaTimer* timer = *iter;

        CTickCount timeLeft = (timer->GetStartTime() + timer->GetDelay()) - currentTime;

        if (time == 0 || timeLeft.ToDouble() <= time)
        {
            CWebAssemblyUserData udata = TIMER_TO_WASM_USERDATA(timer);

            argStream.WritePointer(outTimers + (count * sizeof(udata)), &udata);

            count++;
            maxItemCount--;
        }
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyTimerDefs::IsTimer, env, args, results)
{
    CLuaTimer* timer;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(timer);

    return argStream.Return(timer ? true : false);
}

WebAssemblyApi(CWebAssemblyTimerDefs::GetTimerDetails, env, args, results)
{
    CLuaTimer*                       timer;
    CWebAssemblyMemoryPointerAddress outTimerDetails;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(timer);
    argStream.ReadPointerAddress(outTimerDetails);

    if (!timer || outTimerDetails == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    struct
    {
        uint32_t timeLeft;
        uint32_t repeats;
        uint32_t delay;
    } details;
    memset(&details, 0, sizeof(details));

    details.timeLeft = timer->GetTimeLeft().ToInt();
    details.repeats = timer->GetRepeats();
    details.delay = timer->GetDelay().ToInt();

    argStream.WritePointer(outTimerDetails, &details);

    return argStream.Return(true);
}
