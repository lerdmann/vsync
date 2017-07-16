;****************************************************************************************;
;*                                                                                      *;
;* COPYRIGHT    Copyright (C) 2017 Lars Erdmann                                         *;
;*                                                                                      *;
;* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. *;
;* If a copy of the MPL was not distributed with this file,                             *;
;* You can obtain one at https://mozilla.org/MPL/2.0/.                                  *;
;*                                                                                      *;
;* This Source Code Form is "Incompatible With Secondary Licenses",                     *;
;* as defined by the Mozilla Public License, v. 2.0.                                    *;
;*                                                                                      *;
;****************************************************************************************;
.686p
OPTION LANGUAGE:C,OFFSET:GROUP

include devsym.inc

_DATA segment dword public use16 'DATA'
EXTRN C Strat:NEAR
_DATA ends

CONST segment dword public use16 'CONST'
CONST ends

_BSS  segment dword public use16 'BSS'
_BSS  ends

_TEXT segment dword public use16 'CODE'
StratEntry PROC FAR PUBLIC
   push es
   push bx
   call Strat
   pop  bx
   pop  es
ASSUME bx:PTR Packet
   mov  es:[bx.PktStatus],ax
   ret	
StratEntry ENDP

GetCSLimit PROC NEAR PUBLIC
   mov ax,CGROUP
   lsl ax,ax
   ret
GetCSLimit ENDP

GetDSLimit PROC NEAR PUBLIC
   mov ax,DGROUP
   lsl ax,ax
   ret
GetDSLimit ENDP
_TEXT ends

CODE segment dword public use16 'CODE'
CODE ENDS

RMCode segment dword public use16 'CODE'
RMCode ENDS



DDHEADER segment dword public use16 'FAR_DATA'
DDHEADER ends

DGROUP GROUP DDHEADER,CONST,_BSS,_DATA
CGROUP GROUP _TEXT,CODE,RMCode

end