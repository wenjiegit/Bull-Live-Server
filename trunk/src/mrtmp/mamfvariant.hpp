#ifndef MAMFVariant_H
#define MAMFVariant_H

#include <MString>
#include <MHash>

#include "mamftypes.hpp"

#define VAR_ENUM_VALUE_BOOL         3
#define VAR_ENUM_VALUE_BYTEARRAY    20
#define VAR_ENUM_VALUE_DATE         15
#define VAR_ENUM_VALUE_DOUBLE       12
#define VAR_ENUM_VALUE_INT16        5
#define VAR_ENUM_VALUE_INT32        6
#define VAR_ENUM_VALUE_INT64        7
#define VAR_ENUM_VALUE_INT8         4
#define VAR_ENUM_VALUE_MAP          19
#define VAR_ENUM_VALUE_NULL         1
#define VAR_ENUM_VALUE_NUMERIC      13
#define VAR_ENUM_VALUE_STRING       17
#define VAR_ENUM_VALUE_TIME         16
#define VAR_ENUM_VALUE_TIMESTAMP    14
#define VAR_ENUM_VALUE_TYPED_MAP    18
#define VAR_ENUM_VALUE_UINT16       9
#define VAR_ENUM_VALUE_UINT32       10
#define VAR_ENUM_VALUE_UINT64       11
#define VAR_ENUM_VALUE_UINT8        8
#define VAR_ENUM_VALUE_UNDEFINED    2

class MAMFVariant
{
public:
    enum Type {
        Invalid     = 0,

        V_NULL      = VAR_ENUM_VALUE_NULL,
        V_UNDEFINED = VAR_ENUM_VALUE_UNDEFINED,
        V_BOOL      = VAR_ENUM_VALUE_BOOL,
        V_INT8      = VAR_ENUM_VALUE_INT8,
        V_INT16     = VAR_ENUM_VALUE_INT16,
        V_INT32     = VAR_ENUM_VALUE_INT32,
        V_INT64     = VAR_ENUM_VALUE_INT64,
        V_UINT8     = VAR_ENUM_VALUE_UINT8,
        V_UINT16    = VAR_ENUM_VALUE_UINT16,
        V_UINT32    = VAR_ENUM_VALUE_UINT32,
        V_UINT64    = VAR_ENUM_VALUE_UINT64,
        V_DOUBLE    = VAR_ENUM_VALUE_DOUBLE,
        V_NUMERIC   = VAR_ENUM_VALUE_NUMERIC,
        V_TIMESTAMP = VAR_ENUM_VALUE_TIMESTAMP,
        V_DATE      = VAR_ENUM_VALUE_DATE,
        V_TIME      = VAR_ENUM_VALUE_TIME,
        V_STRING    = VAR_ENUM_VALUE_STRING,
        V_TYPED_MAP = VAR_ENUM_VALUE_TYPED_MAP,
        V_MAP       = VAR_ENUM_VALUE_MAP,
        V_BYTEARRAY = VAR_ENUM_VALUE_BYTEARRAY,
        V_OBJECT,
        V_UserType
    };

    MAMFVariant();
    ~MAMFVariant();

    MAMFVariant(const bool    &val);
    MAMFVariant(const mint8   &val);
    MAMFVariant(const mint16  &val);
    MAMFVariant(const mint32  &val);
    MAMFVariant(const mint64  &val);
    MAMFVariant(const muint8  &val);
    MAMFVariant(const muint16 &val);
    MAMFVariant(const muint32 &val);
    MAMFVariant(const muint64 &val);
    MAMFVariant(const double  &val);

    MAMFVariant(const char *pValue);
    explicit MAMFVariant(const MString &value);
    MAMFVariant(const char *pValue, muint32 len);

    MAMFVariant(const MAMFObject  &val);

    MAMFVariant & operator=(const MAMFVariant &val);
    MAMFVariant & operator=(const bool    &val);
    MAMFVariant & operator=(const mint8   &val);
    MAMFVariant & operator=(const mint16  &val);
    MAMFVariant & operator=(const mint32  &val);
    MAMFVariant & operator=(const mint64  &val);
    MAMFVariant & operator=(const muint8  &val);
    MAMFVariant & operator=(const muint16 &val);
    MAMFVariant & operator=(const muint32 &val);
    MAMFVariant & operator=(const muint64 &val);
    MAMFVariant & operator=(const double  &val);
    MAMFVariant & operator=(const char *pVal);
    MAMFVariant & operator=(const MString &val);

    // TODO : operator= object

    void reset();

    template <class T>
    MAMFVariant(const T &value)
        : m_type(V_UserType)
    {
        m_value.userType = (void*)new T(value);
    }

    template <class T>
    T value() {
        T *v = reinterpret_cast<T*>(m_value.userType);

        return *v;
    }

    bool   toBool();
    mint8  toInt8();
    mint16 toInt16();
    mint32 toInt32();
    mint64 toInt64();

    muint8  toUint8();
    muint16 toUint16();
    muint32 toUint32();
    muint64 toUint64();
    double  toDouble();

    MString toString();

    // TODO : toObject object

    operator bool();

    operator mint8();
    operator mint16();
    operator mint32();
    operator mint64();

    operator muint8();
    operator muint16();
    operator muint32();
    operator muint64();

    operator double();

    operator MString();

    // TODO : operator object

private:
    union {
        bool    b;
        mint8   i8;
        mint16  i16;
        mint32  i32;
        mint64  i64;
        muint8  ui8;
        muint16 ui16;
        muint32 ui32;
        muint64 ui64;
        double  d;
        MString *s;
        MAMFObject *obj;
        //VariantMap *m;

        void *userType;
    } m_value;

    enum Type m_type;

public:
    friend ostream & operator<<(ostream &o, const MAMFVariant& a);
};

#endif // MAMFVariant_H
