#include <iostream>
#include <SFML\Graphics.hpp>
#include <vector>
#include <fstream>
#include "klasy.h"

enum StanGry
{
    MENU,
    GRA,
    RANKING
};

struct Komunikaty
{
    Komunikat pomoc;
    Komunikat przegrana;
    Komunikat wyjscie;
    Komunikat F1;
};

bool graWstrzymana = false;
bool pomocAktywna = false;
bool przegrana = false;
bool wyjscieAktywne = false;
sf::Font czcionka;
sf::Texture teksturaSerca;

// ladowanie czcionki i tekstury
bool zaladuj(sf::Font &czcionka, sf::Texture &teksturaSerca)
{
    if (!czcionka.loadFromFile("../assets/Pricedown Bl.otf"))
    {
        std::cout << "Nie udalo sie zaladowac czcionki!" << std::endl;
        return false;
    }
    if (!teksturaSerca.loadFromFile("../assets/serce.png"))
    {
        std::cout << "Nie udalo sie zaladowac obrazka serca!" << std::endl;
        return false;
    }
    return true;
}

void wyswietlMenu(sf::RenderWindow &window, sf::Font &czcionka, StanGry &stan, bool &graTrwa, Menu &menu)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            graTrwa = false;
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
                    stan = StanGry::GRA;
                }
                else if (opcja == 1)
                {
                    stan = StanGry::RANKING;
                }
                else if (opcja == 2)
                {
                    window.close();
                    graTrwa = false;
                }
            }
        }
    }
    window.clear();
    menu.rysuj(window);
    window.display();
}

void wyswietlRanking(sf::RenderWindow &window, sf::Font &czcionka, StanGry &stan, bool &GraTrwa, Ranking &ranking)
{

    sf::Event event;

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            GraTrwa = false;
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
    ranking.rysuj(window);
    window.display();
}

void uruchomGre(sf::RenderWindow &window, sf::Font &czcionka, StanGry &stan, bool &graTrwa, sf::Texture teksturaSerca, Komunikaty &komunikaty, Ranking &ranking)
{
    Gracz gracz(teksturaSerca);
    std::vector<Wrog> wrogowie;
    std::vector<Pocisk> pociskiGracza;
    std::vector<Pocisk> pociskiWroga;
    UstawTekst ustawTekst(czcionka, sf::Vector2f(400, 50), sf::Vector2f(280, 300)); // Dodajemy instancj© UstawTekst

    // parametry wrogow
    int kolumny = 8;
    int wiersze = 4;
    float odstepK = 80.f;
    float odstepW = 60.f;
    // tworzenie wrogow
    for (int kolumna = 0; kolumna < kolumny; ++kolumna)
    {
        for (int rzad = 0; rzad < wiersze; ++rzad)
        {
            float x = kolumna * odstepK + 95.f; // odstep miedzy kolumnami
            float y = rzad * odstepW + 80;      // odstep miedzy rzedami
            wrogowie.emplace_back(x, y);
        }
    }

    sf::Clock zegar;
    float kierunek = 1.0f;      // predkosc wrogow w poziomie
    float interwalRuchu = 2.5f; // wrogowie przesuwani co 1s
    float czasOdOstatniegoRuchu = 0.0f;
    float czasOdOstatniegoStrzalu = 1.5f;
    float minimalnyCzasStrzalu = 1.0f;
    float czasOdOstatniegoStrzaluWroga = 0.f;
    float minimalnyCzasStrzaluWroga = 1.0f;
    bool zmienKierunek = false;
    bool przesunietoPredzej = false;

    sf::RectangleShape zaciemnienie(sf::Vector2f(window.getSize().x, window.getSize().y));
    zaciemnienie.setFillColor(sf::Color(0, 0, 0, 200)); // czarny, przezroczystosc 150

    while (graTrwa)
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
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                graTrwa = false;
            }
            // obsluga przyciskow gdy gra jest aktywna
            if (przegrana)
            {
                // pobranie znakow od uzytkownika
                ustawTekst.pobierzZnak(window);

                // Je˜li wci˜ni©to Enter
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !ustawTekst.pobierzWejscie().empty())
                {
                    std::string nick = ustawTekst.pobierzWejscie();
                    ranking.dodajWynik(nick, gracz.pobierzPunkty());

                    // reset stanu gry, powrot do menu
                    przegrana = false;
                    graWstrzymana = false;
                    stan = StanGry::MENU;
                    return;
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                // // resetowanie gry po przegranej
                // if (przegrana && event.key.code == sf::Keyboard::Enter)
                // {
                //     ranking.dodajWynik("Gracz", gracz.pobierzPunkty());
                //     przegrana = false;
                //     graWstrzymana = false;
                //     return;
                // }
                // otwieranie zapytania o wyjscie
                if (!pomocAktywna && !przegrana && event.key.code == sf::Keyboard::Escape)
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
                        stan = StanGry::MENU;
                        ranking.dodajWynik("Gracz", gracz.pobierzPunkty());
                        graWstrzymana = false;
                        wyjscieAktywne = false;
                        przegrana = false;
                        return;
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
                    // return;
                }
                // strzaly gracza
                if (!graWstrzymana && event.key.code == sf::Keyboard::Space && czasOdOstatniegoStrzalu >= minimalnyCzasStrzalu)
                {
                    float x = gracz.pobierzPozycje().x + gracz.pobierzRozmiar().x / 2 - 2.5f;
                    float y = gracz.pobierzPozycje().y;
                    pociskiGracza.emplace_back(x, y, -1.0f); // -1.0f - leci do gory
                    czasOdOstatniegoStrzalu = 0.0f;
                }
            }
        }
        // logika obiektow
        if (!graWstrzymana)
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

                zmienKierunek = false;
                // sprawdzenie czy ktorys dotarl do krawedzi
                for (auto &wrog : wrogowie)
                {
                    if (wrog.pobierzKsztalt().getPosition().x <= 15 ||
                        wrog.pobierzKsztalt().getPosition().x + wrog.pobierzKsztalt().getSize().x >= 945)
                    {
                        zmienKierunek = true;
                        break;
                    }
                    // jezeli dotr¥ do doˆu koniec gry
                    if (wrog.pobierzKsztalt().getPosition().y + wrog.pobierzKsztalt().getSize().y >= 550)
                    {
                        przegrana = true;
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
                        continue;
                    }
                }
                // reset czasu ruchu
                czasOdOstatniegoRuchu = 0.0f;
            }

            // aktualizacja pociskow gracz, kolizja oraz wyjscie poza okno
            for (auto pociskIt = pociskiGracza.begin(); pociskIt != pociskiGracza.end();)
            { // ruch posisku
                pociskIt->ruszaj(deltaTime);

                bool wrogZniszczony = false;

                for (auto wrogIt = wrogowie.begin(); wrogIt != wrogowie.end();)
                {
                    // Sprawdzenie kolizji pocisku z wrogiem
                    if (pociskIt->pobierzObszar().intersects(sf::FloatRect(wrogIt->pobierzKsztalt().getPosition(), sf::Vector2f(50, 25))))
                    {
                        // jesli kolizja to usuwa wroga
                        wrogIt = wrogowie.erase(wrogIt);
                        wrogZniszczony = true;
                        gracz.dodajPunkty(10); // Zakomentowana linia dodania punkt¢w
                        break;
                    }
                    else
                    {
                        ++wrogIt;
                    }
                }

                // usuwanie pociskow po zderzeniu lub ktore wyszly poza zakres
                if (wrogZniszczony || pociskIt->pobierzObszar().top + pociskIt->pobierzObszar().height < 0)
                {
                    pociskIt = pociskiGracza.erase(pociskIt);
                }
                else
                {
                    ++pociskIt;
                }
            }

            // tworzenie pociskow wroga
            if (czasOdOstatniegoStrzaluWroga >= minimalnyCzasStrzaluWroga && !wrogowie.empty())
            {
                int iloscWrogow = wrogowie.size();
                for (int i = 0; i < iloscWrogow / 6; i++)
                {
                    int indeksWroga = rand() % wrogowie.size();
                    sf::Vector2f pozycjaWroga = wrogowie[indeksWroga].pobierzKsztalt().getPosition();
                    pociskiWroga.emplace_back(pozycjaWroga.x + 20.f, pozycjaWroga.y + 20.f, 1.f);
                    czasOdOstatniegoStrzaluWroga = 0.f;
                }
            }

            // aktualizaja pociskow wrogow
            for (auto pociskIt = pociskiWroga.begin(); pociskIt != pociskiWroga.end();)
            {
                pociskIt->ruszaj(deltaTime);

                // sprawdzenie kolizji miedzy pociskami a graczem
                if (pociskIt->pobierzObszar().intersects(sf::FloatRect(gracz.pobierzPozycje(), sf::Vector2f(60, 30))))
                {
                    // jezeli pocisk trafi to gracz traci zycie
                    gracz.stracZycie();
                    pociskIt = pociskiWroga.erase(pociskIt);

                    if (gracz.pobierzLiczbeZyc() == 0)
                    {
                        przegrana = true;
                        graWstrzymana = true;
                    }
                }
                // usuniecie pociskow spoza ekranu
                else if (pociskIt->pobierzObszar().top > 600)
                {
                    pociskIt = pociskiWroga.erase(pociskIt);
                }
                else
                {
                    ++pociskIt;
                }
            }

            // sterowanie gracza
            gracz.steruj(deltaTime);
        }

        // Rysowanie
        window.clear();
        gracz.rysuj(window, czcionka);
        komunikaty.F1.rysuj(window);
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
        if (pomocAktywna)
        {
            window.draw(zaciemnienie);
            komunikaty.pomoc.rysuj(window);
        }
        if (przegrana)
        {
            window.draw(zaciemnienie);
            komunikaty.przegrana.rysuj(window);
            komunikaty.przegrana.rysuj(window);
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
    sf::Font czcionka;
    sf::Texture teksturaSerca;
    zaladuj(czcionka, teksturaSerca);

    sf::RenderWindow window(sf::VideoMode(960, 600), "Space invaders");

    // Inicjalizacja komunikat¢w
    Komunikaty komunikaty = {
        Komunikat(czcionka),
        Komunikat(czcionka),
        Komunikat(czcionka),
        Komunikat(czcionka)};

    komunikaty.pomoc.ustawTekst(
        "Pomoc:\n\n"
        "-  <-  ->  - Poruszanie graczem\n"
        "- Spacja - Strzal\n"
        "- F1 - Wyswietlnie pomocy\n"
        "- ESC - Powrot\n\n"
        "Wcisnij ESC aby wrocic.",
        sf::Color::Black, sf::Color::Blue);
    komunikaty.pomoc.ustawPozycje(50.f, 30.f);

    komunikaty.przegrana.ustawTekst("                  Przegrales\nWpisz swoj nick i wcisnij ENTER", sf::Color::Red, sf::Color::Black);
    komunikaty.przegrana.ustawPozycje(180, 200);

    komunikaty.wyjscie.ustawTekst("Czy na pewno chcesz wyjsc? (T/N)", sf::Color::White, sf::Color::Black);
    komunikaty.wyjscie.ustawPozycje(60, 250);

    komunikaty.F1.ustawTekst("F1 - pomoc", sf::Color::Cyan, sf::Color::Black);
    komunikaty.F1.ustawPozycje(360.f, 10.f);

    StanGry stan = MENU;
    bool graTrwa = true;

    Menu menu(czcionka);
    Ranking ranking("ranking.txt", czcionka);

    while (graTrwa)
    {
        switch (stan)
        {
        case MENU:
            wyswietlMenu(window, czcionka, stan, graTrwa, menu);
            break;
        case RANKING:
            wyswietlRanking(window, czcionka, stan, graTrwa, ranking);
            break;
        case GRA:
            uruchomGre(window, czcionka, stan, graTrwa, teksturaSerca, komunikaty, ranking);
            break;
        default:
            break;
        }
    }
    return 0;
}
