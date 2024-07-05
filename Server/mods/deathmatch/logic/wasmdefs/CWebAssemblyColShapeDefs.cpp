/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyColShapeDefs.cpp
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

#include "CColCircle.h"
#include "CColCuboid.h"
#include "CColSphere.h"
#include "CColRectangle.h"
#include "CColPolygon.h"
#include "CColTube.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyColShapeDefs.h"

void CWebAssemblyColShapeDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_col_circle", "u*f");
    SetFunctionType("create_col_cuboid", "u**");
    SetFunctionType("create_col_sphere", "u*f");
    SetFunctionType("create_col_rectangle", "u**");
    SetFunctionType("create_col_polygon", "u**i");
    SetFunctionType("create_col_tube", "u*ff");

    SetFunctionType("get_col_shape_radius", "fu");
    SetFunctionType("set_col_shape_radius", "buf");
    SetFunctionType("get_col_shape_size", "bu*");
    SetFunctionType("set_col_shape_size", "bu*");
    SetFunctionType("get_col_polygon_points", "iu*i");
    SetFunctionType("get_col_polygon_point_position", "bui*");
    SetFunctionType("set_col_polygon_point_position", "bui*");
    SetFunctionType("add_col_polygon_point", "bu*i");
    SetFunctionType("remove_col_polygon_point", "bui");

    SetFunctionType("is_inside_col_shape", "bu*");
    SetFunctionType("get_col_shape_type", "iu");

    SetFunctionType("get_col_polygon_height", "bu*");
    SetFunctionType("set_col_polygon_height", "bu*");
}

void CWebAssemblyColShapeDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_col_circle", CreateColCircle },
        { "create_col_cuboid", CreateColCuboid },
        { "create_col_sphere", CreateColSphere },
        { "create_col_rectangle", CreateColRectangle },
        { "create_col_polygon", CreateColPolygon },
        { "create_col_tube", CreateColTube },

        { "get_col_shape_radius", GetColShapeRadius },
        { "set_col_shape_radius", SetColShapeRadius },
        { "get_col_shape_size", GetColShapeSize },
        { "set_col_shape_size", SetColShapeSize },
        { "get_col_polygon_points", GetColPolygonPoints },
        { "get_col_polygon_point_position", GetColPolygonPointPosition },
        { "set_col_polygon_point_position", SetColPolygonPointPosition },
        { "add_col_polygon_point", AddColPolygonPoint },
        { "remove_col_polygon_point", RemoveColPolygonPoint },

        { "is_inside_col_shape", IsInsideColShape },
        { "get_col_shape_type", GetColShapeType },

        { "get_col_polygon_height", GetColPolygonHeight },
        { "set_col_polygon_height", SetColPolygonHeight }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::CreateColCircle, env, args, results)
{
    CVector2D position;
    float32_t radius;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector2D(position);
    argStream.ReadFloat32(radius);

    if (radius < 0.1f)
    {
        radius = 0.1f;
    }

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CColCircle* col = CStaticFunctionDefinitions::CreateColCircle(resource, position, radius);

    if (!col)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(col);
    }

    return argStream.Return((CElement*)col);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::CreateColCuboid, env, args, results)
{
    CVector position;
    CVector size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(position);
    argStream.ReadVector3D(size);

    size.fX = std::max(size.fX, 0.1f);
    size.fY = std::max(size.fY, 0.1f);
    size.fZ = std::max(size.fZ, 0.1f);

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CColCuboid* col = CStaticFunctionDefinitions::CreateColCuboid(resource, position, size);

    if (!col)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(col);
    }

    return argStream.Return((CElement*)col);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::CreateColSphere, env, args, results)
{
    CVector   position;
    float32_t radius;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(position);
    argStream.ReadFloat32(radius);

    if (radius < 0.1f)
    {
        radius = 0.1f;
    }

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CColSphere* col = CStaticFunctionDefinitions::CreateColSphere(resource, position, radius);

    if (!col)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(col);
    }

    return argStream.Return((CElement*)col);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::CreateColRectangle, env, args, results)
{
    CVector2D position;
    CVector2D size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);

    size.fX = std::max(size.fX, 0.1f);
    size.fY = std::max(size.fY, 0.1f);

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CColRectangle* col = CStaticFunctionDefinitions::CreateColRectangle(resource, position, size);

    if (!col)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(col);
    }

    return argStream.Return((CElement*)col);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::CreateColPolygon, env, args, results)
{
    CVector2D center;
    struct
    {
        float32_t x;
        float32_t y;
    }*        pointList;
    uint32_t  pointCount;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector2D(center);
    argStream.ReadPointer(pointList);
    argStream.ReadUInt32(pointCount, 0xffffffff);

    if (!pointList || pointCount < 3)
    {
        return argStream.ReturnNull();
    }

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    std::vector<CVector2D> points;
    points.push_back(center);

    for (; pointCount > 0; pointCount--, pointList++)
    {
        points.push_back(CVector2D(pointList->x, pointList->y));
    }

    CColPolygon* col = CStaticFunctionDefinitions::CreateColPolygon(resource, points);

    if (!col)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(col);
    }

    return argStream.Return((CElement*)col);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::CreateColTube, env, args, results)
{
    CVector   position;
    float32_t radius;
    float32_t height;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadVector3D(position);
    argStream.ReadFloat32(radius);
    argStream.ReadFloat32(height);

    if (radius < 0.1f)
    {
        radius = 0.1f;
    }

    if (height < 0.1f)
    {
        height = 0.1f;
    }

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CColTube* col = CStaticFunctionDefinitions::CreateColTube(resource, position, radius, height);

    if (!col)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(col);
    }

    return argStream.Return((CElement*)col);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::GetColShapeRadius, env, args, results)
{
    CColShape* colShape;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(colShape);

    if (!colShape)
    {
        return argStream.Return(0.0f);
    }

    float radius;

    if (!CStaticFunctionDefinitions::GetColShapeRadius(colShape, radius))
    {
        return argStream.Return(0.0f);
    }

    return argStream.Return(radius);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::SetColShapeRadius, env, args, results)
{
    CColShape* colShape;
    float32_t  radius;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(colShape);
    argStream.ReadFloat32(radius);

    if (!colShape)
    {
        return argStream.Return(0.0f);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetColShapeRadius(colShape, radius));
}

WebAssemblyApi(CWebAssemblyColShapeDefs::GetColShapeSize, env, args, results)
{
    CColShape* col;
    struct
    {
        float32_t x;
        float32_t y;
        float32_t z;
    }* size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadPointer(size);

    if (!col || !size)
    {
        return argStream.Return(false);
    }

    memset(size, 0, sizeof(*size));

    switch (col->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
            {
                CVector2D colSize = static_cast<CColRectangle*>(col)->GetSize();

                size->x = colSize.fX;
                size->y = colSize.fY;
            }

            break;
        case COLSHAPE_CUBOID:
            {
                CVector colSize = static_cast<CColCuboid*>(col)->GetSize();

                size->x = colSize.fX;
                size->y = colSize.fY;
                size->z = colSize.fZ;
            }

            break;
        case COLSHAPE_TUBE:
            {
                size->x = static_cast<CColTube*>(col)->GetHeight();
            }

            break;
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::SetColShapeSize, env, args, results)
{
    CColShape* col;
    CVector    size;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadVector3D(size);

    if (!col)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetColShapeSize(col, size));
}

WebAssemblyApi(CWebAssemblyColShapeDefs::GetColPolygonPoints, env, args, results)
{
    CColShape* col;
    struct
    {
        float32_t x;
        float32_t y;
    }*         outPoints;
    uint32_t   maxItemSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadPointer(outPoints);
    argStream.ReadUInt32(maxItemSize, 0xffffffff);

    if (!col || col->GetShapeType() != COLSHAPE_POLYGON || !outPoints)
    {
        return argStream.Return(0);
    }

    CColPolygon* colPol = static_cast<CColPolygon*>(col);

    uint32_t count = 0;

    std::vector<CVector2D>::const_iterator iter = colPol->IterBegin();

    for (; iter != colPol->IterEnd() && maxItemSize > 0; iter++, maxItemSize--)
    {
        CVector2D point = *iter;

        outPoints->x = point.fX;
        outPoints->y = point.fY;

        count++;
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::GetColPolygonPointPosition, env, args, results)
{
    CColShape* col;
    uint32_t   pointIndex;
    struct
    {
        float32_t x;
        float32_t y;
    }*         outPosition;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadUInt32(pointIndex);
    argStream.ReadPointer(outPosition);

    if (!col || col->GetShapeType() != COLSHAPE_POLYGON || !outPosition || pointIndex < 1)
    {
        return argStream.Return(false);
    }

    CColPolygon* colPol = static_cast<CColPolygon*>(col);
    CVector2D    point;

    if (!CStaticFunctionDefinitions::GetColPolygonPointPosition(colPol, pointIndex - 1, point))
    {
        return argStream.Return(false);
    }

    outPosition->x = point.fX;
    outPosition->y = point.fY;

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::SetColPolygonPointPosition, env, args, results)
{
    CColShape* col;
    uint32_t   pointIndex;
    CVector2D  position;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadUInt32(pointIndex);
    argStream.ReadVector2D(position);

    if (!col || col->GetShapeType() != COLSHAPE_POLYGON || pointIndex < 1)
    {
        return argStream.Return(false);
    }

    CColPolygon* colPol = static_cast<CColPolygon*>(col);

    return argStream.Return(CStaticFunctionDefinitions::SetColPolygonPointPosition(colPol, pointIndex - 1, position));
}

WebAssemblyApi(CWebAssemblyColShapeDefs::AddColPolygonPoint, env, args, results)
{
    CColShape* col;
    CVector2D  position;
    uint32_t   pointIndex;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadVector2D(position);
    argStream.ReadUInt32(pointIndex, 0);

    if (!col || col->GetShapeType() != COLSHAPE_POLYGON)
    {
        return argStream.Return(false);
    }

    CColPolygon* colPol = static_cast<CColPolygon*>(col);

    if (pointIndex == 0)
    {
        return argStream.Return(CStaticFunctionDefinitions::AddColPolygonPoint(colPol, position));
    }

    return argStream.Return(CStaticFunctionDefinitions::AddColPolygonPoint(colPol, pointIndex - 1, position));
}

WebAssemblyApi(CWebAssemblyColShapeDefs::RemoveColPolygonPoint, env, args, results)
{
    CColShape* col;
    uint32_t   pointIndex;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadUInt32(pointIndex);

    if (!col || col->GetShapeType() != COLSHAPE_POLYGON || pointIndex < 1)
    {
        return argStream.Return(false);
    }

    CColPolygon* colPol = static_cast<CColPolygon*>(col);

    if (colPol->CountPoints() <= 3)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::RemoveColPolygonPoint(colPol, pointIndex - 1));
}

WebAssemblyApi(CWebAssemblyColShapeDefs::IsInsideColShape, env, args, results)
{
    CColShape* col;
    CVector    position;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadVector3D(position);

    if (!col)
    {
        return argStream.Return(false);
    }

    bool inside;

    if (!CStaticFunctionDefinitions::IsInsideColShape(col, position, inside))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::GetColShapeType, env, args, results)
{
    CColShape* col;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);

    if (!col)
    {
        return argStream.Return(0);
    }

    return argStream.Return((uint32_t)col->GetShapeType());
}

WebAssemblyApi(CWebAssemblyColShapeDefs::GetColPolygonHeight, env, args, results)
{
    CColShape* col;
    float32_t* floor;
    float32_t* ceil;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadPointer(floor);
    argStream.ReadPointer(ceil);

    if (!col || col->GetShapeType() != COLSHAPE_POLYGON)
    {
        return argStream.Return(false);
    }

    static_cast<CColPolygon*>(col)->GetHeight(*floor, *ceil);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyColShapeDefs::SetColPolygonHeight, env, args, results)
{
    CColShape* col;
    float32_t  floor;
    float32_t  ceil;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(col);
    argStream.ReadFloat32(floor);
    argStream.ReadFloat32(ceil);

    if (!col || col->GetShapeType() != COLSHAPE_POLYGON)
    {
        return argStream.Return(false);
    }

    if (floor > ceil)
    {
        std::swap(floor, ceil);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetColPolygonHeight(static_cast<CColPolygon*>(col), floor, ceil));
}
