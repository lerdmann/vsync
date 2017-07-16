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
#ifndef _DECLS_H_
#define _DECLS_H_

USHORT GetCSLimit(VOID);
USHORT GetDSLimit(VOID);
USHORT queryVGAAddrOffset(VOID);
VOID   pollVSYNCSignal(USHORT off);

#define STRINGIFY(s) #s
#define TOSTRING(s) STRINGIFY(s)

#define PCI_FUNC 0x0b
#define PCI_GETBIOS_INFO    0
#define PCI_FIND_DEVICE     1
#define PCI_FIND_CLASS_CODE 2
#define PCI_READ_CONFIG     3
#define PCI_WRITE_CONFIG    4

typedef struct _PCI_PARM {
   UCHAR PCISubFunc;
   union {
      struct {
         USHORT DeviceID;
         USHORT VendorID;
         UCHAR  Index;
      } Parm_Find_Dev;
      struct {
         ULONG ClassCode;
         UCHAR Index;
      } Parm_Find_ClassCode;
      struct {
         UCHAR BusNum;
         UCHAR DevFunc;
         UCHAR ConfigReg;
         UCHAR Size;
      } Parm_Read_Config;
      struct {
         UCHAR BusNum;
         UCHAR DevFunc;
         UCHAR ConfigReg;
         UCHAR Size;
         ULONG Data;
      } Parm_Write_Config;
   };
} PCI_PARM;


typedef struct _PCI_DATA {
   UCHAR bReturn;
   union {
      struct {
         UCHAR HWMech;
         UCHAR MajorVer;
         UCHAR MinorVer;
         UCHAR LastBus; 
      } Data_Bios_Info;
      struct {
         UCHAR BusNum;
         UCHAR DevFunc;
      } Data_Find_Dev;
      struct {
         ULONG Data;
      } Data_Read_Config;
   };
} PCI_DATA;

typedef struct _PCI_DEVICE {
   UCHAR BusNum;
   UCHAR DevFunc;
} PCI_DEVICE;

#define MAX_DEV           32
#define MAX_FUNC           8
enum sizes { BSIZE = 1, WSIZE=2, DWSIZE=4};

#define PCI_DISPLAY_BASECLASS	0x03
#define PCI_VGA_SUBCLASS	0x00
#define PCI_VGA_INTERFACE       0x00

#define PCIREG_INT_LINE         0x3C
#define PCIREG_INT_PIN          0x3D

#endif

