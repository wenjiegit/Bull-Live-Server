
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MSTRINGLIST_HPP
#define MSTRINGLIST_HPP

#include <list>
#include "mstring.hpp"

using namespace std;

class MStringList : public list<MString>
{
public:
    explicit MStringList();
    ~MStringList();

    bool isEmpty() const;
    int length();

    MString &operator[](int i);
    const MString& operator[](int i) const;

    MStringList &operator<<(const MString &str);
    MStringList &operator<<(const MStringList &other);
    MStringList &operator=(const MStringList &other);

    MString &at(int pos);
    const MString& at(int pos) const;

    MString join(const MString &sep);

    MStringList mid(int start, int length);

public:
    friend ostream & operator<<(ostream &o, const MStringList& a);
};

typedef MStringList::iterator MStringListItor;
typedef MStringList::const_iterator MStringListConstItor;

#endif // MSTRINGLIST_HPP
