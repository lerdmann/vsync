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
int main(int argc,char *argv[])
{
   APIRET rc;
   HFILE  hf;
   ULONG  ulAction,i;


   if (NO_ERROR == DosOpen("\\DEV\\VSYNC$",&hf,&ulAction,0,FILE_NORMAL,OPEN_ACTION_OPEN_IF_EXISTS,OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE,NULL)) {
      for (i=0;i<10000;i++) {
         DosDevIOCtl(hf,0x80,1,NULL,0,NULL,NULL,0,NULL);
      } /* endfor */
      DosClose(hf);
   } /* endif */
   return 0;
}

