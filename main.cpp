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
#include <vector>

enum Atom{
	Hydrogen,
	Oxygen,
	Null
};


std::map<Atom, int> barrier; //tracks how many atoms are at the barrier.

std::map<char, std::deque<int>> tunnel; //what atoms are in the tunnel.
std::mutex m; //protect the tunnel
int pipeLength = 100; //length of the tunnel...

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
		tunnel[c].push_front(2); 	
		m.unlock();
	}
}

/*
void pushMonitor(){ //move the tunnel forward
	while(1){
		m.lock();
		tunnel.push_front(' ');
		m.unlock();
		usleep(1000 * 100);
	}
}
*/

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
	//std::thread pm(pushMonitor);	
	while(1){ //console screen
		waddch(stdscr, '>'); //ejector...
		for(std::pair<const char, std::deque<int>>& p : tunnel){
			for(int i = 0; i < p.second.size(); i++){
				int x = p.second[i];
				int ch = p.first;
				wmove(stdscr, 0, x - 1); //clear previous position!
				waddch(stdscr, ' ');
				wmove(stdscr, 0, x);
				waddch(stdscr, ch);
				p.second[i] = p.second[i] + 1;
				if(p.second[i] > pipeLength){
					p.second.pop_back();
				}	
			}
			
		}
		wmove(stdscr, 0, 1);
		
		usleep(1000 * 100);
	}	

	endwin(); //end curses screen
	return 0;
}

