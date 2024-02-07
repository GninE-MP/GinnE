/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/xml/CXMLErrorCodes.h
 *  PURPOSE:     XML error code enumerations
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CXMLErrorCodes
{
public:
    enum Code
    {
        NoError = 0,
        NoFileSpecified = 1,
        OtherError = 2
    };
};
