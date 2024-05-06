/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyElementDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyElementDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    // Create/destroy
    DEFINE_WASM_FUNCTION(CreateElement, env, args, results);
    DEFINE_WASM_FUNCTION(DestroyElement, env, args, results);
    DEFINE_WASM_FUNCTION(CloneElement, env, args, results);

    // Is/get
    DEFINE_WASM_FUNCTION(IsElement, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementWithinColShape, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementWithinMarker, env, args, results);

    DEFINE_WASM_FUNCTION(GetElementChildren, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementChild, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementChildrenCount, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementID, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementByID, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementByIndex, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementParent, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementMatrix, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementPosition, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementRotation, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementVelocity, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementTurnVelocity, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementType, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementsByType, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementInterior, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementsWithinColShape, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementsWithinRange, env, args, results);

    DEFINE_WASM_FUNCTION(GetElementDimension, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementZoneName, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementColShape, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementAlpha, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementDoubleSided, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementHealth, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementModel, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementInWater, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementSyncer, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementCollisionsEnabled, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementFrozen, env, args, results);
    DEFINE_WASM_FUNCTION(GetLowLODElement, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementLowLOD, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementCallPropagationEnabled, env, args, results);

    // Visible to
    DEFINE_WASM_FUNCTION(ClearElementVisibleTo, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementVisibleTo, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementVisibleTo, env, args, results);

    // Element data
    DEFINE_WASM_FUNCTION(GetElementData, env, args, results);
    DEFINE_WASM_FUNCTION(GetAllElementData, env, args, results);
    DEFINE_WASM_FUNCTION(HasElementData, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementData, env, args, results);
    DEFINE_WASM_FUNCTION(RemoveElementData, env, args, results);
    DEFINE_WASM_FUNCTION(AddElementDataSubscriber, env, args, results);
    DEFINE_WASM_FUNCTION(RemoveElementDataSubscriber, env, args, results);
    DEFINE_WASM_FUNCTION(HasElementDataSubscriber, env, args, results);

    // Attachement
    DEFINE_WASM_FUNCTION(AttachElements, env, args, results);
    DEFINE_WASM_FUNCTION(DetachElements, env, args, results);
    DEFINE_WASM_FUNCTION(IsElementAttached, env, args, results);
    DEFINE_WASM_FUNCTION(GetAttachedElements, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementAttachedTo, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementAttachedOffsets, env, args, results);
    DEFINE_WASM_FUNCTION(GetElementAttachedOffsets, env, args, results);

    // Set
    DEFINE_WASM_FUNCTION(SetElementID, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementParent, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementMatrix, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementPosition, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementRotation, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementVelocity, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementTurnVelocity, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementInterior, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementDimension, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementAlpha, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementDoubleSided, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementHealth, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementModel, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementSyncer, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementCollisionsEnabled, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementFrozen, env, args, results);
    DEFINE_WASM_FUNCTION(SetLowLODElement, env, args, results);
    DEFINE_WASM_FUNCTION(SetElementCallPropagationEnabled, env, args, results);
};
