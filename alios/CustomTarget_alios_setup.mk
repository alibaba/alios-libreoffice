# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#- Env ------------------------------------------------------------------------
ALIOSGEN := $(SRCDIR)/alios/generated
ALIOSRES := $(ALIOSGEN)/resources


#- Top level  -----------------------------------------------------------------
$(eval $(call gb_CustomTarget_CustomTarget,ALIOS/ALIOS_setup))



$(call gb_CustomTarget_get_target,ALIOS/ALIOS_setup): $(ALIOSGEN)/native-code.h



#- Generate dynamic files  ---------------------------------------------------
$(ALIOSGEN)/native-code.h: $(BUILDDIR)/config_host.mk $(SRCDIR)/alios/CustomTarget_alios_setup.mk
	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),ENV,2)
	mkdir -p $(ALIOSGEN) $(ALIOSRES) $(ALIOSRES)/services \
	         $(ALIOSRES)/share/config $(ALIOSRES)/share/filter $(ALIOSRES)/program \
	         $(ALIOSGEN) $(WORKDIR)/ALIOS;

	# generate file with call declarations
	$(SRCDIR)/solenv/bin/native-code.py \
	    -C -g core -g writer -g draw \
	    > $(ALIOSGEN)/native-code.h


#- clean ALIOS  -----------------------------------------------------------------
$(call gb_CustomTarget_get_clean_target,ALIOS/ALIOS_setup):
	$(call gb_Output_announce,$(subst $(WORKDIR)/Clean/,,$@),$(false),ENV,2)
	rm -rf $(ALIOSRES) $(ALIOSGEN)/native-code.h $(ALIOSAPPXC)
	rm -rf $(WORKDIR)/ALIOS



# vim: set noet sw=4 ts=4:
