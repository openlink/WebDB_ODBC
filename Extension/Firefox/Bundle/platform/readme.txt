ABI Naming

Each ABI is named with a string [TARGET_XPCOM_ABI] of the following format:

{CPU_ARCH}-{TARGET_COMPILER_ABI}

{CPU_ARCH} [Platforms] represents the CPU architecture and may be either:

    * x86 - i386 and higher series (including x86-64 CPUs in 32-bit mode)
    * ppc - PowerPC series
    * Alpha - Alpha series
    * x86_64 - AMD64/EMT64 series in 64-bit mode (32-bit mode is still considered x86)
    * sparc - SPARC series
    * ia64 - Itanium series

{TARGET_COMPILER_ABI} [Platforms] represents the compiler ABI and may be either:

    * msvc - Microsoft Visual C++
    * n32 - IRIX 6 C++ Compiler
    * gcc2 - GNU C++ Compiler 2.x
    * gcc3 - GNU C++ Compiler 3.x or 4.x
    * sunc - Sun C++ Compiler
    * ibmc - IBM C++ Compiler

For example:

    * Firefox built with the GNU C++ Compiler 4.0.0 for the Intel Pentium processor would have XPCOM ABI of x86-gcc3
