/* config_host/config_version.h.  Generated from config_version.h.in by configure.  */
/*
Version settings
  version 4.0.0.1 is Major.Minor.Micro.Patch
*/

#ifndef CONFIG_VERSION_H
#define CONFIG_VERSION_H

#define LIBO_VERSION_STRINGIFY_inner(s) #s
#define LIBO_VERSION_STRINGIFY(s) LIBO_VERSION_STRINGIFY_inner(s)

#define LIBO_VERSION_MAJOR 6
#define LIBO_VERSION_MINOR 0
#define LIBO_VERSION_MICRO 5
#define LIBO_VERSION_PATCH 2
#define LIBO_THIS_YEAR 2018

#define LIBO_VERSION_DOTTED \
    LIBO_VERSION_STRINGIFY(LIBO_VERSION_MAJOR) "." \
    LIBO_VERSION_STRINGIFY(LIBO_VERSION_MINOR) "." \
    LIBO_VERSION_STRINGIFY(LIBO_VERSION_MICRO) "." \
    LIBO_VERSION_STRINGIFY(LIBO_VERSION_PATCH)

#endif
