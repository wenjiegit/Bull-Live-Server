#include "mamftypes.hpp"

MAMF0Any *MAMFObject::value(int index)
{
    mAssert(index < values.size());

    pair<MString, MAMF0Any *> &p = values.at(index);
    return p.second;
}

MString MAMFObject::key(int index)
{
    mAssert(index < values.size());

    pair<MString, MAMF0Any *> &p = values.at(index);
    return p.first;
}

MString MAMFObject::value(const MString &k)
{
    int index = indexOf(k);
    if (index > 0) {
        MAMF0Any *any = value(index);
        MAMF0ShortString *str = dynamic_cast<MAMF0ShortString *>(any);
        if (!str) return "";
        return str->var;
    }

    return "";
}

int MAMFObject::indexOf(const MString &key)
{
    for (unsigned int i = 0; i < values.size(); ++i) {
        pair<MString, MAMF0Any *> &p = values.at(i);
        const MString &k = p.first;
        if (k == key) {
            return i;
        }
    }

    return -1;
}

void MAMFObject::setValue(const MString &key, MAMF0Any *any)
{
    values.push_back(make_pair<MString, MAMF0Any *>(key, any));
}

void MAMFObject::clear()
{
    for (int i = 0; i < values.size(); ++i) {
        pair<MString, MAMF0Any *> &p = values.at(i);
        MAMF0Any *any = p.second;
        mFree(any);
    }

    values.clear();
}


void MAMF0StrictArray::clear()
{
    vector<MAMF0Any *>::iterator iter;
    for (iter = values.begin(); iter != values.end(); ++iter) {
        MAMF0Any *any = *iter;
        mFree(any);
    }

    values.clear();
}
