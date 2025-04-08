#pragma once
#if DEBUG
/**
 * @brief Halts application's execution if expression is false and prints message.
 * @param __EXPRESSION__ expression to be evaluated.
 * @param __MESSAGE__ Message to be printed on fail
 */
#define CHECK_MESSAGE(__EXPRESSION__,__MESSAGE__)\
if(!(!!(__EXPRESSION__)))\
{\
    fprintf(stderr, "%s %d: Assertion failed on expression: %s: %s", __FILE__, __LINE__, #__EXPRESSION__,  __MESSAGE__);\
    abort();\
}
#else
/**
 * @brief Check is disabled in non debug builds.
 * @param __EXPRESSION__ Not used.
 * @param __MESSAGE__ Not used.
 */
#define CHECK_MESSAGE(__EXPRESSION__, __MESSAGE__)
#endif

#if DEBUG
/**
 * @brief Halts application's execution if expression is false.
 * @param __EXPRESSION__ expression to be evaluated.
 */
#define CHECK(__EXPRESSION__)\
if(!(!!(__EXPRESSION__)))\
{\
    fprintf(stderr, "%s %d: Assertion failed on expression: %s", __FILE__, __LINE__, #__EXPRESSION__);\
    abort();\
}
#else
/**
 * @brief Check is disabled in non debug builds.
 * @param __EXPRESSION__ Not used.
 */
#define CHECK(__EXPRESSION__)
#endif
