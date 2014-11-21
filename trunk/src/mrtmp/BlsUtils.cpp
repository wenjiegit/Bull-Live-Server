#include "BlsUtils.hpp"

#include <MStringList>

MString getValue(MString &line)
{
    MStringList temp = line.split(Internal_CMD_Delimer);
    if (line.endWith("\n")) {
        line.erase(line.size()-1, 1);
    }

    MString ret;
    if (temp.size() == 2) {
        ret = temp.at(1);
    }

    return ret;
}

MString getKey(MString &line)
{
    MStringList temp = line.split(Internal_CMD_Delimer);

    MString ret;
    if (temp.size() == 2) {
        ret = temp.at(0);
    }

    return "";
}
