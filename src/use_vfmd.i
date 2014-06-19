/*
 * SWIG Interface file for using vfmd (for parsing the core vfmd syntax)
 * from other-language code
 */


/* Wherever applicable, allow a scripting-language string
   to be passed to a vfmd API method. */

%apply (char *STRING, int LENGTH) { (const char *stringData, int stringLength) };
%ignore VfmdByteArray::VfmdByteArray(const char *str);
%ignore VfmdOutputDevice::write(const char *stringData);

/* For converting a VfmdByteArray to a string in the
   scripting language. */

%include "cstring.i"
%cstring_output_allocate_size(char **swigStr, int *swigStrLen, free(*$1));
%extend VfmdByteArray {
    void toString(char **swigStr, int *swigStrLen) {
        unsigned int len = $self->size();
        char *s = (char *) malloc(len);
        memcpy(s, $self->data(), len);
        (*swigStr) = s;
        (*swigStrLen) = (int) len;
    }
}

/* When objects created in script-land are passed as the
   following arguments to C++-land, C++ code takes ownership
   of the objects. Script-language should not attempt to free
   these objects. */

%apply SWIGTYPE *DISOWN { VfmdOutputDevice *outputDeviceD } /* VfmdRenderer */

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
