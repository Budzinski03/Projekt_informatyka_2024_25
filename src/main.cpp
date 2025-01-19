#include <iostream>
#include <SFML\Graphics.hpp>
#include <vector>
#include <fstream>
#include "klasy.h"
#include <sfml/Audio.hpp>

enum StanGry
{
    MENU,
    GRA,
    RANKING,
    USTAWIENIA,
    WYBOR
};

struct Komunikaty
{
    Komunikat pomoc;
    Komunikat koniecGry;
    Komunikat wyjscie;
    Komunikat F1;
    Komunikat czyNowaGra;
};

// globalny obiekt zasobow, deklaracja jest w klasy.h
Zasoby zasoby;

// flagi
bool graWstrzymana = false;
bool pomocAktywna = false;
bool koniecGry = false;
bool wyjscieAktywne = false;
bool zmienKierunek = false;

int aktualnyPoziom = 1;
int zyciaBossa = 4; // zycia bossa ladowane sa jako zycia bossa+aktualnyPoziom

// parametry
float kierunek = 1.0f;     // predkosc wrogow w poziomie
float interwalRuchu = 3.f; // wrogowie przesuwani co 3s
float czasOdOstatniegoRuchu = 0.0f;
float czasOdOstatniegoStrzalu = 0.f;
float minimalnyCzasStrzalu = 1.f;
float czasOdOstatniegoStrzaluWroga = 0.f;
float minimalnyCzasStrzaluWroga = 1.0f;

// parametry wrogow
int kolumny = 7;
int wiersze = 3;
float odstepK = 80.f;
float odstepW = 40.f;

struct StanGracza
{
    float x;    // pozycja x
    float y;    // pozycja y
    int zycia;  // liczba żyć gracza
    int punkty; // punkty gracza
};

struct StanOslon
{
    float x;
    float y;
    int zycia;
};

struct StanWroga
{
    float x; // pozycja x
    float y; // pozycja y
    int typ; // typ wroga
    int zycia;
};

// ladowanie czcionki i tekstury
bool zaladuj(Zasoby &zasoby)
{
    if (!zasoby.czcionka.loadFromFile("../assets/Pricedown Bl.otf"))
    {
        std::cout << "Nie udalo sie zaladowac czcionki!" << std::endl;
        return false;
    }
    if (!zasoby.teksturaSerca.loadFromFile("../assets/serce.png"))
    {
        std::cout << "Nie udalo sie zaladowac obrazka serca!" << std::endl;
        return false;
    }
    if (!zasoby.teksturaTlo1.loadFromFile("../assets/tlo1.png"))
    {
        std::cout << "Nie udalo sie zaladowac tla nr 1";
        return false;
    }
    if (!zasoby.teksturaTlo2.loadFromFile("../assets/tlo2.png"))
    {
        std::cout << "Nie udalo sie zaladowac tla nr 2";
        return false;
    }
    if (!zasoby.teksturaTlo3.loadFromFile("../assets/tlo3.png"))
    {
        std::cout << "Nie udalo sie zaladowac tla nr 3";
        return false;
    }
    // tworzenie czarnej tekstury do tla
    zasoby.czarneTlo.create(960, 600, sf::Color::Black);
    zasoby.czarnaTekstura.loadFromImage(zasoby.czarneTlo);

    if (!zasoby.bufferStrzalu.loadFromFile("../assets/strzal.wav"))
    {
        std::cout << "Nie udalo sie zaladowac dzwieku strzalu!" << std::endl;
        return false;
    }
    zasoby.dzwiekStrzalu.setBuffer(zasoby.bufferStrzalu);
    return true;
}

// funkcja do zapisu stanu gry do pliku
void zapiszStanGry(const std::string &nazwaPliku, const Gracz &gracz, const std::vector<Wrog> &wrogowie, const std::vector<Oslona> &oslony)
{
    FILE *fp = fopen(nazwaPliku.c_str(), "wb");
    if (!fp)
    {
        std::cout << "Nie mozna otworzyc pliku do zapisu stanu gry!" << std::endl;
        return;
    }

    // zapis stanu gracza
    StanGracza stanGracza = {gracz.pobierzPozycje().x, gracz.pobierzPozycje().y, gracz.pobierzLiczbeZyc(), gracz.pobierzPunkty()};
    fwrite(&stanGracza, sizeof(StanGracza), 1, fp);

    // zapis zmiennych globalnych
    fwrite(&aktualnyPoziom, sizeof(int), 1, fp);
    fwrite(&kolumny, sizeof(int), 1, fp);
    fwrite(&wiersze, sizeof(int), 1, fp);
    fwrite(&minimalnyCzasStrzalu, sizeof(float), 1, fp);
    fwrite(&minimalnyCzasStrzaluWroga, sizeof(float), 1, fp);
    fwrite(&zmienKierunek, sizeof(bool), 1, fp);
    fwrite(&kierunek, sizeof(float), 1, fp);
    fwrite(&interwalRuchu, sizeof(float), 1, fp);

    //zapis stanu oslon
    int liczbaOslon = oslony.size();
    fwrite(&liczbaOslon, sizeof(int), 1, fp);
    for (const auto &oslona : oslony)
    {
        StanOslon stanOslon = {oslona.pobierzKsztalt().getPosition().x, oslona.pobierzKsztalt().getPosition().y, oslona.pobierzZycia()};
        fwrite(&stanOslon, sizeof(StanOslon), 1, fp);
    }

    // Zapis stanu wrogów
    int liczbaWrogow = wrogowie.size();
    fwrite(&liczbaWrogow, sizeof(int), 1, fp);
    for (const auto &wrog : wrogowie)
    {
        StanWroga stanWroga = {wrog.pobierzKsztalt().getPosition().x, wrog.pobierzKsztalt().getPosition().y, wrog.pobierzTyp(), wrog.pobierzZycia()};
        fwrite(&stanWroga, sizeof(StanWroga), 1, fp);
    }

    fclose(fp);
}

// funkcja do odczytu stanu gry
bool odczytajStanGry(const std::string &nazwaPliku, Gracz &gracz, std::vector<Wrog> &wrogowie, std::vector<Oslona> &oslony)
{
    FILE *fp = fopen(nazwaPliku.c_str(), "rb");
    if (!fp)
    {
        std::cout << "Nie mozna otworzyc pliku do odczytu stanu gry!" << std::endl;
        return false;
    }

    // odczyt stanu gracza
    StanGracza stanGracza;
    fread(&stanGracza, sizeof(StanGracza), 1, fp);
    gracz.ustawPozycje(stanGracza.x, stanGracza.y);
    gracz.ustawZycia(stanGracza.zycia, zasoby.teksturaSerca);
    gracz.ustawPunkty(stanGracza.punkty);

    // odczyt zmiennych globalnych
    fread(&aktualnyPoziom, sizeof(int), 1, fp);
    fread(&kolumny, sizeof(int), 1, fp);
    fread(&wiersze, sizeof(int), 1, fp);
    fread(&minimalnyCzasStrzalu, sizeof(float), 1, fp);
    fread(&minimalnyCzasStrzaluWroga, sizeof(float), 1, fp);
    fread(&zmienKierunek, sizeof(bool), 1, fp);
    fread(&kierunek, sizeof(float), 1, fp);
    fread(&interwalRuchu, sizeof(float), 1, fp);

    // odczyt stanu oslon
    int liczbaOslon;
    fread(&liczbaOslon, sizeof(int), 1, fp);
    oslony.clear();
    for (int i = 0; i < liczbaOslon; i++)
    {
        StanOslon stanOslon;
        fread(&stanOslon, sizeof(StanOslon), 1, fp);
        Oslona oslona(stanOslon.x, stanOslon.y, stanOslon.zycia);
        oslony.push_back(oslona);
    }

    // odczyt stanu wrogow
    int liczbaWrogow;
    fread(&liczbaWrogow, sizeof(int), 1, fp);
    wrogowie.clear();
    for (int i = 0; i < liczbaWrogow; i++)
    {
        StanWroga stanWroga;
        fread(&stanWroga, sizeof(StanWroga), 1, fp);
        Wrog wrog(stanWroga.x, stanWroga.y, stanWroga.typ, stanWroga.zycia);
        wrogowie.push_back(wrog);
    }

    fclose(fp);

    return true;
}

// funckja do usuwania pliku
void usunStanGry(const std::string &nazwaPliku)
{
    if (std::remove(nazwaPliku.c_str()) != 0)
    {
        std::cout << "Nie mozna usunac pliku stanu gry!" << std::endl;
    }
    else
    {
        std::cout << "Plik stanu gry zostal usuniety." << std::endl;
    }
}

void wyswietlMenu(sf::RenderWindow &window, StanGry &stan, Menu &menu)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Down)
                menu.przesunWDol();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                menu.przesunWGore();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                int opcja = menu.pobierzWybranaOpcje();
                if (opcja == 0)
                {
                    stan = StanGry::WYBOR;
                }
                else if (opcja == 1)
                {
                    stan = StanGry::RANKING;
                }
                else if (opcja == 2)
                {
                    stan = StanGry::USTAWIENIA;
                }
                else if (opcja == 3)
                {
                    window.close();
                }
            }
        }
    }
    window.clear();
    window.draw(zasoby.tlo);
    menu.rysuj(window);
    window.display();
}

void wyswietlUstawienia(sf::RenderWindow &window, StanGry &stan, Ustawienia &ustawienia)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Left)
            {
                ustawienia.przesunWLewo();
            }
            else if (event.key.code == sf::Keyboard::Right)
            {
                ustawienia.przesunWPrawo();
            }
            else if (event.key.code == sf::Keyboard::Enter)
            {
                // po wciesnieciu enter tlo jest ustawiane na nowe, pobierajac aktualnie wybrane tlo w ustawieniach
                if (ustawienia.pobierzWybranaOpcje() == 0)
                {
                    zasoby.tlo.setTexture(zasoby.teksturaTlo1);
                }
                else if (ustawienia.pobierzWybranaOpcje() == 1)
                {
                    zasoby.tlo.setTexture(zasoby.teksturaTlo2);
                }
                else if (ustawienia.pobierzWybranaOpcje() == 2)
                {
                    zasoby.tlo.setTexture(zasoby.teksturaTlo3);
                }
                else if (ustawienia.pobierzWybranaOpcje() == 3)
                {
                    zasoby.tlo.setTexture(zasoby.czarnaTekstura);
                }
                stan = MENU; // po zatwierdzeniu powrot do menu
            }
            else if (event.key.code == sf::Keyboard::Escape)
            {
                stan = StanGry::MENU; // powrot do menu bez zmian
            }
        }
    }
    window.clear();
    ustawienia.rysuj(window);
    window.display();
}

void wyswietlRanking(sf::RenderWindow &window, StanGry &stan, Ranking &ranking)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape)
            {
                stan = StanGry::MENU;
                return;
            }
        }
    }
    window.clear();
    window.draw(zasoby.tlo);
    ranking.rysuj(window);
    window.display();
}

void przesunWrogow(std::vector<Wrog> &wrogowie, bool &zmienKierunek, bool &przesunietoPredzej, std::vector<Oslona> &oslony)
{
    // ruch wrogow co okreslony czas
    if (czasOdOstatniegoRuchu >= interwalRuchu)
    {
        // jesli w poprzednim kroku byl ruch w dol, to sie wykonuje i blokuje nizsze wywolanie
        if (zmienKierunek)
        {
            for (auto &wrog : wrogowie)
            {
                wrog.przesun(kierunek * odstepK, 0.0f);
                przesunietoPredzej = true;
            }
        }
        // zerowanie flagi przesuniecia
        zmienKierunek = false;
        // sprawdzenie czy ktorys dotarl do krawedzi
        for (auto &wrog : wrogowie)
        {
            if (wrog.pobierzKsztalt().getPosition().x <= 15 ||
                wrog.pobierzKsztalt().getPosition().x + wrog.pobierzKsztalt().getSize().x >= 930)
            {
                zmienKierunek = true;
                break;
            }
            if (wrog.pobierzKsztalt().getPosition().y >= 430)
            {
                oslony.clear();
            }
            // jezeli dotrtarl do dolu koniec gry
            if (wrog.pobierzKsztalt().getPosition().y + wrog.pobierzKsztalt().getSize().y >= 530)
            {
                koniecGry = true;
                graWstrzymana = true;
            }
        }

        // jesli wrog dotarl do krawedzi to zmienia sie kierunek
        if (zmienKierunek)
        {
            kierunek = -kierunek;

            // przesuniecie w dol
            for (auto &wrog : wrogowie)
            {
                wrog.przesun(0.0f, odstepW);
            }
        }
        else
        {
            if (!przesunietoPredzej)
            {
                // ruch poziomy, jesli na samym poczatku nie zostalo wykonane
                for (auto &wrog : wrogowie)
                {
                    wrog.przesun(kierunek * odstepK, 0.0f);
                }
            }
            else
            {
                // jesli wczesniej wykonano ruch to sie zeruje
                przesunietoPredzej = false;
                czasOdOstatniegoRuchu = 0.f;
            }
        }

        // reset czasu ruchu
        czasOdOstatniegoRuchu = 0.0f;
    }
}

void aktualizujPociski(std::vector<Pocisk> &pociskiGracza, std::vector<Pocisk> &pociskiWroga, float &deltaTime, Gracz &gracz, std::vector<Wrog> &wrogowie, std::vector<Oslona> &oslony)
{
    // aktualizacja pociskow gracz, kolizja oraz wyjscie poza okno
    for (auto pociskIt = pociskiGracza.begin(); pociskIt != pociskiGracza.end();) // pociskIt to iterator
    {
        pociskIt->ruszaj(deltaTime); // ruch posisku

        bool pociskZniszczony = false;

        // sprawdzenie kolizji pocisku z oslonami
        for (auto oslonaIt = oslony.begin(); oslonaIt != oslony.end();)
        {
            if (pociskIt->pobierzObszar().intersects(sf::FloatRect(oslonaIt->pobierzKsztalt().getPosition(), oslonaIt->pobierzKsztalt().getSize())))
            {
                pociskIt = pociskiGracza.erase(pociskIt);
                pociskZniszczony = true;
                break; // wychodzimy z petli oslony, poniewaz pocisk zostal juz usuniety
            }
            else
            {
                ++oslonaIt;
            }
        }

        if (pociskZniszczony)
        {
            continue; // przechodzimy do kolejnego pocisku gracza
        }

        for (auto wrogIt = wrogowie.begin(); wrogIt != wrogowie.end();)
        {
            // sprawdzenie kolizji pocisku z wrogiem
            if (pociskIt->pobierzObszar().intersects(sf::FloatRect(wrogIt->pobierzKsztalt().getPosition(), wrogIt->pobierzKsztalt().getSize())))
            {
                if (!wrogIt->stracZycie())
                {
                    gracz.dodajPunkty(wrogIt->pobierzPunkty() * 0.5 * aktualnyPoziom); // dodanie punktow w zaleznosci od wartosci wroga
                    // jesli kolizja to wrog jest niszczony
                    wrogIt = wrogowie.erase(wrogIt);
                }
                else
                {
                    ++wrogIt;
                }
                // po kolizji pocisk jest niszczony
                pociskIt = pociskiGracza.erase(pociskIt);
                pociskZniszczony = true;
                break;
            }
            else
            {
                ++wrogIt;
            }
        }
        // usuwanie pociskow po zderzeniu lub ktore wyszly poza zakres
        if (!pociskZniszczony)
        {
            if (pociskIt->pobierzObszar().top + pociskIt->pobierzObszar().height < 20)
            {
                pociskIt = pociskiGracza.erase(pociskIt);
            }
            else
            {
                ++pociskIt;
            }
        }
    }

    // tworzenie pociskow wroga
    if (czasOdOstatniegoStrzaluWroga >= minimalnyCzasStrzaluWroga && !wrogowie.empty())
    {
        int iloscWrogow = wrogowie.size();
        for (int i = 0; i < iloscWrogow / 14 + 1; i++) // 12 + 2
        {
            int indeksWroga = rand() % wrogowie.size();
            if (!wrogowie[indeksWroga].czyStrzelil)
            {
                sf::Vector2f pozycjaWroga = wrogowie[indeksWroga].pobierzKsztalt().getPosition() + sf::Vector2f(wrogowie[indeksWroga].pobierzKsztalt().getSize().x / 2, wrogowie[indeksWroga].pobierzKsztalt().getSize().y);
                // dla bossa wieksza predkosc i inny kolor
                sf::Color kolorPocisku = wrogowie[indeksWroga].pobierzTyp() == 4 ? sf::Color::Red : sf::Color::Yellow;
                float predksocPocisku = wrogowie[indeksWroga].pobierzTyp() == 4 ? 1.25f : 1.f;
                int typWroga = wrogowie[indeksWroga].pobierzTyp();

                pociskiWroga.emplace_back(pozycjaWroga.x, pozycjaWroga.y, predksocPocisku, kolorPocisku, typWroga);

                wrogowie[indeksWroga].czyStrzelil = true; // zapobiegniecie podwojnemu wylosowaniu i wystrzeleniu
                czasOdOstatniegoStrzaluWroga = 0.f;
            }
        }
    }
    // reset flagi czyStrzelil dla wszystkich wrogow
    for (auto &wrog : wrogowie)
    {
        wrog.czyStrzelil = false;
    }

    // aktualizaja pociskow wrogow

    for (auto pociskIt = pociskiWroga.begin(); pociskIt != pociskiWroga.end();)
    {
        pociskIt->ruszaj(deltaTime);

        bool pociskZniszczony = false;

        // sprawdzenie kolizji miedzy pociskami a oslonami
        for (auto oslonaIt = oslony.begin(); oslonaIt != oslony.end();)
        {
            if (pociskIt->pobierzObszar().intersects(sf::FloatRect(oslonaIt->pobierzKsztalt().getPosition(), oslonaIt->pobierzKsztalt().getSize())))
            {
                if (!oslonaIt->stracZycie())
                {
                    // jesli kolizja to wrog jest niszczony
                    oslonaIt = oslony.erase(oslonaIt);
                }
                else
                {
                    ++oslonaIt;
                }
                // po kolizji pocisk jest niszczony
                pociskIt = pociskiWroga.erase(pociskIt);
                pociskZniszczony = true;
                break;
            }
            else
            {
                ++oslonaIt;
            }
        }

        if (pociskZniszczony)
        {
            continue; // przechodzimy do kolejnego pocisku wroga
        }

        // sprawdzenie kolizji miedzy pociskami a graczem
        if (pociskIt->pobierzObszar().intersects(sf::FloatRect(gracz.pobierzPozycje(), gracz.pobierzRozmiar())))
        {
            // jezeli pocisk trafi to gracz traci zycie, w zaleznosci od obrazen pocisku
            bool zycia = gracz.stracZycie(pociskIt->pobierzObrazenia());
            pociskIt = pociskiWroga.erase(pociskIt); // niszczenie pocisku po trafieniu

            if (!zycia)
            {
                koniecGry = true;
                graWstrzymana = true;
            }
        }
        // usuniecie pociskow spoza ekranu
        else if (pociskIt->pobierzObszar().top > 580 - pociskIt->pobierzObszar().getSize().y)
        {
            pociskIt = pociskiWroga.erase(pociskIt);
        }
        else
        {
            ++pociskIt;
        }
    }
}

bool obslugaZdarzen(sf::RenderWindow &window, sf::Event &event, UstawTekst &ustawTekst, Ranking &ranking, bool &graWstrzymana, StanGry stan, Gracz &gracz, std::vector<Pocisk> &pociskiGracza, std::vector<Wrog> &wrogowie, std::vector<Oslona> &oslony)
{
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            zapiszStanGry("stan_gry.dat", gracz, wrogowie, oslony); // zapisanie stanu gry przy zamknieciu okna
            window.close();
        }
        // obsluga przyciskow gdy gra jest aktywna
        if (koniecGry)
        {
            // pobranie znakow od uzytkownika
            ustawTekst.pobierzZnak(window);

            // zatwierdzenie tekstu enterem
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !ustawTekst.pobierzWejscie().empty())
            {
                std::string nick = ustawTekst.pobierzWejscie();
                ranking.dodajWynik(nick, gracz.pobierzPunkty());

                // usuniecie pliku stanu gry po przegranej
                usunStanGry("stan_gry.dat");

                // przy nowej grze zerowanie parametrow
                aktualnyPoziom = 1;
                kierunek = 1;
                interwalRuchu = 3.f;
                minimalnyCzasStrzalu = 1.f;
                minimalnyCzasStrzaluWroga = 1.f;
                kolumny = 7;
                wiersze = 3;
                return false;
            }
        }

        if (event.type == sf::Event::KeyPressed)
        {
            // otwieranie zapytania o wyjscie
            if (!pomocAktywna && !koniecGry && event.key.code == sf::Keyboard::Escape)
            {
                graWstrzymana = true;
                wyjscieAktywne = true;
            }
            // powrot z pomocy
            if (pomocAktywna && event.key.code == sf::Keyboard::Escape)
            {
                pomocAktywna = false;
                graWstrzymana = false;
            }
            // zapytanie o wyjscie
            if (wyjscieAktywne)
            {
                // powrot do menu
                if (event.key.code == sf::Keyboard::T)
                {
                    wyjscieAktywne = false;
                    zapiszStanGry("stan_gry.dat", gracz, wrogowie, oslony);
                    return false;
                }
                // wznowienie gry
                else if (event.key.code == sf::Keyboard::N)
                {
                    graWstrzymana = false;
                    wyjscieAktywne = false;
                }
            }
            // otwarcie pomocy
            if (event.key.code == sf::Keyboard::F1 && !wyjscieAktywne)
            {
                graWstrzymana = true;
                pomocAktywna = true;
            }
            // strzaly gracza po wciscnieciu spacji
            if (!graWstrzymana && event.key.code == sf::Keyboard::Space && czasOdOstatniegoStrzalu >= minimalnyCzasStrzalu)
            {
                zasoby.dzwiekStrzalu.play();
                float x = gracz.pobierzPozycje().x + gracz.pobierzRozmiar().x / 2 - 2.5f;
                float y = gracz.pobierzPozycje().y;
                pociskiGracza.emplace_back(x, y, -1.0f); // -1.0f - leci do gory
                czasOdOstatniegoStrzalu = 0.0f;
            }
        }
    }
    return true;
}

void zwiekszPoziom(Gracz &gracz)
{
    aktualnyPoziom++;
    interwalRuchu -= 0.2f;             // zmniejszenie interwalu ruchu wrogow
    minimalnyCzasStrzalu -= 0.2f;      // zmniejszenei minimalnego strzalu
    minimalnyCzasStrzaluWroga -= 0.2f; // zmniejszenie minimalnego strzalu
    kolumny += 1;
    wiersze += 1;
    if (interwalRuchu < 0.5f)
        interwalRuchu = 0.5f; // minimalny interwal ruchu
    if (minimalnyCzasStrzalu < 0.2f)
        minimalnyCzasStrzalu = 0.2f; // minimalny czas strzalu
    if (minimalnyCzasStrzaluWroga < 0.5f)
        minimalnyCzasStrzaluWroga = 0.5f; // minimalny czas strzalu wroga
    if (kolumny > 11)
        kolumny = 11;
    if (wiersze > 5)
        wiersze = 5;

    gracz.dodajPunkty(aktualnyPoziom * 100); // bonusowe punkty za poziom
}

void resetujGre(Gracz &gracz, std::vector<Wrog> &wrogowie, std::vector<Pocisk> &pociskiGracza, std::vector<Pocisk> &pociskiWroga, std::vector<Oslona> &oslony)
{
    gracz.resetuj();  // powrot do pierwotnej pozycji
    wrogowie.clear(); // czyszczenie wektorow by ponownie zaladowac
    pociskiGracza.clear();
    pociskiWroga.clear();
    oslony.clear();
    kierunek = 1.0f; // zerowanie kierunku, aby wrogowie przesuwali sie w prawo

    // jesli jest boss to przenosi reszte wrogow w dol o pozycjaBossaY
    float pozycjaBossaY = 0.f;
    if (aktualnyPoziom % 2 == 0)
    {
        pozycjaBossaY = 60.f;
    }
    // tworzenie wrogow
    for (int kolumna = 0; kolumna < kolumny; ++kolumna)
    {
        for (int rzad = 0; rzad < wiersze; ++rzad)
        {
            float x = kolumna * odstepK + 95.f;              // odstep miedzy kolumnanmi +95
            float y = rzad * odstepW + 90.f + pozycjaBossaY; // odstep miedzy rzedami, jesli jest boss to przesunieci w dol
            int typ;
            if (rzad == 0)
                typ = 1;
            else if (rzad == 1)
                typ = 2;
            else
                typ = 3;
            wrogowie.emplace_back(x, y, typ);
        }
    }

    // dodanie bossa na koncu poziomu
    if (aktualnyPoziom % 2 == 0)
    {
        // ustawienie pozycji bossa na srodku
        float x1 = wrogowie.front().pobierzKsztalt().getPosition().x;
        float x2 = wrogowie.back().pobierzKsztalt().getPosition().x + wrogowie.back().pobierzKsztalt().getSize().x;
        // boss - pozycja, typ-4, zycia Bossa
        wrogowie.emplace_back((x1 + x2) / 2 - 130.f / 2, 80.f, 4, zyciaBossa + aktualnyPoziom);
    }
    pozycjaBossaY = 0.f;

    // tworzenie oslon
    for (int i = 0; i < 4; ++i)
    {
        float x = i * (104.f + 100.f) + 124.f;
        float y = 470.f;
        int zycia = 10 * aktualnyPoziom;
        oslony.emplace_back(x, y, zycia);
    }
}

void sprawdzPoziom(Gracz &gracz, std::vector<Wrog> &wrogowie, std::vector<Pocisk> &pociskiGracza, std::vector<Pocisk> &pociskiWroga, std::vector<Oslona> &oslony)
{
    if (wrogowie.empty())
    {
        gracz.dodajZycie(aktualnyPoziom, zasoby.teksturaSerca);
        zwiekszPoziom(gracz);
        resetujGre(gracz, wrogowie, pociskiGracza, pociskiWroga, oslony);
    }
}

void czyNowaGra(sf::RenderWindow &window, StanGry &stan, Komunikaty &komunikaty)
{
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::N)
                {
                    usunStanGry("stan_gry.dat");

                    // przy nowej grze zerowanie parametrow
                    aktualnyPoziom = 1;
                    minimalnyCzasStrzalu = 1.f;
                    minimalnyCzasStrzaluWroga = 1.f;
                    kolumny = 7;
                    wiersze = 3;
                    kierunek = 1;
                    interwalRuchu = 3;
                    stan = StanGry::GRA;
                    return;
                }
                if (event.key.code == sf::Keyboard::T)
                {
                    stan = StanGry::GRA;
                    return;
                }
            }
        }
        window.clear();
        window.draw(zasoby.tlo);
        komunikaty.czyNowaGra.rysuj(window);
        window.display();
    }
}

void uruchomGre(sf::RenderWindow &window, StanGry &stan, Komunikaty &komunikaty, Ranking &ranking)
{
    Gracz gracz(zasoby.teksturaSerca);
    std::vector<Wrog> wrogowie;
    std::vector<Pocisk> pociskiGracza;
    std::vector<Pocisk> pociskiWroga;
    std::vector<Oslona> oslony;
    UstawTekst ustawTekst(zasoby.czcionka, sf::Vector2f(400, 50), sf::Vector2f(280, 400)); // dodanie obiektu do wpisywania tekstu

    // sprawdzanie czy plik istnieje i odczytanie stanu
    if (!odczytajStanGry("stan_gry.dat", gracz, wrogowie, oslony))
    {
        resetujGre(gracz, wrogowie, pociskiGracza, pociskiWroga, oslony);
    }

    sf::Clock zegar;
    bool przesunietoPredzej = false;

    sf::RectangleShape zaciemnienie(sf::Vector2f(window.getSize().x, window.getSize().y));
    zaciemnienie.setFillColor(sf::Color(0, 0, 0, 200)); // czarny, przezroczystosc 200

    // Tworzenie ramki
    sf::RectangleShape ramka;
    ramka.setSize(sf::Vector2f(window.getSize().x - 40.f, window.getSize().y - 40.f)); // rozmiar o 40 px mniej niz okno
    ramka.setFillColor(sf::Color::Transparent);                                        // wypelnienie prostokata przezroczyste
    ramka.setOutlineThickness(20);                                                     // grubosc obramowania prostokąta
    ramka.setOutlineColor(sf::Color(255, 0, 0, 120));                                  // kolor obramowania - czerwony z przycemnieniem
    ramka.setPosition(20, 20);

    // linia graniczna gracza
    sf::Vertex linia[] =
        {
            sf::Vertex(sf::Vector2f(20, 550), sf::Color::Red),  // 1 punkt - czerwony
            sf::Vertex(sf::Vector2f(940, 550), sf::Color::Blue) // 2 punkt - niebieski
        };

    while (window.isOpen())
    {
        float deltaTime = zegar.restart().asSeconds();
        // odswiezanie tylko gdy gra jest aktywna
        if (!graWstrzymana)
        {
            czasOdOstatniegoRuchu += deltaTime;
            czasOdOstatniegoStrzalu += deltaTime;
            czasOdOstatniegoStrzaluWroga += deltaTime;
        }

        sf::Event event;
        // jesli zwroci false, koniec gry, i okno do wpisania nicku
        if (!obslugaZdarzen(window, event, ustawTekst, ranking, graWstrzymana, stan, gracz, pociskiGracza, wrogowie, oslony))
        {
            // reset stanu gry, powrot do menu
            koniecGry = false;
            graWstrzymana = false;
            stan = StanGry::MENU;
            return;
        }

        // logika obiektow
        if (!graWstrzymana)
        {
            przesunWrogow(wrogowie, zmienKierunek, przesunietoPredzej, oslony);
            aktualizujPociski(pociskiGracza, pociskiWroga, deltaTime, gracz, wrogowie, oslony);
            // sterowanie graczem
            gracz.steruj(deltaTime);
            sprawdzPoziom(gracz, wrogowie, pociskiGracza, pociskiWroga, oslony);
        }

        // Rysowanie
        window.clear();
        window.draw(ramka);
        window.draw(zasoby.tlo);
        window.draw(linia, 2, sf::Lines);
        for (const auto &wrog : wrogowie)
        {
            wrog.rysuj(window);
        }
        for (auto &pocisk : pociskiGracza)
        {
            pocisk.rysuj(window);
        }
        for (auto &pocisk : pociskiWroga)
        {
            pocisk.rysuj(window);
        }
        for (auto &oslona : oslony)
        {
            oslona.rysuj(window);
        }

        komunikaty.F1.rysuj(window);
        gracz.rysuj(window, zasoby.czcionka);

        if (pomocAktywna)
        {
            window.draw(zaciemnienie);
            komunikaty.pomoc.rysuj(window);
        }
        if (koniecGry)
        {
            window.draw(zaciemnienie);
            komunikaty.koniecGry.rysuj(window);
            komunikaty.koniecGry.rysuj(window);
            ustawTekst.rysuj(window); // okno do wpisywania nickow
        }
        if (wyjscieAktywne)
        {
            window.draw(zaciemnienie);
            komunikaty.wyjscie.rysuj(window);
        }
        window.display();
    }
}

int main()
{
    zaladuj(zasoby);

    // poczatkowy stan gry
    StanGry stan = MENU;

    sf::RenderWindow window(sf::VideoMode(960, 600), "Space invaders");

    Menu menu(zasoby.czcionka);
    Ranking ranking("../assets/ranking.txt", zasoby.czcionka);
    Ustawienia ustawienia(zasoby.czcionka, zasoby);

    // inicjalizacja komunikatow
    Komunikaty komunikaty = {
        Komunikat(zasoby.czcionka),
        Komunikat(zasoby.czcionka),
        Komunikat(zasoby.czcionka),
        Komunikat(zasoby.czcionka),
        Komunikat(zasoby.czcionka)};

    komunikaty.pomoc.ustawTekst(
        "                                         Pomoc\n\n"
        "-  <-  ->  - Poruszanie graczem\n"
        "- Spacja - Strzal\n"
        "- F1 - Wyswietlnie pomocy\n"
        "- ESC - Powrot\n\n"
        "Wcisnij ESC aby wrocic.",
        sf::Color::Black, sf::Color::Blue);
    komunikaty.pomoc.ustawPozycje(50.f, 30.f);

    komunikaty.koniecGry.ustawTekst("                     koniec gry\n\n\nWpisz swoj nick i wcisnij ENTER", sf::Color::Red, sf::Color::Black);
    komunikaty.koniecGry.ustawPozycje(((window.getSize().x - komunikaty.koniecGry.pobierzTekst().getLocalBounds().width) / 2), 120);

    komunikaty.wyjscie.ustawTekst("Czy na pewno chcesz wyjsc? (T/N)", sf::Color::White, sf::Color::Black);
    komunikaty.wyjscie.ustawPozycje(((window.getSize().x - komunikaty.wyjscie.pobierzTekst().getLocalBounds().width) / 2), window.getSize().y / 3);

    komunikaty.F1.ustawTekst("F1 - pomoc", sf::Color::Cyan, sf::Color::Black);
    komunikaty.F1.ustawPozycje(((window.getSize().x - komunikaty.F1.pobierzTekst().getLocalBounds().width) / 2), 15.f);

    komunikaty.czyNowaGra.ustawTekst("  czy chcesz kontynuuowac\npoprzednia rozrywke? [T/N]", sf::Color::White, sf::Color::Black);
    komunikaty.czyNowaGra.ustawPozycje(((window.getSize().x - komunikaty.czyNowaGra.pobierzTekst().getLocalBounds().width) / 2), window.getSize().y / 3);

    while (window.isOpen())
    {
        switch (stan)
        {
        case MENU:
            wyswietlMenu(window, stan, menu);
            break;
        case RANKING:
            wyswietlRanking(window, stan, ranking);
            break;
        case GRA:
            uruchomGre(window, stan, komunikaty, ranking);
            break;
        case USTAWIENIA:
            wyswietlUstawienia(window, stan, ustawienia);
            break;
        case WYBOR:
            if (std::ifstream("stan_gry.dat"))
            {
                czyNowaGra(window, stan, komunikaty);
            }
            else
                stan = StanGry::GRA;
            break;
        default:
            break;
        }
    }
    return 0;
}
