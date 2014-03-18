#ifndef VFMDBLOCKUTILS_H
#define VFMDBLOCKUTILS_H

#include "vfmdline.h"

bool isHorizontalRuleLine(const VfmdByteArray &line);
int numOfBlockquotePrefixBytes(const VfmdByteArray &line);

#endif // VFMDBLOCKUTILS_H
