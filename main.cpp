#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <cmath>
#include <list>
#include "Point.h"

#define LENGTH 500
#define WIDTH 500
#define MILIEU_X (LENGTH/2)
#define MILIEU_Y (WIDTH/2)

#define DIST_MAX 1191 //distance max du capteur (dépend du délai d'acquisition)

#define PI 3.1415926535897932384626

#define DT 10

using namespace std;

list<Point> points;

void push_Point(int dist, int angle){
	points.push_back(Point((long double) dist * cos((long double) angle/180 * PI), (long double) dist * sin((long double) angle/180 * PI)));
}

void update_liste(){
	for(auto pt: points){
		int c = pt.get_couleur();
		if(c > DT) pt.set_couleur(c-DT);
		else points.pop_front();
	}
}

void draw_liste(SDL_Renderer *ren){
	for(auto pt: points){
		int c = pt.get_couleur();
		SDL_SetRenderDrawColor(ren, c, c, c, 255);
		SDL_RenderDrawPoint(ren, MILIEU_X + pt.get_x(), MILIEU_Y + pt.get_y());

	}
	SDL_RenderPresent(ren);
}

int main(int argc, char* argv[]){
	//Initialize the window

	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}

	SDL_Window *win = SDL_CreateWindow("Wave",
										SDL_WINDOWPOS_CENTERED,
										SDL_WINDOWPOS_CENTERED,
										LENGTH, WIDTH, SDL_WINDOW_RESIZABLE);

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	/*
	SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
													SDL_TEXTUREACCESS_STREAMING,
													LENGTH,
													WIDTH);
	*/

	int file = open("/dev/ttyACM0", O_RDWR|O_SYNC);

	struct termios old_term, new_term;

	if(tcgetattr(file, &old_term) != 0){
		file = open("/dev/ttyACM1", O_RDWR|O_SYNC);
		if(tcgetattr(file, &old_term) != 0){
			cout << "Couldn't get old terminal attributes\n";
			return -1;
		}
	}

	new_term = old_term;

	new_term.c_cflag &= ~PARENB;
	new_term.c_cflag &= ~CSTOPB;
	new_term.c_cflag &= ~CRTSCTS;
	new_term.c_cflag &= ~CSIZE;
	new_term.c_cflag |= CREAD | CLOCAL;

	//new_term.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

	new_term.c_cc[VTIME] = 0;
	new_term.c_cc[VMIN] = 1;

	cfsetospeed(&new_term, B9600);
	cfsetispeed(&new_term, B9600);

	tcflush(file, TCIFLUSH);

	char read_buf[1];

	int cur_angle(0), cur_dist(0);

	bool read_angle(true);

	//SDL_RenderPresent(ren);

	//int count_to_end(10000);


	/*
	for(int i(0); i<WIDTH; i++){
		SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
		SDL_RenderDrawPoint(ren, 150, i);

		SDL_RenderPresent(ren);
	}
	*/

	while(true){

		int n = read(file, &read_buf, sizeof(read_buf));

		if(n <= 0){
				SDL_DestroyRenderer(ren);
				SDL_DestroyWindow(win);
				SDL_Quit();
				return 0;
				//cout << "Failed to read" << endl;
			//return -1;
		}
		else{
			//printf("\nchar is : %c\n", read_buf[0]);
			if(read_buf[0] == '\n'){
				printf("angle is : %d and distance is : %d\n", cur_angle, cur_dist);
				push_Point(cur_dist, cur_angle);

				update_liste();
				draw_liste(ren);
				

				cur_angle = 0;
				cur_dist = 0;
			}
			if(read_buf[0] == ' ' || read_buf[0] == '\n'){
				read_angle = !read_angle;
			}
			else{
				if(read_angle){
					cur_angle = 10*cur_angle + (int)read_buf[0] - '0';
				}
				else{
					cur_dist = 10*cur_dist + (int)read_buf[0] - '0';
				}
			}
		}
	}

	if(tcsetattr(file, 0, &old_term) != 0){
		cout << "Couldn't revert terminal to default\n";
	}
}
