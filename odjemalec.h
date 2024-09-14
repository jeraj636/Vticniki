#ifndef ODJEMALEC_H
#define ODJEMALEC_H

#ifdef LINUX

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <thread>
#endif
class Odjemalec
{
public:
#ifdef LINUX
    void zazeni(std::string naslov, int port);
    void poslji(std::string vsebina);
    std::string prejmi();
    ~Odjemalec();
    static void beri_iz_povezave(Odjemalec *o);

private:
    int m_vticnik_fd;
    int m_port;
    sockaddr_in m_naslov_streznika;
    hostent *m_streznik;
#endif
};
#endif