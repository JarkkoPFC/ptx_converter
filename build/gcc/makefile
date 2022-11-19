#============================================================================
# ptx_conv - PTX Texture Converter
#
# Copyright (c) 2022, Jarkko Lempiainen, Inc.
# All rights reserved.
#----------------------------------------------------------------------------
#
# make options:
#   build=<build>       - options: debug (default), release, retail
#   platform=<platform> - options: linux32, linux64 (default), win32, win64
# make targets:
#   exes                - compile all libraries and executables (default)
#   clean               - delete exes, libraries and object files for the target
#   clean_int           - delete intermediate files
#   <exe name>          - compile given executable
#
# examples:
# to compile linux64 release build of all executables:
#   make build=release exes
#
# to compile win64 release build of all executables:
#   make build=release platform=win64 exes
#
# to clean all linux64 release build files:
#   make build=release clean
#
# to clean and rebuild linux64 debug libraries and executables:
#   make clean exes


# build settings
ifndef build
build=debug
endif
$(if $(filter $(build),debug release retail),,$(error unknown target build "$(build)"))
ifeq "$(build)" "debug"
CFLAGS=-ggdb
CDEFS=-D PFC_DEBUG
else ifeq "$(build)" "release"
CFLAGS=-O2 -fno-strict-aliasing
CDEFS=-D PFC_RELEASE
else ifeq "$(build)" "retail"
CFLAGS=-O3 -fno-strict-aliasing
CDEFS=-D PFC_RETAIL
endif

# platform settings
ifndef platform
platform=linux64
endif
$(if $(filter $(platform),linux32 linux64 win32 win64),,$(error unknown target platform "$(platform)"))
ifeq "$(platform)" "linux32"
CFLAGS+=-m32
CDEFS+=-D PFC_PLATFORM_LINUX32
else ifeq "$(platform)" "linux64"
CFLAGS+=-m64 -mcx16
CDEFS+=-D PFC_PLATFORM_LINUX64
else ifeq "$(platform)" "win32"
CFLAGS+=-m32
CDEFS+=-D PFC_PLATFORM_WIN32
else ifeq "$(platform)" "win64"
CFLAGS+=-m64
CDEFS+=-D PFC_PLATFORM_WIN64
endif

# generic settings
.DEFAULT_GOAL:=exes
MD=mkdir -p $(1)
RD=rm -r -f $(1)
RM=rm -f $(1)
EXEDIR=../../bin
INTDIR_ROOT=../../_intermediate
INTDIR=$(INTDIR_ROOT)/$(platform)_gcc/$(build)
EXTLIBDIR=../../mini_sxp/sxp_extlibs/lib/$(platform)_gcc
SRCDIR=../..
MINI_SXP_LIB=../../mini_sxp/lib/$(platform)_gcc/mini_sxp_$(build).a
EXTLIBS=jpeglib libpng libtiff libwebp nvtexturetools openjpeg zlib
CLIBS=
# CLIBS=-lgdi32 -lcomdlg32 -lcomctl32 -lole32 -loleaut32 -luuid

# compiler settings
CC=g++
CINCS=-I../.. -I../../mini_sxp -I../../mini_sxp/sxp_extlibs/zlib/src
CWARNS=-Wall -Wno-non-template-friend -Wno-comments -Wno-unknown-pragmas -Wno-int-in-bool-context
CDEFS+=-D PFC_COMPILER_GCC
CFLAGS+=-c -msse2 $(CWARNS) $(CDEFS) $(CINCS)

# linker settings
LD=g++
LDFLAGS=-pthread

# archiver settings
AR=ar
ARFLAGS=rcs

# executables
EXECUTABLES=PTX_CONV
# unittest exe
PTX_CONV_EXE:=$(EXEDIR)/ptx_conv_$(platform)_$(build)
PTX_CONV_EXE_DIRS:=src
PTX_CONV_EXE_LDFLAGS:=-Wl,--no-as-needed -lrt

# helper functions
SRC_FILES=$(filter-out $(2),$(foreach DIR,$(1),$(wildcard $(SRCDIR)/$(DIR)/*.cpp)))
OBJ_FILES=$(subst $(SRCDIR),$(INTDIR),$(1:.cpp=.o))

# executable target template
define EXE_TEMPLATE
ALL_EXES+=$($(1)_EXE)
ALL_EXE_DIRS+=$(firstword $(subst /, ,$($(1)_EXE_DIRS)))
$(1)_EXE_SRC:=$(call SRC_FILES,$($(1)_EXE_DIRS),$($(1)_EXE_EXCL))
$(1)_EXE_OBJ:=$$(call OBJ_FILES,$$($(1)_EXE_SRC))
.PHONY $(notdir $($(1)_EXE:_$(platform)_$(build).exe=)):
$(notdir $($(1)_EXE:_$(platform)_$(build).exe=)) $($(1)_EXE): $($(1)_EXE_DEPS) $(ALL_LIBS) $$($(1)_EXE_OBJ)
	@echo Creating executable $($(1)_EXE)
#	@$(MD) $(dir $($(1)_EXE))
	@$(LD) $(LDFLAGS) $($(1)_EXE_LDFLAGS) -o $($(1)_EXE) $$($(1)_EXE_OBJ) $(ALL_LIBS) $(MINI_SXP_LIB) $$(addprefix $(EXTLIBDIR)/,$(addsuffix _$(build).a,$(EXTLIBS))) $(CLIBS)
endef

# setup targets for all libraries and exes
$(foreach EXE,$(EXECUTABLES),$(eval $(call EXE_TEMPLATE,$(EXE))))

.PHONY: exes
exes: $(ALL_EXES)

$(INTDIR)/%.o: $(SRCDIR)/%.cpp
	@echo $(subst $(SRCDIR)/,,$<)
	@$(call MD,$(dir $@))
	@$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@echo Deleting executables
	@$(foreach EXE,$(ALL_EXES),$(call RM,$(EXE)))
	@echo Deleting intermediate directories
	@$(call RD,$(INTDIR_ROOT))

.PHONY: clean_int
clean_int:
	@echo Deleting intermediate directories
	@$(call RD,$(INTDIR_ROOT))
