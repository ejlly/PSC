# -*- coding: utf-8 -*-
"""
Created on Wed Feb 16 16:01:31 2022

@author: adrie
"""

import matplotlib.pyplot as plt
import numpy as np
import random
from math import cos, sin


# Unités : m, s, rad


g = 9.81

t = 600 # Durée d'acquisition
dt = 1e-3 # Pas de temps
N = 10 # Nombre de tirages

d_a = 5e-3 * g # Précision de l'accéléromètre
d_omega = 1e-2 # Précision du gyromètre

couleur = ["blue", "orange"]
nom = ["Apogee","Ellipse"]
err_a = [15e-6 * g, 14e-6 * g] # Biais de l'accéléromètre
err_omega = [0.08 * (np.pi / 180 / 3600), 7 * (np.pi / 180 / 3600)] # Biais du gyromètre, valeur en °/h convertie en rad/s

n = int(t / dt) # Nombre d'itérations


def mouvement_parking():
    a = np.zeros(n) # Accélération
    omega = np.zeros(n) # Vitesse angulaire
    omega[n//4 : n//4 + 4*int(1/dt)] = - np.pi * 0.5 * 0.25
    omega[n//2 : n//2 + 4*int(1/dt)] = - np.pi * 0.5 * 0.25
    omega[3*n//4 : 3*n//4 + 4*int(1/dt)] = - np.pi * 0.5 * 0.25
    phi_0 = np.pi * 0.5 # Angle initial
    v_0 = 0.5 # Vitesse initiale
    return a,omega,phi_0,v_0

a,omega,phi_0,v_0 = mouvement_parking()

plt.figure()



def trajectoire(): # Trajectoire réelle
    v = np.zeros(n)
    phi = np.zeros(n)
    x = np.zeros(n)
    y = np.zeros(n)
    dist = 0
    
    v[0] = v_0
    phi[0] = phi_0
    
    for i in range(n-1):
        v[i+1] = v[i] + a[i] * dt
        phi[i+1] = phi[i] + omega[i] * dt
        x[i+1] = x[i] + v[i] * cos(phi[i]) * dt
        y[i+1] = y[i] + v[i] * sin(phi[i]) * dt
        dist += v[i] * dt
    
    plt.plot(x, y, linewidth=1, color="red", label="Trajectoire réelle")
    plt.plot(x[n-1], y[n-1], 'o', color='red')
    
    return phi[n-1],dist,np.array([x[n-1],y[n-1]])


def tirage(couleur,nom,err_a, err_omega): # Tirage d'une trajectoire
    v = np.zeros(n)
    phi = np.zeros(n)
    x = np.zeros(n)
    y = np.zeros(n)
    
    v[0] = v_0
    phi[0] = phi_0
    
    for i in range(n-1):
        v[i+1] = v[i] + (a[i] + err_a + d_a * 0 * random.uniform(-1,1)) * dt
        phi[i+1] = phi[i] + (omega[i] + err_omega + d_omega * 0 * random.uniform(-1,1)) * dt
        x[i+1] = x[i] + v[i] * cos(phi[i]) * dt
        y[i+1] = y[i] + v[i] * sin(phi[i]) * dt
    
    plt.plot(x, y, linewidth=1, linestyle="dashed", label=nom, color=couleur)
    plt.legend()
    plt.plot(x[n-1], y[n-1], '.', color=couleur)
    return np.array([x[n-1],y[n-1]]),phi[n-1]

angle_reel, d, pt_reel = trajectoire()

for i in range(2):  
    pt_calcule,angle_calcule = tirage(couleur[i],nom[i],err_a[i],err_omega[i])
    deviation = np.linalg.norm(pt_reel - pt_calcule)
    deviation_relative = deviation / d * 100
    dev_angle =  np.abs(angle_reel - angle_calcule) * 180 / np.pi
    print("-"*10,nom[i],"-"*10)
    print("Déviation :", deviation,"m")
    print("Déviation relative :", deviation_relative,"%")
    print("Dérive angulaire : "+str(dev_angle)+"°")
