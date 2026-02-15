// VST3 SDK compatibility stubs
// When VST3 SDK is present, real headers will be used instead

#ifndef VST3SDK_STUB_H
#define VST3SDK_STUB_H

#include <cstdint>

// Basic types
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef float float32;
typedef double float64;
typedef wchar_t char16;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef int TBool;

#ifndef nullptr
#define nullptr NULL
#endif

// Result codes
enum tresult {
    kResultOk = 1,
    kResultFalse = 0,
    kResultNotImplemented = -1,
    kResultFailed = -2,
    kResultInvalidArgument = -3,
    kResultNotSupported = -4,
    kResultCancel = -5
};

// GUID/FUID
struct TUID {
    uint8 data[16];
};

// Plugin API - empty definition (actual definition comes from SDK)
#ifndef PLUGIN_API
#define PLUGIN_API
#endif

// Forward declarations
struct FUnknown;

#endif // VST3SDK_STUB_H
