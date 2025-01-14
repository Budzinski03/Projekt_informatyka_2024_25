#include <iostream>
#include <SFML\Graphics.hpp>
#include <vector>
#include <fstream>

class Menu
{
public:
    Menu(const sf::Font &);
    void rysuj(sf::RenderWindow &window);
    void przesunWGore();
    void przesunWDol();
    int pobierzWybranaOpcje() const;

private:
    int wybranaOpcja;
    sf::Font czcionka;
    std::vector<sf::Text> opcje;
};

Menu::Menu(const sf::Font &czcionka) : wybranaOpcja(0)
{
    std::string tekstOpcji[] = {"Nowa gra", "Ranking", "Wyjscie"};

    for (int i = 0; i < 3; i++)
    {
        sf::Text tekst;
        tekst.setFont(czcionka);
        // jesli i = wybranaOpcja to kolor czerowny, jesli nie to bialy
        tekst.setFillColor(i == wybranaOpcja ? sf::Color::Red : sf::Color::White);
        tekst.setString(tekstOpcji[i]);
        tekst.setPosition(sf::Vector2f(960 / 10, 600 / (6) * (i + 1)));
        opcje.push_back(tekst);
    }
}

void Menu::rysuj(sf::RenderWindow &window)
{
    for (const auto &opcja : opcje)
    {
        window.draw(opcja);
    }
}

void Menu::przesunWGore()
{
    if (wybranaOpcja - 1 >= 0)
    {
        opcje[wybranaOpcja].setFillColor(sf::Color::White);
        wybranaOpcja--;
        opcje[wybranaOpcja].setFillColor(sf::Color::Red);
    }
}

void Menu::przesunWDol()
{
    // sprawdzenie czy mozna przesunac w dol
    if (wybranaOpcja + 1 < opcje.size())
    {
        opcje[wybranaOpcja].setFillColor(sf::Color::White);
        wybranaOpcja++;
        opcje[wybranaOpcja].setFillColor(sf::Color::Red);
    }
}

int Menu::pobierzWybranaOpcje() const
{
    return wybranaOpcja;
}

class Gracz
{
public:
    Gracz(const sf::Texture &teksturaSerca);
    void steruj(float deltaTime);
    void rysuj(sf::RenderWindow &window, sf::Font &czcionka);
    sf::Vector2f pobierzPozycje() const;
    void stracZycie();
    int pobierzLiczbeZyc() const;
    sf::Vector2f pobierzRozmiar() const;
    void dodajPunkty(int punkty);
    int pobierzPunkty() const;

private:
    sf::RectangleShape ksztalt;
    float predkosc;
    std::vector<sf::Sprite> serca;
    int liczbaZyc;
    int punkty;
    sf::Text tekstPunkty;
};

Gracz::Gracz(const sf::Texture &teksturaSerca) : predkosc(200.f), liczbaZyc(2), punkty(0)
{
    ksztalt.setSize(sf::Vector2f(60, 30));
    ksztalt.setFillColor(sf::Color::Green);
    ksztalt.setPosition(375, 550);

    tekstPunkty.setCharacterSize(30);
    tekstPunkty.setFillColor(sf::Color::Blue);

    // inicjalizacja serc
    for (int i = 0; i < liczbaZyc; ++i)
    {
        sf::Sprite serce;
        serce.setTexture(teksturaSerca);
        serce.setScale(0.05f, 0.05f);
        serce.setPosition(20 + i * 30, 20);
        serca.push_back(serce);
    }
}

// obsluga wejscia
void Gracz::steruj(float deltaTime)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        ksztalt.move(-predkosc * deltaTime, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        ksztalt.move(predkosc * deltaTime, 0);

    // ograniczenie ruchu w oknie
    if (ksztalt.getPosition().x < 0)
    {
        ksztalt.setPosition(0, ksztalt.getPosition().y);
    }
    if (ksztalt.getPosition().x > 910)
    {
        ksztalt.setPosition(910, ksztalt.getPosition().y);
    }
}

void Gracz::rysuj(sf::RenderWindow &window, sf::Font &czcionka)
{
    window.draw(ksztalt);

    // rysowanie serc
    for (const auto &serce : serca)
    {
        window.draw(serce);
    }

    // rysowanie punktow
    //  Rysowanie punkt¢w
    tekstPunkty.setFont(czcionka);
    tekstPunkty.setPosition(760, 10);
    tekstPunkty.setString("Punkty: " + std::to_string(punkty));
    window.draw(tekstPunkty);
}

sf::Vector2f Gracz::pobierzPozycje() const
{
    return ksztalt.getPosition();
}

void Gracz::stracZycie()
{
    if (liczbaZyc > 0)
        liczbaZyc--;
    serca.pop_back();
}

int Gracz::pobierzLiczbeZyc() const
{
    return liczbaZyc;
}

sf::Vector2f Gracz::pobierzRozmiar() const
{
    return ksztalt.getSize();
}

void Gracz::dodajPunkty(int nowePunkty)
{
    punkty += nowePunkty;
}

int Gracz::pobierzPunkty() const
{
    return punkty;
}

class Wrog
{
public:
    Wrog(float x, float y);
    void rysuj(sf::RenderWindow &window) const;
    const sf::RectangleShape &pobierzKsztalt() const; // Zwraca referencj©
    void ustawPozycje(float x, float y);
    void przesun(float dx, float dy);

private:
    sf::RectangleShape ksztalt;
};

Wrog::Wrog(float x, float y)
{
    ksztalt.setSize(sf::Vector2f(50, 25));
    ksztalt.setFillColor(sf::Color::Red);
    ksztalt.setPosition(x, y);
}

void Wrog::rysuj(sf::RenderWindow &window) const
{
    window.draw(ksztalt);
}

const sf::RectangleShape &Wrog::pobierzKsztalt() const
{
    return ksztalt; // Zwracamy referencj©
}

void Wrog::ustawPozycje(float x, float y)
{
    ksztalt.setPosition(x, y);
}

void Wrog::przesun(float dx, float dy)
{
    ksztalt.move(dx, dy);
}

class Pocisk
{
public:
    Pocisk(float x, float y, float kierunek);
    void ruszaj(float deltaTime);
    void rysuj(sf::RenderWindow &window);
    sf::FloatRect pobierzObszar() const;

private:
    sf::RectangleShape ksztalt;
    float predkosc;
    float kierunek; //-1 dla gracza +1 dla wroga
};

Pocisk::Pocisk(float x, float y, float kierunek) : predkosc(300.f), kierunek(kierunek)
{
    ksztalt.setSize(sf::Vector2f(5, 10));
    // jezeli kierunek < 0 to magenta, w innym przypadku yellow
    ksztalt.setFillColor(kierunek < 0 ? sf::Color::Magenta : sf::Color::Yellow);
    ksztalt.setPosition(x, y);
}

// ruch pocisk¢w
void Pocisk::ruszaj(float deltaTime)
{
    ksztalt.move(0, kierunek * predkosc * deltaTime);
}

void Pocisk::rysuj(sf::RenderWindow &window)
{
    window.draw(ksztalt);
}

sf::FloatRect Pocisk::pobierzObszar() const
{
    return ksztalt.getGlobalBounds();
}

class Ranking
{
public:
    Ranking(const std::string &nazwaPliku, const sf::Font &czcionka);
    void dodajWynik(const std::string &nazwaGracza, int punkty);
    void zaladuj();
    void zapisz();
    void rysuj(sf::RenderWindow &window);

private:
    std::string nazwaPliku;
    std::vector<std::pair<std::string, int>> wyniki;
    sf::Text tekstRanking;
    sf::Font czcionka;
};

Ranking::Ranking(const std::string &nazwaPliku, const sf::Font &czcionka) : nazwaPliku(nazwaPliku), czcionka(czcionka)
{
    zaladuj();
}

void Ranking::dodajWynik(const std::string &nazwaGracza, int punkty)
{
    wyniki.push_back({nazwaGracza, punkty});
    std::sort(wyniki.begin(), wyniki.end(), [](const auto &a, const auto &b)
              { return a.second > b.second; });
    if (wyniki.size() > 10) // max 10 wynikow
    {
        wyniki.pop_back();
    }
    zapisz();
}

void Ranking::zaladuj()
{
    wyniki.clear();
    std::ifstream plik(nazwaPliku);
    if (plik.is_open())
    {
        std::string nazwaGracza;
        int punkty;
        while (plik >> nazwaGracza >> punkty)
        {
            wyniki.push_back({nazwaGracza, punkty});
        }
        plik.close();
    }
}

void Ranking::zapisz()
{
    std::ofstream plik(nazwaPliku);
    if (plik.is_open())
    {
        for (const auto &wynik : wyniki)
        {
            plik << wynik.first << " " << wynik.second << "\n";
        }
        plik.close();
    }
}

void Ranking::rysuj(sf::RenderWindow &window)
{
    std::string tekst;
    int poz = 1;
    for (const auto &wynik : wyniki)
    {
        tekst += std::to_string(poz) + ". " + wynik.first + ": " + std::to_string(wynik.second) + "\n";
        poz++;
    }
    tekstRanking.setFont(czcionka);
    tekstRanking.setString(tekst);
    tekstRanking.setCharacterSize(30);
    tekstRanking.setFillColor(sf::Color::White);
    tekstRanking.setPosition(100, 100);
    window.draw(tekstRanking);
}

class Komunikat
{
public:
    Komunikat(const sf::Font &czcionka);
    void ustawTekst(const std::string &tekst, const sf::Color &kolor, const sf::Color &kolorObramowania);
    void rysuj(sf::RenderWindow &window);
    void ustawPozycje(float, float);
    std::string pobierzTekst();

private:
    sf::Text tekstKomunikatu;
};

Komunikat::Komunikat(const sf::Font &czcionka)
{
    tekstKomunikatu.setFont(czcionka);
    tekstKomunikatu.setCharacterSize(40);
    tekstKomunikatu.setPosition(60, 250);
}

void Komunikat::ustawTekst(const std::string &tekst, const sf::Color &kolor, const sf::Color &kolorObramowania)
{
    tekstKomunikatu.setString(tekst);
    tekstKomunikatu.setFillColor(kolor);
    tekstKomunikatu.setOutlineThickness(2);
    tekstKomunikatu.setOutlineColor(kolorObramowania);
}

void Komunikat::rysuj(sf::RenderWindow &window)
{
    window.draw(tekstKomunikatu);
}

void Komunikat::ustawPozycje(float x, float y)
{
    tekstKomunikatu.setPosition(x, y);
}

std::string Komunikat::pobierzTekst()
{
    return tekstKomunikatu.getString().toAnsiString();
}

class UstawTekst
{
private:
    sf::Text tekst;
    std::string wejscie;
    sf::RectangleShape tlo;

public:
    UstawTekst(sf::Font &czcionka, sf::Vector2f rozmiar, sf::Vector2f pozycja);
    void pobierzZnak(sf::RenderWindow &window);
    void rysuj(sf::RenderWindow &window);
    void czysc();
    sf::Text pobierzTekst();
    std::string pobierzWejscie() const;
    void wysrodkujTekst(); 
};

UstawTekst::UstawTekst(sf::Font &czcionka, sf::Vector2f rozmiar, sf::Vector2f pozycja)
{
    tekst.setFont(czcionka);
    tekst.setCharacterSize(30);
    tekst.setFillColor(sf::Color::Cyan);
    tekst.setPosition(pozycja.x, pozycja.y); // wysrodkowanie tekstu

    tlo.setSize(rozmiar);
    tlo.setFillColor(sf::Color(50, 50, 50, 200));
    tlo.setOutlineColor(sf::Color::White);
    tlo.setOutlineThickness(2);
    tlo.setPosition(pozycja);
}

void UstawTekst::wysrodkujTekst()
{
    sf::FloatRect granice = tekst.getLocalBounds();
    tekst.setOrigin(granice.width / 2, 0);
    tekst.setPosition(tlo.getPosition().x + tlo.getSize().x / 2, tlo.getPosition().y + tlo.getSize().y/2 - 20 );
}

void UstawTekst::pobierzZnak(sf::RenderWindow &window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {

        if (event.type == (sf::Event::Closed))
        {
            window.close();
        }
        // wywolywane za kazdym razem gdy uzytkownik wpisuje znak
        if (event.type == sf::Event::TextEntered)
        {
            // gdy uzytkowik wcisnie backspace, to usuwa ostatni znak
            if (event.text.unicode == '\b')
            {
                if (!wejscie.empty())
                {
                    wejscie.pop_back();
                }
            }
            else if(event.text.unicode == '\e')
            {
                continue;
            }
            // jezeli wprowadzony dane to znak ASCII to jest konwertowany na typ char i dodajemy do ciagu input
            else if (event.text.unicode < 128 && wejscie.size() < 15)
            {
                wejscie += static_cast<char>(event.text.unicode);
            }
            // aktualizacja tekstu
            tekst.setString(wejscie);
            wysrodkujTekst();
        }
    }
}

void UstawTekst::rysuj(sf::RenderWindow &window)
{
    window.draw(tlo);
    window.draw(tekst);
}

void UstawTekst::czysc()
{
    wejscie = "";
    tekst.setString("");
}

sf::Text UstawTekst::pobierzTekst()
{
    return tekst;
}

std::string UstawTekst::pobierzWejscie() const
{
    return wejscie;
}

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
    if (!czcionka.loadFromFile("Pricedown Bl.otf"))
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
    float czasOdOstatniegoRuchu = 1.5f;
    float czasOdOstatniegoStrzalu = 0.0f;
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
                ustawTekst.pobierzZnak(window); // Pobieranie znak¢w od u¾ytkownika
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
                {
                    std::string nick = ustawTekst.pobierzWejscie();
                    ranking.dodajWynik(nick, gracz.pobierzPunkty());
                    przegrana = false;
                    graWstrzymana = false;
                    stan = StanGry::MENU; // Przej˜cie do menu
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
                if (event.key.code == sf::Keyboard::F1 &&  !wyjscieAktywne)
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
                int indeksWroga = rand() % wrogowie.size();
                sf::Vector2f pozycjaWroga = wrogowie[indeksWroga].pobierzKsztalt().getPosition();
                pociskiWroga.emplace_back(pozycjaWroga.x + 20.f, pozycjaWroga.y + 20.f, 1.f);
                czasOdOstatniegoStrzaluWroga = 0.f;
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
            ustawTekst.rysuj(window); //okno do wpisywania nickow
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
