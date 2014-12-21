
/*
 * Copyright (C) wenjie.zhao
 */

#include "BlsAMF0Serializer.hpp"

#include <MLoger>

#define Check_Marker(type) \
    int ret = E_SUCCESS; \
    mint8 marker; \
    if ((ret = buffer.read1Bytes(marker)) != E_SUCCESS) \
    { \
        return ret; \
    } \
    if (marker != type) { \
        ret = E_AMF_TYPE_ERROR; \
        return ret;\
    }

static int objectEOF(MStream &stream, bool &eof)
{
    int ret = E_SUCCESS;

    mint32 var;
    if ((ret = stream.read3Bytes(var)) != E_SUCCESS) {
        return ret;
    }

    eof = (var == AMF0_OBJECT_END);
    if (!eof) {
        stream.skip(-3);
    }

    return ret;
}

BlsAMF0Serializer::BlsAMF0Serializer()
{
}

BlsAMF0Serializer::~BlsAMF0Serializer()
{
}

int BlsAMF0Serializer::readShortString(MStream &buffer, MString &var)
{
    Check_Marker(AMF0_SHORT_STRING);

    return readShortString_UTF8(buffer, var);
}

int BlsAMF0Serializer::readShortString_UTF8(MStream &buffer, MString &var)
{
    int ret = E_SUCCESS;
    mint16 len;
    if((ret = buffer.read2Bytes(len)) != E_SUCCESS) {
        return ret;
    }

    if(buffer.left() < len) {
        return E_SPACE_NOT_ENOUGH;
    }

    return buffer.readString(len, var);
}

int BlsAMF0Serializer::writeShortString(MStream &buffer, const MString &var)
{
    buffer.write1Bytes(AMF0_SHORT_STRING);

    return writeShortString_UTF8(buffer, var);
}

int BlsAMF0Serializer::writeShortString_UTF8(MStream &buffer, const MString &var)
{
    buffer.write2Bytes((mint16)var.size());
    buffer.writeString(var);

    return E_SUCCESS;
}

int BlsAMF0Serializer::readDouble(MStream &buffer, double &var)
{
    Check_Marker(AMF0_NUMBER);

    return buffer.read8Bytes(var);
}

int BlsAMF0Serializer::writeDouble(MStream &buffer, double value)
{
    buffer.write1Bytes(AMF0_NUMBER);
    buffer.write8Bytes(value);

    return E_SUCCESS;
}

int BlsAMF0Serializer::readObject(MStream &buffer, BlsAMF0Object &var)
{
    Check_Marker(AMF0_OBJECT);

    while (!buffer.end()) {
        bool eof;
        if ((ret = objectEOF(buffer, eof)) != E_SUCCESS) {
            var.values.clear();
            return ret;
        }

        if (eof) break;

        MString key;
        if ((ret = readShortString_UTF8(buffer, key)) != E_SUCCESS) {
            var.values.clear();
            return ret;
        }

        BlsAMF0Any *any = NULL;
        if ((ret = read(buffer, &any)) != E_SUCCESS) {
            var.values.clear();
            return ret;
        }

        var.setValue(key, any);
    }

    return ret;
}

int BlsAMF0Serializer::writeObject(MStream &buffer, BlsAMF0Object &var)
{
    int ret = E_SUCCESS;
    buffer.write1Bytes(AMF0_OBJECT);

    int size = var.values.size();
    for (int i = 0; i < size; ++i) {
        const MString &key = var.key(i);
        BlsAMF0Any *any = var.value(i);

        if (!any) {
            return E_VALUE_IS_ZERO;
        }

        if ((ret = writeShortString_UTF8(buffer, key)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = write(buffer, any)) != E_SUCCESS) {
            return ret;
        }
    }
    buffer.write3Bytes(AMF0_OBJECT_END);

    return ret;
}

int BlsAMF0Serializer::readEcmaArray(MStream &buffer, BlsAMF0EcmaArray &var)
{
    Check_Marker(AMF0_ECMA_ARRAY);

    // read ecma count
    mint32 count;
    if ((ret = buffer.read4Bytes(count)) != E_SUCCESS) {
        return ret;
    }
    var.count = count;

    while (!buffer.end()) {
        bool eof;
        if ((ret = objectEOF(buffer, eof)) != E_SUCCESS) {
            var.values.clear();
            return ret;
        }

        if (eof) break;

        MString key;
        if ((ret = readShortString_UTF8(buffer, key)) != E_SUCCESS) {
            var.values.clear();
            return ret;
        }

        BlsAMF0Any *any = NULL;
        if ((ret = read(buffer, &any)) != E_SUCCESS) {
            var.values.clear();
            return ret;
        }

        var.setValue(key, any);
    }

    return ret;
}

int BlsAMF0Serializer::writeEcmaArray(MStream &buffer, BlsAMF0EcmaArray &var)
{
    int ret = E_SUCCESS;
    buffer.write1Bytes(AMF0_ECMA_ARRAY);
    buffer.write4Bytes(var.count);

    int size = var.values.size();
    for (int i = 0; i < size; ++i) {
        const MString &key = var.key(i);
        BlsAMF0Any *any = var.value(i);

        if (!any) {
            return E_VALUE_IS_ZERO;
        }

        if ((ret = writeShortString_UTF8(buffer, key)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = write(buffer, any)) != E_SUCCESS) {
            return ret;
        }
    }
    buffer.write3Bytes(AMF0_OBJECT_END);

    return ret;
}

int BlsAMF0Serializer::readStrictArray(MStream &buffer, BlsAMF0StrictArray &var)
{
    Check_Marker(AMF0_STRICT_ARRAY);

    // read strict count
    mint32 count;
    if ((ret = buffer.read4Bytes(count)) != E_SUCCESS) {
        return ret;
    }
    var.count = count;

    for (int i = 0; i < count && !buffer.end(); ++i) {
        BlsAMF0Any *any = NULL;
        if ((ret = read(buffer, &any)) != E_SUCCESS) {
            var.values.clear();
            return ret;
        }

        var.values.push_back(any);
    }

    return ret;
}

int BlsAMF0Serializer::writeStrictArray(MStream &buffer, BlsAMF0StrictArray &var)
{
    int ret = E_SUCCESS;
    buffer.write1Bytes(AMF0_STRICT_ARRAY);
    buffer.write4Bytes(var.count);

    vector<BlsAMF0Any *> &values = var.values;
    for (int i = 0; i < values.size(); ++i) {
        BlsAMF0Any* any = values.at(i);
        if (!any) {
            return E_VALUE_IS_ZERO;
        }

        if ((ret = write(buffer, any)) != E_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

/*
int MAMF0Serializer::readAMF3Object(MStream &buffer, MAMFVariant &variant)
{
    return E_TYPE_NOT_SUPPORTED;
}

int MAMF0Serializer::writeAMF3Object(MStream &buffer, MAMFVariant &variant)
{
    return E_TYPE_NOT_SUPPORTED;
}
*/
int BlsAMF0Serializer::readBoolean(MStream &buffer, bool &var)
{
    Check_Marker(AMF0_BOOLEAN);

    mint8 bl;
    if ((ret = buffer.read1Bytes(bl)) != E_SUCCESS) {
        return ret;
    }
    var = bl;

    return ret;
}

int BlsAMF0Serializer::writeBoolean(MStream &buffer, bool value)
{
    buffer.write1Bytes(AMF0_BOOLEAN);
    mint8 v = value ? 0x01 : 0x00;
    buffer.write1Bytes(v);

    return E_SUCCESS;
}

int BlsAMF0Serializer::readNull(MStream &buffer)
{
    M_UNUSED(buffer);
    Check_Marker(AMF0_NULL);
    return ret;
}

int BlsAMF0Serializer::writeNull(MStream &buffer)
{
    mint8 v = AMF0_NULL;
    buffer.write1Bytes(v);

    return E_SUCCESS;
}

int BlsAMF0Serializer::readUndefined(MStream &buffer)
{
    M_UNUSED(buffer);
    Check_Marker(AMF0_UNDEFINED);
    return ret;
}

int BlsAMF0Serializer::writeUndefined(MStream &buffer)
{
    mint8 v = AMF0_UNDEFINED;
    buffer.write1Bytes(v);

    return E_SUCCESS;
}

int BlsAMF0Serializer::read(MStream &buffer, BlsAMF0Any **var)
{
    int ret = E_SUCCESS;
    mint8 marker;
    if ((ret = buffer.read1Bytes(marker)) != E_SUCCESS) {
        return ret;
    }

    buffer.skip(-1);

    switch (marker) {
    case AMF0_NUMBER:
    {
        BlsAMF0Number *num = new BlsAMF0Number;
        *var = num;
        return BlsAMF0Serializer::readDouble(buffer, num->var);
    }
    case AMF0_BOOLEAN:
    {
        BlsAMF0Boolean *bl = new BlsAMF0Boolean;
        *var = bl;
        return BlsAMF0Serializer::readBoolean(buffer, bl->var);
    }
    case AMF0_SHORT_STRING:
    {
        BlsAMF0ShortString *sstr = new BlsAMF0ShortString;
        *var = sstr;
        return BlsAMF0Serializer::readShortString(buffer, sstr->var);
    }
    case AMF0_OBJECT:
    {
        BlsAMF0Object *amf0_obj = new BlsAMF0Object;
        *var = amf0_obj;
        return BlsAMF0Serializer::readObject(buffer, *amf0_obj);
    }
    case AMF0_NULL:
    {
        BlsAMF0Null *nill = new BlsAMF0Null;
        *var = nill;
        return BlsAMF0Serializer::readNull(buffer);
    }
    case AMF0_UNDEFINED:
    {
        BlsAMF0Undefined *und = new BlsAMF0Undefined;
        *var = und;
        return BlsAMF0Serializer::readUndefined(buffer);
    }
    case AMF0_ECMA_ARRAY:
    {
        BlsAMF0EcmaArray *ecma_array = new BlsAMF0EcmaArray;
        *var = ecma_array;
        return BlsAMF0Serializer::readEcmaArray(buffer, *ecma_array);
    }
    case AMF0_STRICT_ARRAY:
    {
        BlsAMF0StrictArray *strict_array = new BlsAMF0StrictArray;
        *var = strict_array;
        return BlsAMF0Serializer::readStrictArray(buffer, *strict_array);
    }
    case AMF0_AMF3_OBJECT:
    {
        // TODO
        return E_TYPE_NOT_SUPPORTED;
    }
    default:
        return E_TYPE_NOT_SUPPORTED;
    }

    return ret;
}

int BlsAMF0Serializer::write(MStream &buffer, BlsAMF0Any *any)
{
    int ret = E_SUCCESS;
    mint8 marker = any->type;

    switch (marker) {
    case AMF0_NUMBER:
    {
        BlsAMF0Number *number = dynamic_cast<BlsAMF0Number *>(any);
        mAssert(number);
        return BlsAMF0Serializer::writeDouble(buffer, number->var);
    }
    case AMF0_BOOLEAN:
    {
        BlsAMF0Boolean *bl = dynamic_cast<BlsAMF0Boolean *>(any);
        mAssert(bl);
        return BlsAMF0Serializer::writeBoolean(buffer, bl->var);
    }
    case AMF0_SHORT_STRING:
    {
        BlsAMF0ShortString *sstr = dynamic_cast<BlsAMF0ShortString *>(any);
        mAssert(sstr);
        return BlsAMF0Serializer::writeShortString(buffer, sstr->var);
    }
    case AMF0_OBJECT:
    {
        BlsAMF0Object *amf0_obj = dynamic_cast<BlsAMF0Object *>(any);
        mAssert(amf0_obj);
        return BlsAMF0Serializer::writeObject(buffer, *amf0_obj);
    }
    case AMF0_NULL:
    {
        BlsAMF0Null *nill = dynamic_cast<BlsAMF0Null *>(any);
        mAssert(nill);
        return BlsAMF0Serializer::writeNull(buffer);
    }
    case AMF0_UNDEFINED:
    {
        BlsAMF0Undefined *und = dynamic_cast<BlsAMF0Undefined *>(any);
        mAssert(und);
        return BlsAMF0Serializer::writeUndefined(buffer);
    }
    case AMF0_ECMA_ARRAY:
    {
        BlsAMF0EcmaArray *ecma_array = dynamic_cast<BlsAMF0EcmaArray *>(any);
        mAssert(ecma_array);
        return BlsAMF0Serializer::writeEcmaArray(buffer, *ecma_array);
    }
    case AMF0_STRICT_ARRAY:
    {
        BlsAMF0StrictArray *strict_array = dynamic_cast<BlsAMF0StrictArray *>(any);
        mAssert(strict_array);
        return BlsAMF0Serializer::writeStrictArray(buffer, *strict_array);
    }
    case AMF0_AMF3_OBJECT:
    {
        // TODO
        return E_TYPE_NOT_SUPPORTED;
    }
    default:
        return E_TYPE_NOT_SUPPORTED;
    }

    return ret;
}
