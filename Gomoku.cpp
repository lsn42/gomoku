#include <stdio.h>
#include <windows.h>

#include <exception>
#include <stdexcept>
#include <vector>

using std::vector;

class BlackWhitePiece;
template<class P>
class BoardGame;

class BlackWhitePiece
{
  friend class BoardGame<BlackWhitePiece>;

 private:
  char type;

 public:
  BlackWhitePiece(): type(0){};
  BlackWhitePiece(bool type): type(type ? 1 : 2){};
  BlackWhitePiece(const BlackWhitePiece& p) = default;
  bool get_type() const { return type; }
  char display() const
  {
    if (type == 1)
    {
      return '@';
    }
    else if (type == 2)
    {
      return 'O';
    }
    else
    {
      return 0;
    }
  }
  bool operator==(const BlackWhitePiece& p) { return this->type == p.type; }
};

template<class P>
class BoardGame
{
 private:
  int width, height;
  vector<vector<P>> board;

 public:
  BoardGame() = delete;
  BoardGame(int width, int height):
    width(width), height(height), board(width, vector<P>(height))
  {
  }

  P get_piece(int x, int y) const { return board[x][y]; }

  void place(P p, int x, int y) { board[x][y] = p; }
  void remove(int x, int y) { board[x][y] = P(); }
  void display()
  {
    for (int i = 0; i < height; ++i)
    {
      char display;
      for (int j = 0; j < width; ++j)
      {
        display = board[i][j].display();
        if (display != 0)
        {
          printf("%c", display);
        }
        else
        {
          printf("+");
        }
        if (j != width - 1)
        {
          printf("-");
        }
      }
      if (i != height - 1)
      {
        printf("\n");
      }
    }
  }
};
class Gomoku: public BoardGame<BlackWhitePiece>
{
  const BlackWhitePiece BLACK = BlackWhitePiece(true);
  const BlackWhitePiece WHITE = BlackWhitePiece(false);

 private:
  bool player;
  int last_x, last_y;

 public:
  Gomoku(): BoardGame<BlackWhitePiece>(15, 15), player(true) {}

  bool get_state() const { return player; }

  void place(int x, int y)
  {
    if (get_piece(x, y).display() == 0)
    {
      last_x = x;
      last_y = y;
      ((BoardGame<BlackWhitePiece>*)this)->place(BlackWhitePiece(player), x, y);
      player = !player;
    }
    else
    {
      throw std::logic_error("overlap");
    }
  }
  void remove() = delete;
  void display() { ((BoardGame<BlackWhitePiece>*)this)->display(); }
  bool is_end()
  {
    BlackWhitePiece last_player = (!player) ? BLACK : WHITE;
    int i = last_x, j = last_y;
    int count = 0;

    // horizontal
    while (i > -1 && get_piece(i, j) == last_player)
    {
      i--;
      count++;
    }
    i = last_x + 1;
    while (i < 15 && get_piece(i, j) == last_player)
    {
      i++;
      count++;
    }
    if (count >= 5)
    {
      return true;
    }

    // vertical
    i = last_x;
    count = 0;
    while (j > -1 && get_piece(i, j) == last_player)
    {
      j--;
      count++;
    }
    j = last_y + 1;
    while (j < 15 && get_piece(i, j) == last_player)
    {
      j++;
      count++;
    }
    if (count >= 5)
    {
      return true;
    }

    // slope 1
    j = last_y;
    count = 0;
    while (i > -1 && j > -1 && get_piece(i, j) == last_player)
    {
      i--;
      j--;
      count++;
    }
    i = last_x + 1;
    j = last_y + 1;
    while (i < 15 && j < 15 && get_piece(i, j) == last_player)
    {
      i++;
      j++;
      count++;
    }
    if (count >= 5)
    {
      return true;
    }

    // slope 2
    i = last_x;
    j = last_y;
    count = 0;
    while (i < 15 && j > -1 && get_piece(i, j) == last_player)
    {
      i++;
      j--;
      count++;
    }
    i = last_x - 1;
    j = last_y + 1;
    while (i > -1 && j < 15 && get_piece(i, j) == last_player)
    {
      i--;
      j++;
      count++;
    }
    if (count >= 5)
    {
      return true;
    }

    return false;
  }
  bool winner() { return !player; }
};

int main(void)
{
  Gomoku g;

  HANDLE out_h = GetStdHandle(STD_OUTPUT_HANDLE); // output handle
  HANDLE in_h = GetStdHandle(STD_INPUT_HANDLE);   // input handle

  INPUT_RECORD mr; // mouse record
  DWORD res;       // ?

  COORD h_c = {0, 0};  // home coord
  COORD i_c = {31, 2}; // info coord
  COORD e_c = {31, 0}; // exit coord
  // COORD d_c = {0, 16}; // debug coord

  // disable quick edit
  DWORD cm; // console mode
  GetConsoleMode(in_h, &cm);
  cm &= ~ENABLE_QUICK_EDIT_MODE;
  SetConsoleMode(in_h, cm);

  // hide cursor
  CONSOLE_CURSOR_INFO ci; // cursor info
  GetConsoleCursorInfo(out_h, &ci);
  ci.bVisible = false;
  SetConsoleCursorInfo(out_h, &ci);

  // display
  SetConsoleCursorPosition(out_h, h_c);
  g.display();

  SetConsoleCursorPosition(out_h, e_c);
  printf("exit");

  while (true)
  {
    ReadConsoleInput(in_h, &mr, 1, &res);

    if (mr.EventType == MOUSE_EVENT)
    {
      COORD p_c = mr.Event.MouseEvent.dwMousePosition;

      if (mr.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
      {
        if (!(p_c.X % 2) && p_c.X < 15 * 2 && p_c.Y < 15)
        { // valid position to place
          try
          {
            g.place(p_c.Y, p_c.X / 2);
            MessageBeep(MB_OK);
          }
          catch (std::logic_error& e)
          {
            if (strcmp(e.what(), "overlap") == 0)
            {
              MessageBeep(MB_ICONERROR);
            }
            else
            {
              throw e;
            }
          }
          if (g.is_end())
          {
            SetConsoleCursorPosition(out_h, h_c);
            g.display();
            SetConsoleCursorPosition(out_h, i_c);
            if (g.winner())
            {
              printf("Black won the game!");
            }
            else
            {
              printf("White won the game!");
            }
            break;
          }
        }
        else if (p_c.X >= e_c.X && p_c.X < e_c.X + 4 && p_c.Y == e_c.Y)
        {
          goto end;
        }
        else
        {
          MessageBeep(MB_ICONERROR);
        }
      }
      SetConsoleCursorPosition(out_h, h_c);
      g.display();

      // info
      SetConsoleCursorPosition(out_h, i_c);
      if (g.get_state())
      {
        printf("Black turn");
      }
      else
      {
        printf("White turn");
      }

      // debug
      // SetConsoleCursorPosition(out_h, d_c);
      // printf("[Cursor Position] X: %3u  Y: %3u", p_c.X, p_c.Y);
    }
  }

  while (true)
  {
    ReadConsoleInput(in_h, &mr, 1, &res);
    if (mr.EventType == MOUSE_EVENT &&
      mr.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
      break;
    }
  }
end:
  Sleep(100);
  SetConsoleCursorPosition(out_h, i_c);
  printf("Game ended, click anywhere to exit");
  while (true)
  {
    ReadConsoleInput(in_h, &mr, 1, &res);
    if (mr.EventType == MOUSE_EVENT &&
      mr.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
      break;
    }
  }

  CloseHandle(out_h);
  CloseHandle(in_h);
  return 0;
}