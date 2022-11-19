#include <iostream>
#include <string>
#include <cstdlib>
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <random>

const int TWIDTH = 4;
const int THEIGHT = 4; 

const int FIELD_WIDTH = 12;
const int FIELD_HEIGHT = 18;

std::wstring tetromino[7];
unsigned char *field = nullptr;

using namespace std::chrono_literals;

void init_game() {
	
	field = new unsigned char[FIELD_WIDTH * FIELD_HEIGHT];
	tetromino[0].append(L"....");
	tetromino[0].append(L".XX.");
	tetromino[0].append(L".XX.");
	tetromino[0].append(L"....");

	tetromino[1].append(L"....");
	tetromino[1].append(L".X..");
	tetromino[1].append(L".X..");
	tetromino[1].append(L".XX.");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"..X.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".X..");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"..X.");

	tetromino[5].append(L"..X.");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"....");

	tetromino[6].append(L"....");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L".XX.");
}

void render() {
	for (size_t x = 0; x < FIELD_WIDTH; ++x) {
		for (size_t y = 0; y < FIELD_HEIGHT; ++y) {
			if (field[y * FIELD_WIDTH + x] == 9) mvaddch(y, x, L'#'); 
			else if(field[y * FIELD_WIDTH + x] == 8) mvaddch(y, x, L'A'); 
			else if(field[y * FIELD_WIDTH + x] == 7) mvaddch(y, x, L'O');
			else if(field[y * FIELD_WIDTH + x] == 0) mvaddch(y, x, '.'); 
		}
	}
	refresh();
}

int rotate(int px, int py, int rotation) {
	switch(rotation % 4) {
		case 0: return py * TWIDTH + px;
		case 1: return 12 + py - (px * 4);
		case 2: return 15 - (py * 4) - px;
		case 3: return 3 - py + (px * 4); 
	}
	return 0;
}

bool does_piece_fit(int n_tetromino, int rotation, int pos_x, int pos_y) {

	for (size_t x = 0; x <= TWIDTH; ++x) {
		for (size_t y = 0; y <= THEIGHT; ++y) {
			int index = rotate(x, y, rotation);
			int field_index = (pos_y + y) * FIELD_WIDTH + (pos_x + x);

			if (pos_x + x >= 0 && pos_x + x < FIELD_WIDTH)
				if (pos_y + y >= 0 && pos_y + y < FIELD_HEIGHT)
					if (tetromino[n_tetromino][index] == L'X' && (field[field_index] == 9 || field[field_index] == 7))
						return false;
		}
	}

	return true;
}

void draw_tetromino(int n_tetromino, int fx, int fy, int rotation, bool fallen) {
	for (size_t x = 0; x < TWIDTH; ++x) {
		for (size_t y = 0; y < THEIGHT; ++y) {
			int index = rotate(x, y, rotation);
			if (tetromino[n_tetromino][index] == L'X') { 
				field[(fy + y) * FIELD_WIDTH + (fx + x)] = fallen ? 7 : 8;
			}
		}
	}
}

void draw_field() {
	for (size_t x = 0; x < FIELD_WIDTH; ++x) {
		for (size_t y = 0; y < FIELD_HEIGHT; ++y) {
			if (field[y * FIELD_WIDTH + x] != 7)
				field[y * FIELD_WIDTH + x] = (x == 0 || x == FIELD_WIDTH - 1 || y == FIELD_HEIGHT - 1) ? 9 : 0;
		}
	}
}

void fall(const int &n_tetromino, const int &rotation, const int &pos_x, int &pos_y, 
		bool &fallen, const bool &is_running) {
	while (is_running) {
		std::this_thread::sleep_for(300ms);
		if (!fallen && does_piece_fit(n_tetromino, rotation, pos_x, pos_y + 1)) {
			++pos_y;
		}
		else { 
			fallen = true;
		}
	}
}

int check_filled() {
	for (size_t y = 0; y < FIELD_HEIGHT - 1; ++y) {
		for (size_t x = 1; field[y * FIELD_WIDTH + x] == 7; ++x) {
			if (x == FIELD_WIDTH - 2) return y;	
		}
	}
	return -1;
}

void remove_row(int row) {
	for (size_t y = row; y > 0; --y) {
		for (size_t x = 1; x < FIELD_WIDTH - 1; ++x) {
			field[y * FIELD_WIDTH + x] = field[(y - 1) * FIELD_WIDTH + x];
		}
	}
}

int main(int argc, char **argv) {
	
	initscr();
	noecho();
	curs_set(0); 
	nodelay(stdscr, TRUE);
	init_game();

	int c_tetromino = random() % 7;	
	int c_rotation = 0;	
	int c_x = FIELD_WIDTH / 2 - 2;	
	int c_y = 0;	

	bool is_running = true;
	bool fallen = false;

	draw_field();

	std::thread falling(fall, 
			std::cref(c_tetromino), 
			std::cref(c_rotation), 
			std::cref(c_x), std::ref(c_y), std::ref(fallen), std::cref(is_running));

	while (is_running) {

		int ch = getch();
		switch(ch) {
			case 'w':
				if (does_piece_fit(c_tetromino, c_rotation + 1, c_x, c_y)) 
					++c_rotation;
				break;
			case 'a':
				if (does_piece_fit(c_tetromino, c_rotation, c_x - 1, c_y)) 
					--c_x;
				break;
			case 'd':
				if (does_piece_fit(c_tetromino, c_rotation, c_x + 1, c_y)) 
					++c_x;
				break;
			case 'q':
				endwin();
				exit(0);
				break;
		}
		
		draw_field();
		draw_tetromino(c_tetromino, c_x, c_y, c_rotation, fallen);

		if (fallen) {
			fallen = false;
			c_tetromino = random() % 7;
			c_x = FIELD_WIDTH / 2 - 2;
			c_y = 0;
			c_rotation = 0;
			int row;
			while ((row = check_filled()) != -1) {
				remove_row(row);
			}
			if (!does_piece_fit(c_tetromino, c_rotation, c_x, c_y)) { is_running = false; };
		}
		render();
	}

	clear();
	mvaddstr(FIELD_HEIGHT / 2, FIELD_WIDTH / 2, "Game over");
	refresh();
	std::this_thread::sleep_for(2000ms);	
	endwin();
	falling.join();	
	return 0;
}
