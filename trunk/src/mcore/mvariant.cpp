#include "mvariant.hpp"

#include <string.h>

MVariant::MVariant()
    : m_type(V_Invalid)
{
}

MVariant::~MVariant()
{
    switch (m_type) {
    case V_String:
        delete m_value.v_string;
        break;
    default:
        break;
    }
}

void MVariant::reset()
{
    switch (m_type) {
    case V_String:
        delete m_value.v_string;
        break;
    default:
        break;
    }

    memset(&m_value, 0, sizeof (m_value));
}

MVariant::MVariant(bool val)
    : m_type(V_Bool)
{
    m_value.v_bool = val;
}

MVariant::MVariant(const char *val, muint32 size)
    : m_type(V_String)
{
   m_value.v_string = new MString(val, size ? size : strlen(val));
}

MVariant::MVariant(const MString &val)
    : m_type(V_String)
{
    m_value.v_string = new MString(val);
}

bool MVariant::toBool()
{
    if (m_type != V_Bool) {
        throw "mvariant type error";
    }

    return m_value.v_bool;
}

MString MVariant::toString()
{
    if (m_type != V_String) {
        throw "mvariant type error";
    }

    MString *ret = m_value.v_string;
    return *ret;
}

MVariant::operator string()
{
    return toString();
}
