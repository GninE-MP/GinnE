/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyColShapeDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyColShapeDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreateColCircle, env, args, results);
    DEFINE_WASM_FUNCTION(CreateColCuboid, env, args, results);
    DEFINE_WASM_FUNCTION(CreateColSphere, env, args, results);
    DEFINE_WASM_FUNCTION(CreateColRectangle, env, args, results);
    DEFINE_WASM_FUNCTION(CreateColPolygon, env, args, results);
    DEFINE_WASM_FUNCTION(CreateColTube, env, args, results);

    DEFINE_WASM_FUNCTION(GetColShapeRadius, env, args, results);
    DEFINE_WASM_FUNCTION(SetColShapeRadius, env, args, results);
    DEFINE_WASM_FUNCTION(GetColShapeSize, env, args, results);
    DEFINE_WASM_FUNCTION(SetColShapeSize, env, args, results);
    DEFINE_WASM_FUNCTION(GetColPolygonPoints, env, args, results);
    DEFINE_WASM_FUNCTION(GetColPolygonPointPosition, env, args, results);
    DEFINE_WASM_FUNCTION(SetColPolygonPointPosition, env, args, results);
    DEFINE_WASM_FUNCTION(AddColPolygonPoint, env, args, results);
    DEFINE_WASM_FUNCTION(RemoveColPolygonPoint, env, args, results);

    DEFINE_WASM_FUNCTION(IsInsideColShape, env, args, results);
    DEFINE_WASM_FUNCTION(GetColShapeType, env, args, results);

    DEFINE_WASM_FUNCTION(GetColPolygonHeight, env, args, results);
    DEFINE_WASM_FUNCTION(SetColPolygonHeight, env, args, results);
};
