#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <sys\stat.h>
#include <windows.h>
#include "../DLLCode/lunardll.cpp"


unsigned char buffer[0x10000];

bool DoWait=true;


unsigned int Recompress(int argc, char*argv[])  {
FILE  *TheFile;
char* FileToRecompress;
char* RecompressedFile;
unsigned int ROMPosition;
unsigned int Format;
unsigned int Format2;
unsigned int size;
struct stat StatBuffer;
unsigned int RawSize;
unsigned int i;

   //handle the command line stuff
   if ((argc<6)||(argc>6))
   {
      printf("To use this program, type:\n recomp.exe FileToRecompress FileToSaveAs/InsertTo OffsetToSave(h) Format\n            Format2\n\n");
      printf("Add 1000 to Format to skip wait for exit.\n");
      printf("An OffsetToSave of 0 will create a new file instead of inserting.\n");
      printf("For a list of formats, see the LunarDecompress function documentation in\nLunarDLL.h and the list of identifiers in LunarDLL.def.\n");
      return 0;
   }

   FileToRecompress=argv[1];
   RecompressedFile=argv[2];
   if (strcmp(FileToRecompress,RecompressedFile)==0)
   {
      printf("File names are identical...bad idea!\n");
      return 0;
   }
   sscanf(argv[3],"%X",&ROMPosition);
   sscanf(argv[4],"%i",&Format);
   sscanf(argv[5],"%i",&Format2);

   printf("Source File        %s\nDestination File   %s (%s)\n\n",FileToRecompress,RecompressedFile, ROMPosition ? "insert" : "overwrite");

   //get the data to compress
   TheFile=fopen(FileToRecompress,"rb");
   if (!TheFile)
   {
      printf("Could not open file %s for reading.\n",FileToRecompress);
      return 0;
   }
   fseek(TheFile,0,0);
   fstat(_fileno(TheFile), &StatBuffer);
   RawSize=StatBuffer.st_size;
   if (RawSize>0x10000)
   {
      printf("File to compress is too large...\n");
      fclose(TheFile);
      return 0;
   }
   fseek(TheFile, 0, 0);
   fread(buffer,RawSize,1,TheFile);
   fclose(TheFile);

   //access DLL
   if (!LunarLoadDLL())
   {
      printf("Could not load Lunar Compress.dll...\n");
      return 0;
   }

   if (Format>=1000)
   {  //1.04+ to allow skipping waiting for terminate
      i=Format/1000;
      Format%=1000;
      if (i&1)
         DoWait=false;
   }

   i=LunarVersion();
   printf("DLL Version         %2i.%02i\n",i/100,i%100);
   printf("Data Format        %6i\n",Format);
   printf("Data Format 2      %6i\n",Format2);
   printf("Destination Offset %6X\n",ROMPosition);

   size=LunarRecompress(buffer,buffer,RawSize,0x10000,Format,Format2);
   
   //we don't need the DLL anymore
   LunarCloseFile();
   LunarUnloadDLL();

   if (!size)
   {
      printf("\nEither recompression failed or the format isn't supported.\n");
      return 0;
   }
   printf("Compressed Size    %6X\nUncompressed Size  %6X\n",size,RawSize);

   //now write the data out to the ROM or file
   TheFile=fopen(RecompressedFile,ROMPosition ? "r+b" : "wb");
   if (!TheFile)
   {
      printf("Could not open file %s for writing.\n",RecompressedFile);
      return 0;
   }

   fseek(TheFile,ROMPosition,0);
   fwrite(buffer,size,1,TheFile);
   fclose(TheFile);

   printf("\nDone!\n");
   return 1;   
}


void WaitForTerminate()  {
HANDLE ConsoleInput=GetStdHandle(STD_INPUT_HANDLE);
INPUT_RECORD record;
DWORD number;

   if (ConsoleInput==INVALID_HANDLE_VALUE)
      return;

   //turn off QuickEdit for WinNT so we get mouse click events correctly
   //(no effect on a pre-existing cmd.exe window)
   GetConsoleMode(ConsoleInput, &number);//ENABLE_MOUSE_INPUT |
   SetConsoleMode(ConsoleInput, (number | ENABLE_MOUSE_INPUT | 0x80) & ~0x40);  //enable mouse required for running from .bat files

   //If enter key has already been depressed, exit now.
   //This is to skip the "keep window open" code when
   //not running from a shortcut. Win9x only.
   if (PeekConsoleInput(ConsoleInput,&record,1,&number))
      if (number)
         if (record.EventType==KEY_EVENT)
            if (record.Event.KeyEvent.uChar.AsciiChar==VK_RETURN)
               return;

   printf("\nHit any key or click on the screen to exit.\n");
   while (true)
   {
      ReadConsoleInput(ConsoleInput,&record,1,&number);
      switch (record.EventType)  {
         case KEY_EVENT:  //any key pressed
            return;
         case MOUSE_EVENT: //any mouse button pressed
            if (record.Event.MouseEvent.dwButtonState)
               return;
      }
   }
}

int main(int argc, char *argv[])
{
unsigned int i;

   SetConsoleTitle("Recompress - Lunar Compress DLL Example");

   printf("Data Recompression Program Example Version 1.04\n");
   printf("Programmed by FuSoYa, Defender of Relm\n");
   printf("FuSoYa's Niche - http://fusoya.eludevisibility.org\n\n");

   i=Recompress(argc,argv);

   if (DoWait)
      WaitForTerminate();

   return (i ? 0 : 1);  //0 is successful program termination, 1 is fail
}

