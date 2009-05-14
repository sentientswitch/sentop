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

//Functions.
void NCInit();
void NCExit();
unsigned int GetStat(char* statsFile);
struct snap_all GetSnapAll();

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

  mvprintw(1, 1, "tx_bytes: ");
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
