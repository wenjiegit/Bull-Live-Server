#define MAMF0SERIALIZER_HPP
#ifdef MAMF0SERIALIZER_HPP

#include <MStream>

#include "mamfvariant.hpp"
#include "mamftypes.hpp"

class MAMF0Serializer
{
public:
    MAMF0Serializer();
    ~MAMF0Serializer();

    static int readShortString(MStream &buffer, MString &var);
    static int readShortString_UTF8(MStream &buffer, MString &var);
    static int writeShortString(MStream &buffer, const MString &var);
    static int writeShortString_UTF8(MStream &buffer, const MString &var);

    static int readDouble(MStream &buffer, double &var);
    static int writeDouble(MStream &buffer, double value);

    static int readObject(MStream &buffer, MAMF0Object &var);
    static int writeObject(MStream &buffer, MAMF0Object &var);

    static int readEcmaArray(MStream &buffer, MAMF0EcmaArray &var);
    static int writeEcmaArray(MStream &buffer, MAMF0EcmaArray &var);

    static int readStrictArray(MStream &buffer, MAMF0StrictArray &var);
    static int writeStrictArray(MStream &buffer, MAMF0StrictArray &var);

    static int readAMF3Object(MStream &buffer, MAMFVariant &variant);
    static int writeAMF3Object(MStream &buffer, MAMFVariant &variant);

    static int readBoolean(MStream &buffer, bool &var);
    static int writeBoolean(MStream &buffer, bool value);

    static int readNull(MStream &buffer);
    static int writeNull(MStream &buffer);

    static int readUndefined(MStream &buffer);
    static int writeUndefined(MStream &buffer);

    static int read(MStream &buffer, MAMF0Any **var);
    static int write(MStream &buffer, MAMF0Any *any);
};

#endif	// MAMF0SERIALIZER_HPP
