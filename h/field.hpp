#ifndef FIELD_H_SENTRY
#define FIELD_H_SENTRY

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

class Field : public Fl_Widget {
  class Cell : public Fl_Box {
    Field *master;
    unsigned char visible;
  private:
    void InvertState();
  public:
    Cell(Field *m = 0, int x = 0, int y = 0, int w = 0, int h = 0,
        const char *l = 0, Fl_Boxtype t = FL_FLAT_BOX)
      : Fl_Box(t, x, y, w, h, l), visible(0)
    {
      color(FL_BLACK, FL_BLACK);
    }
    void REinit(Field*, int, int, int);
    void Resize(int);
    void SetColor(Fl_Color c) { color(c); redraw(); }
    unsigned int State() const { return visible; }
    void Raise();
    void Die();
    int handle(int);
    ~Cell() {  }
  };
  class List {
    struct Node {
      Cell *cell;
      Node *next;
      Node(Cell *c, Node *n) : cell(c), next(n) { } 
    };
    void DeleteList(Node*);
    Node *first;
  public:
    List() : first(0) { }
    void Push(Cell *c) { first = new Node(c, first); }
    Cell* Pop();
    ~List() { DeleteList(first); }
  };
  Fl_Window *win;
  Cell **cells;
  List dead_cells;
  List new_cells;
  unsigned int wsize, hsize;
  unsigned int maxi, maxj, rowsize, colsize;
  unsigned char pause, run, paint_flag, grid_enable, erase_flag;
  enum position { corner, sidev, sideh, internal };
private:
  int handle(int);
  void draw();
  void DrawGrid();
 // void InvertGrid(int);
  void DisableGrid(int);
  void ShowGrid(int);
  void CalculatedCells();
  position DefinePosition(int, int);
  void Corner(int, int, int&);
  void SideVertical(int, int, int&);
  void SideHorizontal(int, int, int&);
  void Internal(int, int, int&);
  void KillAllTheCells();
  static void timeout_cb(void*);
public:
  int PaintMouse() const { return paint_flag; }
  int EraseCells() const { return erase_flag; }
  Field(Fl_Window*, int, int, int, int);
  ~Field();
  int Run();
};

#endif
