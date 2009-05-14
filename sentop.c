//                                              //
//      sentop.c                                //
//                                              //
//      contact: sentientswitch@gmail.com       //
//                                              //


#include <ncurses.h>
#include <stdio.h>

//Global Vars.
char STATS_PATH[100] = "/sys/devices/pci0000:00/0000:00:1c.5/0000:04:00.0/net/eth0/statistics/";

//Functions.
unsigned int GetStat(char* statsFile);


void NCInit() {
  initscr();
  keypad(stdscr, TRUE);
}

void NCExit() {
  endwin();
}

//
//  GetStat
//
//  Takes name of statistics file.
//
//  Returns number from statistics file as unsigned int.
//
unsigned int GetStat(char statsFile[]) {
  unsigned int retVal;
  char filePath[200];
  FILE *filePtr;

  //Get full file path
  sprintf(filePath, "%s%s", STATS_PATH, statsFile);

  //Open, read, close.
  filePtr = fopen(filePath, "r");
  fscanf(filePtr, "%u", &retVal);
  fclose(filePtr);

  return retVal;
}

int main (int argc, char* argv[]) {
  unsigned int txBytes;
  char sTxBytes[20]; //String to hold txBytes.

  txBytes = GetStat("tx_bytes");
  sprintf(sTxBytes, "%u", txBytes);

  //Init ncurses.
  NCInit();

  //Do stuff.
  printw(sTxBytes);

  //Print it on the screen.
  refresh();

  //Get user input.
  getch();

  //End ncurses.
  NCExit();

  return 0;
}
