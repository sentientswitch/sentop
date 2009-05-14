//--------------------------------------------------------------//
//      sentop.c                                                //
//                                                              //
//      contact: sentientswitch@gmail.com                       //
//--------------------------------------------------------------//


#include <ncurses.h>
#include <stdio.h>

//Global Vars.
char STATS_PATH[100] = "/sys/devices/pci0000:00/0000:00:1c.5/0000:04:00.0/net/eth0/statistics/"; //Interface statistics path.
static WINDOW* ROOT_WIN; //Pointer to root window.

//Functions.
void NCInit();
void NCExit();
unsigned int GetStat(char* statsFile);

//--------------------------------------------------------------//
//  NCInit                                                      //
//                                                              //
//  Functions for starting ncurses environment.                 //
//--------------------------------------------------------------//
void NCInit() {
  ROOT_WIN = initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
}

//--------------------------------------------------------------//
//  NCExit                                                      //
//                                                              //
//  Functions for ending ncurses.                               //
//--------------------------------------------------------------//
void NCExit() {
  endwin();
}

//--------------------------------------------------------------//
//  GetStat                                                     //
//                                                              //
//  Takes name of statistics file.                              //
//                                                              //
//  Returns number from statistics file as unsigned int.        //
//--------------------------------------------------------------//
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

//--------------------------------------------------------------//
//  main                                                        //
//--------------------------------------------------------------//
int main (int argc, char* argv[]) {
  bool contLoop = true; //True until we want to exit main loop.
  char inpCmd; //Store user input.
  unsigned int txBytes;
  char sTxBytes[20]; //String to hold txBytes.

  txBytes = GetStat("tx_bytes");
  sprintf(sTxBytes, "%u", txBytes);

  //Init ncurses.
  NCInit();

  //Do stuff.
  box(ROOT_WIN, ACS_VLINE, ACS_HLINE);
  printw(sTxBytes);

  //Print it on the screen.
  refresh();

  //Get user input.
  while (contLoop) {
    inpCmd = getch();
    if (inpCmd == 'q') {
      contLoop = false;
    }
  }

  //End ncurses.
  NCExit();

  return 0;
}
