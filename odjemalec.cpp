#include "odjemalec.h"

#ifdef LINUX
void Odjemalec::beri_iz_povezave(Odjemalec *o)
{
    while (true)
    {

        char buffer[256];
        bzero(buffer, 256);
        int n = read(o->m_vticnik_fd, buffer, 255);
        if (n <= 0)
        {
            break;
        }
        std::cout << buffer << "\n";
    }
}

void Odjemalec::zazeni(std::string naslov, int port)
{
    m_port = port;

    //* Odpiranje vticnika
    m_vticnik_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_vticnik_fd < 0)
    {
        std::cout << "Napaka pri odpiranju vticnika!\n";
        exit(1);
    }

    //* Iskanje streznika
    m_streznik = gethostbyname(naslov.c_str());
    if (m_streznik == nullptr)
    {
        std::cout << "Napaka pri iskanju strenika: " << naslov << ":" << port << "\n";
        close(m_vticnik_fd);
        exit(1);
    }

    //* Nastavljanje drugih parametrov streznika
    bzero((char *)&m_naslov_streznika, sizeof(m_naslov_streznika));
    m_naslov_streznika.sin_family = AF_INET;

    bcopy((char *)m_streznik->h_addr,
          (char *)&m_naslov_streznika.sin_addr.s_addr,
          m_streznik->h_length);

    m_naslov_streznika.sin_port = htons(m_port);

    //* Povezovanje s streznikom
    if (connect(m_vticnik_fd, (sockaddr *)&m_naslov_streznika, sizeof(m_naslov_streznika)) < 0)
    {
        std::cout << "Napaka pri povezovanjnu!\n";
        close(m_vticnik_fd);
        exit(1);
    }
}
void Odjemalec::poslji(std::string vsebina)
{
    int n = write(m_vticnik_fd, vsebina.c_str(), vsebina.size());
    if (n < 0)
        std::cout << "Napaka pri posiljanju!\n";
}
std::string Odjemalec::prejmi()
{
    char buff[256];
    int n = read(m_vticnik_fd, buff, 255);
    return buff;
}

Odjemalec::~Odjemalec()
{
    close(m_vticnik_fd);
}
int main(int argc, char *argv[])
{
    Odjemalec odjemalec;
    odjemalec.zazeni("localhost", atoi(argv[1]));
    std::thread nova_nit(Odjemalec::beri_iz_povezave, &odjemalec);
    nova_nit.detach();
    while (1)
    {
        std::string sporocilo;
        std::cin >> sporocilo;
        odjemalec.poslji(sporocilo);
    }
}
#endif