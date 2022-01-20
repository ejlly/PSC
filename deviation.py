import matplotlib.pyplot as plt
import numpy as np
from scipy.integrate import solve_ivp

def n(x,z):
    '''définition de l'indice du milieu'''
    return 1 + abs(z-1)

def dn_x(x,z):
	return 0
    #h=1e-6
    #return (n(x+h,z)-n(x-h,z))/(2*h)

def dn_z(x,z):
	#return 1 if z > 1 else -1
    h=1e-6
    return (n(x,z+h)-n(x,z-h))/(2*h)


c = 1 #célérité de l'onde dans le vide...

def systeme(t, Y):

    x, z = Y[0], Y[2]
    d_x, d_z = Y[1], Y[3]

    d2x = c**2/n(x,z)**3 * dn_x(x,z) - 1/n(x,z) * (dn_x(x,z) * d_x + dn_z(x,z) * d_z) * d_x
    d2z = c**2/n(x,z)**3 * dn_z(x,z) - 1/n(x,z) * (dn_x(x,z) * d_x + dn_z(x,z) * d_z) * d_z

    return (d_x, d2x, d_z, d2z)

def syst(t, Y):

	x, z = Y[0], Y[2]
	d_x, d_z = Y[1], Y[3]

	d2x = 1/2 * dn_x(x,z)*n(x,z)
	d2z = 1/2 * dn_z(x,z)*n(x,z)

	return (d_x, d2x, d_z, d2z)

def res(alpha):
	Y = [1, c*np.cos(alpha), 1, c*np.sin(alpha)]
	t_span = [0, 10] #à voir...?

	solution = solve_ivp(syst, t_span, Y)

	plt.plot(solution.y[0], solution.y[2])


def draw(alpha):
	x_cur, z_cur = [0],[0]
	dx = 1e-5
	N = 5

	A = n(0,0) * np.sin(alpha)

	x_cur.append(dx)
	z_cur.append(np.tan(alpha) * dx)

	for _ in range(N):
		x, z = x_cur[-1], z_cur[-1]
		x_cur.append(x + dx)
		z_cur.append(z + dx/A*((n(x,z))**2 - A**2)**.5)

	plt.plot(x_cur, z_cur)


def main():
	alpha = -1
	N_rayon = 30
	for _ in range(N_rayon+1):
		print(alpha)
		alpha += 2/N_rayon
		res(alpha)

	plt.show()

main()
