#pragma once
#define CHECK_MESSAGE(...) CHECK_M(__VA_ARGS__)
#define  CHECK_M(__EXPRESSION__, __MESSAGE__)\
if(!!(!(__EXPRESSION__)))\
{\
    fprintf(stderr, "%s %d: Assertion failed on expression: %s: %s", __FILE__, __LINE__, #__EXPRESSION__,  __MESSAGE__);\
    abort();\
}
#define CHECK(__EXPRESSION__)\
if(!!(!(__EXPRESSION__)))\
{\
    fprintf(stderr, "%s %d: Assertion failed on expression: %s", __FILE__, __LINE__, #__EXPRESSION__);\
    abort();\
}
