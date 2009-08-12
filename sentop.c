//--------------------------------------------------------------//
//      sentop.c                                                //
//                                                              //
//      contact: sentientswitch@gmail.com                       //
//--------------------------------------------------------------//


#include <ncurses.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

//Global Vars.
char STATS_PATH[100] = "/sys/class/net/eth0/statistics/"; //Interface statistics path.
static WINDOW* ROOT_WIN; //Pointer to root window.
unsigned int TIME_DELAY = 3; //Time between data points.

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

  time_t time;
};

//--------------------------------------------------------------//
//  snap_min                                                    //
//                                                              //
//  Structure to hold a snapshot of basic statistics.           //
//--------------------------------------------------------------//
struct snap_min {
  unsigned int rx_bytes,           tx_bytes;
  time_t time;
};


//--------------------------------------------------------------//
//  graph                                                       //
//                                                              //
//  Structure to store graph data and options.                  //
//--------------------------------------------------------------//
struct graph_data {
  ////Graph Options.
  //unsigned char samples; //Number of samples to store.
  //char graphChar; //Character to draw graph with.

  //Data.
  unsigned int maxVal; //Keeps track of maximum value observed so far.
  unsigned int vals[]; //Array of data points.
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
  cbreak(); //Get individual key presses.
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0); //Make cursor invisible.

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

  //Get full file path.
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

  //Get time.
  retSnap.time                 = time(NULL);

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

  //Get time.
  retSnap.time                 = time(NULL);

  return retSnap;
}

//--------------------------------------------------------------//
//  FillSnapAll                                                 //
//                                                              //
//  Copies a snap_all struct from one to another.               //
//--------------------------------------------------------------//
void FillSnapAll(struct snap_all* srcSnap, struct snap_all* destSnap) {
  destSnap->rx_bytes       = srcSnap->rx_bytes;         destSnap->tx_bytes       = srcSnap->tx_bytes;
  destSnap->rx_packets     = srcSnap->rx_packets;       destSnap->tx_packets     = srcSnap->tx_packets;
  destSnap->rx_errors      = srcSnap->rx_errors;        destSnap->tx_errors      = srcSnap->tx_errors;
  destSnap->rx_dropped     = srcSnap->rx_dropped;       destSnap->tx_dropped     = srcSnap->tx_dropped;
  destSnap->rx_fifo_errors = srcSnap->rx_fifo_errors;   destSnap->tx_fifo_errors = srcSnap->tx_fifo_errors;
  destSnap->rx_compressed  = srcSnap->rx_compressed;    destSnap->tx_compressed  = srcSnap->tx_compressed;

  destSnap->rx_crc_errors    = srcSnap->rx_crc_errors;
  destSnap->rx_frame_errors  = srcSnap->rx_frame_errors;
  destSnap->rx_length_errors = srcSnap->rx_length_errors;
  destSnap->rx_missed_errors = srcSnap->rx_missed_errors;
  destSnap->rx_over_errors   = srcSnap->rx_over_errors;

  destSnap->tx_carrier_errors   = srcSnap->tx_carrier_errors;
  destSnap->tx_heartbeat_errors = srcSnap->tx_heartbeat_errors;
  destSnap->tx_window_errors    = srcSnap->tx_window_errors;
  destSnap->tx_aborted_errors   = srcSnap->tx_aborted_errors;

  destSnap->collisions = srcSnap->collisions;
  destSnap->multicast  = srcSnap->multicast;

  destSnap->time = srcSnap->time;
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
//  PrintRates                                                  //
//                                                              //
//  Prints tx and rx rates.                                     //
//--------------------------------------------------------------//
void PrintRates(WINDOW* targetWin, float rxRate, float txRate) {
  char sBuffer[20];

  sprintf(sBuffer, "%f", rxRate);
  attron(COLOR_PAIR(1));        mvwprintw(targetWin, 9, 3,  "rx rate:        ");    standend();        wprintw(targetWin, sBuffer);

  sprintf(sBuffer, "%f", txRate);
  attron(COLOR_PAIR(2));        mvwprintw(targetWin, 9, 31, "tx rate:        ");    standend();        wprintw(targetWin, sBuffer);
}

//--------------------------------------------------------------//
//  PrintGraph                                                  //
//                                                              //
//  Prints a graph of network activity.                         //
//--------------------------------------------------------------//
void PrintGraph(WINDOW* targetWin, struct graph_data graphData) {

}

//--------------------------------------------------------------//
//  GetRates                                                    //
//                                                              //
//  Calculates the average rx and tx rates between two times.   //
//---- 
  deltaT = time - timep;

  rxRate  = rx - rxp;
  rxRate /= deltaT;
  rxRate /= 1024;

  txRate  = tx - txp;
  txRate /= deltaT;
  txRate /= 1024;

----------------------------------------------------------//
void GetRates(unsigned int rx,   unsigned int rxp, 
              unsigned int tx,   unsigned int txp, 
              unsigned int time, unsigned int timep,
              float* rxRate,     float* txRate) {
  unsigned int deltaT;

  deltaT = time - timep;

  *rxRate  = rx - rxp;
  *rxRate /= deltaT;
  *rxRate /= 1024;

  *txRate  = tx - txp;
  *txRate /= deltaT;
  *txRate /= 1024;
}

//--------------------------------------------------------------//
//  main                                                        //
//--------------------------------------------------------------//
int main (int argc, char* argv[]) {
  bool contLoop = true;         //True until we want to exit main loop.
  char inpCmd;                  //Store user input (keystroke).
  struct snap_all snapAll;      //Main snapshot structure.
  struct snap_all snapAllPrev;  //Snapshot of previous data.
  struct graph_data graphData;  //Holds previous rx,tx rates for graphing purposes.
  float rxRate, txRate;         //Holds last calculated rx and tx rates.

  //Init ncurses.
  NCInit();

  //Draw windows etc.
  box(ROOT_WIN, ACS_VLINE, ACS_HLINE);

  //Fill window with data before 1st refresh.
  snapAll = GetSnapAll();
  PrintSnapAll(ROOT_WIN, &snapAll);

  //Main loop for user interaction.
  while (contLoop) {
    refresh();

/*    inpCmd = getch();

    switch (inpCmd) {
      case 'q':
        contLoop = false;
        break;
      default:*//*
        FillSnapAll(&snapAll, &snapAllPrev);
    sleep(TIME_DELAY);
        snapAll = GetSnapAll();

        PrintSnapAll(ROOT_WIN, &snapAll);
        PrintRates(ROOT_WIN,
                   snapAll.rx_bytes, snapAllPrev.rx_bytes,
                   snapAll.tx_bytes, snapAllPrev.tx_bytes,
                   snapAll.time,     snapAllPrev.time);
    //}*/

    FillSnapAll(&snapAll, &snapAllPrev); //Copy current data to snapAllPrev.
    sleep(TIME_DELAY); //Wait before getting more data.
    snapAll = GetSnapAll(); //Get data.

    PrintSnapAll(ROOT_WIN, &snapAll); //Print the data to the main window.
    GetRates(snapAll.rx_bytes, snapAllPrev.rx_bytes,
             snapAll.tx_bytes, snapAllPrev.tx_bytes,
             snapAll.time,     snapAllPrev.time,
             &rxRate,          &txRate); //Calculate average rx & tx data rates for wait period. 
    PrintRates(ROOT_WIN, rxRate, txRate);
    //PrintGraph(ROOT_WIN, graphData);
  }

  //End ncurses.
  NCExit();

  return 0;
}
