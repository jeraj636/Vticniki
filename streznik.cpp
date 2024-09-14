
#include "streznik.h"
#ifdef LINUX
Objekt *Igra::poisci_objekt_po_id(int id)
{
    for (int i = 0; i < 15; i++)
    {
        if (objekti[i] == nullptr)
            break;
        if (objekti[i]->objekt_id == id)
            return objekti[i];
    }
    return nullptr;
}
void Igra::nastavi()
{
    for (int i = 0; i < 15; i++)
        objekti[i] = nullptr;
    st_igralcev = 0;
}

void Streznik::zazeni(int st_porta)
{
    m_port = st_porta;

    //* Odpiranje streznika
    m_vticnik_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_vticnik_fd < 0)
    {
        std::cout << "Napaka pri odpiranju vticnika!\n";
        exit(1);
    }

    //* Inicializacija naslova streznika
    bzero((char *)&m_naslov_streznika, sizeof(m_naslov_streznika));
    m_naslov_streznika.sin_family = AF_INET;
    m_naslov_streznika.sin_port = htons(m_port);
    m_naslov_streznika.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_vticnik_fd, (sockaddr *)&m_naslov_streznika, sizeof(m_naslov_streznika)) < 0)
    {
        std::cout << "Napaka pri bindanju m_naslov_streznika v m_vticnik_fd!\n";
        exit(1);
    }

    //* Ustvarjanje niti za poslusanje
    std::thread nit_poslusaj(poslusaj);
    nit_poslusaj.detach();
    while (!nit_poslusaj.joinable())
        ;
    nit_poslusaj.join();
}

void Streznik::poslusaj()
{
    for (;;)
    {
        if (Igra::st_igralcev < 14)
        {
            Odjemalec odjemalec;

            listen(m_vticnik_fd, 5);

            //* Sprejemanje povezava
            odjemalec.velikost_odjemalca = sizeof(odjemalec.naslov_odjemalca);
            odjemalec.vticnik_fd = accept(m_vticnik_fd, (sockaddr *)&odjemalec.naslov_odjemalca, &odjemalec.velikost_odjemalca);
            if (odjemalec.vticnik_fd < 0)
            {
                std::cout << "Napaka pri sprejemanju povezave!\n";
                close(m_vticnik_fd);
                exit(1);
            }

            Igra::objekti[Igra::st_igralcev] = new Objekt;
            Igra::objekti[Igra::st_igralcev]->objekt_id = Igra::st_igralcev;
            Igra::objekti[Igra::st_igralcev]->odjemalec = odjemalec;
            std::cout << "Nova povezava: " << Igra::st_igralcev << "\n";
            std::thread nova_nit(std::thread(vzdrzuj_povezavo, odjemalec, Igra::objekti[Igra::st_igralcev++]));
            nova_nit.detach();
        }
    }
}
void Streznik::vzdrzuj_povezavo(Odjemalec odjeamlec, Objekt *objekt)
{
    while (1)
    {
        char buffer[256];
        bzero(buffer, 256);
        int n = read(odjeamlec.vticnik_fd, buffer, 255);

        std::cout << objekt->objekt_id << "  ::  " << buffer << "\n";

        if (buffer[0] == 'p')
        {
            int id_naslovnika = buffer[1] - '0';
            Objekt *naslovnik = Igra::poisci_objekt_po_id(id_naslovnika);
            write(naslovnik->odjemalec.vticnik_fd, buffer, strlen(buffer));
        }
        if (n < 0)
        {
            std::cout << "Napaka pri branju: " << objekt->objekt_id << "\n";
        }
        else if (n == 0) //*Povezava se je zaprla
        {
            break;
        }
        if (buffer[0] == 'k') //* POvezava se je zaprla
        {
            break;
        }
    }

    std::cout << "Signal za konec povezave: " << objekt->objekt_id << "\n";

    //* izbris objekta iz tabele
    Igra::objekti[objekt->objekt_id] = Igra::objekti[Igra::st_igralcev];
    Igra::objekti[Igra::st_igralcev--] = nullptr;

    delete objekt;

    close(odjeamlec.vticnik_fd);

    if (Igra::st_igralcev == 0)
    {
        close(Streznik::m_vticnik_fd);
        exit(1);
    }
}
#endif
int main(int argc, char *argv[])
{
    Streznik::zazeni(atoi(argv[1]));
}