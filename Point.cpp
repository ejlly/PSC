#include "Point.h"
#include <iostream>

Pair::Pair(ld x_coords, ld y_coords){x = x_coords; y = y_coords;}

bool Pair::operator==(const Pair &b){
	return ld_abs(b.x - x) < Pair::seuil && ld_abs(b.y - y) < Pair::seuil;
}

void Pair::print(){
	std::cout << x << " " << y;
}

Point::Point(ld x_coords, ld y_coords){
	x = x_coords;
	y = y_coords;
	couleur = 255;
}

double Point::get_couleur(){return couleur;}

void Point::set_couleur(double c){this->couleur = c;}

ld Position::get_origine_x(){
	return origine.get_x();
}

ld Position::get_origine_y(){
	return origine.get_y();
}

void Position::deplacer(ld x, ld y){
	origine.set_x(origine.get_x() + x);
	origine.set_y(origine.get_y() + y);
}

void Position::deplacer(ld x, ld y, ld angle){
	origine.set_x(origine.get_x() + x);
	origine.set_y(origine.get_y() + y);
	o_angle += angle;
}


void Position::print_position(){
	origine.print();
}

Pair Position::convert(int dist, int angle){
	return Pair((long double) dist * cos((long double) (angle+o_angle)/180 * PI) - origine.get_x(), - (long double) dist * sin((long double) (angle+o_angle)/180 * PI) - origine.get_y());
}

Point Position::convert_p(int dist, int angle){
	return Point((long double) dist * cos((long double) (angle+o_angle)/180 * PI) - origine.get_x(), - (long double) dist * sin((long double) (angle+o_angle)/180 * PI) - origine.get_y());
}
