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
#include <math.h>

enum Atom{
	Hydrogen,
	Oxygen,
	Null
};


struct Particle{
	double y;
	double x;
	double yv;
	double xv;

	Particle(double y, double x, double yv, double xv) : y(y), x(x), yv(yv), xv(xv){}
};

std::vector<struct Particle> particles;
std::mutex mP;

std::map<char, int> barrier; //tracks how many atoms are at the barrier.
std::mutex mB; //protect the barrier
//std::condition_variable cv; //wait on this condition.

std::map<char, std::deque<int>> tunnel; //what atoms are in the tunnel.
std::mutex mT; //protect the tunnel
int pipeLength = 50; //length of the tunnel..

void keyboardMonitor(WINDOW * win){
	char c = '0';
	while(1){		
		c = getch();
		if(c == 'H' || c == 'O'){
			std::string msg = "Generating atom " + std::string(1, c) + "\n";
			waddstr(win, msg.c_str()); 
			wrefresh(win);	
				
			mT.lock();
			tunnel[c].push_front(2); 	
			mT.unlock();
		}
		else{
			continue;
		}
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

void barrierMonitor(WINDOW* win){
	std::string text = "SCIENCE TERMINAL\n";
	waddstr(win, text.c_str());
	wrefresh(win);
	while(1){
		mB.lock();
		if(barrier['O'] >= 1 && barrier['H'] >= 2){
			barrier['O']--;
			barrier['H']-=2;
			std::string w =  "WATER!!!\n";
			waddstr(win, w.c_str()); 
			wrefresh(win);	
			struct Particle p(2, 2, 0, 1);
			mP.lock();
			particles.emplace_back(p);
			mP.unlock();
		}
		mB.unlock();
	}

}


void poolMonitor(WINDOW*win){
	//box(win,0,0);
	while(1){
		mP.lock();
		for(struct Particle & p : particles){ //erase the particles
			wmove(win, std::round(p.y), std::round(p.x)); //clear position!
			waddch(win, ' ');
		}

		for(struct Particle & p : particles){ //move the particles
			p.x += p.xv;
			p.y += p.yv;	
		}	

		for(struct Particle & p : particles){ //collision
			if(p.y > 50 || p.y < 0){
				p.y = p.y - p.yv;
				p.yv = -1 * p.yv;
			}	
			if(p.x > 50 || p.x < 0){
				p.x = p.x - p.xv;
				p.xv = -1 * p.xv;
			}
		}


		for(struct Particle & p : particles){ //draw the particles
			wmove(win, std::round(p.y), std::round(p.x)); //clear position!
			waddch(win, '*');
		}

		for(struct Particle & p : particles){ //gravity acceleration
			p.yv += 0.98;
		}

		mP.unlock();			
		wrefresh(win);	
		

		usleep(1000 * 100);
	}

}

void cursor_set_color_string(const char * color){
	printf("\e]12;%s\a", color);
	fflush(stdout);

}



int main(){
	std::system("clear");
	cursor_set_color_string("black"); 
	initscr(); //initialize curses screen.
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
		
	WINDOW * term = newwin(50,20, 10, 50); //embedded terminal!
	WINDOW * pool = newwin(50,100, 10, 5);

	std::thread keym(keyboardMonitor, term);
	std::thread barrierm(barrierMonitor, term);
	std::thread poolm(poolMonitor, pool);
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
					wmove(stdscr, 0, x);
					waddch(stdscr, ' ');
					mB.lock();	
					barrier[ch]++;
					mB.unlock();
				}	
			}
			
		}
		wmove(stdscr, 0, 1);
		
		usleep(1000 * 100);
	}	

	endwin(); //end curses screen
	return 0;
}

