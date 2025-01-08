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
    void steruj(float);
    void rysuj(sf::RenderWindow &);
    void stracZycie();
    sf::Vector2f pobierzPozycje() const;
    int pobierzLiczbeZyc() const;

private:
    sf::RectangleShape ksztalt;
    float predkosc;
    int liczbaZyc;
    std::vector<sf::Sprite> serca;
};

Gracz::Gracz(const sf::Texture &teksturaSerca) : predkosc(200.f), liczbaZyc(2)
{
    ksztalt.setSize(sf::Vector2f(50, 20));
    ksztalt.setFillColor(sf::Color::Green);
    ksztalt.setPosition(375, 550);

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
    if (ksztalt.getPosition().x > 910)
    {
        ksztalt.setPosition(910, ksztalt.getPosition().y);
    }
}

void Gracz::rysuj(sf::RenderWindow &window)
{
    window.draw(ksztalt);

    // rysowanie serc
    for (const auto &serce : serca)
    {
        window.draw(serce);
    }
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
    // jezeli kierunek < 0 to magenta, w innym przypadku red
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
    tekstPunkty.setPosition(760, 20);
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
    void ustawTekst(const std::string &tekst, const sf::Color &kolor, const sf::Color &kolorObramowania);
    void rysuj(sf::RenderWindow &window);
    void ustawPozycje(float, float);

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

int main()
{
    sf::RenderWindow window(sf::VideoMode(960, 600), "Space invaders");

    bool gra = false; // flaga gry
    bool przegrana = false;
    bool wygrana = false;
    bool ranking = false;
    bool wyswietlKomunikat = false;
    bool pomoc = false;
    bool powrotZPomocy = false; //zapobiegniecie przerywania gry przy wychodzeniu z pomocy

    // zaladowanie czcionki
    sf::Font czcionka;
    if (!czcionka.loadFromFile("Pricedown Bl.otf"))
    {
        std::cout << "Nie udalo sie zaladowac czcionki!" << std::endl;
        return 0;
    }

    sf::Texture teksturaSerca;
    if (!teksturaSerca.loadFromFile("../assets/hearth.png"))
    {
        std::cout << "Nie udalo sie zaladowac obrazka serca!" << std::endl;
        return -1;
    }

    Menu menu(czcionka);
    Ranking rank("ranking.txt");
    Komunikat komunikat(czcionka);

    Gracz gracz(teksturaSerca);
    std::vector<Wrog> wrogowie;
    std::vector<Pocisk> pociskiGracza;
    std::vector<Pocisk> pociskiWroga;
    Punkty punkty;

    Komunikat komunikatPomoc(czcionka);
    komunikatPomoc.ustawTekst(
        "Pomoc:\n\n"
        "-  <-  ->  - Poruszanie graczem\n"
        "- Spacja - Strzal\n"
        "- F1 - Wyswietlnie pomocy\n"
        "- ESC - Powrot\n\n"
        "Wcisnij ESC aby wrocic.",
        sf::Color::Red, sf::Color::White);
    komunikatPomoc.ustawPozycje(50.f, 30.f);

    Komunikat komunikatF1(czcionka);
    komunikatF1.ustawTekst("F1 - pomoc", sf::Color::Cyan, sf::Color::Black);
    komunikatF1.ustawPozycje(360.f, 10.f);

    // dodanie przeciwnikow
    int kolumny = 12;
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

    // czas pomiedzy strzalami wrogow
    float czasOdOstatniegoStrzaluWroga = 0.f;
    float minimalnyCzasStrzaluWroga = 1.5f;

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
                if (pomoc)
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    {
                        pomoc = false;
                        powrotZPomocy = true;
                        std::cout<<"Zamykam pomoc\n";
                    }
                }
                else
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
                    {
                        std::cout << "Otwieram pomoc!";
                        pomoc = true;
                    }
                    // do poprawy, wychodzi do menu nawet  w oknie pomocy
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && !powrotZPomocy)
                    {
                        gra = false;
                        rank.dodajWynik("Gracz", punkty.pobierzPunkty());
                        //powrotZPomocy = false;
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && powrotZPomocy)
                    {
                        powrotZPomocy = false;
                    }
                    // po nacisnieciu spacji wystrzal oraz czas miedzy strzalami = minimalnyCzasStrzalu
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && czasOdOstatniegoStrzalu >= minimalnyCzasStrzalu)
                    {
                        pociskiGracza.emplace_back(gracz.pobierzPozycje().x + 22.5f, gracz.pobierzPozycje().y, -1.f);
                        czasOdOstatniegoStrzalu = 0.0f;
                    }
                }
            }
            else if (wyswietlKomunikat)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    wyswietlKomunikat = false;
                    przegrana = false;
                    wygrana = false;
                    gra = false;
                }
            }
            else if (ranking)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
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
                        gracz = Gracz(teksturaSerca);
                        wrogowie.clear();
                        pociskiGracza.clear();
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
                if (!dotknietoKrawedzi && (wrog.pobierzPozycje().x <= 0 || wrog.pobierzPozycje().x + 40 >= 960))
                {
                    dotknietoKrawedzi = true; // zmiana kierunku
                }

                // std::cout<<"Pozycja wroga Y: "<<wrog.pobierzPozycje().y<<std::endl;

                // sprawdzenie czy ktorys wrog dotarl do dolnej krawedzi
                if (wrog.pobierzPozycje().y + 20 >= 600)
                {
                    przegrana = true;
                    wyswietlKomunikat = true;
                    komunikat.ustawTekst("Przegrale\346! Wcisnij ESC, aby opuscic gre!", sf::Color::Red, sf::Color::White);
                    komunikat.ustawPozycje(120, 250);
                }
            }

            if (wrogowie.empty())
            {
                wygrana = true;
                wyswietlKomunikat = true;
                komunikat.ustawTekst("Wygrales! Wcisnij ESC, aby opuscic gre", sf::Color::Green, sf::Color::White);
                komunikat.ustawPozycje(120, 250);
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

            for (auto it = pociskiGracza.begin(); it != pociskiGracza.end();)
            {
                it->ruszaj(deltaTime.asSeconds());

                bool wrogZniszczony = false;
                // Sprawdzanie kolizji mi©dzy pociskami a przeciwnikami
                for (auto wrogIt = wrogowie.begin(); wrogIt != wrogowie.end();)
                {

                    if (it->pobierzObszar().intersects(sf::FloatRect(wrogIt->pobierzPozycje(), sf::Vector2f(40, 20))))
                    {
                        // jesli kolizja do wrog usuawny
                        wrogIt = wrogowie.erase(wrogIt);
                        wrogZniszczony = true;
                        punkty.dodaj(10);
                        break;
                    }
                    else
                    {
                        ++wrogIt;
                    }
                }

                // usuwanie pociskow po zderzeniu lub ktore wyszly poza zakres
                if (wrogZniszczony || it->pobierzObszar().top + it->pobierzObszar().height < 0)
                {
                    it = pociskiGracza.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            czasOdOstatniegoStrzaluWroga += deltaTime.asSeconds();
            if (czasOdOstatniegoStrzaluWroga >= minimalnyCzasStrzaluWroga && !wrogowie.empty())
            {
                int indeksWroga = rand() % wrogowie.size();
                sf::Vector2f pozycjaWroga = wrogowie[indeksWroga].pobierzPozycje();
                pociskiWroga.emplace_back(pozycjaWroga.x + 20.f, pozycjaWroga.y + 20.f, 1.f);
                czasOdOstatniegoStrzaluWroga = 0.f;
            }

            for (auto it = pociskiWroga.begin(); it != pociskiWroga.end();)
            {
                it->ruszaj(deltaTime.asSeconds());

                // kolizja z graczem + przegrana jezeli dotknie pocisku 3 razy
                if (it->pobierzObszar().intersects(sf::FloatRect(gracz.pobierzPozycje(), sf::Vector2f(50, 20))))
                {
                    gracz.stracZycie();
                    it = pociskiWroga.erase(it);

                    if (gracz.pobierzLiczbeZyc() == 0)
                    {
                        przegrana = true;
                        wyswietlKomunikat = true;
                        komunikat.ustawTekst("Przegrales! Wcisnij ESC aby opuscic gre!", sf::Color::Red, sf::Color::White);          
                        komunikat.ustawPozycje(120, 250);              
                    }
                }
                // usuniecie pociskow spoza ekranu
                else if (it->pobierzObszar().top > 600)
                {
                    it = pociskiWroga.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        // renderowanie
        window.clear();
        if (pomoc)
        {
            komunikatPomoc.rysuj(window);
        }
        else if (gra)
        {
            gracz.rysuj(window);
            komunikatF1.rysuj(window);

            for (auto &wrog : wrogowie)
            {
                wrog.rysuj(window);
            }

            for (auto &pocisk : pociskiGracza)
            {
                pocisk.rysuj(window);
            }
            punkty.rysuj(window, czcionka);

            for (auto &pocisk : pociskiWroga)
            {
                pocisk.rysuj(window);
            }
        }
        else if (ranking)
        {
            rank.rysuj(window, czcionka);
        }
        else if (!wyswietlKomunikat)
        {
            menu.rysuj(window);
        }
        // komunikaty o wygranej lub prezgranej
        if (wyswietlKomunikat)
        {
            komunikat.rysuj(window);
        }
        window.display();
    }

    return 0;
}