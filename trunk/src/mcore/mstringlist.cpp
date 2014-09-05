
/*
 * Copyright (C) wenjie.zhao
 */


#include "mstringlist.hpp"

#include <iostream>

MStringList::MStringList()
{
}

MStringList::~MStringList()
{
}

bool MStringList::isEmpty() const
{
    return empty();
}

int MStringList::length()
{
    return size();
}

MString &MStringList::operator [](int i)
{
    list<MString>::iterator iter = begin();
    for (int c = 0; c < i; ++c) {
        ++iter;
    }
    return *iter;
}

const MString &MStringList::operator [](int i) const
{
    MStringListConstItor iter = begin();
    for (int c = 0; c < i; ++c) {
        ++iter;
    }
    return *iter;
}

MStringList &MStringList::operator <<(const MString &str)
{
    this->push_back(str);
    return *this;
}

MStringList &MStringList::operator <<(const MStringList &other)
{
    for (unsigned int i = 0; i < other.size(); ++i) {
        this->push_back(other.at(i));
    }

    return *this;
}

MStringList &MStringList::operator=(const MStringList &other)
{
    this->clear();
    return *this << other;
}

MString &MStringList::at(int pos)
{
    mAssert((unsigned int)pos < size());
    return (*this)[pos];
}

const MString &MStringList::at(int pos) const
{
    mAssert((unsigned int)pos < size());
    return (*this)[pos];
}

MString MStringList::join(const MString &sep)
{
    if (size() == 1) {
        return front();
    }

    MString ret;
    for (list<MString>::iterator iter = begin(); iter != end(); ++iter) {
        ret.append(*iter);
        ret.append(sep);
    }

    return ret;
}

MStringList MStringList::mid(int start, int length)
{
    // // TODO : imp
    MStringList ret;

    mAssert(true);
    return ret;
}

ostream & operator<<(ostream &o, const MStringList& a)
{
    o << "(";
    for (unsigned int i = 0; i < a.size(); ++i) {
        const MString &str = a[i];
        o << "\"" << str << "\"";
        if (i < a.size() -1) {
            o << ",";
        }
    }
    o << ")";

    return o;
}
