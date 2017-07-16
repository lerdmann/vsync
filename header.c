/****************************************************************************************/
/*                                                                                      */
/* COPYRIGHT    Copyright (C) 2017 Lars Erdmann                                         */
/*                                                                                      */
/* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. */
/* If a copy of the MPL was not distributed with this file,                             */
/* You can obtain one at https://mozilla.org/MPL/2.0/.                                  */
/*                                                                                      */
/* This Source Code Form is "Incompatible With Secondary Licenses",                     */
/* as defined by the Mozilla Public License, v. 2.0.                                    */
/*                                                                                      */
/****************************************************************************************/
#define INCL_BASE
#include <os2.h>
#include <devhdr.h>
#include <dhcalls.h>
#include <strat2.h>
#include <reqpkt.h>
void StratEntry(PRPH prph);

struct SysDev3 _based(_segname("DDHEADER")) theHeader = {
   {
   (unsigned long)-1,
   DEV_CHAR_DEV | DEV_SHARE | DEV_30 | DEVLEV_3, /* DEV_30 = Open/Close requests will be received: necessary to support IOCTL calls */
   (unsigned short)StratEntry,
   0,
   "VSYNC$  ",
   0,0,0,0
   },
   DEV_IOCTL2 | DEV_16MB | DEV_INITCOMPLETE
};

