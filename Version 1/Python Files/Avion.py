#systeme de vol du drone
 



import time
import numpy as np
import serial
arduino = serial.Serial("COM7", 115200)
arduino.close()
input = np.empty(16)           # l'aruino envoi x en i0, y en en i1, batterie en i2, signal 1 a 4 en 3->6,                               
output = np.empty(16)            #le pi envoi les comande servo moteurs en i0/i1, comande moteur principale en i2

def ecriture_serie(valeur, indice):
    if(arduino.is_open):
        mess = str((valeur<<4)+indice )+ '\n'
        arduino.write(mess.encode('utf-8'))
        return 1
    else : 
        return 0


def lecture_serie():    
    mess = arduino.readline()
    str_mess = mess.decode("utf-8")
    str_mess = str_mess[0:len(str_mess)-2]
    nombre = int(str_mess)
    nombre = nombre & 15
    return (int(str_mess)-nombre)>>4, nombre

def ouverture_port_arduino():
    mess = ""
    if(arduino.is_open):
        mess = "Ouverture liaison serie : deja ouverte"
    else:
        try:
            arduino.open()
            mess ="Ouverture liaison serie : liaison ouverte"
        except:
            mess ="Ouverture liaison serie : Impossible d'ouvrir le port "
    return(mess)

def init_tableau():
    for i in range(0, 15):
        input[i]=0

def initialisation():
    init_tableau()
    ouverture_port_arduino()


if __name__ == "__main__" :

    initialisation()
    t0 = time.time()
    last_time = time.time()
    while 1:    #Boucle principalle


        while arduino.in_waiting == 0:
            a=1
        if(arduino.in_waiting > 0 and arduino.is_open):
            valeur, i = lecture_serie()
            input[i]= valeur 
            print("valeur", str(input[0]), "indice", str(i))

        if input[0] == 40  :
            arduino.write(b'16\n')
        #else :            ecriture_serie(0, 0)

        
        while time.time()-0.01 < last_time :
                a=2
        last_time = time.time()

        


    arduino.close()

            