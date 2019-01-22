/* config_host/config_gpgme.h.  Generated from config_gpgme.h.in by configure.  */
/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/* Configuration for gpgme++.
 */

#ifndef CONFIG_GPGME_H
#define CONFIG_GPGME_H

// Defined if gpgme support is available
#define HAVE_FEATURE_GPGME 1

// Defined if gpg and gpgme signature verification is available
#define HAVE_FEATURE_GPGVERIFY 1

// Defined if gpgconf --create-socketdir works
#define HAVE_GPGCONF_SOCKETDIR 0

#if HAVE_FEATURE_GPGME
# include "config_lgpl.h"
#endif

// Defined if gpgme supports exporting minimal key.
#define GPGME_CAN_EXPORT_MINIMAL_KEY 1

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */