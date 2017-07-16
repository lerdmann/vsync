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



include devhlp.inc
include iodelay.inc

VGA_MISC_REG_R    EQU 3CCh
VGA_STATUS1_REG_R EQU 30Ah


_DATA segment dword public use16 'DATA'
EXTRN C Device_Help:DWORD
EXTRN C gVSyncEndCount:DWORD
EXTRN C gVSyncStartCountYield:DWORD
EXTRN C gVSyncStartCountPause:DWORD
EXTRN C gTimeDiff:QWORD
EXTRN C gLastTimeStamp:QWORD
EXTRN C gpYieldFlag:FAR PTR BYTE    ; retrieved via DevHelp_GetDOSVar, DHGETDOSV_YIELDFLAG
EXTRN PASCAL DOSIODELAYCNT:ABS
_DATA ends




_TEXT segment dword public use16 'CODE'	; need to select DWORD alignment so that DevIODelay can use ALIGN 4
ASSUME ds:_DATA

pause MACRO
   db 0f3h,090h
ENDM

queryVGAAddrOffset PROC NEAR PUBLIC
    mov dx,VGA_MISC_REG_R
    in  al,dx
    DevIODelay <cx>    
    test al,1         ; check IOS (I/O address select) bit
    jnz short @F
    mov  ax,0b0h
    jmp short quit
@@:
    mov  ax,0d0h
quit:    
    ret	
queryVGAAddrOffset ENDP


pollVSYNCSignal PROC NEAR PUBLIC, off:WORD
    and sp,0FFFCh
    push esi
    push edi
    mov gVSyncEndCount,0
    mov gVSyncStartCountYield,0
    mov gVSyncStartCountPause,0
    les si,gpYieldFlag
ASSUME si:PTR BYTE
    mov dx,VGA_STATUS1_REG_R
    add dx,off

@@: in al,dx
    DevIODelay <cx>
    test al,1000b   ; check VR (vertical retrace) bit
    jz short @F
    pause
    inc gVSyncEndCount
    jmp short @B    ; repeat for as long as it is set (wait for end of vertical retrace)

@@: in al,dx
    DevIODelay <cx>
    test al,1000b   ; check VR (vertical retrace) bit
    jnz short @F
    test es:[si],0FFh
    jz short cont
    push dx
    mov dl,DevHlp_Yield
    call DWORD PTR Device_Help
    pop dx
    inc gVSyncStartCountYield
    jmp short @B
cont:
    pause
    inc gVSyncStartCountPause
    jmp short @B    ; repeat for as long as it is not set (wait for start of vertical retrace)
@@:
    rdtsc
    mov esi,DWORD PTR gLastTimeStamp
    mov edi,DWORD PTR gLastTimeStamp[4]
    mov DWORD PTR gLastTimeStamp   ,eax
    mov DWORD PTR gLastTimeStamp[4],edx
    sub eax,esi
    sbb edx,edi
    mov DWORD PTR gTimeDiff   ,eax
    mov DWORD PTR gTimeDiff[4],edx
    pop edi
    pop esi
    ret
pollVSYNCSignal ENDP
_TEXT ends

END

