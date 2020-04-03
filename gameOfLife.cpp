#include "../Clock.hpp"
#include <iostream>

//table for quicker look-up of whether neighbors are living or dead
const bool prev_and_alive_neighbors_to_next[2][9] = {
  {false, false, false, true, false, false, false, false, false},
  {false, false, false, true, true, false, false, false, false}
};

//check neighbors of single cell
bool advance_single(const bool*prev, const unsigned int R, const unsigned int C, int r, int c) {
  unsigned int columnDown2 = C+2;
  unsigned int living_neighbors = 0;
  for (int i=-1; i<=1; ++i) {
    for (int j=-1; j<=1; ++j) {
	    living_neighbors += prev[(r+i)*(columnDown2)+(c+j)];
    }
  }
  bool is_this_cell_living = prev[r*(columnDown2)+c];
  return prev_and_alive_neighbors_to_next[is_this_cell_living][living_neighbors];
}

//check neighbors of all cells
void advance(bool*cur, const bool*prev, const unsigned int R, const unsigned int C) {
  unsigned int rowToRight = R+1;
  unsigned int columnDown = C+1;
  unsigned int columnDown2 = C+2;
  for (unsigned int i=1; i<rowToRight; ++i)
    for (unsigned int j=1; j<columnDown; ++j)
      cur[i*(columnDown2)+j] = advance_single(prev, R, C, i, j);
}

void print_board(const bool*board, const unsigned int R, const unsigned int C) {
  for (unsigned int i=1; i<R+1; ++i) {
    for (unsigned int j=1; j<C+1; ++j)
      std::cout << int(board[i*(C+2)+j]);
    std::cout << std::endl;
  }
}

int main(int argc, char**argv) {
  if (argc == 4) {
    const unsigned int R = 1<<10;
    const unsigned int C = 1<<11;

    unsigned long random_seed = atoi(argv[1]);
    unsigned int NUM_REPS = atoi(argv[2]);
    bool print = bool( atoi(argv[3]) );

    srand(random_seed);
    bool*prev = (bool*)calloc((R+2)*(C+2),sizeof(bool));
    bool*cur = (bool*)calloc((R+2)*(C+2),sizeof(bool));

    for (unsigned int i=1; i<R+1; ++i)
      for (unsigned int j=1; j<C+1; ++j)
	     cur[i*(C+2)+j] = (rand() % 2 == 0);

    if (print)
      print_board(cur, R, C);

    Clock c;

    for (unsigned int rep=0; rep<NUM_REPS; ++rep) {
      std::swap(cur, prev);
      advance(cur, prev, R, C);
    }

    c.ptock();

    if (print)
      print_board(cur, R, C);
  }
  else
    std::cerr << "usage game-of-life <generations> <0:don't print, 1:print>" << std::endl;
  return 0;
}
