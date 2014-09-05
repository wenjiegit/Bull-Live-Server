
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MObject_HPP
#define MObject_HPP

#include <list>
#include <map>

#include "mglobal.hpp"
#include "mstring.hpp"

class MObject;

typedef void (MObject::*TimerHandle)(void);
typedef std::list<MObject *> MObjectList;
typedef std::list<MObject *>::iterator MObjectListIterator;

class MObject
{
public:
    MObject(MObject *parent = 0);
    virtual ~MObject();

    void deleteLater();
    void setParent(MObject *parent);
    MObject *parent();

public:
    virtual void timerA() {}
    virtual void timerB() {}
    virtual void timerC() {}
    virtual void timerD() {}
    virtual void timerE() {}
    virtual void timerF() {}
    virtual void timerG() {}

private:
    void removeChilden();

private:
    MObjectList objectList;
    MObject *_parent;
};

#endif // MObject_HPP

