
/*
 * Copyright (C) wenjie.zhao
 */


#include "mobject.hpp"

#include <vector>
#include <set>

#include "mcoreapplication.hpp"

MObject::MObject(MObject *parent)
{
    _parent = parent;
    if (_parent) {
        setParent(_parent);
    }
}

MObject::~MObject()
{
    removeChilden();
    if (_parent) {
        _parent->objectList.remove(this);
    }
}

void MObject::deleteLater()
{
    mApp()->deleteLater(this);
}

void MObject::setParent(MObject *parent)
{
    _parent = parent;
    if (_parent) {
        _parent->objectList.push_back(this);
    }
}

MObject *MObject::parent()
{
    return _parent;
}

void MObject::removeChilden()
{
    for (MObjectListIterator iter = objectList.begin(); iter != objectList.end(); ++iter) {
        MObject *obj = *iter;
        obj->setParent(0);
        if (obj) {
            delete obj;
        }
    }

    if (!objectList.empty()) {
        objectList.clear();
    }
}

