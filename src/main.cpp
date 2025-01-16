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
    RANKING
};

struct Komunikaty
{
    Komunikat pomoc;
    Komunikat koniecGry;
    Komunikat wyjscie;
    Komunikat F1;
};

struct Zasoby
{
    sf::Font czcionka;
    sf::Texture teksturaSerca;
    sf::SoundBuffer bufferStrzalu;
    sf::Sound dzwiekStrzalu;
};

Zasoby zasoby;

// flagi
bool graWstrzymana = false;
bool pomocAktywna = false;
bool koniecGry = false;
bool wyjscieAktywne = false;

int aktualnyPoziom = 1;
bool bossPokonany = false;

// parametry
float kierunek = 1.0f;     // predkosc wrogow w poziomie
float interwalRuchu = 3.f; // wrogowie przesuwani co 1s
float czasOdOstatniegoRuchu = 0.0f;
float czasOdOstatniegoStrzalu = 1.5f;
float minimalnyCzasStrzalu = 0.6f;
float czasOdOstatniegoStrzaluWroga = 0.f;
float minimalnyCzasStrzaluWroga = 1.0f;

// parametry wrogow
int kolumny = 6;
int wiersze = 3;
float odstepK = 80.f;
float odstepW = 40.f;

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
    if (!zasoby.bufferStrzalu.loadFromFile("../assets/strzal.wav"))
    {
        std::cout << "Nie udalo sie zaladowac dzwieku strzalu!" << std::endl;
        return false;
    }
    zasoby.dzwiekStrzalu.setBuffer(zasoby.bufferStrzalu);
    return true;
}

void wyswietlMenu(sf::RenderWindow &window, StanGry &stan, bool &graTrwa, Menu &menu)
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

void wyswietlRanking(sf::RenderWindow &window, StanGry &stan, bool &GraTrwa, Ranking &ranking)
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

void przesunWrogow(std::vector<Wrog> &wrogowie, bool &zmienKierunek, bool &przesunietoPredzej)
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
            // jezeli dotr? do do?u koniec gry
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
                wrog.przesun(0.0f, odstepW / 2);
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
                // continue;
            }
        }
        // reset czasu ruchu
        czasOdOstatniegoRuchu = 0.0f;
    }
}

void aktualizujPociski(std::vector<Pocisk> &pociskiGracza, std::vector<Pocisk> &pociskiWroga, float &deltaTime, Gracz &gracz, std::vector<Wrog> &wrogowie)
{
    // aktualizacja pociskow gracz, kolizja oraz wyjscie poza okno
    for (auto pociskIt = pociskiGracza.begin(); pociskIt != pociskiGracza.end();)
    { // ruch posisku
        pociskIt->ruszaj(deltaTime);

        bool wrogZniszczony = false;

        for (auto wrogIt = wrogowie.begin(); wrogIt != wrogowie.end();)
        {
            // Sprawdzenie kolizji pocisku z wrogiem
            if (pociskIt->pobierzObszar().intersects(sf::FloatRect(wrogIt->pobierzKsztalt().getPosition(), wrogIt->pobierzKsztalt().getSize())))
            {
                gracz.dodajPunkty(wrogIt->pobierzPunkty()); // dodanie punktow w zaleznosci od wartosci wroga
                // jesli kolizja to usuwa wroga
                wrogIt = wrogowie.erase(wrogIt);
                wrogZniszczony = true;
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
        for (int i = 0; i < iloscWrogow / 10 + 2; i++)
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

        // sprawdzenie kolizji miedzy pociskami a graczem
        if (pociskIt->pobierzObszar().intersects(sf::FloatRect(gracz.pobierzPozycje(), gracz.pobierzRozmiar())))
        {
            // jezeli pocisk trafi to gracz traci zycie, w zaleznosci od obrazen pocisku
            bool zycia = gracz.stracZycie(pociskIt->pobierzObrazenia()); //
            pociskIt = pociskiWroga.erase(pociskIt);                     // niszczenie pocisku po trafieniu

            if (!zycia)
            {
                koniecGry = true;
                graWstrzymana = true;
            }
        }
        // usuniecie pociskow spoza ekranu
        else if (pociskIt->pobierzObszar().top > 580-pociskIt->pobierzObszar().getSize().y)
        {
            pociskIt = pociskiWroga.erase(pociskIt);
        }
        else
        {
            ++pociskIt;
        }
    }
}

bool obslugaZdarzen(sf::RenderWindow &window, sf::Event event, bool &graTrwa, UstawTekst &ustawTekst, Ranking &ranking, bool &graWstrzymana, StanGry stan, Gracz &gracz, std::vector<Pocisk> &pociskiGracza)
{
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            graTrwa = false;
        }
        // obsluga przyciskow gdy gra jest aktywna
        if (koniecGry)
        {
            // pobranie znakow od uzytkownika
            ustawTekst.pobierzZnak(window);

            // Jesli wcicnisto Enter
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !ustawTekst.pobierzWejscie().empty())
                return false;
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
                    koniecGry = true;
                    wyjscieAktywne = false;

                    // pobranie znakow od uzytkownika
                    ustawTekst.pobierzZnak(window);
                    ustawTekst.czysc();

                    // Jesli wcisnieto Enter
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !ustawTekst.pobierzWejscie().empty())
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
                // return;
            }
            // strzaly gracza
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
    interwalRuchu -= 0.2f;             // Zmniejszenie interwalu ruchu wrogow
    minimalnyCzasStrzalu -= 0.1f;      // Zmniejszenie minimalnego czasu strzalu gracza
    minimalnyCzasStrzaluWroga -= 0.1f; // Zmniejszenie minimalnego czasu strzalu wroga
    if (interwalRuchu < 0.5f)
        interwalRuchu = 0.5f; // Ustawienie minimalnego interwalu ruchu
    if (minimalnyCzasStrzalu < 0.2f)
        minimalnyCzasStrzalu = 0.2f; // Ustawienie minimalnego czasu strzalu
    if (minimalnyCzasStrzaluWroga < 0.2f)
        minimalnyCzasStrzaluWroga = 0.2f; // Ustawienie minimalnego czasu strzalu wroga

    gracz.dodajPunkty(aktualnyPoziom * 100);
}

void resetujGre(Gracz &gracz, std::vector<Wrog> &wrogowie, std::vector<Pocisk> &pociskiGracza, std::vector<Pocisk> &pociskiWroga)
{
    gracz.resetuj();
    wrogowie.clear();
    pociskiGracza.clear();
    pociskiWroga.clear();

    // tworzenie wrogow
    for (int kolumna = 0; kolumna < kolumny + aktualnyPoziom - 1; ++kolumna)
    {
        for (int rzad = 0; rzad < wiersze + aktualnyPoziom - 1; ++rzad)
        {
            float x = kolumna * odstepK + 95.f; // odstep miedzy kolumnanmi
            float y = rzad * odstepW + 120;     // odstep miedzy rzedami
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

    // dodanie bossan na koncu poziomu
    if (aktualnyPoziom % 2 == 0)
    {
        float x1 = wrogowie.front().pobierzKsztalt().getPosition().x;
        float x2 = wrogowie.back().pobierzKsztalt().getPosition().x + wrogowie.back().pobierzKsztalt().getSize().x;
        wrogowie.emplace_back((x1 + x2) / 2 - 80.f / 2, 50.f, 4); // Przykladowy boss
    }
}

void sprawdzPoziom(Gracz &gracz, std::vector<Wrog> &wrogowie, std::vector<Pocisk> &pociskiGracza, std::vector<Pocisk> &pociskiWroga)
{
    if (wrogowie.empty())
    {
        resetujGre(gracz, wrogowie, pociskiGracza, pociskiWroga);
        gracz.dodajZycie(aktualnyPoziom, zasoby.teksturaSerca);
        zwiekszPoziom(gracz);
        if (bossPokonany || aktualnyPoziom % 2 != 0)
        {
            bossPokonany = false;
        }
    }
}

void uruchomGre(sf::RenderWindow &window, StanGry &stan, bool &graTrwa, Komunikaty &komunikaty, Ranking &ranking)
{
    Gracz gracz(zasoby.teksturaSerca);
    std::vector<Wrog> wrogowie;
    std::vector<Pocisk> pociskiGracza;
    std::vector<Pocisk> pociskiWroga;
    UstawTekst ustawTekst(zasoby.czcionka, sf::Vector2f(400, 50), sf::Vector2f(280, 400)); // dodanie obiektu do wpisywania tekstu

    resetujGre(gracz, wrogowie, pociskiGracza, pociskiWroga);

    sf::Clock zegar;
    bool zmienKierunek = false;
    bool przesunietoPredzej = false;

    sf::RectangleShape zaciemnienie(sf::Vector2f(window.getSize().x, window.getSize().y));
    zaciemnienie.setFillColor(sf::Color(0, 0, 0, 200)); // czarny, przezroczystosc 150

    // Tworzenie ramki
    sf::RectangleShape ramka;
    ramka.setSize(sf::Vector2f(920, 560));            // Rozmiar odpowiada wielkosci okna
    ramka.setFillColor(sf::Color::Transparent);       // Wype�nienie przezroczyste
    ramka.setOutlineThickness(20);                    // Grubo�� obramowania
    ramka.setOutlineColor(sf::Color(255, 0, 0, 120)); // Kolor ramki
    ramka.setPosition(20, 20);

    sf::Vertex linia[] =
        {
            sf::Vertex(sf::Vector2f(20, 550), sf::Color::Red),  // Punkt pocz�tkowy (czerwony)
            sf::Vertex(sf::Vector2f(940, 550), sf::Color::Blue) // Punkt ko�cowy (niebieski)
        };

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
        // jesli zwroci false, koniec gry, i okno do wpisania nicku
        if (!obslugaZdarzen(window, event, graTrwa, ustawTekst, ranking, graWstrzymana, stan, gracz, pociskiGracza))
        {
            std::string nick = ustawTekst.pobierzWejscie();
            ranking.dodajWynik(nick, gracz.pobierzPunkty());

            // reset stanu gry, powrot do menu
            koniecGry = false;
            graWstrzymana = false;
            stan = StanGry::MENU;
            return;
        }

        // logika obiektow
        if (!graWstrzymana)
        {
            przesunWrogow(wrogowie, zmienKierunek, przesunietoPredzej);
            aktualizujPociski(pociskiGracza, pociskiWroga, deltaTime, gracz, wrogowie);
            // sterowanie graczem
            gracz.steruj(deltaTime);
            sprawdzPoziom(gracz, wrogowie, pociskiGracza, pociskiWroga);
        }

        // Rysowanie
        window.clear();
        window.draw(ramka);
        window.draw(linia, 2, sf::Lines);
        gracz.rysuj(window, zasoby.czcionka);
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

    sf::RenderWindow window(sf::VideoMode(960, 600), "Space invaders");

    // Inicjalizacja komunikat?w
    Komunikaty komunikaty = {
        Komunikat(zasoby.czcionka),
        Komunikat(zasoby.czcionka),
        Komunikat(zasoby.czcionka),
        Komunikat(zasoby.czcionka)};

    komunikaty.pomoc.ustawTekst(
        "Pomoc:\n\n"
        "-  <-  ->  - Poruszanie graczem\n"
        "- Spacja - Strzal\n"
        "- F1 - Wyswietlnie pomocy\n"
        "- ESC - Powrot\n\n"
        "Wcisnij ESC aby wrocic.",
        sf::Color::Black, sf::Color::Blue);
    komunikaty.pomoc.ustawPozycje(50.f, 30.f);

    komunikaty.koniecGry.ustawTekst("                     koniec gry\n\n\nWpisz swoj nick i wcisnij ENTER", sf::Color::Red, sf::Color::Black);
    komunikaty.koniecGry.ustawPozycje(180, 120);

    komunikaty.wyjscie.ustawTekst("Czy na pewno chcesz wyjsc? (T/N)", sf::Color::White, sf::Color::Black);
    komunikaty.wyjscie.ustawPozycje(150, 250);

    komunikaty.F1.ustawTekst("F1 - pomoc", sf::Color::Cyan, sf::Color::Black);
    komunikaty.F1.ustawPozycje(360.f, 10.f);

    StanGry stan = MENU;
    bool graTrwa = true;

    Menu menu(zasoby.czcionka);
    Ranking ranking("ranking.txt", zasoby.czcionka);

    while (graTrwa)
    {
        switch (stan)
        {
        case MENU:
            aktualnyPoziom = 1;
            wyswietlMenu(window, stan, graTrwa, menu);
            break;
        case RANKING:
            wyswietlRanking(window, stan, graTrwa, ranking);
            break;
        case GRA:
            uruchomGre(window, stan, graTrwa, komunikaty, ranking);
            break;
        default:
            break;
        }
    }
    return 0;
}
