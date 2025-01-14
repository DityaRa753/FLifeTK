# FLifeTK
![Cells, example](./cells.png)
## Introduction
Conway's Game of Life is another implementation. See more at https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
`FLifeTk` is a cross-platform cellular automaton.
# Build instruction
## Prerequisite
1. C++98 compliant compiler, **llvm** or **gcc**;
2. Fast Light Toolkit min. 1.3.8(FLTK), download https://www.fltk.org/ or usage package manager(pkg. name fltk or libfltk1.3);
3. fltk-config, script to compile, build-time dependencies, most likely it's include with fltk package;
4. make.
### Debian/Ubuntu
```
apt install libfltk1.3 libfltk1.3-dev
```
### macOS
```
brew install fltk
```
# How to compile?
Just go to project directory and do:
```
make
```
# How to start?
Run **./flifetk** in current directory and make(click) cells at the field, then press '**Space**' and watch.
# Control(Just press a key):
By default, for make a cell need to press 'Mouse 1' or 'Mouse 2' at the field, but, some keys override this behavior, switching them to hover mode. To cancel this mode, simply press **this** key again.

1. ESC(Escape) to exit;
2. 'Space' to pause or run;
3. 'c' to clear screen(kill all cells);
4. 'g' to hide grid(a little lag is possible) or to show grid;
5. 'd' to draw mode(hover) or to click mode;
6. 'e' to erase mode(hover), clear existing cells or to click node.
# Author
Petukhovsky Roman, romario586225@gmail.com
# License 
If you really need to look into the LICENSE