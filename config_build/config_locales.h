/* config_host/config_locales.h.  Generated from config_locales.h.in by configure.  */
/* Configuration of restriction on supported locales, by the
 * --with-locales option.
 *
 * (Note that this is totally unrelated to what UI language(s) were
 * configured with the --with-lang option.)
 *
 * Normally, especially for desktop OSes, LibreOffice is built to
 * support all the locales and character encodings that it has code
 * (data) for in the source code. By using the --with-locales
 * configure switch, this can be restricted.
 *
 * This file defines feature test macros for those languages for which
 * at least one locale is to be supported. In some cases, a feature
 * test macro for a script is provided in addition or instead.
 *
 * Only languages for which there exists separate code and/or data in
 * the source code need to be mentioned here.
 */

#ifndef CONFIG_LOCALES_H
#define CONFIG_LOCALES_H

/* In the normal case WITH_LOCALE_ALL is one and this
 * overrides the others which are zero.
 */
#define WITH_LOCALE_ALL 1

/* When --with-locales was used, WITH_LOCALE_ALL is zero and some of
 * these are one.
 */
#define WITH_LOCALE_ca 0
#define WITH_LOCALE_cu 0
#define WITH_LOCALE_dz 0
#define WITH_LOCALE_el 0
#define WITH_LOCALE_he 0
#define WITH_LOCALE_hi 0
#define WITH_LOCALE_hr 0
#define WITH_LOCALE_hu 0
#define WITH_LOCALE_is 0
#define WITH_LOCALE_ja 0
#define WITH_LOCALE_ko 0
#define WITH_LOCALE_ku 0
#define WITH_LOCALE_ln 0
#define WITH_LOCALE_my 0
#define WITH_LOCALE_ne 0
#define WITH_LOCALE_ro 0
#define WITH_LOCALE_ru 0
#define WITH_LOCALE_sid 0
#define WITH_LOCALE_th 0
#define WITH_LOCALE_tr 0
#define WITH_LOCALE_uk 0
#define WITH_LOCALE_zh 0

#define WITH_LOCALE_FOR_SCRIPT_Cyrl 0
#define WITH_LOCALE_FOR_SCRIPT_Deva 0

#endif
