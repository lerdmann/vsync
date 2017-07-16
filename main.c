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
#define INCL_DOSDEVIOCTL
#include <os2.h>
#include <strat2.h>
#include <reqpkt.h>
#include <devcmd.h>
#include <dhcalls.h>
#include <rmcalls.h>
#include <memory.h>
#include <string.h>
#include "decls.h"

PFN Device_Help     = NULL;
ULONG RMFlags       = 0UL;
PFNRM RM_Help0      = NULL;
PFNRM RM_Help3      = NULL;
PVOID gpYieldFlag   = NULL;
ULONG gVSyncEndCount   = 0UL;
ULONG gVSyncStartCountYield = 0UL;
ULONG gVSyncStartCountPause = 0UL;
ULONGLONG gLastTimeStamp = {0};
ULONGLONG gTimeDiff = {0};
USHORT gVGAOff      = 0;
HFILE  ghOEM        = 0;
PCI_DEVICE gpciDev  = {0};
UCHAR gCmdLine[128] = {0};
UCHAR gSignOnBanner[] = {"\r\n\r\n"TOSTRING(BLDSTRING)"\r\n\r\n"};
UCHAR gErrOEMHlpMsg[] = {"\r\n\r\nCannot attach to OEMHLP$, aborting ...\r\n\r\n"};
UCHAR gErrNOVGAMsg[] = {"\r\n\r\nNo VGA device found, aborting ...\r\n\r\n"};

DRIVERSTRUCT        gDriverStruct =
{
   "VSYNC.SYS",
   TOSTRING(BLDINFO),
   TOSTRING(VENDOR),
   VERMAJOR,
   VERMINOR,
   YEAR,MONTH,DAY,
   DRF_STATIC,
   DRT_OS2,
   DRS_CHAR,
   NULL
};

ADAPTERSTRUCT       gAdapterStruct = 
{
   "VGA VSYNC",
   AS_NO16MB_ADDRESS_LIMIT,
   AS_BASE_DISPLAY,
   AS_SUB_VGA,
   AS_INTF_VGA_GEN,
   AS_HOSTBUS_PLANAR,
   AS_BUSWIDTH_16BIT,
   NULL,
   NULL
};

HDRIVER             ghDriver  = 0UL;
HADAPTER            ghAdapter = 0UL;


PCI_DEVICE findPCIDevice(UCHAR baseClass,UCHAR subClass,UCHAR interface)
{
   PCI_PARM p;
   PCI_DATA d;
   PCI_DEVICE retVal;

   _fmemset(&retVal,0xFF,sizeof(retVal));
   _fmemset(&p,0,sizeof(p));
   _fmemset(&d,0,sizeof(d));

   p.PCISubFunc                    = PCI_FIND_CLASS_CODE;
   p.Parm_Find_ClassCode.ClassCode = MAKEULONG(MAKEUSHORT(interface,subClass),MAKEUSHORT(baseClass,0));
   p.Parm_Find_ClassCode.Index     = 0; /* find the first instance */

   DosDevIOCtl(&d,&p,OEMHLP_PCI,IOCTL_OEMHLP,ghOEM);

   if (0 != d.bReturn) {
      return retVal;
   } /* endif */

   retVal.BusNum  = d.Data_Find_Dev.BusNum;
   retVal.DevFunc = d.Data_Find_Dev.DevFunc;

   return retVal;
}



USHORT Strat(PRPH prph)
{
   PRPINITIN pIn;
   PRPINITOUT pOut;
   PRP_GENIOCTL pGen;
   APIRET rc;
   USHORT usAction;


   switch (prph->Cmd) {
   case CMDInit:
      pIn = (PRPINITIN)prph;
      pOut = (PRPINITOUT)prph;
      Device_Help = pIn->DevHlpEP;

      _fstrncpy(gCmdLine,pIn->InitArgs,sizeof(gCmdLine)-1);
      _fstrupr(gCmdLine);
      if (_fstrstr(gCmdLine,"/V")) {
         DosPutMessage(1,_fstrlen(gSignOnBanner),gSignOnBanner);
      } /* endif */

      rc = DevHelp_GetDOSVar(DHGETDOSV_YIELDFLAG,0,&gpYieldFlag);

      usAction = 0;
      if (NO_ERROR != DosOpen("\\DEV\\OEMHLP$",&ghOEM,&usAction,0UL,FILE_NORMAL,FILE_OPEN,OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE,0UL)) {
         pOut->Unit = 0;
         pOut->CodeEnd = 0;
         pOut->DataEnd = 0;
         pOut->BPBArray = NULL;
         DosPutMessage(1,_fstrlen(gErrOEMHlpMsg),gErrOEMHlpMsg);
         return STDON | STERR | ERROR_I24_GEN_FAILURE;
      }

      gpciDev = findPCIDevice(PCI_DISPLAY_BASECLASS,PCI_VGA_SUBCLASS,PCI_VGA_INTERFACE);
      if ((0xFF == gpciDev.BusNum) && (0xFF == gpciDev.DevFunc)) {
         pOut->Unit = 0;
         pOut->CodeEnd = 0;
         pOut->DataEnd = 0;
         pOut->BPBArray = NULL;
         DosPutMessage(1,_fstrlen(gErrNOVGAMsg),gErrNOVGAMsg);
         return STDON | STERR | ERROR_I24_GEN_FAILURE;
      } /* endif */

      gVGAOff = queryVGAAddrOffset();

      if (RMRC_SUCCESS == RMCreateDriver(&gDriverStruct,&ghDriver)) {
         ADJUNCT        adjAdaptNum,adjBDF;

         adjAdaptNum.pNextAdj       = &adjBDF;
         adjAdaptNum.AdjLength      = ADJ_HEADER_SIZE + sizeof(USHORT);
         adjAdaptNum.AdjType        = ADJ_ADAPTER_NUMBER;
         adjAdaptNum.Adapter_Number = 0;
         adjBDF.pNextAdj            = NULL;
         adjBDF.AdjLength           = ADJ_HEADER_SIZE + sizeof(USHORT);
         adjBDF.AdjType             = ADJ_PCI_DEVFUNC;
         adjBDF.PCI_DevFunc         = MAKEUSHORT(gpciDev.DevFunc,gpciDev.BusNum);
         gAdapterStruct.pAdjunctList= &adjAdaptNum;

         RMCreateAdapter( ghDriver, &ghAdapter, &gAdapterStruct, NULL, NULL);
      }

      pOut->Unit = 0;
      pOut->CodeEnd = GetCSLimit();
      pOut->DataEnd = GetDSLimit();
      pOut->BPBArray = NULL;
      return STDON;
      break;
   case CMDInitComplete:
      break;
   case CMDShutdown:
      break;
   case CMDInitBase:
      /* don't try to load this driver via the BASEDEV= keyword */
      pOut = (PRPINITOUT)prph;
      pOut->Unit = 0;
      pOut->CodeEnd = 0;
      pOut->DataEnd = 0;
      pOut->BPBArray = NULL;
      return STDON | STERR | ERROR_I24_GEN_FAILURE;
      break;
   case CMDOpen:
      return STDON;
      break;
   case CMDClose:
      return STDON;
      break;
   case CMDGenIOCTL:
      pGen = (PRP_GENIOCTL)prph;
      if ((0x80 == pGen->Category) && (1 == pGen->Function)) {
         pollVSYNCSignal(gVGAOff);
         return STDON;
      } else {
         return STDON | STERR | ERROR_I24_BAD_COMMAND;
      } /* endif */
      break;
   default: 
      return STDON | STERR | ERROR_I24_BAD_COMMAND;
      break;
   } /* endswitch */
   return STDON;
}
