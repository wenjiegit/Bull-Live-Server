
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MSTRING_H
#define MSTRING_H

#include <string>
#include "mglobal.hpp"

using namespace std;

typedef string::iterator MStringIterator;
class MStringList;

/*!
    @brief extend of standard string
    never used in this format :
        string * a = new MString;
        delete a;
    this will lead to mem leak.
*/

class MString : public string
{
public:
    MString();
    MString(const char *str);
    MString(const char *str, int n);
    MString(const string &str);
    //MString(const string &str, size_t pos, size_t len = npos);
    MString(size_t n, char c);

    virtual ~MString();

    string toStdString();

    void chop(int n);
    MString trimmed() const;
    void truncate(int position);

    /**
    * @brief C-Style sprintf
    * @see int vsprintf(char *str, const char *format, va_list ap)
    */
    MString &sprintf(const char *cformat, ...);

    bool contains(const MString &str) const;
    bool contains(const string &str) const;
    bool contains(const char *str) const;
    bool contains(const char *str, size_t len) const;

    bool startWith(const MString &str) const;
    bool startWith(const string &str) const;
    bool startWith(const char *str) const;

    bool endWith(const MString &str) const;
    bool endWith(const string &str) const;
    bool endWith(const char *str) const;

    /**
    * @brief replace string
    * @retval *this
    * @snippet
    * Mstring a("///////a/bc////cc");
    * a.replace("//", "/");  // now a = "/a/bc/c"
    * a.replace("//", "/", false);  // a = "//////a/bc////cc"
    */
    MString &replace(const MString &before, const MString &after, bool replaceAll = true);

    int toInt(bool *ok = 0, int base = 10);
    short toShort(bool *ok = 0, int base = 10);
    MString toHex();
    muint64 toInt64(bool *ok = 0, int base = 10);

    inline bool isEmpty() const { return empty(); }

    MStringList split(const MString &sep);

    /**
     *  prepend \a str
     *  @snippet
     *  MString pd("before");
     *  pd.prepend("123");      // pd : "123before"
     */
    MString &prepend(const MString &str);
    MString &prepend(const char *str, int size);

    MString & operator <<(mint32 value);
    MString & operator <<(muint64 value);
    MString & operator <<(const MString &str);

public:
    static MString number(mint32 n, int base = 10);
    static MString number(muint64 n, int base = 10);
    static MString number(size_t n, int base = 10);
};

#endif // MSTRING_H
