
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef __MMD5_H__
#define __MMD5_H__

#include <string>

class MMD5
{
public:
    static std::string md5(unsigned char *data, int len);
    static std::string md5(const std::string &str);
};

#endif    // __MMD5_H__

