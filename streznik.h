#ifndef STREZNIK_H
#define STREZNIK_H

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <fcntl.h>

//* Razred, ki ima podatke o odjemalcu
class Odjemalec
{
public:
    int vticnik_fd;
    sockaddr_in naslov_odjemalca;
    socklen_t velikost_odjemalca;
};

class Objekt
{
public:
    int objekt_id;
    Odjemalec odjemalec;
};

//* Razred, ki hrani splošne podatke o igri ki jih strežnik potrebuje
class Igra
{
public:
    static void nastavi();
    static inline Objekt *objekti[15];
    static inline int st_igralcev;
    static Objekt *poisci_objekt_po_id(int id);
};

class Streznik
{
public:
    static void zazeni(int st_porta);

private:
    static inline int m_vticnik_fd;
    static inline sockaddr_in m_naslov_streznika;
    static inline int m_port;
    static void poslusaj();
    static void vzdrzuj_povezavo(Odjemalec odjeamlec, Objekt *objekt);
};
#endif