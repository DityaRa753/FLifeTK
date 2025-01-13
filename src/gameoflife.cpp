#include "field.hpp"

enum { window_size = 1000 };

int main(int argc, char **argv)
{
  Fl_Window *window = new Fl_Window(window_size, window_size);
  Field field(window, 300, 300, window_size, window_size);
  window->end();
  window->show(argc, argv);
  return field.Run();
}
