#include "mamfvariant.hpp"

#include <string.h>
#include <iostream>

MAMFVariant::MAMFVariant()
    : m_type(Invalid)
{
    memset(&m_value, 0, sizeof (m_value));
}

MAMFVariant::~MAMFVariant()
{
    reset();
}

MAMFVariant::MAMFVariant(const bool    &val)
    : m_type(V_BOOL)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.b = val;
}

MAMFVariant::MAMFVariant(const mint8   &val)
    : m_type(V_INT8)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.i8 = val;
}

MAMFVariant::MAMFVariant(const mint16  &val)
    : m_type(V_INT64)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.i16 = val;
}

MAMFVariant::MAMFVariant(const mint32  &val)
    : m_type(V_INT32)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.i32 = val;
}

MAMFVariant::MAMFVariant(const mint64  &val)
    : m_type(V_INT64)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.i64 = val;
}

MAMFVariant::MAMFVariant(const muint8  &val)
    : m_type(V_UINT8)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.ui8 = val;
}

MAMFVariant::MAMFVariant(const muint16 &val)
    : m_type(V_UINT16)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.ui16 = val;
}

MAMFVariant::MAMFVariant(const muint32 &val)
    : m_type(V_UINT32)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.ui32 = val;
}

MAMFVariant::MAMFVariant(const muint64 &val)
    : m_type(V_UINT64)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.ui64 = val;
}

MAMFVariant::MAMFVariant(const double  &val)
    : m_type(V_DOUBLE)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.d = val;
}

MAMFVariant::MAMFVariant(const char *pValue)
    : m_type(V_STRING)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.s = new MString(pValue, strlen(pValue));
}

MAMFVariant::MAMFVariant(const MString &value)
    : m_type(V_STRING)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.s = new MString(value);
}

MAMFVariant::MAMFVariant(const char *pValue, muint32 len)
    : m_type(V_STRING)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.s = new MString(pValue, len ? len : strlen(pValue));
}

MAMFVariant::MAMFVariant(const MAMFObject  &val)
    : m_type(V_OBJECT)
{
    memset(&m_value, 0, sizeof (m_value));
    m_value.obj = new MAMFObject;
    m_value.obj->values = val.values;
}

MAMFVariant & MAMFVariant::operator=(const MAMFVariant &val)
{
    reset();

    m_type = val.m_type;

    switch (m_type) {
    case V_STRING:
        m_value.s = new MString(*val.m_value.s);
        break;
    default:
        break;
    }
    m_value = val.m_value;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const bool    &val)
{
    reset();
    m_type = V_BOOL;
    m_value.b = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const mint8   &val)
{
    reset();
    m_type = V_INT8;
    m_value.i8 = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const mint16  &val)
{
    m_type = V_INT16;
    m_value.i16 = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const mint32  &val)
{
    reset();
    m_type = V_INT32;
    m_value.i32 = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const mint64  &val)
{
    reset();
    m_type = V_INT64;
    m_value.i64 = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const muint8  &val)
{
    reset();
    m_type = V_UINT8;
    m_value.ui8 = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const muint16 &val)
{
    reset();
    m_type = V_UINT16;
    m_value.ui16 = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const muint32 &val)
{
    reset();
    m_type = V_UINT32;
    m_value.ui32 = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const muint64 &val)
{
    reset();
    m_type = V_UINT64;
    m_value.ui64 = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const double  &val)
{
    reset();
    m_type = V_DOUBLE;
    m_value.d = val;

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const char *pVal)
{
    reset();
    m_type = V_STRING;
    m_value.s = new MString(pVal);

    return *this;
}

MAMFVariant & MAMFVariant::operator=(const MString &val)
{
    reset();
    m_type = V_STRING;
    m_value.s = new MString(val);

    return *this;
}

void MAMFVariant::reset()
{
    switch (m_type) {
    case V_STRING:
        delete m_value.s;
        break;
    default:
        break;
    }

    memset(&m_value, 0, sizeof(m_value));
}

bool MAMFVariant::toBool()
{
    if (m_type != V_BOOL) {
        throw "MAMFVariant::toBool failed, type shoud be BOOL.";
    }

    return m_value.b;
}

mint8  MAMFVariant::toInt8()
{
    if (m_type != V_INT8) {
        throw "MAMFVariant::toInt8 failed, type shoud be int8.";
    }

    return m_value.i8;
}

mint16 MAMFVariant::toInt16()
{
    if (m_type != V_INT16) {
        throw "MAMFVariant::toInt16 failed, type shoud be int16.";
    }

    return m_value.i16;
}

mint32 MAMFVariant::toInt32()
{
    if (m_type != V_INT32) {
        throw "MAMFVariant::toInt32 failed, type shoud be int32.";
    }

    return m_value.i32;
}

mint64 MAMFVariant::toInt64()
{
    if (m_type != V_INT64) {
        throw "MAMFVariant::toInt64 failed, type shoud be int64.";
    }

    return m_value.i64;
}

muint8  MAMFVariant::toUint8()
{
    if (m_type != V_UINT8) {
        throw "MAMFVariant::toUint8 failed, type shoud be uint8.";
    }

    return m_value.ui8;
}

muint16 MAMFVariant::toUint16()
{
    if (m_type != V_UINT16) {
        throw "MAMFVariant::toUint16 failed, type shoud be uint16.";
    }

    return m_value.ui16;
}

muint32 MAMFVariant::toUint32()
{
    if (m_type != V_UINT32) {
        throw "MAMFVariant::toUint32 failed, type shoud be uint32.";
    }

    return m_value.ui32;
}

muint64 MAMFVariant::toUint64()
{
    if (m_type != V_UINT64) {
        throw "MAMFVariant::toUint64 failed, type shoud be uint64.";
    }

    return m_value.ui64;
}

double  MAMFVariant::toDouble()
{
    if (m_type != V_INT8) {
        throw "MAMFVariant::toInt8 failed, type shoud be int8.";
    }

    return m_value.d;
}

MString MAMFVariant::toString()
{
    if (m_type != V_STRING) {
        throw "mvariant type error";
    }

    return *m_value.s;
}

MAMFVariant::operator bool()
{
    return toBool();
}

MAMFVariant::operator mint8()
{
    return toInt8();
}

MAMFVariant::operator mint16()
{
    return toInt16();
}

MAMFVariant::operator mint32()
{
    return toInt32();
}

MAMFVariant::operator mint64()
{
    return toInt64();
}

MAMFVariant::operator muint8()
{
    return toUint8();
}

MAMFVariant::operator muint16()
{
    return toUint16();
}

MAMFVariant::operator muint32()
{
    return toUint32();
}

MAMFVariant::operator muint64()
{
    return toUint64();
}

MAMFVariant::operator double()
{
    return toDouble();
}

MAMFVariant::operator MString()
{
    return toString();
}

//V_NULL      = VAR_ENUM_VALUE_NULL,
//V_UNDEFINED = VAR_ENUM_VALUE_UNDEFINED,
//V_BOOL      = VAR_ENUM_VALUE_BOOL,
//V_INT8      = VAR_ENUM_VALUE_INT8,
//V_INT16     = VAR_ENUM_VALUE_INT16,
//V_INT32     = VAR_ENUM_VALUE_INT32,
//V_INT64     = VAR_ENUM_VALUE_INT64,
//V_UINT8     = VAR_ENUM_VALUE_UINT8,
//V_UINT16    = VAR_ENUM_VALUE_UINT16,
//V_UINT32    = VAR_ENUM_VALUE_UINT32,
//V_UINT64    = VAR_ENUM_VALUE_UINT64,
//V_DOUBLE    = VAR_ENUM_VALUE_DOUBLE,
//_V_NUMERIC  = VAR_ENUM_VALUE_NUMERIC,
//V_TIMESTAMP = VAR_ENUM_VALUE_TIMESTAMP,
//V_DATE      = VAR_ENUM_VALUE_DATE,
//V_TIME      = VAR_ENUM_VALUE_TIME,
//V_STRING    = VAR_ENUM_VALUE_STRING,
//V_TYPED_MAP = VAR_ENUM_VALUE_TYPED_MAP,
//V_MAP       = VAR_ENUM_VALUE_MAP,
//V_BYTEARRAY = VAR_ENUM_VALUE_BYTEARRAY,
//V_OBJECT,
//V_UserType

ostream & operator<<(ostream &o, const MAMFVariant& a)
{
    MAMFVariant &b = const_cast<MAMFVariant&>(a);
    switch (b.m_type) {
    case MAMFVariant::V_STRING:
        o << *b.m_value.s;
        break;
    default:
        break;
    }

    return o;
}
