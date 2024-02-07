/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaTrainTrackDefs.h"
#include <lua/CLuaFunctionParser.h>

#ifndef GninE_CLIENT
    #include "CTrainTrackManager.h"
    #include "CGame.h"
#endif

void CLuaTrainTrackDefs::LoadFunctions()
{
    // CLuaCFunctions::AddFunction("getDefaultTrack", ArgumentParser<GetDefaultTrack>);
}

void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    // lua_newclass(luaVM);

    // lua_classfunction(luaVM, "getDefault", "getDefaultTrack");

    // lua_registerclass(luaVM, "TrainTrack", "Element");
}

auto CLuaTrainTrackDefs::GetDefaultTrack(uchar trackID) -> CLuaTrainTrackDefs::TrainTrack
{
    if (trackID > 3)
        throw std::invalid_argument("Bad default track ID (0-3)");

#ifdef GninE_CLIENT
    return trackID;
#else
    return g_pGame->GetTrainTrackManager()->GetTrainTrackByIndex(trackID);
#endif
}
