/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyUtilDefs.cpp
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
#include "CColShape.h"
#include "CMarker.h"
#include "CSpatialDatabase.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyElementDefs.h"

void CWebAssemblyElementDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_element", "ess");
    SetFunctionType("destroy_element", "be");
    SetFunctionType("clone_element", "ee*b");

    SetFunctionType("is_element", "be");
    SetFunctionType("is_element_within_col_shape", "bee");
    SetFunctionType("is_element_within_marker", "bee");
    SetFunctionType("is_element_in_water", "be");
    SetFunctionType("is_element_frozen", "be");
    SetFunctionType("is_element_low_lod", "be");
    SetFunctionType("set_element_call_propagation_enabled", "beb");
    SetFunctionType("is_element_call_propagation_enabled", "be");

    SetFunctionType("get_element_by_id", "esi");
    SetFunctionType("get_element_by_index", "esi");
    SetFunctionType("get_element_children", "ies*i");
    SetFunctionType("get_element_child", "eei");
    SetFunctionType("get_element_children_count", "ie");
    SetFunctionType("get_element_id", "xesx");
    SetFunctionType("get_element_parent", "ee");
    SetFunctionType("get_element_matrix", "be*b");
    SetFunctionType("get_element_position", "be*");
    SetFunctionType("get_element_rotation", "be*s");
    SetFunctionType("get_element_velocity", "be*");
    SetFunctionType("get_element_turn_velocity", "be*");
    SetFunctionType("get_elements_by_type", "ise*i");
    SetFunctionType("get_element_type", "xesx");
    SetFunctionType("get_element_interior", "ie");
    SetFunctionType("get_elements_within_col_shape", "ies*i");
    SetFunctionType("get_elements_within_range", "i*f*isii");
    SetFunctionType("get_element_dimension", "ie");
    SetFunctionType("get_element_zone_name", "xesxb");
    SetFunctionType("get_element_col_shape", "ee");
    SetFunctionType("get_element_alpha", "ie");
    SetFunctionType("is_element_double_sided", "be");
    SetFunctionType("get_element_health", "fe");
    SetFunctionType("get_element_model", "ie");
    SetFunctionType("get_element_syncer", "ee");
    SetFunctionType("get_element_collisions_enabled", "be");
    SetFunctionType("get_low_lod_element", "ee");

    SetFunctionType("attach_elements", "bee**");
    SetFunctionType("detach_elements", "bee");
    SetFunctionType("is_element_attached", "be");
    SetFunctionType("get_attached_elements", "ie*i");
    SetFunctionType("get_element_attached_to", "ee");
    SetFunctionType("set_element_attached_offsets", "be**");
    SetFunctionType("get_element_attached_offsets", "be**");

    SetFunctionType("get_element_data", "");
    SetFunctionType("get_all_element_data", "");
    SetFunctionType("has_element_data", "");
    SetFunctionType("set_element_data", "besb*x");
    SetFunctionType("remove_element_data", "");
    SetFunctionType("add_element_data_subscriber", "");
    SetFunctionType("remove_element_data_subscriber", "");
    SetFunctionType("has_element_data_subscriber", "");

    SetFunctionType("set_element_id", "");
    SetFunctionType("set_element_parent", "");
    SetFunctionType("set_element_matrix", "");
    SetFunctionType("set_element_position", "");
    SetFunctionType("set_element_rotation", "");
    SetFunctionType("set_element_velocity", "");
    SetFunctionType("set_element_turn_velocity", "");
    SetFunctionType("set_element_visible_to", "");
    SetFunctionType("clear_element_visible_to", "");
    SetFunctionType("is_element_visible_to", "");
    SetFunctionType("set_element_interior", "");
    SetFunctionType("set_element_dimension", "");
    SetFunctionType("set_element_alpha", "");
    SetFunctionType("set_element_double_sided", "");
    SetFunctionType("set_element_health", "");
    SetFunctionType("set_element_model", "");
    SetFunctionType("set_element_syncer", "");
    SetFunctionType("set_element_collisions_enabled", "");
    SetFunctionType("set_element_frozen", "");
    SetFunctionType("set_low_lod_element", "");
}

void CWebAssemblyElementDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_element", CreateElement },
        { "destroy_element", DestroyElement },
        { "clone_element", CloneElement },

        // Get
        { "is_element", IsElement },
        { "is_element_within_col_shape", IsElementWithinColShape },
        { "is_element_within_marker", IsElementWithinMarker },
        { "is_element_in_water", IsElementInWater },
        { "is_element_frozen", IsElementFrozen },
        { "is_element_low_lod", IsElementLowLOD },
        { "set_element_call_propagation_enabled", SetElementCallPropagationEnabled },
        { "is_element_call_propagation_enabled", IsElementCallPropagationEnabled },

        { "get_element_by_id", GetElementByID },
        { "get_element_by_index", GetElementByIndex },
        { "get_element_children", GetElementChildren },
        { "get_element_child", GetElementChild },
        { "get_element_children_count", GetElementChildrenCount },
        { "get_element_id", GetElementID },
        { "get_element_parent", GetElementParent },
        { "get_element_matrix", GetElementMatrix },
        { "get_element_position", GetElementPosition },
        { "get_element_rotation", GetElementRotation },
        { "get_element_velocity", GetElementVelocity },
        { "get_element_angular_velocity", GetElementTurnVelocity },
        { "get_elements_by_type", GetElementsByType },
        { "get_element_type", GetElementType },
        { "get_element_interior", GetElementInterior },
        { "get_elements_within_col_shape", GetElementsWithinColShape },
        { "get_elements_within_range", GetElementsWithinRange },
        { "get_element_dimension", GetElementDimension },
        { "get_element_zone_name", GetElementZoneName },
        { "get_element_col_shape", GetElementColShape },
        { "get_element_alpha", GetElementAlpha },
        { "is_element_double_sided", IsElementDoubleSided },
        { "get_element_health", GetElementHealth },
        { "get_element_model", GetElementModel },
        { "get_element_syncer", GetElementSyncer },
        { "get_element_collisions_enabled", GetElementCollisionsEnabled },
        { "get_low_lod_element", GetLowLODElement },

        // Attachement
        { "attach_elements", AttachElements },
        { "detach_elements", DetachElements },
        { "is_element_attached", IsElementAttached },
        { "get_attached_elements", GetAttachedElements },
        { "get_element_attached_to", GetElementAttachedTo },
        { "set_element_attached_offsets", SetElementAttachedOffsets },
        { "get_element_attached_offsets", GetElementAttachedOffsets },

        // Element data
        { "get_element_data", GetElementData },
        { "get_all_element_data", GetAllElementData },
        { "has_element_data", HasElementData },
        { "set_element_data", SetElementData },
        { "remove_element_data", RemoveElementData },
        { "add_element_data_subscriber", AddElementDataSubscriber },
        { "remove_element_data_subscriber", RemoveElementDataSubscriber },
        { "has_element_data_subscriber", HasElementDataSubscriber },

        // Set
        { "set_element_id", SetElementID },
        { "set_element_parent", SetElementParent },
        { "set_element_matrix", SetElementMatrix },
        { "set_element_position", SetElementPosition },
        { "set_element_rotation", SetElementRotation },
        { "set_element_velocity", SetElementVelocity },
        { "set_element_angular_velocity", SetElementTurnVelocity },
        { "set_element_visible_to", SetElementVisibleTo },
        { "clear_element_visible_to", ClearElementVisibleTo },
        { "is_element_visible_to", IsElementVisibleTo },
        { "set_element_interior", SetElementInterior },
        { "set_element_dimension", SetElementDimension },
        { "set_element_alpha", SetElementAlpha },
        { "set_element_double_sided", SetElementDoubleSided },
        { "set_element_health", SetElementHealth },
        { "set_element_model", SetElementModel },
        { "set_element_syncer", SetElementSyncer },
        { "set_element_collisions_enabled", SetElementCollisionsEnabled },
        { "set_element_frozen", SetElementFrozen },
        { "set_low_lod_element", SetLowLODElement }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyElementDefs::CreateElement, env, args, results)
{
    SString typeName;
    SString id;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(typeName);
    argStream.ReadString(id);

    if (typeName.empty())
    {
        return argStream.ReturnNull("Invalid typename.");
    }

    CResource* resource = argStream.GetScript()->GetStoreContext()->GetResource();

    CDummy* element = CStaticFunctionDefinitions::CreateElement(resource, typeName.c_str(), id.c_str());

    if (!element || !resource)
    {
        return argStream.ReturnNull("Couldn't create element!");
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(element);
    }
    
    return argStream.Return((CElement*)element);
}

WebAssemblyApi(CWebAssemblyElementDefs::DestroyElement, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::DestroyElement(element));
}

WebAssemblyApi(CWebAssemblyElementDefs::CloneElement, env, args, results)
{
    CElement* element;
    CVector   position;
    bool      cloneChildren;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    argStream.ReadVector3D(position, element->GetPosition());
    argStream.ReadBoolean(cloneChildren);

    CResource* resource = argStream.GetScript()->GetStoreContext()->GetResource();

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CElement* newElement = CStaticFunctionDefinitions::CloneElement(resource, element, position, cloneChildren);

    if (!newElement)
    {
        argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(newElement);
    }

    return argStream.Return(newElement);
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElement, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementWithinColShape, env, args, results)
{
    CElement*  element;
    CColShape* colShape;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadUserData(colShape);

    if (!element || !colShape)
    {
        return argStream.Return(false);
    }

    bool within;

    if (!CStaticFunctionDefinitions::IsElementWithinColShape(element, colShape, within))
    {
        return argStream.Return(false);
    }

    return argStream.Return(within);
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementWithinMarker, env, args, results)
{
    CElement* element;
    CMarker*  marker;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadUserData(marker);

    if (!element || !marker)
    {
        return argStream.Return(false);
    }

    bool within;

    if (!CStaticFunctionDefinitions::IsElementWithinMarker(element, marker, within))
    {
        return argStream.Return(false);
    }

    return argStream.Return(within);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementChildren, env, args, results)
{
    CElement*                        element;
    SString                          type;
    CWebAssemblyMemoryPointerAddress childrenPtr;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadString(type);
    argStream.ReadPointerAddress(childrenPtr);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!element || childrenPtr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    CChildListType::const_iterator iter = element->IterBegin();

    uint32_t childrenCount = 0;

    if (type.empty())
    {
        for (uint32_t i = 0; iter != element->IterEnd() && childrenCount < maxSize; iter++, childrenCount++, i++)
        {
            CWebAssemblyUserData udata = ELEMENT_TO_WASM_USERDATA(*iter);

            argStream.WritePointer(childrenPtr + (i * sizeof(CWebAssemblyUserData)), &udata);
        }
    }
    else
    {
        uint32_t typeHash = CElement::GetTypeHashFromString(type);

        for (uint32_t i = 0; iter != element->IterEnd() && childrenCount < maxSize; iter++, i++)
        {
            if ((*iter)->GetTypeHash() != typeHash)
            {
                continue;
            }

            CWebAssemblyUserData udata = ELEMENT_TO_WASM_USERDATA(*iter);

            argStream.WritePointer(childrenPtr + (i * sizeof(CWebAssemblyUserData)), &udata);

            childrenCount++;
        }
    }

    return argStream.Return(childrenCount);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementChild, env, args, results)
{
    CElement* element;
    uint32_t  index;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadUInt32(index);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    CElement* child = CStaticFunctionDefinitions::GetElementChild(element, index);

    if (!child)
    {
        return argStream.Return(false);
    }

    return argStream.Return(child);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementChildrenCount, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(0);
    }

    uint32_t count;

    if (!CStaticFunctionDefinitions::GetElementChildrenCount(element, count))
    {
        return argStream.Return(0);
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementID, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress ptr;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadPointerAddress(ptr);
    argStream.ReadUInt32(maxSize);

    if (!element || ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.ReturnNull();
    }

    SString type = element->GetName();

    size_t size = std::min(type.length(), maxSize);

    argStream.WritePointer(ptr, type.c_str(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementByID, env, args, results)
{
    SString  id;
    uint32_t index;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(id);
    argStream.ReadUInt32(index);

    CElement* element = CStaticFunctionDefinitions::GetElementByID(id.c_str(), index);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(element);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementByIndex, env, args, results)
{
    SString  type;
    uint32_t index;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(type);
    argStream.ReadUInt32(index);

    CElement* element = CStaticFunctionDefinitions::GetElementByIndex(type.c_str(), index);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(element);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementParent, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    CElement* parent = CStaticFunctionDefinitions::GetElementParent(element);

    if (!parent)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(parent);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementMatrix, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress outMatrix;
    bool                             badSyntax;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadPointerAddress(outMatrix);
    argStream.ReadBoolean(badSyntax, true);

    if (!element || outMatrix == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    CMatrix matrix;

    if (!CStaticFunctionDefinitions::GetElementMatrix(element, matrix))
    {
        return argStream.Return(false);
    }

    float fData = badSyntax ? 1.0f : 0.0f;

    struct
    {
        float32_t rx;
        float32_t ry;
        float32_t rz;

        float32_t fx;
        float32_t fy;
        float32_t fz;

        float32_t ux;
        float32_t uy;
        float32_t uz;

        float32_t px;
        float32_t py;
        float32_t pz;
    } mData;
    memset(&mData, 0, sizeof(mData));

    mData.rx = matrix.vRight.fX;
    mData.ry = matrix.vRight.fY;
    mData.rz = matrix.vRight.fZ;

    mData.fx = matrix.vFront.fX;
    mData.fy = matrix.vFront.fY;
    mData.fz = matrix.vFront.fZ;

    mData.ux = matrix.vUp.fX;
    mData.uy = matrix.vUp.fY;
    mData.uz = matrix.vUp.fZ;

    mData.px = matrix.vPos.fX;
    mData.py = matrix.vPos.fY;
    mData.pz = matrix.vPos.fZ;

    argStream.WritePointer(outMatrix, &mData);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementPosition, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress ptr;
    
    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadPointerAddress(ptr);
    
    if (!element || ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    CVector position;

    if (!CStaticFunctionDefinitions::GetElementPosition(element, position))
    {
        return argStream.Return(false);
    }

    struct
    {
        float x;
        float y;
        float z;
    } pos;
    memset(&pos, 0, sizeof(pos));

    pos.x = position.fX;
    pos.y = position.fY;
    pos.z = position.fZ;

    argStream.WritePointer(ptr, &pos);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementRotation, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress ptr;
    SString                          rotationOrderString;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadPointerAddress(ptr);
    argStream.ReadString(rotationOrderString);

    if (!element || ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    eEulerRotationOrder rotationOrder = EULER_DEFAULT;
    
    StringToEnum(rotationOrderString, rotationOrder);

    CVector rotation;

    if (!CStaticFunctionDefinitions::GetElementRotation(element, rotation, rotationOrder))
    {
        return argStream.Return(false);
    }

    struct
    {
        float x;
        float y;
        float z;
    } pos;
    memset(&pos, 0, sizeof(pos));

    pos.x = rotation.fX;
    pos.y = rotation.fY;
    pos.z = rotation.fZ;

    argStream.WritePointer(ptr, &pos);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementVelocity, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    CVector velocity;

    if (!CStaticFunctionDefinitions::GetElementVelocity(element, velocity))
    {
        return argStream.Return(false);
    }

    struct
    {
        float x;
        float y;
        float z;
    } out;
    memset(&out, 0, sizeof(out));

    out.x = velocity.fX;
    out.y = velocity.fY;
    out.z = velocity.fZ;

    argStream.WritePointer(ptr, &out);
    
    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementTurnVelocity, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress ptr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadPointerAddress(ptr);

    if (!element)
    {
        return argStream.Return(false);
    }

    CVector turnVelocity;

    if (!CStaticFunctionDefinitions::GetElementTurnVelocity(element, turnVelocity))
    {
        return argStream.Return(false);
    }

    struct
    {
        float x;
        float y;
        float z;
    } outVelocity;

    argStream.WritePointer(ptr, &outVelocity);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementType, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress ptr;
    uint32_t                         maxBytes;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(0);
    }

    argStream.ReadPointerAddress(ptr);
    argStream.ReadUInt32(maxBytes);

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    SString type = element->GetTypeName();

    if (type.empty())
    {
        return argStream.Return(0);
    }

    size_t size = type.length();

    argStream.WritePointer(ptr, type.data(), type.length());

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementsByType, env, args, results)
{
    SString                          elementType;
    CElement*                        startAt;
    CWebAssemblyMemoryPointerAddress ptr;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(elementType);
    argStream.ReadUserData(startAt);
    argStream.ReadPointerAddress(ptr);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    CElement* root = CStaticFunctionDefinitions::GetRootElement();

    if (!startAt)
    {
        startAt = root;
    }

    uint32_t elementCount = 0;
    uint32_t typeHash = CElement::GetTypeHashFromString(elementType);

    if (startAt == root)
    {
        CElementListSnapshot elements;

        startAt->GetEntitiesFromRoot(typeHash, elements);

        size_t count = elements.size();

        for (size_t i = 0; i < count && i < maxSize; i++)
        {
            CWebAssemblyUserData udata = ELEMENT_TO_WASM_USERDATA(elements[i]);

            argStream.WritePointer(ptr + (i * sizeof(udata)), &udata);
        }

        elementCount = std::min(count, maxSize);
    }
    else
    {
        CElementListSnapshot elements;

        startAt->FindAllChildrenByTypeIndex(typeHash, elements);

        size_t count = elements.size();

        for (size_t i = 0; i < count && i < maxSize; i++)
        {
            CWebAssemblyUserData udata = ELEMENT_TO_WASM_USERDATA(elements[i]);

            argStream.WritePointer(ptr + (i * sizeof(udata)), &udata);
        }

        elementCount = std::min(count, maxSize);
    }

    return argStream.Return(elementCount);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementInterior, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(-1);
    }

    uint8_t interior;

    if (!CStaticFunctionDefinitions::GetElementInterior(element, interior))
    {
        return argStream.Return(-1);
    }

    return argStream.Return((int32_t)interior);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementsWithinColShape, env, args, results)
{
    CColShape*                       colShape;
    SString                          elementType;
    CWebAssemblyMemoryPointerAddress listPtr;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(colShape);
    argStream.ReadString(elementType);
    argStream.ReadPointerAddress(listPtr);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!colShape || listPtr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    uint32_t elementCount = 0;

    std::list<CElement*>::iterator iter = colShape->CollidersBegin();

    for (; iter != colShape->CollidersEnd() && elementCount < maxSize; iter++)
    {
        CElement* element = *iter;

        if (elementType.empty() || elementType != element->GetTypeName())
        {
            continue;
        }

        if (element->IsBeingDeleted())
        {
            continue;
        }

        CWebAssemblyUserData udata = ELEMENT_TO_WASM_USERDATA(element);

        argStream.WritePointer(listPtr + (elementCount * sizeof(CWebAssemblyUserData)), &udata);

        elementCount++;
    }

    return argStream.Return(elementCount);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementsWithinRange, env, args, results)
{
    CVector                          pos;
    float32_t                        radius;
    CWebAssemblyMemoryPointerAddress listPtr;
    uint32_t                         maxSize;
    SString                          elementType;
    int32_t                          interior;
    int32_t                          dimension;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(pos);
    argStream.ReadFloat32(radius);
    argStream.ReadPointerAddress(listPtr);
    argStream.ReadUInt32(maxSize, 0xffffffff);
    argStream.ReadString(elementType);
    argStream.ReadInt32(interior, -1);
    argStream.ReadInt32(dimension, -1);

    if (!listPtr)
    {
        return argStream.Return(0);
    }

    uint32_t typeHash = !elementType.empty() ? CElement::GetTypeHashFromString(elementType) : 0;

    CElementResult result;
    GetSpatialDatabase()->SphereQuery(result, CSphere { pos, radius });

    uint32_t elementCount = 0;

    CElementResult::iterator iter = result.begin();

    float radiusSq = radius * radius;

    for (; iter != result.end() && elementCount < maxSize; iter++)
    {
        CElement* element = *iter;

        if (typeHash && typeHash != element->GetTypeHash())
        {
            continue;
        }

        if (interior != -1 && interior != (int32_t)element->GetInterior())
        {
            continue;
        }

        if (dimension != -1 && dimension != (int32_t)element->GetDimension())
        {
            continue;
        }

        if ((element->GetPosition() - pos).LengthSquared() > radiusSq)
        {
            continue;
        }

        CWebAssemblyUserData udata = ELEMENT_TO_WASM_USERDATA(element);

        argStream.WritePointer(listPtr + (elementCount * sizeof(CWebAssemblyUserData)), &udata);

        elementCount++;
    }
    
    return argStream.Return(elementCount);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementDimension, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(-1);
    }

    if (IS_OBJECT(element) && CStaticFunctionDefinitions::IsObjectVisibleInAllDimensions(element))
    {
        return argStream.Return(-1);
    }

    uint16_t dimension;

    if (!CStaticFunctionDefinitions::GetElementDimension(element, dimension))
    {
        return argStream.Return(-1);
    }

    return argStream.Return((int32_t)dimension);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementZoneName, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress ptr;
    uint32_t                         maxSize;
    bool                             citiesOnly;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadPointerAddress(ptr);
    argStream.ReadUInt32(maxSize, 0xffffffff);
    argStream.ReadBoolean(citiesOnly);

    if (!element)
    {
        return argStream.Return(0);
    }

    SString zoneName;

    if (!CStaticFunctionDefinitions::GetElementZoneName(element, zoneName, citiesOnly))
    {
        return argStream.Return(0);
    }

    if (zoneName.empty())
    {
        return argStream.Return(0);
    }

    size_t size = zoneName.size();

    argStream.WritePointer(ptr, zoneName.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementColShape, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    CColShape* colShape = CStaticFunctionDefinitions::GetElementColShape(element);

    if (!colShape)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return((CElement*)colShape);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementAlpha, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(0);
    }

    uint8_t alpha;

    if (!CStaticFunctionDefinitions::GetElementAlpha(element, alpha))
    {
        return argStream.Return(0);
    }

    return argStream.Return((uint32_t)alpha);
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementDoubleSided, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    bool isDoubleSided;

    if (!CStaticFunctionDefinitions::IsElementDoubleSided(element, isDoubleSided))
    {
        return argStream.Return(false);
    }

    return argStream.Return(isDoubleSided);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementHealth, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(0.0f);
    }

    float32_t health;

    if (!CStaticFunctionDefinitions::GetElementHealth(element, health))
    {
        return argStream.Return(0.0f);
    }

    return argStream.Return(health);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementModel, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(0);
    }

    uint16_t model;

    if (!CStaticFunctionDefinitions::GetElementModel(element, model))
    {
        return argStream.Return(0);
    }

    return argStream.Return((uint32_t)model);
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementInWater, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    bool within;

    if (!CStaticFunctionDefinitions::IsElementInWater(element, within))
    {
        return argStream.Return(false);
    }

    return argStream.Return(within);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementSyncer, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    CElement* syncer = NULL;

    if (!CStaticFunctionDefinitions::GetElementSyncer(syncer))
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(syncer);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementCollisionsEnabled, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::GetElementCollisionsEnabled(element));
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementFrozen, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    bool frozen;

    if (!CStaticFunctionDefinitions::IsElementFrozen(element, frozen))
    {
        return argStream.Return(false);
    }

    return argStream.Return(frozen);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetLowLODElement, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    CElement* lowLODElement = NULL;
    
    if (!CStaticFunctionDefinitions::GetLowLodElement(element, lowLODElement))
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(lowLODElement);
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementLowLOD, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    bool isLowLOD;

    if (!CStaticFunctionDefinitions::IsElementLowLod(element, isLowLOD))
    {
        return argStream.Return(false);
    }

    return argStream.Return(isLowLOD);
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementCallPropagationEnabled, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    bool enabled;

    if (!CStaticFunctionDefinitions::IsElementCallPropagationEnabled(element, enabled))
    {
        return argStream.Return(false);
    }

    return argStream.Return(enabled);
}

WebAssemblyApi(CWebAssemblyElementDefs::ClearElementVisibleTo, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementVisibleTo, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementVisibleTo, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementData, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::GetAllElementData, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::HasElementData, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementData, env, args, results)
{
    CElement*                        element;
    SString                          key;
    bool                             syncMode;
    CWebAssemblyMemoryPointerAddress dataPtr;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadString(key);
    argStream.ReadBoolean(syncMode, true);
    argStream.ReadPointerAddress(dataPtr);
    argStream.ReadUInt32(maxSize);

    if (!element || dataPtr == WEB_ASSEMBLY_NULL_PTR || key.empty())
    {
        return argStream.Return(false);
    }

    ESyncType syncType = syncMode ? ESyncType::BROADCAST : ESyncType::LOCAL;

    if (key.length() > MAX_CUSTOMDATA_NAME_LENGTH)
    {
        key = key.Left(MAX_CUSTOMDATA_NAME_LENGTH);
    }

    CLuaArgument arg;
    arg.SetString("this is a web assembly data!");

    if (!CStaticFunctionDefinitions::SetElementData(element, key.c_str(), arg, syncType))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::RemoveElementData, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::AddElementDataSubscriber, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::RemoveElementDataSubscriber, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::HasElementDataSubscriber, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::AttachElements, env, args, results)
{
    CElement* element;
    CElement* targetElement;
    CVector   posOffset;
    CVector   rotOffset;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadUserData(targetElement);
    argStream.ReadVector3D(posOffset, posOffset);
    argStream.ReadVector3D(rotOffset, rotOffset);

    if (!element || !targetElement)
    {
        return argStream.Return(false);
    }

    if (!CStaticFunctionDefinitions::AttachElements(element, targetElement, posOffset, rotOffset))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::DetachElements, env, args, results)
{
    CElement* element;
    CElement* targetElement;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadUserData(targetElement);

    if (!CStaticFunctionDefinitions::DetachElements(element, targetElement))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::IsElementAttached, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::IsElementAttached(element));
}

WebAssemblyApi(CWebAssemblyElementDefs::GetAttachedElements, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress listPtr;
    uint32_t                         maxSize;
    
    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadPointerAddress(listPtr);
    argStream.ReadUInt32(maxSize);

    if (!element || listPtr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    uint32_t elementsCount = 0;

    std::list<CElement*>::const_iterator iter = element->AttachedElementsBegin();

    for (; iter != element->AttachedElementsEnd() && elementsCount < maxSize; iter++)
    {
        CWebAssemblyUserData udata = ELEMENT_TO_WASM_USERDATA(*iter);

        argStream.WritePointer(listPtr + (elementsCount * sizeof(CWebAssemblyUserData)), &udata);

        elementsCount++;
    }

    return argStream.Return(elementsCount);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementAttachedTo, env, args, results)
{
    CElement* element;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);

    if (!element)
    {
        return argStream.ReturnNull();
    }

    CElement* attachedTo = CStaticFunctionDefinitions::GetElementAttachedTo(element);

    if (!attachedTo)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(attachedTo);
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementAttachedOffsets, env, args, results)
{
    CElement* element;
    CVector   posOffset;
    CVector   rotOffset;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadVector3D(posOffset, posOffset);
    argStream.ReadVector3D(rotOffset, rotOffset);

    if (!element)
    {
        return argStream.Return(false);
    }

    if (!CStaticFunctionDefinitions::SetElementAttachedOffsets(element, posOffset, rotOffset))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyElementDefs::GetElementAttachedOffsets, env, args, results)
{
    CElement*                        element;
    CWebAssemblyMemoryPointerAddress posOffsetPtr;
    CWebAssemblyMemoryPointerAddress rotOffsetPtr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadPointerAddress(posOffsetPtr);
    argStream.ReadPointerAddress(rotOffsetPtr);

    if (!element)
    {
        return argStream.Return(false);
    }

    CVector posOffset;
    CVector rotOffset;

    if (!CStaticFunctionDefinitions::GetElementAttachedOffsets(element, posOffset, rotOffset))
    {
        return argStream.Return(false);
    }

    struct
    {
        float32_t x;
        float32_t y;
        float32_t z;
    } offset;
    memset(&offset, 0, sizeof(offset));

    if (posOffsetPtr != WEB_ASSEMBLY_NULL_PTR)
    {
        offset.x = posOffset.fX;
        offset.y = posOffset.fY;
        offset.z = posOffset.fZ;

        argStream.WritePointer(posOffsetPtr, &offset);
    }

    if (rotOffsetPtr != WEB_ASSEMBLY_NULL_PTR)
    {
        offset.x = rotOffset.fX;
        offset.y = rotOffset.fY;
        offset.z = rotOffset.fZ;

        argStream.WritePointer(rotOffsetPtr, &offset);
    }

    return argStream.Return(!(posOffsetPtr == WEB_ASSEMBLY_NULL_PTR || rotOffsetPtr == WEB_ASSEMBLY_NULL_PTR));
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementID, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementParent, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementMatrix, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementPosition, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementRotation, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementVelocity, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementTurnVelocity, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementInterior, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementDimension, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementAlpha, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementDoubleSided, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementHealth, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementModel, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementSyncer, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementCollisionsEnabled, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementFrozen, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetLowLODElement, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyElementDefs::SetElementCallPropagationEnabled, env, args, results)
{
    CElement* element;
    bool      enabled;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(element);
    argStream.ReadBoolean(enabled);

    if (!element)
    {
        return argStream.Return(false);
    }

    if (!CStaticFunctionDefinitions::SetElementCallPropagationEnabled(element, enabled))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}
