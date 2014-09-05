
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MCORE_HPP
#define MCORE_HPP

#include "mglobal.hpp"

extern "C" {
/*!
    Returns the CRC-16 checksum of the first \a len bytes of \a data.

    The checksum is independent of the byte order (endianness).

    \note This function is a 16-bit cache conserving (16 entry table)
    implementation of the CRC-16-CCITT algorithm.
*/
extern muint16 mChecksum(const char *data, uint len);

/*!
    check if the char \a c is a upper char.
*/
extern inline bool mIsUpper(char c);

/*!
    change the char c to lower char.
*/
extern inline char mToLower(char c);
}

#endif // MCORE_HPP
