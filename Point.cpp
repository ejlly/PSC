#include "Point.h"

Pair::Pair(ld x_coords, ld y_coords){
	Pair tmp = {x_coords, y_coords};
}

bool Pair::operator==(const Pair &b){
	return ld_abs(b.x - x) < Pair::eps && ld_abs(b.y - y) < Pair::eps;
}

Point::Point(ld x_coords, ld y_coords){
	x = x_coords;
	y = y_coords;
	couleur = 255;
}

int Point::get_couleur(){return couleur;}

void Point::set_couleur(int c){couleur = c;}

Carte::Carte(){ return;}

void Carte::add(Pair pt){
	if(carte.contains(pt)){
		carte[pt]++;
		return;
	}
	carte[pt] = 1;
}

void Carte::add(ld x, ld y){
	add(Pair(x,y));
}
