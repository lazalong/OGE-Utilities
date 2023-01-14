#pragma once

/*
 *  OGE Open Game Engine
 *  Copyright (c) 2023 Steven Gay (lazalong@gmail.com)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#ifndef __OGE_H__
#define __OGE_H__

#if defined( WIN32 ) || defined( __WIN32__ ) || defined( _WIN32 )
#   define OGE_PLATFORM OGE_PLATFORM_WIN32

#elif defined( __APPLE_CC__)
#   define OGE_PLATFORM OGE_PLATFORM_APPLE

#else
#   define OGE_PLATFORM OGE_PLATFORM_LINUX
#endif

// https://developercommunity.visualstudio.com/t/issue-in-corecrth-header-results-in-an-undefined-m/433021
// LATER may not be necessary anymore
#define _CRT_HAS_CXX17 0

#ifdef _MSC_VER
//#   pragma warning (disable : MSB8028) // LATER doesn't work. Is there a way?
#endif

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h> // only defines bool, false and true

//#if ! defined HEAPSIZE || HEAPSIZE < 1
//#   define HEAPSIZE 10
//#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int_least8_t i8;
typedef short i16;          // short
typedef int i32;            // int
typedef long i64;           //

#define OGE_VERSION "0.1"
#define OGE_FRAMERATE 16 // ms between frames
#define OGE_HALF_FRAMERATE 8

#define OGE_USE_LEAK_CHECK 1
#define LOG_NORMAL

// Forward delcarations of structs to avoid many #includes
typedef struct OgeEngine OgeEngine;
typedef struct OgeHashTable OgeHashTable;

#endif
