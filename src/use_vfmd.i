/*
 * SWIG Interface file for using vfmd (for parsing the core vfmd syntax)
 * from other-language code
 */


/* Interface for using vfmd */

%{
#include "vfmdbytearray.h"
#include "vfmddocument.h"
#include "vfmdoutputdevice.h"
#include "renderers/htmlrenderer.h"
#include "renderers/treerenderer.h"
%}

%include "vfmdbytearray.h"
%include "vfmddocument.h"
%include "vfmdoutputdevice.h"
%include "renderers/vfmdrenderer.h"
%include "renderers/htmlrenderer.h"
%include "renderers/treerenderer.h"
