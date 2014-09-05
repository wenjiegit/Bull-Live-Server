
/*
 * Copyright (C) wenjie.zhao
 */


#include "mdir.hpp"
#include <merrno.hpp>
#include <sys/stat.h>
#include <unistd.h>

MDir::MDir(const MString &dirName)
    : _dirName(dirName)
{
    // process multi "//" to one "/"
    _dirName.push_back('/');
    _dirName.replace("//", "/");
}

MDir::~MDir()
{
}

bool MDir::createDir(const MString &dirName)
{
    // process multi "//" to one "/"
    MString dn = dirName;
    dn.push_back('/');
    dn.replace("//", "/");

    int index = dn.startWith("/") ? 1 : 0;
    for (unsigned int i = index; i < dn.size(); ++i) {
        char &c = dn[i];

        if (c == '/') {
            c = '\0';
            int ret = access(dn.c_str(), F_OK);
            if (ret == 0) {
                c = '/';
                continue;
            }

            ret = mkdir(dn.c_str(), 0744);
            if (ret == 0) {
                c = '/';
                continue;
            }

//            if (errno == ENOENT) {
//                ret = mkdir(dn.c_str(), 0744);
//                if (ret == 0) {
//                    continue;
//                }
//            }

            merrno = errno;
            return false;
        }
    }

    return true;
}

bool MDir::createDir()
{
    return MDir::createDir(this->_dirName);
}

MStringList MDir::entryList(bool recusive)
{
    MStringList ret;

    // TODO : add process code

    return ret;
}

bool MDir::isDir()
{
    struct stat buf;
    if (stat(_dirName.c_str(), &buf) != 0) {
        merrno = errno;
        return false;
    }

    if (S_ISDIR(buf.st_mode)) {
        return true;
    }

    return false;
}

bool MDir::exists()
{
    // only access returns 0, we consider the dir is existed.
    if (MDir::exists(this->_dirName)){
        return true;
    }

    return false;
}

bool MDir::exists(const MString &dirName)
{
    // only access returns 0, we consider the dir is existed.
    if (access(dirName.c_str(), F_OK) == 0) {
        return true;
    }

    return false;
}

bool MDir::remove()
{
    return  false;
}

bool MDir::remove(const MString &dirName)
{
    return  false;
}

MStringList MDir::entryListByDir(const MString &dirName)
{
    MStringList ret;

    // TODO : add process code
    return ret;
}

//int MDir::listDir(FileType type,std::vector<std::string>& paths)
//{
//    struct dirent *file;
//    struct stat st;

//    if(!d){
//        if(!(d = opendir(dir_name.c_str()))){
//            merrno = E_OPENDIR;
//            return -1;
//        }
//    }

//    while ((file = readdir(d)) != NULL) {
//        //ingore . and ..
//        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..")) {
//            continue;
//        }

//        std::string path_temp = dir_name + file->d_name;
//        if (type == DIR_ && (stat(path_temp.c_str(), &st) == 0) && S_ISDIR(st.st_mode)) {
//            paths.push_back(path_temp + '/');
//        } else {
//            paths.push_back(path_temp);
//        }
//    }

//    return 0;
//}

//int MDir::deleteDir()
//{
//    /*struct dirent *file;
//    struct stat st;

//    if(!d){
//        if(!(d = opendir(dir_name.c_str()))){
//            return -1;
//        }
//    }

//    while((file = readdir(d)) != NULL){
//        //ingore . and ..
//        if(strncmp(file->d_name,".",1) == 0){
//            continue;
//        }

//        std::string path_temp = dir_name + file->d_name;
//        if((stat(path_temp.c_str(), &st) == 0) && S_ISDIR(st.st_mode)){
//            rmdir(path_temp.c_str());
//        }else{
//            remove(path_temp.c_str());
//        }
//    }*/

//    return 0;
//}
