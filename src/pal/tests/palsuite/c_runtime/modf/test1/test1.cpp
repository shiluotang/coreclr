// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

/*=============================================================================
**
** Source: test1.c (modf)
**
** Purpose: Test to ensure that modf return the correct values
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
**               fabs
**
**===========================================================================*/

#include <palsuite.h>

// binary64 (double) has a machine epsilon of 2^-52 (approx. 2.22e-16). However, this 
// is slightly too accurate when writing tests meant to run against libm implementations
// for various platforms. 2^-50 (approx. 8.88e-16) seems to be as accurate as we can get.
//
// The tests themselves will take PAL_EPSILON and adjust it according to the expected result
// so that the delta used for comparison will compare the most significant digits and ignore
// any digits that are outside the double precision range (15-17 digits).

// For example, a test with an expect result in the format of 0.xxxxxxxxxxxxxxxxx will use
// PAL_EPSILON for the variance, while an expected result in the format of 0.0xxxxxxxxxxxxxxxxx
// will use PAL_EPSILON / 10 and and expected result in the format of x.xxxxxxxxxxxxxxxx will
// use PAL_EPSILON * 10.
#define PAL_EPSILON 8.8817841970012523e-16

#define PAL_NAN     sqrt(-1.0)
#define PAL_POSINF -log(0.0)
#define PAL_NEGINF  log(0.0)

/**
 * Helper test structure
 */
struct test
{
    double value;             /* value to test the function with */
    double expected;          /* expected result */   
    double variance;          /* maximum delta between the expected and actual result */
    double expected_intpart;  /* expected result */
    double variance_intpart;  /* maximum delta between the expected and actual result */
};

/**
 * validate
 *
 * test validation function
 */
void __cdecl validate(double value, double expected, double variance, double expected_intpart, double variance_intpart)
{
    double result_intpart;
    double result = modf(value, &result_intpart);

    /*
     * The test is valid when the difference between result
     * and expected is less than or equal to variance
     */
    double delta = fabs(result - expected);
    double delta_intpart = fabs(result_intpart - expected_intpart);

    if ((delta > variance) || (delta_intpart > variance_intpart))
    {
        Fail("modf(%g) returned %20.17g with an intpart of %20.17g when it should have returned %20.17g with an intpart of %20.17g",
             value, result, result_intpart, expected, expected_intpart);
    }
}

/**
 * validate
 *
 * test validation function for values returning NaN
 */
void __cdecl validate_isnan(double value)
{
    double result_intpart;
    double result = modf(value, &result_intpart);

    if (!_isnan(result) || !_isnan(result_intpart))
    {
        Fail("modf(%g) returned %20.17g with an intpart of %20.17g when it should have returned %20.17g with an intpart of %20.17g",
             value, result, result_intpart, PAL_NAN, PAL_NAN);
    }
}

/**
 * main
 * 
 * executable entry point
 */
int __cdecl main(int argc, char **argv)
{
    struct test tests[] = 
    {
        /* value                   expected                variance         expected_intpart     variance_intpart */
        {  0,                      0,                      PAL_EPSILON,         0,                   PAL_EPSILON },
        {  0.31830988618379067,    0.31830988618379067,    PAL_EPSILON,         0,                   PAL_EPSILON },       // value:  1 / pi
        {  0.43429448190325183,    0.43429448190325183,    PAL_EPSILON,         0,                   PAL_EPSILON },       // value:  log10(e)
        {  0.63661977236758134,    0.63661977236758134,    PAL_EPSILON,         0,                   PAL_EPSILON },       // value:  2 / pi
        {  0.69314718055994531,    0.69314718055994531,    PAL_EPSILON,         0,                   PAL_EPSILON },       // value:  ln(2)
        {  0.70710678118654752,    0.70710678118654752,    PAL_EPSILON,         0,                   PAL_EPSILON },       // value:  1 / sqrt(2)
        {  0.78539816339744831,    0.78539816339744831,    PAL_EPSILON,         0,                   PAL_EPSILON },       // value:  pi / 4
        {  1,                      0,                      PAL_EPSILON,         1,                   PAL_EPSILON * 10 },
        {  1.1283791670955126,     0.1283791670955126,     PAL_EPSILON,         1,                   PAL_EPSILON * 10 },  // value:  2 / sqrt(pi)
        {  1.4142135623730950,     0.4142135623730950,     PAL_EPSILON,         1,                   PAL_EPSILON * 10 },  // value:  sqrt(2)
        {  1.4426950408889634,     0.4426950408889634,     PAL_EPSILON,         1,                   PAL_EPSILON * 10 },  // value:  log2(e)
        {  1.5707963267948966,     0.5707963267948966,     PAL_EPSILON,         1,                   PAL_EPSILON * 10 },  // value:  pi / 2
        {  2.3025850929940457,     0.3025850929940457,     PAL_EPSILON,         2,                   PAL_EPSILON * 10 },  // value:  ln(10)
        {  2.7182818284590452,     0.7182818284590452,     PAL_EPSILON,         2,                   PAL_EPSILON * 10 },  // value:  e
        {  3.1415926535897932,     0.1415926535897932,     PAL_EPSILON,         3,                   PAL_EPSILON * 10 },  // value:  pi
        {  PAL_POSINF,             0,                      PAL_EPSILON,         PAL_POSINF,          0 }
        
    };

    /* PAL initialization */
    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }

    for (int i = 0; i < (sizeof(tests) / sizeof(struct test)); i++)
    {
        validate( tests[i].value,  tests[i].expected, tests[i].variance,  tests[i].expected_intpart, tests[i].variance_intpart);
        validate(-tests[i].value, -tests[i].expected, tests[i].variance, -tests[i].expected_intpart, tests[i].variance_intpart);
    }

    validate_isnan(PAL_NAN);

    PAL_Terminate();
    return PASS;
}
