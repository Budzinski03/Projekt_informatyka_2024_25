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
    tekstPunkty.setPosition(760, 10);
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
};

UstawTekst::UstawTekst(sf::Font &czcionka, sf::Vector2f rozmiar, sf::Vector2f pozycja)
{
    tekst.setFont(czcionka);
    tekst.setCharacterSize(30);
    tekst.setFillColor(sf::Color::Cyan);
    tekst.setPosition(pozycja.x + 5.f, pozycja.y + 30.f);

    tlo.setSize(rozmiar);
    tlo.setFillColor(sf::Color(50, 50, 50, 200));
    tlo.setOutlineColor(sf::Color::White);
    tlo.setOutlineThickness(2);
    tlo.setPosition(pozycja);
}

void UstawTekst::pobierzZnak(sf::RenderWindow &window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {

        if (event.type == (sf::Event::Closed) || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
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
            // jezeli wprowadzony dane to znak ASCII to jest konwertowany na typ char i dodajemy do ciagu input
            else if (event.text.unicode < 128 && wejscie.size() < 15)
            {
                wejscie += static_cast<char>(event.text.unicode);
            }
            // aktualizacja tekstu
            tekst.setString(wejscie);
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

struct StanyGry
{
    bool gra = false;
    bool przegrana = false;
    bool wygrana = false;
    bool ranking = false;
    bool wyswietlKomunikat = false;
    bool pomoc = false;
    bool powrotZPomocy = false;
    bool podajNick = false;
    bool dotknietoKrawedziWczesniej = false;
    bool ruchWPrawo = true;
};

struct Zrodla
{
    sf::Font czcionka;
    sf::Texture teksturaSerca;
};

struct ObiektyGry
{
    Menu *menu;
    Ranking *rank;
    Komunikat *komunikat;
    Komunikat *komunikatPomoc;
    Komunikat *komunikatF1;
    Gracz *gracz;
    std::vector<Wrog> wrogowie;
    std::vector<Pocisk> pociskiGracza;
    std::vector<Pocisk> pociskiWroga;
    Punkty *punkty;
    UstawTekst *ustawTekst;
};

struct Parametry
{
    int liczbaWierszy = 4;
    int liczbaKolumn = 14;
    float odstepMiedzyKolumnami = 60.f;
    float odstepMiedzyWierszami = 40.f;
    float predkoscRuchu = 50.f;
    float minimalnyCzasStrzaluWroga = 1.5f;
    float minimalnyCzasStrzalu = 0.3f;
    float czasOdOstatniegoStrzalu = 0.0f;
    float czasOdOstatniegoStrzaluWroga = 0.0f;
};

// ladowanie czcionki i tekstury
bool zaladuj(Zrodla &zrodla)
{
    if (!zrodla.czcionka.loadFromFile("Pricedown Bl.otf"))
    {
        std::cout << "Nie udalo sie zaladowac czcionki!" << std::endl;
        return false;
    }
    if (!zrodla.teksturaSerca.loadFromFile("../assets/hearth.png"))
    {
        std::cout << "Nie udalo sie zaladowac obrazka serca!" << std::endl;
        return false;
    }
    return true;
}

//dynamiczna inicjalizacja obiektow gry
void inicjalizujObiekty(ObiektyGry &obiekty, Zrodla &zrodla)
{
    obiekty.menu = new Menu(zrodla.czcionka);
    obiekty.rank = new Ranking("ranking.txt");
    obiekty.komunikat = new Komunikat(zrodla.czcionka);
    obiekty.komunikatPomoc = new Komunikat(zrodla.czcionka);
    obiekty.komunikatF1 = new Komunikat(zrodla.czcionka);
    obiekty.gracz = new Gracz(zrodla.teksturaSerca);
    obiekty.punkty = new Punkty();
    obiekty.ustawTekst = new UstawTekst(zrodla.czcionka, sf::Vector2f(360.f, 100.f), sf::Vector2f(20.f, 50.f));

    obiekty.komunikatPomoc->ustawTekst(
        "Pomoc:\n\n"
        "-  <-  ->  - Poruszanie graczem\n"
        "- Spacja - Strzal\n"
        "- F1 - Wyswietlnie pomocy\n"
        "- ESC - Powrot\n\n"
        "Wcisnij ESC aby wrocic.",
        sf::Color::Red, sf::Color::White);
    obiekty.komunikatPomoc->ustawPozycje(50.f, 30.f);

    obiekty.komunikatF1->ustawTekst("F1 - pomoc", sf::Color::Cyan, sf::Color::Black);
    obiekty.komunikatF1->ustawPozycje(360.f, 10.f);
}

//
void obslugaZdarzenia(sf::RenderWindow &window, StanyGry &stan, ObiektyGry &obiekty, Zrodla &zrodla, Parametry &parametry)
{
    // obsˆuga zdarzeä
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        //     window.close();
        if (stan.gra)
        {
            if (stan.pomoc)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    stan.pomoc = false;
                    stan.powrotZPomocy = true;
                    std::cout << "Zamykam pomoc\n";
                }
            }
            else
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
                {
                    std::cout << "Otwieram pomoc!";
                    stan.pomoc = true;
                }
                // do poprawy, wychodzi do menu nawet  w oknie pomocy
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && !stan.powrotZPomocy)
                {
                    stan.gra = false;
                    obiekty.rank->dodajWynik(obiekty.ustawTekst->pobierzTekst().getString(), obiekty.punkty->pobierzPunkty());
                    //stan.powrotZPomocy = false;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && stan.powrotZPomocy)
                {
                    stan.powrotZPomocy = false;
                }
                // po nacisnieciu spacji wystrzal oraz czas miedzy strzalami = minimalnyCzasStrzalu
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && parametry.czasOdOstatniegoStrzalu >= parametry.minimalnyCzasStrzalu)
                {
                    obiekty.pociskiGracza.emplace_back(obiekty.gracz->pobierzPozycje().x + 22.5f, obiekty.gracz->pobierzPozycje().y, -1.f);
                    parametry.czasOdOstatniegoStrzalu = 0.0f;
                }
            }
        }
        else if (stan.wyswietlKomunikat)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                stan.wyswietlKomunikat = false;
                stan.przegrana = false;
                stan.wygrana = false;
                stan.gra = false;
            }
        }
        else if (stan.ranking)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                stan.ranking = false;
            }
        }
        // menu
        else
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                obiekty.menu->przesunWGore();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                obiekty.menu->przesunWDol();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                int opcja = obiekty.menu->pobierzWybranaOpcje();
                if (opcja == 0)
                {
                    stan.gra = true;
                    obiekty.ustawTekst->czysc(); // czyszczenie pola do wpisywania
                    // reset stanu gracza
                    *obiekty.gracz = Gracz(zrodla.teksturaSerca);
                    obiekty.wrogowie.clear();
                    obiekty.pociskiGracza.clear();
                    obiekty.punkty->ustawPunkty(0);

                    //parametry wrogow
                    for (int rzad = 0; rzad < 4; ++rzad)
                    {
                        for (int kolumna = 0; kolumna < 12; ++kolumna)
                        {   
                            float x = kolumna * parametry.odstepMiedzyKolumnami + 20;
                            float y = rzad * parametry.odstepMiedzyWierszami + 80;
                            obiekty.wrogowie.emplace_back(x, y);
                        }
                    }
                    //clock.restart();
                    parametry.czasOdOstatniegoStrzalu = 0.f;
                    stan.dotknietoKrawedziWczesniej = false;
                    //przegrana = false;
                    //wygrana = false;
                }
                else if (opcja == 1)
                {
                    stan.ranking = true;
                }
                else if (opcja == 2)
                {
                    window.close();
                }
            }
        }
    }
}

void aktualizujGre(StanyGry &stan, ObiektyGry &obiekty, sf::Clock &clock, Parametry &parametry)
{
    // aktualizacja stanu gry
    // gdy gra jest aktywna i komunikat nie jest wyswietlany
    if (stan.gra && !stan.wyswietlKomunikat)
    {
        sf::Time deltaTime = clock.restart();
        obiekty.gracz->steruj(deltaTime.asSeconds());

        parametry.czasOdOstatniegoStrzalu += deltaTime.asSeconds();

        bool dotknietoKrawedzi = false;
        for (auto &wrog : obiekty.wrogowie)
        {
            // ruch w poziomie
            if (stan.ruchWPrawo)
            {
                //predkosc ruchu * deltaTime
                wrog.przesun(parametry.predkoscRuchu * deltaTime.asSeconds(), 0);
            }
            else
            {
                wrog.przesun(-parametry.predkoscRuchu* deltaTime.asSeconds(), 0);
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
                stan.przegrana = true;
                stan.wyswietlKomunikat = true;
                obiekty.komunikat->ustawTekst("Przegrale\346! Wcisnij ESC, aby opuscic gre!", sf::Color::Red, sf::Color::White);
                obiekty.komunikat->ustawPozycje(120, 250);
            }
        }

        if (obiekty.wrogowie.empty())
        {
            stan.wygrana = true;
            stan.wyswietlKomunikat = true;
            obiekty.komunikat->ustawTekst("Wygrales! Wcisnij ESC, aby opuscic gre", sf::Color::Green, sf::Color::White);
            obiekty.komunikat->ustawPozycje(120, 250);
        }

        // jesli dotykaj¥ kraw©dzi to zmiana kierunku
        if (dotknietoKrawedzi && !stan.dotknietoKrawedziWczesniej)
        {
            stan.ruchWPrawo = !stan.ruchWPrawo; // zmiana kierunku
            for (auto &wrog : obiekty.wrogowie)
            {
                wrog.przesun(0, parametry.odstepMiedzyWierszami); // przesun wrogow w dol o odstep miedzy wierszami
            }
            stan.dotknietoKrawedziWczesniej = true;
        }
        else if (!dotknietoKrawedzi)
        {
            stan.dotknietoKrawedziWczesniej = false;
        }

        for (auto it = obiekty.pociskiGracza.begin(); it != obiekty.pociskiGracza.end();)
        {
            it->ruszaj(deltaTime.asSeconds());

            bool wrogZniszczony = false;
            // Sprawdzanie kolizji mi©dzy pociskami a przeciwnikami
            for (auto wrogIt = obiekty.wrogowie.begin(); wrogIt != obiekty.wrogowie.end();)
            {

                if (it->pobierzObszar().intersects(sf::FloatRect(wrogIt->pobierzPozycje(), sf::Vector2f(40, 20))))
                {
                    // jesli kolizja do wrog usuawny
                    wrogIt = obiekty.wrogowie.erase(wrogIt);
                    wrogZniszczony = true;
                    obiekty.punkty->dodaj(10);
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
                it = obiekty.pociskiGracza.erase(it);
            }
            else
            {
                ++it;
            }
        }

        parametry.czasOdOstatniegoStrzaluWroga += deltaTime.asSeconds();
        if (parametry.czasOdOstatniegoStrzaluWroga >= parametry.minimalnyCzasStrzaluWroga && !obiekty.wrogowie.empty())
        {
            int indeksWroga = rand() % obiekty.wrogowie.size();
            sf::Vector2f pozycjaWroga = obiekty.wrogowie[indeksWroga].pobierzPozycje();
            obiekty.pociskiWroga.emplace_back(pozycjaWroga.x + 20.f, pozycjaWroga.y + 20.f, 1.f);
            parametry.czasOdOstatniegoStrzaluWroga = 0.f;
        }

        for (auto it = obiekty.pociskiWroga.begin(); it != obiekty.pociskiWroga.end();)
        {
            it->ruszaj(deltaTime.asSeconds());

            // kolizja z graczem + przegrana jezeli dotknie pocisku 3 razy
            if (it->pobierzObszar().intersects(sf::FloatRect(obiekty.gracz->pobierzPozycje(), sf::Vector2f(50, 20))))
            {
                obiekty.gracz->stracZycie();
                it = obiekty.pociskiWroga.erase(it);

                if (obiekty.gracz->pobierzLiczbeZyc() == 0)
                {
                    stan.przegrana = true;
                    stan.wyswietlKomunikat = true;
                    obiekty.komunikat->ustawTekst("Przegrales! Wcisnij ESC aby opuscic gre!", sf::Color::Red, sf::Color::White);
                    obiekty.komunikat->ustawPozycje(120, 250);
                    stan.podajNick = true;
                }
            }
            // usuniecie pociskow spoza ekranu
            else if (it->pobierzObszar().top > 600)
            {
                it = obiekty.pociskiWroga.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    else if (stan.podajNick)
    {
        sf::RenderWindow oknoWprowadzania(sf::VideoMode(400.f, 200.f), "SFML Text Input");
        while (oknoWprowadzania.isOpen())
        {
            obiekty.ustawTekst->pobierzZnak(oknoWprowadzania);
            oknoWprowadzania.clear();
            obiekty.ustawTekst->rysuj(oknoWprowadzania);
            oknoWprowadzania.display();
        }
        stan.podajNick = false;
    }
}

void renderuj(sf::RenderWindow &window, StanyGry &stan, ObiektyGry &obiekty, Zrodla &zrodla, Parametry &parametry)
{
    // renderowanie
    window.clear();
    if (stan.pomoc)
    {
        obiekty.komunikatPomoc->rysuj(window);
    }
    else if (stan.gra)
    {
        obiekty.gracz->rysuj(window);
        obiekty.komunikatF1->rysuj(window);

        for (auto &wrog : obiekty.wrogowie)
        {
            wrog.rysuj(window);
        }

        for (auto &pocisk : obiekty.pociskiGracza)
        {
            pocisk.rysuj(window);
        }
        obiekty.punkty->rysuj(window, zrodla.czcionka);

        for (auto &pocisk : obiekty.pociskiWroga)
        {
            pocisk.rysuj(window);
        }
    }
    else if (stan.ranking)
    {
        obiekty.rank->rysuj(window, zrodla.czcionka);
    }
    else if (!stan.wyswietlKomunikat)
    {
        obiekty.menu->rysuj(window);
    }
    // komunikaty o wygranej lub przegranej
    if (stan.wyswietlKomunikat)
    {
        obiekty.komunikat->rysuj(window);
    }
    window.display();
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(960, 600), "Space invaders");

    StanyGry stan;
    Zrodla zrodla;
    ObiektyGry obiekty;
    Parametry parametry;

    if (!zaladuj(zrodla)) return -1;

    inicjalizujObiekty(obiekty, zrodla);

    sf::Clock clock;

    while (window.isOpen())
    {
        obslugaZdarzenia(window, stan, obiekty, zrodla, parametry);
        aktualizujGre(stan, obiekty, clock, parametry);
        renderuj(window, stan, obiekty, zrodla, parametry);
    }

    delete obiekty.menu;
    delete obiekty.rank;
    delete obiekty.komunikat;
    delete obiekty.komunikatPomoc;
    delete obiekty.komunikatF1;
    delete obiekty.gracz;
    delete obiekty.punkty;
    delete obiekty.ustawTekst;
    
    return 0;
}

