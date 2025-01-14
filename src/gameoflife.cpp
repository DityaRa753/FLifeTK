#include "field.hpp"
#include <stdio.h>

enum { x = 150, y = 150, window_size = 1000 };

int main(int argc, char **argv)
{
  Fl_Window *window = new Fl_Window(window_size, window_size);
  Field field(window, x, y, window_size, window_size, argc, argv);
	return field.Run();
}
