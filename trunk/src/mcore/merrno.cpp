
/*
 * Copyright (C) wenjie.zhao
 */


#include "merrno.hpp"
#include <string.h>
#include <string>

const char *mstrerror(int errnum)
{
    switch (errnum) {
    case MERROR::E_SUCCESS:
        return "no error.";
    case MERROR::E_READDIR:
        return "";
    case MERROR::E_STAT:
        return "";
    case MERROR::E_OPENDIR:
        return "";
    case MERROR::E_MKDIR:
        return "";
    case MERROR::E_OPENFILE:
        return "";
    case MERROR::E_READFILE:
        return "";
    case MERROR::E_WRITEFILE:
        return "";
    case MERROR::E_DIR_NAME_EMPTY:
        return "dir name should not be empty";
    case MERROR::E_SPACE_NOT_ENOUGH:
        return "left space is not enough";
    case MERROR::E_AMF_TYPE_ERROR:
        return "amf marker type error";
    case MERROR::E_STREAM_SKIP_ERROR:
        return "MStream skip failed.";
    case MERROR::E_VALUE_IS_ZERO:
        return "Value is 0";
    case MERROR::E_TYPE_NOT_SUPPORTED:
        return "type is not supported";
    case MERROR::E_INVOKE_NO_METHOD:
        return "no such this method";
    case MERROR::E_WRITE_ERROR:
        return "write error";
    case MERROR::E_READ_ERROR:
        return "read error";
    case MERROR::E_URL_NO_TCURL:
        return "no tcurl set";
    case MERROR::E_CONNECTION_FAILED:
        return "conncet failed.";
    case MERROR::E_SOCKET_CLOSE_NORMALLY:
        return "socket close normally.";
    case MERROR::E_STREAM_ID_TOO_SMALL:
        return "stream id is too small, should > 0";
    case MERROR::E_RTMP_PUBLISH_FAILED:
        return "publish stream failed";
    case MERROR::E_STREAM_BADNAME:
        return "NetStream.Publish.BadName";
    case MERROR::E_STREAM_FILE_EXIST:
        return "stream file exist";
    case MERROR::E_VHOST_NOT_EXIST:
        return "vhost is not exist";
    case MERROR::E_ORIGIN_NOT_EXIST:
        return "origin server is empty";

    default:
        return strerror(errnum);
    }

    return "no error";
}

int merrno = MERROR::E_SUCCESS;


void setError(int err)
{
    merrno = err;
}
