#ifndef BLSUTILS_HPP
#define BLSUTILS_HPP

#include <MString>

#define Process_Role_Child      0x01
#define Process_Role_Master     0x02

// eg. key`Internal_CMD_Delimer`value
#define Internal_CMD_Delimer                    ":::"

#define Internal_CMD_PID                        "PID"
#define Internal_CMD_InternalPort               "InternalPort"
#define Internal_CMD_WhoHasBackSource           "WhoHasBackSource"
#define Internal_CMD_WhoHasBackSourceRes        "WhoHasBackSourceRes"
#define Internal_CMD_IHasBackSourced            "IHasBackSourced"
#define Internal_CMD_RemoveHasBackSourceRes     "RemoveHasBackSourceRes"

MString getValue(MString &line);
MString getKey(MString &line);

#endif // BLSUTILS_HPP
