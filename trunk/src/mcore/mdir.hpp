
/*
 * Copyright (C) wenjie.zhao
 */



#ifndef MDIR_HPP
#define MMIR_HPP

#include "mstring.hpp"
#include "mstringlist.hpp"

class MDir
{
public:
    MDir(const MString &dirName);
    virtual ~MDir();

public:
    /*
    * create dir
    * can used to create multi level directory
    */
    static bool createDir(const MString &dirName);
    bool createDir();

    /*
    * return entry list of the dir
    * if @a recusive, then return all file entries.
    */
    MStringList entryList(bool recusive);

    /*
    * return whether this is a dir entry
    */
    bool isDir();

    /*
    * return whether this dir exist
    */
    static bool exists(const MString &dirName);
    bool exists();

    /*
    * remove this dir, return true if success
    */
    bool remove();

    /*
    * @static overload function
    * remove this dir, return true if success
    */
    static bool remove(const MString &dirName);

private:
    MStringList entryListByDir(const MString &dirName);

private:
    MString _dirName;
};

#endif // MDIR_HPP
