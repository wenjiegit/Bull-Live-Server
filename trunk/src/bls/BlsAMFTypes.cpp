#include "BlsAMFTypes.hpp"

BlsAMF0Any *BlsAMFObject::value(int index)
{
    mAssert(index < (int)values.size());

    pair<MString, BlsAMF0Any *> &p = values.at(index);
    return p.second;
}

MString BlsAMFObject::key(int index)
{
    mAssert(index < (int)values.size());

    pair<MString, BlsAMF0Any *> &p = values.at(index);
    return p.first;
}

MString BlsAMFObject::value(const MString &k)
{
    int index = indexOf(k);
    if (index >= 0) {
        BlsAMF0Any *any = value(index);
        BlsAMF0ShortString *str = dynamic_cast<BlsAMF0ShortString *>(any);
        if (!str) return "";
        return str->var;
    }

    return "";
}

int BlsAMFObject::indexOf(const MString &key)
{
    for (unsigned int i = 0; i < values.size(); ++i) {
        BlsAmf0ObjectProperty &p = values.at(i);
        if (p.first == key) {
            return i;
        }
    }

    return -1;
}

void BlsAMFObject::setValue(const MString &key, BlsAMF0Any *any)
{
    values.push_back(make_pair<MString, BlsAMF0Any *>(key, any));
}

void BlsAMFObject::clear()
{
    for (unsigned int i = 0; i < values.size(); ++i) {
        pair<MString, BlsAMF0Any *> &p = values.at(i);
        BlsAMF0Any *any = p.second;
        mFree(any);
    }

    values.clear();
}


void BlsAMF0StrictArray::clear()
{
    vector<BlsAMF0Any *>::iterator iter;
    for (iter = values.begin(); iter != values.end(); ++iter) {
        BlsAMF0Any *any = *iter;
        mFree(any);
    }

    values.clear();
}
