#include <iostream>
#include <SFML\Graphics.hpp>
#include <vector>
#include <fstream>

class Menu
{
public:
    Menu(float, float, const sf::Font &);
    void rysuj(sf::RenderWindow &window);
    void przesunWGore();
    void przesunWDol();
    int pobierzWybranaOpcje() const;

private:
    int wybranaOpcja;
    sf::Font czcionka;
    std::vector<sf::Text> opcje;
};

Menu::Menu(float width, float height, const sf::Font &czcionka) : wybranaOpcja(0)
{
    std::string tekstOpcji[] = {"Nowa gra", "Ranking", "Wyjscie"};

    for (int i = 0; i < 3; i++)
    {
        sf::Text tekst;
        tekst.setFont(czcionka);
        // jesli i = wybranaOpcja to kolor czerowny, jesli nie to bialy
        tekst.setFillColor(i == wybranaOpcja ? sf::Color::Red : sf::Color::White);
        tekst.setString(tekstOpcji[i]);
        tekst.setPosition(sf::Vector2f(800 / 10, 600 / (6) * (i + 1)));
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
    Gracz();
    void steruj(float);
    void rysuj(sf::RenderWindow &);
    sf::Vector2f pobierzPozycje() const;

private:
    sf::RectangleShape ksztalt;
    float predkosc;
};

Gracz::Gracz() : predkosc(200.f)
{
    ksztalt.setSize(sf::Vector2f(50, 20));
    ksztalt.setFillColor(sf::Color::Green);
    ksztalt.setPosition(375, 550);
}

// obsˆuga wej˜cia
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
    if (ksztalt.getPosition().x > 750)
    {
        ksztalt.setPosition(750, ksztalt.getPosition().y);
    }
}

void Gracz::rysuj(sf::RenderWindow &window)
{
    window.draw(ksztalt);
}

sf::Vector2f Gracz::pobierzPozycje() const
{
    return ksztalt.getPosition();
}

class Wrog
{
public:
    Wrog(float x, float y);
    void rysuj(sf::RenderWindow &window);
    sf::Vector2f pobierzPozycje() const;
    void ustawPozycje(float x, float y);
    void przesun(float dx, float dy);

private:
    sf::RectangleShape ksztalt;
    float predkosc;
};

Wrog::Wrog(float x, float y) : predkosc(50.f)
{
    ksztalt.setSize(sf::Vector2f(40, 20));
    ksztalt.setFillColor(sf::Color::Red);
    ksztalt.setPosition(x, y);
}

void Wrog::rysuj(sf::RenderWindow &window)
{
    window.draw(ksztalt);
}

sf::Vector2f Wrog::pobierzPozycje() const
{
    return ksztalt.getPosition();
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
    Pocisk(float x, float y);
    void ruszaj(float deltaTime);
    void rysuj(sf::RenderWindow &window);
    sf::FloatRect pobierzObszar() const;

private:
    sf::RectangleShape ksztalt;
    float predkosc;
};

Pocisk::Pocisk(float x, float y) : predkosc(-300.f)
{
    ksztalt.setSize(sf::Vector2f(5, 10));
    ksztalt.setFillColor(sf::Color::White);
    ksztalt.setPosition(x, y);
}

// ruch pocisk¢w
void Pocisk::ruszaj(float deltaTime)
{
    ksztalt.move(0, predkosc * deltaTime);
}

void Pocisk::rysuj(sf::RenderWindow &window)
{
    window.draw(ksztalt);
}

sf::FloatRect Pocisk::pobierzObszar() const
{
    return ksztalt.getGlobalBounds();
}

class Punkty
{
private:
    int punkty;
    sf::Text tekstPunkty;

public:
    Punkty();
    void dodaj(int punkty);
    void rysuj(sf::RenderWindow &window, const sf::Font &czcionka);
    void ustawPunkty(int);
    int pobierzPunkty();
};

Punkty::Punkty() : punkty(0)
{
    // inicjalizacja tekstu
    tekstPunkty.setCharacterSize(30);
    tekstPunkty.setFillColor(sf::Color::Blue);
}

void Punkty::dodaj(int nowyPunkt)
{
    punkty += nowyPunkt;
}

void Punkty::rysuj(sf::RenderWindow &window, const sf::Font &czcionka)
{
    tekstPunkty.setFont(czcionka);
    tekstPunkty.setPosition(600, 20);
    tekstPunkty.setString("Punkty: " + std::to_string(punkty));
    window.draw(tekstPunkty);
}

void Punkty::ustawPunkty(int nowePunkty)
{
    punkty = nowePunkty;
}

int Punkty::pobierzPunkty()
{
    return punkty;
}

class Ranking
{
public:
    Ranking(const std::string &nazwaPliku);
    void dodajWynik(const std::string &nazwaGracza, int punkty);
    void zaladuj();
    void zapisz();
    void rysuj(sf::RenderWindow &window, const sf::Font &czcionka);

private:
    std::string nazwaPliku;
    std::vector<std::pair<std::string, int>> wyniki;
    sf::Text tekstRanking;
};

Ranking::Ranking(const std::string &nazwaPliku) : nazwaPliku(nazwaPliku)
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

void Ranking::rysuj(sf::RenderWindow &window, const sf::Font &czcionka)
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
    void ustawTekst(const std::string &tekst, const sf::Color &kolor);
    void rysuj(sf::RenderWindow &window);
private:
    sf::Text tekstKomunikatu;
};

Komunikat::Komunikat(const sf::Font &czcionka)
{
    tekstKomunikatu.setFont(czcionka);
    tekstKomunikatu.setCharacterSize(40);
    tekstKomunikatu.setPosition(60, 250);
}

void Komunikat::ustawTekst(const std::string &tekst, const sf::Color &kolor)
{
    tekstKomunikatu.setString(tekst);
    tekstKomunikatu.setFillColor(kolor);
    tekstKomunikatu.setOutlineThickness(2);
    tekstKomunikatu.setOutlineColor(sf::Color::White);
}

void Komunikat::rysuj(sf::RenderWindow &window)
{
    window.draw(tekstKomunikatu);
}



int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space invaders");

    bool gra = false;               //flaga gry
    bool przegrana = false;      
    bool wygrana = false;
    bool ranking = false;
    bool wyswietlKomunikat = false;

    //zaladowanie czcionki
    sf::Font czcionka;
    if (!czcionka.loadFromFile("Pricedown Bl.otf"))
    {
        std::cout << "Nie udalo sie zaladowac czcionki!" << std::endl;
        return 0;
    }
    Menu menu(800, 600, czcionka);
    Ranking rank("ranking.txt");
    Komunikat komunikat(czcionka);


    Gracz gracz;
    std::vector<Wrog> wrogowie;
    std::vector<Pocisk> pociski;
    Punkty punkty;

    // Dodanie przeciwnikow
    int kolumny = 10;
    int wiersze = 4;
    float odstepK = 60.f;
    float odstepW = 40.f;

    // kierunek i szybko˜† ruchu przeciwnik¢w
    bool ruchWPrawo = true;     // czy grupa porusza sie w prawo
    float predkoscRuchu = 50.f; // predkosc ruchu w poziomie

    for (int rzad = 0; rzad < wiersze; ++rzad)
    {
        for (int kolumna = 0; kolumna < kolumny; ++kolumna)
        {
            float x = kolumna * odstepK + 20; // odstep miedzy kolumnami
            float y = rzad * odstepW + 80;    // odstep miedzy rzedami
            wrogowie.emplace_back(x, y);
        }
    }

    // czas do obliczania pˆynno˜ci ruchu
    sf::Clock clock;

    // do ograniczenie czasu miedzy kolejnymi strzalami
    float czasOdOstatniegoStrzalu = 0.f;
    float minimalnyCzasStrzalu = 0.3f;

    // naprawa bledu zwiazanego z wielokrotnym dotykaniem krawedzi
    bool dotknietoKrawedziWczesniej = false;

    // p©tla gry
    while (window.isOpen())
    {
        // obsˆuga zdarzeä
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            //     window.close();
            if (gra)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    gra = false;
                    rank.dodajWynik("Gracz", punkty.pobierzPunkty());
                }                
                // po nacisnieciu spacji wystrzal oraz czas miedzy strzalami = minimalnyCzasStrzalu
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && czasOdOstatniegoStrzalu >= minimalnyCzasStrzalu)
                {
                    pociski.emplace_back(gracz.pobierzPozycje().x + 22.5f, gracz.pobierzPozycje().y);
                    czasOdOstatniegoStrzalu = 0.0f;
                }
            }
            else if (wyswietlKomunikat)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    wyswietlKomunikat = false;
                    przegrana = false;
                    wygrana = false;
                    gra = false;
                }
            }
            else if (ranking)
            {
                if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    ranking = false;
                }
            }
            else
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    menu.przesunWGore();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    menu.przesunWDol();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                {
                    int opcja = menu.pobierzWybranaOpcje();
                    if (opcja == 0)
                    {
                        gra = true;
                        // reset stanu gracza
                        gracz = Gracz();
                        wrogowie.clear();
                        pociski.clear();
                        punkty.ustawPunkty(0);

                        for (int rzad = 0; rzad < wiersze; ++rzad)
                        {
                            for (int kolumna = 0; kolumna < kolumny; ++kolumna)
                            {
                                float x = kolumna * odstepK + 20;
                                float y = rzad * odstepW + 80;
                                wrogowie.emplace_back(x, y);
                            }
                        }
                        clock.restart();
                        czasOdOstatniegoStrzalu = 0.f;
                        dotknietoKrawedziWczesniej = false;
                        przegrana = false;
                        wygrana = false;
                    }
                    else if (opcja == 1)
                    {
                        ranking = true;
                    }
                    else if (opcja == 2)
                    {
                        window.close();
                    }
                }
            }
        }
        // aktualizacja stanu gry //gdy gra jest aktywna i komunikat nie jest wyswietlany
        if (gra && !wyswietlKomunikat)
        {
            sf::Time deltaTime = clock.restart();
            gracz.steruj(deltaTime.asSeconds());

            czasOdOstatniegoStrzalu += deltaTime.asSeconds();

            bool dotknietoKrawedzi = false;
            for (auto &wrog : wrogowie)
            {
                // ruch w poziomie
                if (ruchWPrawo)
                {
                    wrog.przesun(predkoscRuchu * deltaTime.asSeconds(), 0);
                }
                else
                {
                    wrog.przesun(-predkoscRuchu * deltaTime.asSeconds(), 0);
                }

                // sprawdzenie czy wrog dotknal krawedzi ekranu
                if (!dotknietoKrawedzi && (wrog.pobierzPozycje().x <= 0 || wrog.pobierzPozycje().x + 40 >= 800))
                {
                    dotknietoKrawedzi = true; // zmiana kierunku
                }

                // std::cout<<"Pozycja wroga Y: "<<wrog.pobierzPozycje().y<<std::endl;

                // sprawdzenie czy ktorys wrog dotarl do dolnej krawedzi
                if (wrog.pobierzPozycje().y + 20 >= 600)
                {
                    przegrana = true;
                    wyswietlKomunikat = true;
                    komunikat.ustawTekst("Przegrale\346! Wcisnij ESC, aby opuscic gre!", sf::Color::Red);
                }
            }

            if (wrogowie.empty())
            {
                wygrana = true;
                wyswietlKomunikat = true;
                komunikat.ustawTekst("Wygrales! Wcisnij ESC, aby opuscic gre", sf::Color::Green);
            }

            // jesli dotykaj¥ kraw©dzi to zmiana kierunku
            if (dotknietoKrawedzi && !dotknietoKrawedziWczesniej)
            {
                ruchWPrawo = !ruchWPrawo; // zmiana kierunku
                for (auto &wrog : wrogowie)
                {
                    wrog.przesun(0, odstepW); // przesun wrogow w dol
                }
                dotknietoKrawedziWczesniej = true;
            }
            else if (!dotknietoKrawedzi)
            {
                dotknietoKrawedziWczesniej = false;
            }


            for (auto it = pociski.begin(); it != pociski.end();)
            {
                it->ruszaj(deltaTime.asSeconds());

                // usuwanie pociskow ktore wyszly poza zakres
                if (it->pobierzObszar().top + it->pobierzObszar().height < 0)
                {
                    it = pociski.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            // Sprawdzanie kolizji mi©dzy pociskami a przeciwnikami
            for (auto wrogIt = wrogowie.begin(); wrogIt != wrogowie.end();)
            {
                bool wrogZniszczony = false;

                for (auto pociskIt = pociski.begin(); pociskIt != pociski.end();)
                {
                    if (pociskIt->pobierzObszar().intersects(sf::FloatRect(wrogIt->pobierzPozycje(), sf::Vector2f(40, 20))))
                    {
                        // Je˜li kolizja, usuä pocisk i oznacz przeciwnika do usuni©cia
                        pociskIt = pociski.erase(pociskIt);
                        wrogZniszczony = true;
                        break;
                    }
                    else
                    {
                        ++pociskIt;
                    }
                }

                // Usuä przeciwnika, je˜li zostaˆe˜ trafiony
                if (wrogZniszczony)
                {
                    wrogIt = wrogowie.erase(wrogIt);
                    punkty.dodaj(10);
                }
                else
                {
                    ++wrogIt;
                }
            }
        }

        // renderowanie
        window.clear();
        if (gra)
        {
            gracz.rysuj(window);

            for (auto &wrog : wrogowie)
            {
                wrog.rysuj(window);
            }

            for (auto &pocisk : pociski)
            {
                pocisk.rysuj(window);
            }
            punkty.rysuj(window, czcionka);
        }
        else if (ranking)
        {
            rank.rysuj(window, czcionka);
        }
        else if (!wyswietlKomunikat)
        {
            menu.rysuj(window);
        }
        //komunikaty o wygranej lub prezgranej
        if (wyswietlKomunikat)
        {
            komunikat.rysuj(window);
        }
        window.display();

    }

    return 0;
}