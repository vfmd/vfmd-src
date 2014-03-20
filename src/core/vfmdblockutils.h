#ifndef VFMDBLOCKUTILS_H
#define VFMDBLOCKUTILS_H

#include "vfmdline.h"

bool isHorizontalRuleLine(const VfmdLine *line);
int numOfBlockquotePrefixBytes(const VfmdLine *line);

#endif // VFMDBLOCKUTILS_H
