#pragma once
#if DEBUG
#define CHECK_MESSAGE(__EXPRESSION__,__MESSAGE__)\
if(!!(!(__EXPRESSION__)))\
{\
    fprintf(stderr, "%s %d: Assertion failed on expression: %s: %s", __FILE__, __LINE__, #__EXPRESSION__,  __MESSAGE__);\
    abort();\
}
#else
#define CHECK_MESSAGE(__EXPRESSION__, __MESSAGE__)
#endif

#if DEBUG
#define CHECK(__EXPRESSION__)\
if(!!(!(__EXPRESSION__)))\
{\
    fprintf(stderr, "%s %d: Assertion failed on expression: %s", __FILE__, __LINE__, #__EXPRESSION__);\
    abort();\
}
#else
#define CHECK(__EXPRESSION__)
#endif
