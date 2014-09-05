
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MRTMP_HPP
#define MRTMP_HPP

#include <MObject>
#include <MHash>

#include "mamfvariant.hpp"

class MRtmpProtocol;
class MTcpSocket;
class MRtmp;

typedef int (MObject::*RtmpFunction)(const MAMFVariant &v1
        , const MAMFVariant &v2
        , const MAMFVariant &v3
        , const MAMFVariant &v4
        , const MAMFVariant &v5
        , const MAMFVariant &v6);

class MRtmp : public MObject
{
public:
    MRtmp(MTcpSocket *socket, MObject *parent = 0);

    int invoke(const MString & method, const MAMFVariant &v1 = MAMFVariant()
            , const MAMFVariant &v2 = MAMFVariant()
            , const MAMFVariant &v3 = MAMFVariant()
            , const MAMFVariant &v4 = MAMFVariant()
            , const MAMFVariant &v5 = MAMFVariant()
            , const MAMFVariant &v6 = MAMFVariant());

private:
    int defaultMethod(const MAMFVariant &v1 = MAMFVariant()
            , const MAMFVariant &v2 = MAMFVariant()
            , const MAMFVariant &v3 = MAMFVariant()
            , const MAMFVariant &v4 = MAMFVariant()
            , const MAMFVariant &v5 = MAMFVariant()
            , const MAMFVariant &v6 = MAMFVariant());

    int connectApp(const MAMFVariant &v1 = MAMFVariant()
                   , const MAMFVariant &v2 = MAMFVariant()
                   , const MAMFVariant &v3 = MAMFVariant()
                   , const MAMFVariant &v4 = MAMFVariant()
                   , const MAMFVariant &v5 = MAMFVariant()
                   , const MAMFVariant &v6 = MAMFVariant());

private:
    MRtmpProtocol *m_protocol;

    MHash<MString, RtmpFunction> m_methods;
};

#endif // MRTMP_HPP
