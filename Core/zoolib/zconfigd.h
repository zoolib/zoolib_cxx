// Copyright (c) 2000-2007 Andrew Green and Learning in Motion, Inc.
// Copyright (c) 2008-2020 Andrew Green.
// MIT License. http://www.zoolib.org

#ifndef __zconfigd_h__
#define __zconfigd_h__ 1

// Which compiler -- ZCONFIG_Compiler
//#define ZCONFIG_Compiler_CodeWarrior 1
#define ZCONFIG_Compiler_MSVC 2
#define ZCONFIG_Compiler_GCC 4
#define ZCONFIG_Compiler_Clang 8

// Which processor -- ZCONFIG_Processor
#define ZCONFIG_Processor_68K 1
#define ZCONFIG_Processor_PPC 2
#define ZCONFIG_Processor_x86 4
#define ZCONFIG_Processor_Sparc 8
#define ZCONFIG_Processor_Alpha 16
#define ZCONFIG_Processor_ARM 32
#define ZCONFIG_Processor_x86_64 64
#define ZCONFIG_Processor_PPC_64 128
#define ZCONFIG_Processor_ARM_64 256
#define ZCONFIG_Processor_MIPS 512

// Byte order -- ZCONFIG_Endian
#define ZCONFIG_Endian_Big 1
#define ZCONFIG_Endian_Little 2

// Debugging
// ZCONFIG_Debug can take on any value, but by convention use 0 for a non-debug build, 1 for light
// debugging, 2 for heavy. Note that ZCONFIG_Debug is what's _used_ in assert macros, but it takes
// its actual value from ZCONFIG_DebugLevel if we're doing a debug build (.sym file being generated
// on CW, DEBUG defined on gcc). You should therefore define ZCONFIG_DebugLevel in your zconfig.h
// file if it needs to be different from the default value of 1.

#endif // __zconfigd_h__
