#include <curses.h>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <deque>
#include <unistd.h>
#include <time.h>

enum Atom{
	Hydrogen,
	Oxygen,
	Null
};


std::map<Atom, int> barrier; //tracks how many atoms are at the barrier.

std::deque<char> tunnel; //what atoms are in the tunnel.
std::mutex m; //protect the tunnel
int maxLength = 100; //length of the tunnel...

void keyboardMonitor(){
	char c = '0';
	while(1){		
		c = getch();
		if(c == 'H'){
			barrier[Hydrogen]++;
		}	
		else if(c == 'O'){
			barrier[Oxygen]++;
		}
		else{
			continue;
		}
		m.lock();
		tunnel.push_front(c);	
		m.unlock();
	}
}

void pushMonitor(){ //move the tunnel forward
	while(1){
		m.lock();
		tunnel.push_front(' ');
		m.unlock();
		usleep(1000 * 500);
	}
}

//void animateScreen(

int main(){
	std::system("clear");
	initscr(); //initialize curses screen.
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
			
	barrier[Hydrogen] = 0;
	barrier[Oxygen] = 0;
	std::thread keym(keyboardMonitor);
	std::thread pm(pushMonitor);	
	while(1){ //console screen
		waddch(stdscr, '>'); //ejector...
		for(char ch : tunnel){
			waddch(stdscr, ch);
			
		}
		wmove(stdscr, 0, 1);
		
		usleep(1000 * 500);
	}	

	endwin(); //end curses screen
	return 0;
}

