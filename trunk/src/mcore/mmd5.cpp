
/*
 * Copyright (C) wenjie.zhao
 */


#include "mmd5.hpp"

#include <stdio.h>
#include <string.h>

#include "crypto/md5/md5.h"

std::string MMD5::md5(unsigned char *data, int len)
{
    MD5Context ctx;
    MD5Init(&ctx);

    MD5Update(&ctx, data, len); 

    unsigned char res[16];
    MD5Final(&ctx, res);

    std::string ret;
    for (int i =0; i < 16; ++i) {
        char buf[2];
        sprintf(buf, "%02x", res[i]);
        ret.append(buf, 2);
    }

    return ret;
}

std::string MMD5::md5(const std::string &str)
{
    unsigned char p[str.length()];
    memcpy(p, str.data(), str.length());

    return md5(p, str.length());
}
