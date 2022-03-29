# lunar compress
a version of the lunar compress source code compileable by newer visual studio versions
## original source code by FuSoYa
https://fusoya.eludevisibility.org/lc/index.html
## how to compile
1. download the latest version of visual studio: https://visualstudio.microsoft.com/downloads/
2. in the visual studio installer, choose "Desktop development with C++"
3. enable "MSVC v142 - VS 2019 C++" in the installation details (this is optional)
4. open any of the sln file
5. switch debug to release and compile!
## IMPORTANT
at the moment this can only compile the programs, not the dll.  
i am very unsure why this is the case, the dll IS compileable but none of the programs can actually read it.  
after you compiled any of the programs, just copy the original dll from https://fusoya.eludevisibility.org/lc/download/lc190.zip next to the compiled exe for now.  
when you copy the dll, make sure that if you compiled the program in x64, you copy the dll from the x64 folder or else the program will fail to load the dll.
