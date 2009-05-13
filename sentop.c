//                                              //
//      sentop.c                                //
//                                              //
//      contact: sentientswitch@gmail.com       //
//                                              //


#include <ncurses.h>

int main (int argc, char* argv[]) {

  //Init ncurses.
  initscr();

  //Do stuff.
  printw("Hello World !!!");

  //Print it on the screen.
  refresh();

  //Get user input.
  getch();

  //End ncurses.
  endwin();

  return 0;
}
