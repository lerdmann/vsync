# vsync
A device driver to poll the VSYNC signal to support VSYNC synchronization in FireFox (Project Silk)

You need this for building the driver:
1) the IBM OS/2 DDK (for various include files and the used static libraries)
2) Micosoft C 6.0 compiler (cl.exe, contained in IBM OS/2 DDK), IBM assembler (alp.exe, contained in IBM OS/2 toolkit but
   also to be found on hobbes.nmsu.edu), link386.exe (part of every OS/2 installation but also to be found in IBM OS/2
   toolkit), nmake.exe (contained in IBM OS/2 toolkit or you can run Watcom's wmake.exe in "NMAKE mode": wmake -ms)
