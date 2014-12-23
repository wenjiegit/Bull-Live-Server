
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef CMGLOBAL_HPP
#define CMGLOBAL_HPP

#include "merrno.hpp"

#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#include <st.h>

using namespace MERROR;

#define _err_exit(val, fmt, ...) \
    printf(fmt, ##__VA_ARGS__); \
    _exit(val)

#define _err_return(val, fmt, ...) \
    printf(fmt, ##__VA_ARGS__); \
    return val

#define M_UNUSED(name) (void)name;

// types define
typedef unsigned char muint8;
typedef char mint8;
typedef unsigned short muint16;
typedef short mint16;
typedef unsigned int muint32;
typedef int mint32;
typedef long long int mint64;
typedef unsigned long long int muint64;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;

#define EVENT static
#define DECLARE_FRIEND_CLASS(CLASSNAME) friend class CLASSNAME;
#define TIMER_EVENT(func) this, &MObject::func
#define CB_EVENT(func) &MObject::func

template<class T>
class _MAutoFree
{
private:
    T **ptr;
    bool is_array;
public:
    /**
    * auto delete the ptr.
    * @is_array a bool value indicates whether the ptr is a array.
    */
    _MAutoFree(T **_ptr, bool _is_array) {
        ptr = _ptr;
        is_array = _is_array;
    }

    ~_MAutoFree() {
        if (ptr == NULL || *ptr == NULL) {
            return;
        }

        if (is_array) {
            delete[] *ptr;
        } else {
            delete *ptr;
        }

        *ptr = NULL;
    }
};

#define mFree(p) \
    if (p) { \
        delete p; \
        p = NULL; \
    } \
    (void)0
// free the p which represents a array
#define mFreeArray(p) \
    if (p) { \
        delete[] p; \
        p = NULL; \
    } \
    (void)0

// autofree suit
#define mAutoFree(className, instance) \
    _MAutoFree<className> _MAutoFree##instance(&instance, false)

#define mAutoFreeArray(className, instance) \
    _MAutoFree<className> _MAutoFree##instance(&instance, true)

#define mAssert(expression) assert(expression)

#define mSleep(seconds) \
    st_sleep(seconds)

#define mUSleep(us) \
    st_usleep(us)

#define mMSleep(ms) \
    st_usleep(ms * 1000)

/*!
    auto sleep by deconstruct
*/
class _AutoSleep
{
public:
    _AutoSleep(int ms)
    {
        m_ms = ms;
    }

    ~_AutoSleep()
    {
        if (m_ms > 0)
            mMSleep(m_ms);
    }

private:
    int m_ms;
};

#define mAutoSleep(ms) \
    _AutoSleep _AutoSleep##ms(ms)

// for string
#define STR_STR(x) (((string)(x)).c_str())

#define STR_DATA(x) (const_cast<char*>(x.data()))

// for map
#define FOR_MAP(m,k,v,i) for(map< k , v >::iterator i=(m).begin();i!=(m).end();i++)

#define MAP_HAS1(m,k) ((bool)((m).find((k))!=(m).end()))

#define MAP_HAS2(m,k1,k2) ((MAP_HAS1((m),(k1))==true)?MAP_HAS1((m)[(k1)],(k2)):false)

#define MAP_HAS3(m,k1,k2,k3) ((MAP_HAS1((m),(k1)))?MAP_HAS2((m)[(k1)],(k2),(k3)):false)

#define MAP_KEY(i) ((i)->first)

#define MAP_VAL(i) ((i)->second)

#define MAP_ERASE1(m,k) if(MAP_HAS1((m),(k))) (m).erase((k));

#define MAP_ERASE2(m,k1,k2) \
if(MAP_HAS1((m),(k1))){ \
    MAP_ERASE1((m)[(k1)],(k2)); \
    if((m)[(k1)].size()==0) \
        MAP_ERASE1((m),(k1)); \
}

#define MAP_ERASE3(m,k1,k2,k3) \
if(MAP_HAS1((m),(k1))){ \
    MAP_ERASE2((m)[(k1)],(k2),(k3)); \
    if((m)[(k1)].size()==0) \
        MAP_ERASE1((m),(k1)); \
}

// for vector
#define FOR_VECTOR(v,i)                 for(uint32_t i=0;i<(v).size();i++)

#define FOR_VECTOR_ITERATOR(e,v,i)      for(vector<e>::iterator i=(v).begin();i!=(v).end();i++)

#define FOR_VECTOR_WITH_START(v,i,s)    for(uint32_t i=s;i<(v).size();i++)

#define ADD_VECTOR_END(v,i)             (v).push_back((i))

#define ADD_VECTOR_BEGIN(v,i)           (v).insert((v).begin(),(i))

#define VECTOR_VAL(i)                   (*(i))

#define mMin(a, b) (((a) < (b))? (a) : (b))
#define mMax(a, b) (((a) < (b))? (b) : (a))

#endif // CMGLOBAL_HPP
