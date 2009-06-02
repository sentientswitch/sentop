//--------------------------------------------------------------//
//      sentop.c                                                //
//                                                              //
//      contact: sentientswitch@gmail.com                       //
//--------------------------------------------------------------//


#include <ncurses.h>
#include <stdio.h>

//Global Vars.
char STATS_PATH[100] = "/sys/class/net/eth0/statistics/"; //Interface statistics path.
static WINDOW* ROOT_WIN; //Pointer to root window.

//Structs.

//--------------------------------------------------------------//
//  snap_all                                                    //
//                                                              //
//  Structure to hold a snapshot of all statistics.             //
//--------------------------------------------------------------//
struct snap_all {
  unsigned int rx_bytes,           tx_bytes;
  unsigned int rx_packets,         tx_packets;
  unsigned int rx_errors,          tx_errors;
  unsigned int rx_dropped,         tx_dropped;
  unsigned int rx_fifo_errors,     tx_fifo_errors;
  unsigned int rx_compressed,      tx_compressed;

  unsigned int rx_crc_errors,      rx_frame_errors,      rx_length_errors,  rx_missed_errors,  rx_over_errors;
  unsigned int tx_carrier_errors,  tx_heartbeat_errors,  tx_window_errors,  tx_aborted_errors;

  unsigned int collisions,         multicast;
};

//--------------------------------------------------------------//
//  snap_min                                                    //
//                                                              //
//  Structure to hold a snapshot of basic statistics.           //
//--------------------------------------------------------------//
struct snap_min {
  unsigned int rx_bytes,           tx_bytes;
};

//Functions.

void NCInit();
void NCExit();
unsigned int GetStat(char* statsFile);
struct snap_all GetSnapAll();
struct snap_min GetSnapMin();

//--------------------------------------------------------------//
//  NCInit                                                      //
//                                                              //
//  Functions for starting ncurses environment.                 //
//--------------------------------------------------------------//
void NCInit() {
  //Init main window.
  ROOT_WIN = initscr();

  //Various ncurses options.
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  //Colours.
  start_color();
  init_pair(1, COLOR_GREEN,  COLOR_BLACK);
  init_pair(2, COLOR_RED,    COLOR_BLACK);
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
//  GetSnapAll                                                  //
//                                                              //
//  Fills a snap_all structure with data.                       //
//--------------------------------------------------------------//
struct snap_all GetSnapAll() {
  struct snap_all retSnap;

  //Get shared vals.
  //                   rx                                                    tx
  retSnap.rx_bytes             = GetStat("rx_bytes");           retSnap.tx_bytes        = GetStat("tx_bytes");
  retSnap.rx_packets           = GetStat("rx_packets");         retSnap.tx_packets      = GetStat("tx_packets");
  retSnap.rx_errors            = GetStat("rx_errors");          retSnap.tx_errors       = GetStat("tx_errors");
  retSnap.rx_dropped           = GetStat("rx_dropped");         retSnap.tx_dropped      = GetStat("tx_dropped");
  retSnap.rx_fifo_errors       = GetStat("rx_fifo_errors");     retSnap.tx_fifo_errors  = GetStat("tx_fifo_errors");
  retSnap.rx_compressed        = GetStat("rx_compressed");      retSnap.tx_compressed   = GetStat("tx_compressed");

  //Get rx only vals.
  retSnap.rx_crc_errors        = GetStat("rx_crc_errors");
  retSnap.rx_frame_errors      = GetStat("rx_frame_errors");
  retSnap.rx_length_errors     = GetStat("rx_length_errors");
  retSnap.rx_missed_errors     = GetStat("rx_missed_errors");
  retSnap.rx_over_errors       = GetStat("rx_over_errors");

  //Get tx only vals.
  retSnap.tx_carrier_errors    = GetStat("tx_carrier_errors");
  retSnap.tx_heartbeat_errors  = GetStat("tx_heartbeat_errors");
  retSnap.tx_window_errors     = GetStat("tx_window_errors");
  retSnap.tx_aborted_errors    = GetStat("tx_aborted_errors");

  //Get other vals.
  retSnap.collisions           = GetStat("collisions");
  retSnap.multicast            = GetStat("multicast");

  return retSnap;
}

//--------------------------------------------------------------//
//  GetSnapMin                                                  //
//                                                              //
//  Fills a snap_all structure with data.                       //
//--------------------------------------------------------------//
struct snap_min GetSnapMin() {
  struct snap_min retSnap;

  //                   rx                                                    tx
  retSnap.rx_bytes             = GetStat("rx_bytes");           retSnap.tx_bytes        = GetStat("tx_bytes");

  return retSnap;
}

//--------------------------------------------------------------//
//  PrintSnapAll                                                //
//                                                              //
//  Prints a snap_all structure.                                //
//--------------------------------------------------------------//
void PrintSnapAll(WINDOW* targetWin, struct snap_all* snapToPrnt) {
  char sBuffer[20];
  char sBuffer2[20];
  int x1 = 3;
  int x2 = 31;

  sprintf(sBuffer, "%u", snapToPrnt->rx_bytes);                                      sprintf(sBuffer2, "%u", snapToPrnt->tx_bytes);
  attron(COLOR_PAIR(1));        mvwprintw(targetWin, 2, x1, "rx_bytes:       ");     standend();       wprintw(targetWin, sBuffer);
  attron(COLOR_PAIR(2));        mvwprintw(targetWin, 2, x2, "tx_bytes:       ");     standend();       wprintw(targetWin, sBuffer2);
  sprintf(sBuffer, "%u", snapToPrnt->rx_packets);                                    sprintf(sBuffer2, "%u", snapToPrnt->tx_packets);
  attron(COLOR_PAIR(1));        mvwprintw(targetWin, 3, x1, "rx_packets:     ");     standend();       wprintw(targetWin, sBuffer);
  attron(COLOR_PAIR(2));        mvwprintw(targetWin, 3, x2, "tx_packets:     ");     standend();       wprintw(targetWin, sBuffer2);
  sprintf(sBuffer, "%u", snapToPrnt->rx_errors);                                     sprintf(sBuffer2, "%u", snapToPrnt->tx_errors);
  attron(COLOR_PAIR(1));        mvwprintw(targetWin, 4, x1, "rx_errors:      ");     standend();       wprintw(targetWin, sBuffer);
  attron(COLOR_PAIR(2));        mvwprintw(targetWin, 4, x2, "tx_errors:      ");     standend();       wprintw(targetWin, sBuffer2);
  sprintf(sBuffer, "%u", snapToPrnt->rx_dropped);                                    sprintf(sBuffer2, "%u", snapToPrnt->tx_dropped);
  attron(COLOR_PAIR(1));        mvwprintw(targetWin, 5, x1, "rx_dropped:     ");     standend();       wprintw(targetWin, sBuffer);
  attron(COLOR_PAIR(2));        mvwprintw(targetWin, 5, x2, "tx_dropped:     ");     standend();       wprintw(targetWin, sBuffer2);
  sprintf(sBuffer, "%u", snapToPrnt->rx_fifo_errors);                                sprintf(sBuffer2, "%u", snapToPrnt->tx_fifo_errors);
  attron(COLOR_PAIR(1));        mvwprintw(targetWin, 6, x1, "rx_fifo_errors: ");     standend();       wprintw(targetWin, sBuffer);
  attron(COLOR_PAIR(2));        mvwprintw(targetWin, 6, x2, "tx_fifo_errors: ");     standend();       wprintw(targetWin, sBuffer2);
  sprintf(sBuffer, "%u", snapToPrnt->rx_compressed);                                 sprintf(sBuffer2, "%u", snapToPrnt->tx_compressed);
  attron(COLOR_PAIR(1));        mvwprintw(targetWin, 7, x1, "rx_compressed:  ");     standend();       wprintw(targetWin, sBuffer);
  attron(COLOR_PAIR(2));        mvwprintw(targetWin, 7, x2, "tx_compressed:  ");     standend();       wprintw(targetWin, sBuffer2);
}

//--------------------------------------------------------------//
//  main                                                        //
//--------------------------------------------------------------//
int main (int argc, char* argv[]) {
  bool contLoop = true;         //True until we want to exit main loop.
  char inpCmd;                  //Store user input.
  struct snap_all snapAll;      //Main snapshot structure.
  struct snap_min snapMin;      //Basic snapshot structure.

  //Init ncurses.
  NCInit();

  //Draw windows etc.
  box(ROOT_WIN, ACS_VLINE, ACS_HLINE);

  snapAll = GetSnapAll();
  PrintSnapAll(ROOT_WIN, &snapAll);

  //Main loop for user interaction.
  while (contLoop) {
    refresh();
    inpCmd = getch();

    switch (inpCmd) {
      case 'q':
        contLoop = false;
      default:
        snapAll = GetSnapAll();
        PrintSnapAll(ROOT_WIN, &snapAll);
    }
  }

  //End ncurses.
  NCExit();

  return 0;
}
