/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/FileHash.h
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <array>
#include <string>

/**
 * @brief Represents the raw SHA-256 checksum of a file.
 */
using FileHash = std::array<unsigned char, 32>;

/**
 * @brief Generates the string representation of the hash in hexadecimal digits.
 * @param hash Hash of the file
 */
auto GetFileHashString(const FileHash& hash) -> std::string;
