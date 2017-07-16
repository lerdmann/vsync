#****************************************************************************************#
#*                                                                                      *#
#* COPYRIGHT    Copyright (C) 2017 Lars Erdmann                                         *#
#*                                                                                      *#
#* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. *#
#* If a copy of the MPL was not distributed with this file,                             *#
#* You can obtain one at https://mozilla.org/MPL/2.0/.                                  *#
#*                                                                                      *#
#* This Source Code Form is "Incompatible With Secondary Licenses",                     *#
#* as defined by the Mozilla Public License, v. 2.0.                                    *#
#*                                                                                      *#
#****************************************************************************************#
!ifndef DDK
!error DDK must be defined in environment
!endif

!ifndef TOOLKIT
!error TOOLKIT must be defined in environment
!endif

!ifndef VERMAJOR
!error VERMAJOR (driver major version) must be defined in environment
!endif

!ifndef VERMINOR
!error VERMINOR (driver minor version) must be defined in environment
!endif

!ifndef VENDOR
!error VENDOR (Vendor identification) must be defined in environment
!endif

!ifndef DAY
!error DAY (day of build) must be defined in environment
!endif

!ifndef MONTH
!error MONTH (month of build) must be defined in environment
!endif

!ifndef YEAR
!error YEAR (year of build) must be defined in environment
!endif


!if [SET CL=] || [SET LIB=] || [SET INCLUDE=] || [SET PATH=$(TOOLKIT)\bin;$(DDK)\base\tools;]
!endif

.SUFFIXES:
.SUFFIXES: .c .asm .obj .sym .sys 

H   = $(DDK)\base\h
INC = $(DDK)\base\inc
LIB = $(DDK)\base\lib

AFLAGS=-Mb -I:$(INC) +Fl -Lr -Li -Llp:0 +Lm +Ls -Lc
CFLAGS=-nologo -I. -I$(H) -W3 -Answ -G2s -Zlp -B1c1l -B2c2l -B3c3l -DVERMAJOR=$(_VERMAJOR) -DVERMINOR=$(_VERMINOR) -DDAY=$(_DAY) -DMONTH=$(_MONTH) -DYEAR=$(_YEAR) -DVENDOR="$(_VENDOR)" -DBLDSTRING="$(_BLDSTRING)" -DBLDINFO="$(_BLDINFO)" -Fc -c
LFLAGS=/nol /nod /packd /far /exepack /bat /m:FULL /a:16 /far

!ifdef DEBUG
AFLAGS=$(AFLAGS) +Od:MS16
CFLAGS=$(CFLAGS) -Odi -Zi
LFLAGS=$(LFLAGS) /co
!else
CFLAGS=$(CFLAGS) -Oseli
!endif

.c.obj:
    cl.exe $(CFLAGS) $<

.asm.obj:
    alp.exe $(AFLAGS) $<

OBJS = segs.obj header.obj hw.obj main.obj
DEF  = vsync.def

!include "$(H)\version.mak"
_VERMAJOR = $(VERMAJOR)
_VERMINOR = $(VERMINOR)
_VENDOR   = $(VENDOR)
_DAY      = $(DAY)
_MONTH    = $(MONTH)
_YEAR     = $(YEAR)
_VERSION  = $(_VERMAJOR).$(_VERMINOR)
_BLDINFO  = OS/2 Vsync synchronization driver
_BLDSTRING= $(FILEVER)$(_BLDINFO)

vsync.sys: $(OBJS) makefile
        REM Create DEF file <<$(DEF)
PHYSICAL DEVICE VSYNC
DESCRIPTION '$(_BLDSTRING)'
PROTMODE
SEGMENTS
  DDHEADER CLASS 'FAR_DATA'
  _DATA    CLASS 'DATA'
  CONST    CLASS 'CONST'
  _BSS     CLASS 'BSS'
  _TEXT    CLASS 'CODE'
  'CODE'   CLASS 'CODE'
  RMCode   CLASS 'CODE'
IMPORTS
  DOSIODELAYCNT=DOSCALLS.427
<<keep

    link386.exe @<<vsync.lnk
$(LFLAGS) +
$(OBJS: =+)
$@
$*.map
$(LIB)\os2286.lib +
$(LIB)\slibcep.lib +
$(LIB)\dhcalls.lib +
$(LIB)\rmcalls.lib
$(DEF);
<<keep
    mapsym -an $*.map
