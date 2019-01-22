/* config_host/config_global.h.  Generated from config_global.h.in by configure.  */
/*
Global configuration file.

Only for settings that apply to every source file and are unlikely to change often,
such as whether a certain C++11 feature is available.

Do NOT use for settings local to some code or for settings that can change often.
Any change in this header will cause a rebuild of almost everything.

*/

#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

#define HAVE_CXX14_CONSTEXPR 0
#define HAVE_GCC_BUILTIN_ATOMIC 1
#define HAVE_GCC_BUILTIN_FFS 1
/* _Pragma */
#define HAVE_GCC_PRAGMA_OPERATOR 1
#define HAVE_GCC_DEPRECATED_MESSAGE 1
#define HAVE_BROKEN_CONST_ITERATORS 0
#define HAVE_SYSLOG_H 1
/* Compiler supports __attribute__((warn_unused)). */
#define HAVE_GCC_ATTRIBUTE_WARN_UNUSED 1

#endif
