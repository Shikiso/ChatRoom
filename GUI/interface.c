#include <ncurses.h>
#include <panel.h>


int main(){
  initscr();
  cbreak();
  noecho();

  int x, y;
  getmaxyx(stdscr, y, x);

  WINDOW *local_win = newwin(y, x, 10, 10);
  wborder(local_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(local_win);

  getch();
  endwin();

  return 0;
}
