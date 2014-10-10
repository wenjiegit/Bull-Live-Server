
/*
 * Copyright (C) wenjie.zhao
 */

#ifndef MAMFTYPES_HPP
#define	MAMFTYPES_HPP

#include <vector>

using namespace std;

// AMF0 types
#define AMF0_NUMBER         (0x00)
#define AMF0_BOOLEAN        (0x01)
#define AMF0_SHORT_STRING   (0x02)
#define AMF0_OBJECT         (0x03)
#define AMF0_NULL           (0x05)
#define AMF0_UNDEFINED      (0x06)
#define AMF0_ECMA_ARRAY     (0x08)
#define AMF0_OBJECT_END     (0x09)
#define AMF0_STRICT_ARRAY   (0x0a)
#define AMF0_TIMESTAMP      (0x0b)
#define AMF0_LONG_STRING    (0x0c)
#define AMF0_TYPED_OBJECT	(0x10)
#define AMF0_AMF3_OBJECT    (0x11)

// AMF3 types
#define AMF3_UNDEFINED      (0x00)
#define AMF3_NULL           (0x01)
#define AMF3_FALSE          (0x02)
#define AMF3_TRUE           (0x03)
#define AMF3_INTEGER        (0x04)
#define AMF3_DOUBLE         (0x05)
#define AMF3_STRING         (0x06)
#define AMF3_XMLDOC         (0x07)
#define AMF3_DATE           (0x08)
#define AMF3_ARRAY          (0x09)
#define AMF3_OBJECT         (0x0a)
#define AMF3_XML            (0x0b)
#define AMF3_BYTEARRAY      (0x0c)

#include <MHash>
#include <MString>
#include <MLoger>

struct MAMF0Any
{
    MAMF0Any() {}
    virtual ~MAMF0Any() {}

    inline bool isNumber()          { return type == AMF0_NUMBER; }
    inline bool isBoolean()         { return type == AMF0_BOOLEAN; }
    inline bool isShortString()    { return type == AMF0_SHORT_STRING; }
    inline bool isAmf0Object()      { return type == AMF0_OBJECT; }
    inline bool isNull()            { return type == AMF0_NULL; }
    inline bool isUndefined()       { return type == AMF0_UNDEFINED; }
    inline bool isEcmaArray()       { return type == AMF0_ECMA_ARRAY; }
    inline bool isStrictArray()     { return type == AMF0_STRICT_ARRAY; }
    inline bool isAmf3Object()      { return type == AMF3_OBJECT; }

    char type;
};

struct MAMF0Number : public MAMF0Any
{
    MAMF0Number(double v = 0.0)
    {
        type = AMF0_NUMBER;
        var = v;
    }
    virtual ~MAMF0Number() {}

    double var;
};

struct MAMF0Boolean : public MAMF0Any
{
    MAMF0Boolean(bool v = false)
    {
        type = AMF0_BOOLEAN;
        var = v;
    }
    virtual ~MAMF0Boolean() {}

    bool var;
};

struct MAMF0ShortString : public MAMF0Any
{
    MAMF0ShortString(const MString &v = "")
    {
        type = AMF0_SHORT_STRING;
        var = v;
    }
    virtual ~MAMF0ShortString() {}

    MString var;
};

typedef pair<MString, MAMF0Any *> Amf0ObjectProperty;

struct MAMFObject : public MAMF0Any
{
    MAMFObject() {}
    virtual ~MAMFObject() {clear();}

    MAMF0Any *value(int index);
    MString key(int index);
    MString value(const MString &k);
    int indexOf(const MString &key);
    void setValue(const MString &key, MAMF0Any *any);
    void clear();

    vector<Amf0ObjectProperty> values;
};

struct MAMF0Object : public MAMFObject
{
    MAMF0Object()
    {
        type = AMF0_OBJECT;
    }
    virtual ~MAMF0Object() {clear();}
};

struct MAMF0Null : public MAMF0Any
{
    MAMF0Null()
    {
        type = AMF0_NULL;
    }
    virtual ~MAMF0Null() {}
};

struct MAMF0Undefined : public MAMF0Any
{
    MAMF0Undefined()
    {
        type = AMF0_UNDEFINED;
    }
    virtual ~MAMF0Undefined() {}
};

struct MAMF0EcmaArray : public MAMFObject
{
    MAMF0EcmaArray()
    {
        type = AMF0_ECMA_ARRAY;
    }
    virtual ~MAMF0EcmaArray() {clear();}

    mint32 count;
};

struct MAMF0StrictArray : public MAMF0Any
{
    MAMF0StrictArray()
    {
        type = AMF0_STRICT_ARRAY;
    }
    virtual ~MAMF0StrictArray() {clear();}

    void clear();

    vector<MAMF0Any *> values;
    mint32 count;
};

struct MAMF3Object : public MAMFObject
{
    MAMF3Object() {}
    virtual ~MAMF3Object() {clear();}
};

#endif	// MAMFTYPES_HPP
