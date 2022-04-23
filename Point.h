#ifndef POINT_H
#define POINT_H

#define PI 3.1415926535897932384626

#include <map>
#include <cmath>

typedef long double ld;

inline ld ld_abs(ld x){return (x>0)?x:-x;}

class Pair{
	protected:
		ld x,y;
	public:
		constexpr static const ld seuil = 1;

		inline Pair() {x = 0; y = 0;};
		Pair(ld x_coords, ld y_coords);

		inline bool operator< (const Pair& p) const {if(x < p.x) return true;
												else if (y < p.y) return true;
												return false;}
		inline ld get_x() { return x;}
		inline ld get_y() { return y;}
		inline void set_x(ld p_x) {x = p_x;}
		inline void set_y(ld p_y) {y = p_y;}
		inline ld dist(Pair q) {
			return std::pow((q.x - x)*(q.x - x) + (q.y - y)*(q.y - y), .5);
		}
		inline bool operator==(const Pair &b);

		void print();
};

class Point : public Pair{
	private:
		double couleur;
	public:
		Point(ld x_coords, ld y_coords);
		double get_couleur();
		void set_couleur(double color);
};


class Position : public Pair{
	private:
		inline static Pair origine = Pair(0,0);
		inline static ld o_angle = 100;
	
	public:
		static ld get_origine_x();
		static ld get_origine_y();
		static void deplacer(ld x, ld y);
		static void deplacer(ld x, ld y, ld angle);
		void print_position();
		static Pair convert(int dist, int angle);
		static Point convert_p(int dist, int angle);
};

#endif
