// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the MATHISFUNDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// MATHISFUNDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MATHISFUNDLL_EXPORTS
#define MATHISFUNDLL_API __declspec(dllexport)
#else
#define MATHISFUNDLL_API __declspec(dllimport)
#endif

//By default C++ is performing name mangling of types.
//This means that if we have a function named "Sum" the compiler may internally generate a different name for it eg __Sum_ or something different
//By including the following ifdef we're telling the compiler "In case you're compiling C++ language, make sure the method names have C linkage (ie don't change their names)
//For more information: https://stackoverflow.com/questions/1041866/what-is-the-effect-of-extern-c-in-c
#ifdef __cplusplus		
extern "C"
{
#endif

MATHISFUNDLL_API float Sum(float a, float b);
MATHISFUNDLL_API int GetFibonacciNTerm(int Term);
#ifdef __cplusplus
}
#endif