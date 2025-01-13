#include "field.hpp"
#include <FL/Fl_Output.H>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef FOREVER
#define FOREVER 1e20
#endif
#ifndef TIME_DELAY
#define TIME_DELAY 5e8
#endif

enum { wh_cell = 10, dead_color = FL_BLACK, living_color = FL_WHITE };

Field::Field(Fl_Window *a_win, int x, int y, int w, int h)
  : Fl_Widget(x, y, w, h)
{
  int i, j, x1 = 0, y1 = 0;
  win = a_win;
  wsize = w;
  hsize = h;
  rowsize = w / wh_cell;
  colsize = h / wh_cell;
  maxi = rowsize-1;
  maxj = colsize-1;
  pause = 1;
  paint_flag = 0;
  run = 1;
  grid_enable = 1;
  erase_flag = 0;
  win->color(FL_WHITE);
  cells = new Field::Cell*[rowsize];
  for(i = 0; i < rowsize; i++)
    cells[i] = new Field::Cell[colsize];
  for(i = 0; i < rowsize; i++, x1 += wh_cell)
    for(j = 0, y1 = 0; j < colsize; j++, y1 += wh_cell)
      cells[i][j].REinit(this, x1, y1, wh_cell - grid_enable);
}


void Field::List::DeleteList(Node *n)
{
  if(n) {
    Node *next = n->next;
    delete n;
    DeleteList(next);
  }
}

Field::Cell* Field::List::Pop()
{
  if(!first)
    return 0;
  Node *n = first;
  Cell *val = n->cell;
  first = n->next;
  delete n;
  return val;
}

Field::~Field()
{
  for(int i = 0; i < rowsize; i++)
    delete [] cells[i];
  delete [] cells;
}


int Field::Run()
{
  timespec waitsec = {
    0, TIME_DELAY
  };
  timespec unslept;
  while(run || Fl::program_should_quit()) {
    if(pause) {
      Fl::wait(FOREVER);
      continue;
    }
    Fl::lock();
    CalculatedCells();
    nanosleep(&waitsec, &unslept);
    Fl::unlock();
    Fl::awake();
    if(Fl::wait(FOREVER) < 0) {
      perror("Fl::wait() ");
      return 1;
    }
  }
  return 0;
}

void Field::Corner(int i, int j, int &alive) // Matrix 2x2
{
  int k, c, q, v, idx1, idx2;
  alive = 0;
  for(k = i, q = 0; q < 2; q++) {
    idx1 = k + q;
    if(idx1 > maxi)
      idx1 = k - q;
    for(c = j, v = 0; v < 2; v++) {
      idx2 = c + v;
      if(idx2 > maxj)
        idx2 = c - v;
      if(idx1 == i && idx2 == j) // skip cell, we check neighbors.
        continue;
      if(cells[idx1][idx2].State())
        ++alive;
    }
  }
}

void Field::SideVertical(int i, int j, int &alive) // Matrix 3x2
{
  int k, c, q, v, idx1, idx2;
  alive = 0;
  for(k = i-1, q = 0; q < 3; q++) { // we are sure is it possible i-1
    idx1 = k + q;
    for(c = j, v = 0; v < 2; v++) {
      idx2 = c + v;
      if(idx2 > maxj)
        idx2 = c - v;
      if(k == idx1 && c == idx2)
        continue;
      if(cells[idx1][idx2].State())
        ++alive;
    }
  }
}

void Field::SideHorizontal(int i, int j, int &alive)// Matrix2x3
{
  int k, c, v, q, idx1, idx2;
  alive = 0;
  for(k = i, q = 0; q < 2; q++) {
    idx1 = k + q;
    if(idx1 > maxi)
      idx1 = k - q;
    for(c = j-1, v = 0; v < 3; v++) {
      idx2 = c + v;
      if(idx1 == i && idx2 == j)
        continue;
      if(cells[idx1][idx2].State())
        ++alive;
   }
  }
}

void Field::Internal(int i, int j, int &alive) // Matrix 3x3
{
  int k, c;
  alive = 0;
  for(k = i-1; k < i+2; k++) {
    for(c = j-1; c < j+2; c++) {
      if(k == i && c == j)
        continue;
      if(cells[k][c].State())
        ++alive;
    }
  }
}

Field::position Field::DefinePosition(int i, int j)
{
  int calc =
    (i == 0 && j == 0) ||
    (i == maxi && j == maxj) ||
    (i == 0 && j == maxj) ||
    (i == maxi && j == 0);
  if(calc) return corner;
  calc =
    ((i > 0 && i < maxi) && (j == 0 || j == maxj)); // left,right-side
  if(calc) return sidev;
  calc = ((i == 0 || i == maxi) && (j > 0 && j < maxj));
  if(calc) return sideh;
  return internal;
}

void Field::CalculatedCells()
{
  int alive;
  position pos;
  Field::Cell *cell;
  for(int i = 0; i < rowsize; i++) {
    for(int j = 0; j < colsize; j++) {
      pos = DefinePosition(i, j);
      switch(pos) {
        case corner:
          Corner(i, j, alive);
          break;
        case sidev:
          SideVertical(i, j, alive);
          break;
        case sideh:
          SideHorizontal(i, j, alive);
          break;
        case internal:
            Internal(i, j, alive);
          break;
        default:
          break;
      }
      if(cells[i][j].State()) { // cell living.
        if(alive < 2 || alive > 3)
          dead_cells.Push(&cells[i][j]);
      }
      else { // cell dead.
        if(alive == 3)
          new_cells.Push(&cells[i][j]);
      }
    }
  }
  for(cell = dead_cells.Pop(); cell; cell = dead_cells.Pop())
    cell->Die();
  for(cell = new_cells.Pop(); cell; cell = new_cells.Pop())
    cell->Raise();
}

void Field::draw()
{
}
/*
void Field::DrawGrid()
{
  int x1 = 0, x2 = w(), y1 = wh_cell, y2 = wh_cell;
  for(int i = 0; i < maxi; i++) { // horizontal
    fl_line(x1, y1, x2, y1);
    y2 += wh_cell; y1 += wh_cell;
  }
  x1 = wh_cell; x2 = wh_cell; y1 = 0; y2 = h();
  for(int j = 0; j < maxj; j++) { // vertical
    fl_line(x1, y1, x2, y2);
    x1 += wh_cell; x2 += wh_cell;
  }
}*/

void Field::DisableGrid(int newsize)
{
  for(int i = 0; i <= maxi; i++)
    for(int j = 0; j <= maxj; j++)
      cells[i][j].Resize(newsize);
}

void Field::ShowGrid(int newsize)
{
  for(int i = 0; i <= maxi; i++) {
    for(int j = 0; j <= maxj; j++) {
      if(cells[i][j].State()) {
        cells[i][j].SetColor(dead_color);
        cells[i][j].Resize(newsize);
        cells[i][j].SetColor(living_color);
      }
      else {
        cells[i][j].SetColor(living_color);
        cells[i][j].Resize(newsize);
        cells[i][j].SetColor(dead_color);
      }
    }
  }
  win->color(FL_GRAY);
  win->redraw();
}

int Field::handle(int e)
{
  // Need to respond to FOCUS events in order to handle keyboard
  if (e == FL_FOCUS || e == FL_UNFOCUS)
    return 1;
  if(e == FL_SHORTCUT) {
    switch(Fl::event_key()) {
      case ' ':
        if(pause) {
          pause = 0;
          win->cursor(FL_CURSOR_WAIT);
        }
        else{
          win->default_cursor(FL_CURSOR_DEFAULT);
          pause = 1;
        }
        return 1;
      case FL_Escape:
        run = 0;
        this->hide();
        win->hide();
        return 1;
      case 'c':
        if(pause) {
          KillAllTheCells();
          return 1;
        }
        return 0;
      case 'd':
        if(paint_flag)
          paint_flag = 0;
        else
          paint_flag = 1;
        return 1;
      case 'g':
        if(grid_enable) {
          grid_enable = 0;
          DisableGrid(wh_cell);
        }
        else {
          grid_enable = 1;
          ShowGrid(wh_cell-1);
        }
        return 1;
      case 'e':
        if(erase_flag)
          erase_flag = 0;
        else
          erase_flag = 1;
        return 1;
      default:
      break;
    }
  }
  return 0;
}

void Field::KillAllTheCells()
{
  for(int i = 0; i <= maxi; i++)
    for(int j = 0; j <= maxj; j++)
      if(cells[i][j].State())
        cells[i][j].Die();
}

// Cell section
void Field::Cell::REinit(Field *f, int x1, int y1, int cell_size)
{
  resize(x1, y1, cell_size, cell_size);
  master = f;
}

void Field::Cell::InvertState()
{
  if(visible) {
    visible = 0;
    color(FL_BLACK);
  }
  else {
    visible = 1;
    color(FL_WHITE);
  }
}

void Field::Cell::Raise()
{
  visible = 1;
  color(FL_WHITE);
  redraw();
}

void Field::Cell::Die()
{
  visible = 0;
  color(FL_BLACK);
  redraw();
}

int Field::Cell::handle(int e)
{
  switch(e) {
    case FL_PUSH:
      InvertState();
      redraw();
      return 1;
    case FL_ENTER:
      if(master->PaintMouse()) {
        InvertState();
        redraw();
        return 1;
      }
      else if(master->EraseCells()) {
        if(State()) {
          InvertState();
          redraw();
        }
        return 1;
      }
      return 0;
    default:
      return Fl_Widget::handle(e);
  }
}

void Field::Cell::Resize(int newsize)
{
  size(newsize, newsize);
  redraw();
}
