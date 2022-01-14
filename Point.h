#ifndef POINT_H
#define POINT_H

#include <map>

typedef long double ld;

inline ld ld_abs(ld x){return (x>0)?x:-x;}

class Pair{
	protected:
		ld x,y;
		constexpr static const ld eps = 1e-10;
	public:
		inline Pair() {x = 0; y = 0;};
		Pair(ld x_coords, ld y_coords);

		inline bool operator< (const Pair& p) const {if(x < p.x) return true;
												else if (y < p.y) return true;
												return false;}
		inline ld get_x() { return x;}
		inline ld get_y() { return y;}
		inline void set_x(ld p_x) {x = p_x;}
		inline void set_y(ld p_y) {y = p_y;}
		inline bool operator==(const Pair &b);

};

class Point : public Pair{
	private:
		int couleur;
	public:
		Point(ld x_coords, ld y_coords);
		int get_couleur();
		void set_couleur(int color);

};

class Carte{
	public:
		std::map<Pair, int> carte;

		Carte();

		void add(Pair pt);

		void add(ld x, ld y);

};


#endif
