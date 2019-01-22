/* config_host/config_lgpl.h.  Generated from config_lgpl.h.in by configure.  */
/*
 * Include this file to ensure that this code is not
 * compiled into the MPL subset.
 */
#ifndef CONFIG_LGPL_H
#define CONFIG_LGPL_H

#include "config_mpl.h"

#if MPL_HAVE_SUBSET
#  error "Attempting to compile LGPL code into MPL subset"
#endif

#endif
