#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <list>

#include "Point.h"


//TODO : créer un signal catcher qui capte ^C et ^D et kill l'arduino

//SDL Window parameters
#define LENGTH 1000
#define WIDTH 1000
#define MILIEU_X (LENGTH/2)
#define MILIEU_Y (WIDTH/2)

#define DIST_MAX 200 //max distance recorded (implementation dependent)

#define PI 3.1415926535897932384626


#define NB_SURE 1 //TODO : remove this parameter
#define NB_FILTRE 5

#define MAP_MODE 0


#if MAP_MODE == 0
#define COLOR_DECAY 0
#else
#define COLOR_DECAY (.6) //(.6 is a great decay value for radar like display)
#endif

using namespace std;

typedef struct{
	char key[512];

	char& operator[](int index){
		if(index > 127)
			return key[index - 1073741824 + 128];
		else return key[index];
	}

} Input;

struct list_Points{
	list<Point> points;
	
} points;

Position pos = Position();

void push_Point(int dist, int angle){
	points.points.push_back(Position::convert_p(dist, angle));
}

void push_Point(Pair d_a){
	push_Point(d_a.get_x(), d_a.get_y());
}

void aff_Point(SDL_Renderer *ren, Point pt, Pair screen_pos){
	int c = (int) pt.get_couleur();
	SDL_SetRenderDrawColor(ren, c, c, c, 255);
	int const pos_x(MILIEU_X + pt.get_x() - screen_pos.get_x()), pos_y(MILIEU_Y + pt.get_y() - screen_pos.get_y());
	//if(0 <= pos_x && pos_x < LENGTH && 0 <= pos_y && pos_y < WIDTH)
	SDL_RenderDrawPoint(ren, pos_x, pos_y);
}

void update_liste(){
	#if MAP_MODE == 0
	int nb_corbeille(0);
	for(list<Point>::iterator it(points.points.begin()); it != points.points.end(); it++){
		double c = it->get_couleur();
		if(c > COLOR_DECAY) it->set_couleur(c-COLOR_DECAY);
		else nb_corbeille++;
	}
	while(nb_corbeille-->0)
		points.points.pop_front();
	#endif
}

void draw_Points(SDL_Renderer *ren, Pair screen_pos){
	for(auto pt: points.points){
		aff_Point(ren, pt, screen_pos);
	}
	//Draw robot position
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
	SDL_RenderDrawPoint(ren, MILIEU_X + Position::get_origine_x() - screen_pos.get_x(), MILIEU_Y + Position::get_origine_y() - screen_pos.get_y());
	SDL_RenderPresent(ren);
}

void clear_Screen(SDL_Renderer *ren){
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); //Draw robot position
	SDL_RenderClear(ren);
}

#define SCREEN_DX 1
#define SCREEN_DY 1
#define SCREEN_ROT (PI/8)

bool deplacer_ecran(SDL_Event &events, Input in, Pair &screen_pos){
	bool moved(false);

	if(in[SDLK_LEFT] == 1){
		moved = true;
		screen_pos.set_x(screen_pos.get_x()+SCREEN_DX);
	}
	if(in[SDLK_RIGHT] == 1){
		moved = true;
		screen_pos.set_x(screen_pos.get_x()-SCREEN_DX);
	}
	if(in[SDLK_UP] == 1){
		moved = true;
		screen_pos.set_y(screen_pos.get_y()+SCREEN_DY);
	}
	if(in[SDLK_DOWN] == 1){
		moved = true;
		screen_pos.set_y(screen_pos.get_y()-SCREEN_DY);
	}

	return moved;
}

Pair buffer_filtre[]= {Pair(-1,-1), Pair(-1,-1), Pair(-1,-1)};

void filtre1(int dist, int angle){
	//moyenne sur 3 points
	if(buffer_filtre[0].get_x() != -1){
		push_Point(buffer_filtre[0]);
		if(buffer_filtre[2].get_x() >= DIST_MAX - 2 && dist < DIST_MAX - 2)
		buffer_filtre[2].set_x((buffer_filtre[0].get_x() + dist)/2);
	}
	buffer_filtre[0] = buffer_filtre[1];
	buffer_filtre[1] = buffer_filtre[2];
	buffer_filtre[2] = Pair(dist, angle);
}

void filtre2(list_Points &points){
	if(points.points.size() < 2*NB_FILTRE) return;

	auto it(points.points.begin());
	for(int i(0); i<NB_FILTRE; i++) it++;

	for(int i(NB_FILTRE); i<points.points.size() - NB_FILTRE; it++, i++){
		bool is_alone(true);
		auto voisins(it);
			
		for(int j(0); j<NB_FILTRE; j++) voisins--;


		for(int j(1-NB_FILTRE); j < NB_FILTRE; j++, voisins++){
			if(it->dist(*voisins) <= Pair::seuil){
				is_alone = false;
				break;
			}
		}

		if(is_alone){
			it = points.points.erase(it);
			it--;
		}
	}

}


std::chrono::time_point cur_time(std::chrono::steady_clock::now()); //change from seconds to milliseconds
#define UPDATE_TIME 25 //nb of milliseconds
#define UPDATE_DX 0
#define UPDATE_DY .198 //1.98

bool automove(){
	std::chrono::time_point now(std::chrono::steady_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(now-cur_time);
	std::chrono::duration<int, std::milli> tmp1 = tmp;
	if(tmp1.count() > UPDATE_TIME){
		cur_time = now;
	
		Position::deplacer(-UPDATE_DX, -UPDATE_DY, 0);

		return true;
	}
	return false;
}


int main(int argc, char* argv[]){
	//Initialize the window
	
	Pair screen_pos = Pair(0, 0);
	
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}

	SDL_Window *win = SDL_CreateWindow("Wave",
										SDL_WINDOWPOS_CENTERED,

										SDL_WINDOWPOS_CENTERED,
										LENGTH, WIDTH, SDL_WINDOW_RESIZABLE);

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);




	struct termios old_term, new_term;



	//préparation de la carte Arduino
	int file = open("/dev/ttyACM0", O_RDWR); // les deux ports possibles sont /dev/ttyACM0 ou /dev/ttyACM1
	if(tcgetattr(file, &old_term) != 0){
		file = open("/dev/ttyACM1", O_RDWR);
		if(tcgetattr(file, &old_term) != 0){
			tcsetattr(file, 0, &old_term);
			cout << "Couldn't get old terminal attributes\n";
			return -1;
		}
	}
	
	tcflush(file, TCIFLUSH);
	new_term.c_cflag     &=  ~CSTOPB;
	new_term.c_cflag     &=  ~CSIZE;
	new_term.c_cflag     |=  CS8;
	new_term.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	new_term.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

	new_term.c_cc[VTIME] = 0;
	new_term.c_cc[VMIN] = 1;

	cfsetospeed(&new_term, B9600);
	cfsetispeed(&new_term, B9600);

	SDL_Event events;
	Input in;

	tcsetattr(file, TCSANOW, &new_term);

	char read_buf[1] = {97};

	int cur_angle(0), cur_dist(0);

	bool read_angle(true), toNegate(false);

	tcflush(file, TCIFLUSH);

	while(true){
		
		int n = read(file, &read_buf, sizeof(read_buf));

		SDL_PollEvent(&events);
		switch(events.type){
			case SDL_QUIT:
				n = -1; //kill the program
				break;

			case SDL_KEYDOWN:
				in[events.key.keysym.sym] = 1;
				break;

			case SDL_KEYUP:
				if(in[SDLK_p] == 1){
					cout << "paused\n";
					for(int i(0); i<NB_SURE; i++) write(file, "s", 1);
					cout << "Press\"p\" to unpause\n";
					while(SDL_WaitEvent(&events)){
						if(events.type == SDL_KEYDOWN){
							if(events.key.keysym.sym == SDLK_p) break;

							if(events.key.keysym.sym == SDLK_LEFT)
								in[events.key.keysym.sym] = 1;
							if(events.key.keysym.sym == SDLK_RIGHT)
								in[events.key.keysym.sym] = 1;
							if(events.key.keysym.sym == SDLK_UP)
								in[events.key.keysym.sym] = 1;
							if(events.key.keysym.sym == SDLK_DOWN)
								in[events.key.keysym.sym] = 1;
						}

						if(deplacer_ecran(events, in, screen_pos))
							clear_Screen(ren);

						draw_Points(ren, screen_pos);

						if(events.type == SDL_KEYUP)
							in[events.key.keysym.sym] = 0;
						
					}
					
					cout << "unpaused\n";
					for(int i(0); i<NB_SURE; i++) write(file, "t", 1);

				}

				if(in[SDLK_k] == 1){
					n = -1; //kill the program
				}
				
				else if(in[SDLK_m] == 1){ //movement
					for(int i(0); i<NB_SURE; i++) write(file, "s", 1);

					cout << "How dit we move ?\n";

					ld new_delta_x, new_delta_y, new_delta_angle;
					cin >> new_delta_x >> new_delta_y >> new_delta_angle;

					Position::deplacer(new_delta_x, new_delta_y, new_delta_angle);
					
					for(int i(0); i<NB_SURE; i++) write(file, "t", 1);
				}

				in[events.key.keysym.sym] = 0;
				break;
		}

		if(deplacer_ecran(events, in, screen_pos) || automove())
			clear_Screen(ren);

		draw_Points(ren, screen_pos);


		if(n < 0){ //kill the program safely
			
			SDL_DestroyRenderer(ren);
			SDL_DestroyWindow(win);
			SDL_Quit();
			for(int i(0); i<NB_SURE; i++) write(file, "k", 1);
			tcsetattr(file, 0, &old_term);
			cout << "Closed succesfully\n";
			return 0;
		}
		else{
			if(read_buf[0] == 's' || read_buf[0] == 'k' || read_buf[0] == 'm' || read_buf[0] == '.');
			else{
				if((read_buf[0] == ' ' || read_buf[0] == '\n') && toNegate){
					if(read_angle)
						cur_angle = -cur_angle;
					else
						cur_dist = -cur_dist;
					toNegate = false;
				}
				if(read_buf[0] == '\n'){
					//cout << "angle is : " << cur_angle << " and distance is : " << cur_dist << "\n";
					filtre1(cur_dist, cur_angle);
					filtre2(points);

					update_liste();


					cur_angle = 0;
					cur_dist = 0;
				}
				else if(read_buf[0] != ' '){
					//parse integers
					if(read_buf[0] == 'a'){
						read_angle = true;
					}
					else if(read_buf[0] == 'd'){
						read_angle = false;
					}
					else{
						if(read_angle){
							if(read_buf[0] == '-')
								toNegate = true;
							else
								cur_angle = 10*cur_angle + (int)read_buf[0] - '0';
						}
						else{
							if(read_buf[0] == '-')
								toNegate = true;
							else
								cur_dist = 10*cur_dist + (int)read_buf[0] - '0';
						}
					}
				}

			}
		}
		
	}

}
