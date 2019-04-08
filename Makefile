################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#                                   MAKEFILE                                   #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################

#******************************************************************************#
#       Installation directories                                               #
#******************************************************************************#

# Project directories
incdir			:= include
srcdir			:= source

# System directories
#prefix			:= /usr/local
#exec_prefix		:= $(prefix)
#sysconfdir		:= $(prefix)/etc
#includedir		:= $(prefix)/include
#datarootdir		:= $(prefix)/share
#localstatedir	:= $(prefix)/var
#sbindir			:= $(exec_prefix)/sbin
#bindir			:= $(exec_prefix)/bin
#libdir			:= $(exec_prefix)/lib
#datadir			:= $(datarootdir)
#localedir		:= $(datarootdir)/locale
#infodir			:= $(datarootdir)/info
#docdir			:= $(datarootdir)/doc/pkg_name
#htmldir			:= $(docdir)
#pdfdir			:= $(docdir)
#dvidir			:= $(docdir)
#psdir			:= $(docdir)
#mandir			:= $(datarootdir)/man
#man1dir			:= $(mandir)/man1
#man2dir			:= $(mandir)/man2
#man3dir			:= $(mandir)/man3
#man4dir			:= $(mandir)/man4
#man5dir			:= $(mandir)/man5
#man6dir			:= $(mandir)/man6
#man7dir			:= $(mandir)/man7
#man8dir			:= $(mandir)/man8
#man1ext			:= .1
#man2ext			:= .2
#man3ext			:= .3
#man4ext			:= .4
#man5ext			:= .5
#man6ext			:= .6
#man7ext			:= .7
#man8ext			:= .8

#******************************************************************************#
#       Utilities configuration                                                #
#******************************************************************************#

# Utilities names
CXX				:= g++
#INSTALL			:= install

# Utilities flags
CXXFLAGS		:= -I $(incdir) -Wall -std=gnu++17 -O2 -march=native -mtune=native
#INSTALLFLAGS	:=

#******************************************************************************#
#       Makefile variables                                                     #
#******************************************************************************#
vpath	%.h		$(incdir)
vpath	%.cpp	$(srcdir)

#INSTALL_PROGRAM := $(INSTALL)
#INSTALL_DATA	:= $(INSTALL) -m 644

targets			:= flood
objects			:= $(notdir $(patsubst %.cpp, %.o, $(wildcard $(srcdir)/*.cpp)))

#******************************************************************************#
#       Makefile targets                                                       #
#******************************************************************************#
.SUFFIXES:
#.PHONY: install-strip uninstall clean

all: $(targets)

flood: $(objects)
	$(CXX) $(CXXFLAGS) $^ -lpthread -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $? -o $@

#install: $(targets)
#	$(INSTALL) -d $(INSTALLFLAGS) $(DESTDIR)$(bindir)
#	$(INSTALL_PROGRAM) -p $(INSTALLFLAGS) $(targets) $(DESTDIR)$(bindir)

#install-strip:
#	$(MAKE) install INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s'

#uninstall:
#	cd $(DESTDIR)$(bindir) && rm -f $(targets)

clean:
	rm -f $(targets) $(objects)

################################################################################
#                                 END OF FILE                                  #
################################################################################
