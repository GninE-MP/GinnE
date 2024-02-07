/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CZipMaker.h
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/
#pragma once

class CZipMaker
{
public:
    CZipMaker(const SString& strZipPathFilename);
    ~CZipMaker();

    bool IsValid();
    bool Close();
    bool InsertFile(const SString& strSrc, const SString& strDest);
    bool InsertDirectoryTree(const SString& strSrc, const SString& strDest);

protected:
    bool AddFile(const SString& strDest, const std::vector<char>& buffer);

    void* m_uzFile;
};
