/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/xml/CXMLAttributes.h
 *  PURPOSE:     XML attributes container interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <list>

class CXMLAttribute;

class CXMLAttributes
{
public:
    virtual unsigned int   Count() = 0;
    virtual CXMLAttribute* Find(const char* szName) = 0;
    virtual CXMLAttribute* Get(unsigned int uiIndex) = 0;

    virtual CXMLAttribute* Create(const char* szName) = 0;
    virtual CXMLAttribute* Create(const CXMLAttribute& Copy) = 0;
    virtual bool           Delete(const char* szName) = 0;
    virtual void           DeleteAll() = 0;

    virtual std::list<CXMLAttribute*>::iterator ListBegin() = 0;
    virtual std::list<CXMLAttribute*>::iterator ListEnd() = 0;
};
