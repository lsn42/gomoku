#include <stdio.h>
#include <windows.h>

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
  char display()
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
  BlackWhitePiece operator==(const BlackWhitePiece& p)
  {
    return this->type == p.type;
  }
};

template<class P>
class BoardGame
{
 public:
  int width, height;
  vector<vector<P>> board;
  BoardGame() {}
  BoardGame(int width, int height):
    width(width), height(height), board(width, vector<P>(height))
  {
  }
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
 public:
  Gomoku(): BoardGame<BlackWhitePiece>(15, 15) {}
  void display() { ((BoardGame<BlackWhitePiece>*)this)->display(); }
};

int main(void)
{
  Gomoku g;
  bool state = false;

  // g.place(BlackWhitePiece(true), 1, 2);
  // g.display();

  // 获取标准输入输出设备句柄
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFO bInfo;
  INPUT_RECORD mouseRec;
  DWORD res;
  COORD crPos, crHome = {1, 0}, crDisplay = {0, 15};

  DWORD mode;
  GetConsoleMode(hIn, &mode);
  mode &= ~ENABLE_QUICK_EDIT_MODE; //移除快速编辑模式
  SetConsoleMode(hIn, mode);
  CONSOLE_CURSOR_INFO CursorInfo;
  GetConsoleCursorInfo(hOut, &CursorInfo); //获取控制台光标信息
  CursorInfo.bVisible = false;             //隐藏控制台光标
  SetConsoleCursorInfo(hOut, &CursorInfo); //设置控制台光标状态

  // GetConsoleScreenBufferInfo(hOut, &bInfo);
  SetConsoleCursorPosition(hOut, crDisplay);
  printf("[Cursor Position] X: %3u  Y: %3u", crPos.X, crPos.Y);
  SetConsoleCursorPosition(hOut, crHome);
  g.display();
  // SetConsoleCursorPosition(hOut, bInfo.dwCursorPosition);

  while (true)
  {
    ReadConsoleInput(hIn, &mouseRec, 1, &res);

    if (mouseRec.EventType == MOUSE_EVENT)
    {
      if (mouseRec.Event.MouseEvent.dwButtonState ==
        FROM_LEFT_1ST_BUTTON_PRESSED)
      {
        if (mouseRec.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK)
        {
          break; // 左键双击 退出循环
        }
      }

      crPos = mouseRec.Event.MouseEvent.dwMousePosition;
      SetConsoleCursorPosition(hOut, crDisplay);
      printf("[Cursor Position] X: %3u  Y: %3u", crPos.X, crPos.Y);

      switch (mouseRec.Event.MouseEvent.dwButtonState)
      {
        case FROM_LEFT_1ST_BUTTON_PRESSED: // 左键 输出A
          if (!(crPos.X % 2) && crPos.X < 15 * 2 && crPos.Y < 15)
          {
            g.place(BlackWhitePiece(state = !state), crPos.Y, crPos.X / 2);
            MessageBeep(MB_OK);
          }
          // FillConsoleOutputCharacter(hOut, '@', 1, crPos, &res);
          break; // 如果使用printf输出，则之前需要先设置光标的位置

        default:
          break;
      }
      SetConsoleCursorPosition(hOut, crHome);
      g.display();
    }
  }

  CloseHandle(hOut); // 关闭标准输出设备句柄
  CloseHandle(hIn);  // 关闭标准输入设备句柄
  return 0;
}