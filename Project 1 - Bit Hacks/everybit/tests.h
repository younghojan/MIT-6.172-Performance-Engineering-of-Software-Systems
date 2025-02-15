/**
 * Copyright (c) 2012 MIT License by 6.172 Staff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#ifndef TESTS_H
#define TESTS_H

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>

#include "./bitarray.h"


 // ******************************* Prototypes *******************************

 // Will run increasingly larger test cases, until a test case takes longer
 // than time_limit_seconds to complete.
int timed_rotation(const double time_limit_seconds);

// Implement the timing functionality within another function for testing
double timed_rotation_variable(const size_t bit_offset,
    const size_t bit_right_shift_amount,
    const size_t bit_length,
    const size_t bit_sz);

// Runs the testsuite specified in a given file.
void parse_and_run_tests(const char* filename, int min_test);

// Prints a string representation of a bit array.
void bitarray_fprint(FILE* const stream,
    const bitarray_t* const bitarray);

// Prints a string representation of a bit array (as a numerical value).
void bitarray_value_fprint(FILE* const stream,
    const unsigned char value);

#endif  // TESTS_H

