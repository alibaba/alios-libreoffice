/* config_host/config_folders.h.  Generated from config_folders.h.in by configure.  */
/* Configuration of subfolder names in the installation tree. The
 * values of these macros when configured will be strings. The names
 * of these macros reflect what the value would be in an "ideal" world
 * on Linux, not current reality. (For example LIBO_BIN_FOLDER is
 * actually "program" on Linux and most other Unixes.)
 */

#ifndef CONFIG_FOLDERS_H
#define CONFIG_FOLDERS_H

/* where the soffice executable and other end-user-invoked executables are */
#define LIBO_BIN_FOLDER "program"

/* where the *rc / *.ini file are */
#define LIBO_ETC_FOLDER "program"

/* where helper executables run by soffice are */
#define LIBO_LIBEXEC_FOLDER "program"

/* where dynamic libraries loaded directly or programmatically are */
#define LIBO_LIB_FOLDER "program"

/* where read-only resources are in general */
#define LIBO_SHARE_FOLDER "share"

/* where help files are */
#define LIBO_SHARE_HELP_FOLDER "help"

/* where java jars are */
#define LIBO_SHARE_JAVA_FOLDER "program/classes"

/* the presets folder */
#define LIBO_SHARE_PRESETS_FOLDER "presets"

/* LO's own "resources" */
#define LIBO_SHARE_RESOURCE_FOLDER "program/resource"

/* LO's "shell" artwork */
#define LIBO_SHARE_SHELL_FOLDER "program/shell"

/* URE folders */
#define LIBO_URE_BIN_FOLDER "program"
#define LIBO_URE_ETC_FOLDER "program"
#define LIBO_URE_LIB_FOLDER "program"
#define LIBO_URE_MISC_FOLDER "program"
#define LIBO_URE_SHARE_JAVA_FOLDER "program/classes"

#endif
