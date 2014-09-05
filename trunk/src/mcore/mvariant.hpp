#ifndef MVARIANT_H
#define MVARIANT_H

#include "mstring.hpp"
#include "mhash.hpp"

class MVariant
{
public:
    enum V_Type {
        V_Invalid     = 0,

        V_Bool        = 1,
        V_Int         = 2,
        V_UInt        = 3,
        V_LongLong    = 4,
        V_ULongLong   = 5,
        V_Double      = 6,
        V_Char        = 7,
        V_Map         = 8,
        V_List        = 9,
        V_String      = 10,
        V_StringList  = 11,

        V_UserType    = 127,

        V_LastType    = 0xffffffff // need this so that gcc >= 3.4 allocates 32 bits for Type
    };

    MVariant();
    ~MVariant();

    void reset();

    MVariant(bool val);
    MVariant(int val);

    MVariant(const char *val, muint32 size = 0);
    MVariant(const MString &val);

    template <class T>
    MVariant(const T &value)
        : m_type(V_UserType)
    {
        m_value.v_userType = (void*)new T(value);
    }

    bool toBool();
    MString toString();

    template <class T>
    T value() {
        T *v = reinterpret_cast<T*>(m_value.v_userType);

        return *v;
    }

    operator string();

private:
    union {
        bool v_bool;
        int  v_int;
        MString *v_string;

        void *v_userType;
    } m_value;

    enum V_Type m_type;
};

#endif // MVARIANT_H
