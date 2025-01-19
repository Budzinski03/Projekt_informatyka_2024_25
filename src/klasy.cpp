#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include "../include/klasy.h"
#include <iostream>

///////////////////// MENU /////////////////////////

Menu::Menu(const sf::Font &czcionka) : wybranaOpcja(0)
{
    std::string tekstOpcji[] = {"Nowa gra", "Ranking", "Ustawienia", "Wyjscie"};

    for (int i = 0; i < 4; i++)
    {
        sf::Text tekst;
        tekst.setFont(czcionka);
        tekst.setCharacterSize(40);
        // jesli i = wybranaOpcja to kolor czerowny, jesli nie to bialy
        tekst.setFillColor(i == wybranaOpcja ? sf::Color::Red : sf::Color::White);
        tekst.setOutlineThickness(3.f);
        tekst.setOutlineColor(sf::Color::Black);
        tekst.setString(tekstOpcji[i]);
        tekst.setPosition(sf::Vector2f(960 / 8, 600 / (5) * (1 * i + 0.5)));
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

///////////////////// GRACZ /////////////////////////

Gracz::Gracz(const sf::Texture &teksturaSerca) : predkosc(200.f), liczbaZyc(3), punkty(0)
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

// obsluga wejscia / sterowanie
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
    if (serca.size() <= 5) // jezeli <= 5 to wsywietla tyle serc
    {
        for (const auto &serce : serca)
        {
            window.draw(serce);
        }
    }
    else // jesli wiecej to wyswietla serce x ilosc
    {
        tekstZycia.setFont(czcionka);
        tekstZycia.setFillColor(sf::Color::Red);
        tekstZycia.setPosition(50, 15);
        tekstZycia.setString("x" + std::to_string(serca.size()));
        window.draw(serca[0]);
        window.draw(tekstZycia);
    }
    // rysowanie punktow
    tekstPunkty.setFont(czcionka);
    tekstPunkty.setString("Punkty: " + std::to_string(punkty));
    tekstPunkty.setCharacterSize(40);
    // ulozenie tekstu w zaleznosci do wymiarow - ilosci punktow
    tekstPunkty.setPosition(window.getSize().x - (tekstPunkty.getLocalBounds().width) - 30, 15);
    window.draw(tekstPunkty);
}

sf::Vector2f Gracz::pobierzPozycje() const
{
    return ksztalt.getPosition();
}

bool Gracz::stracZycie(int obrazenia)
{
    if (liczbaZyc > obrazenia)
    {
        liczbaZyc -= obrazenia;
        for (int i = 0; i < obrazenia; i++)
            if (!serca.empty())
            {
                serca.pop_back();
            }
        return true;
    }
    serca.clear();
    return false;
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

void Gracz::dodajZycie(int zycie, const sf::Texture &teksturaSerca)
{
    for (int i = 0; i < zycie; i++)
    {
        sf::Sprite serce;
        serce.setTexture(teksturaSerca);
        serce.setScale(0.05f, 0.05f);

        // obliczenie pozycji serca na podstawie istniejacych serc
        float x = 20 + serca.size() * 30;
        serce.setPosition(x, 20);

        serca.push_back(serce);
    }

    // Zwiększenie liczby żyć
    liczbaZyc += zycie;
}

void Gracz::resetuj()
{
    //zresetowanie pozcyji gracza
    ksztalt.setPosition(480.f, 550.f);
}

void Gracz::ustawPozycje(float x, float y)
{
    ksztalt.setPosition(x, y);
}

void Gracz::ustawPunkty(int pobranePunkty)
{
    punkty = pobranePunkty;
}

void Gracz::ustawZycia(int pobraneZycia, sf::Texture &teksturaSerca)
{
    liczbaZyc = pobraneZycia;

    serca.clear(); //czyszczenie wektora
    //dodanie nowych serc
    for (int i = 0; i < liczbaZyc; ++i)
    {
        sf::Sprite serce;
        serce.setTexture(teksturaSerca);
        serce.setScale(0.05f, 0.05f);
        serce.setPosition(20 + i * 30, 20);
        serca.push_back(serce);
    }
}

///////////////////// WROG /////////////////////////

// przyjmuje 3 lub 4 argumenty, jesli 3 - to zycia domyslne 1 ( wpliku nagłówkowym)
Wrog::Wrog(float x, float y, int typ, int zycia) : typ(typ), czyStrzelil(false), zycia(zycia)
{
    switch (typ)
    {
    case 1:
        ksztalt.setFillColor(sf::Color::Red);
        ksztalt.setSize(sf::Vector2f(50, 20));
        wartoscPunktow = 30;
        break;
    case 2:
        ksztalt.setFillColor(sf::Color::Green);
        ksztalt.setSize(sf::Vector2f(50, 20));
        wartoscPunktow = 20;
        break;
    case 3:
        ksztalt.setFillColor(sf::Color::Blue);
        ksztalt.setSize(sf::Vector2f(50, 20));
        wartoscPunktow = 10;
        break;
    case 4:
        ksztalt.setFillColor(sf::Color::Magenta);
        ksztalt.setSize(sf::Vector2f(130, 40));
        wartoscPunktow = 100;
        break;
    default:
        ksztalt.setFillColor(sf::Color::White);
        ksztalt.setSize(sf::Vector2f(80, 40));
        wartoscPunktow = 50;
        break;
    }
    ksztalt.setPosition(x, y);
}

void Wrog::rysuj(sf::RenderWindow &window) const
{
    window.draw(ksztalt);
}

const sf::RectangleShape &Wrog::pobierzKsztalt() const
{
    return ksztalt; // zwracana jest referencja
}

void Wrog::ustawPozycje(float x, float y)
{
    ksztalt.setPosition(x, y);
}

void Wrog::przesun(float dx, float dy)
{
    ksztalt.move(dx, dy);
}

int Wrog::pobierzPunkty()
{
    return wartoscPunktow;
}

int Wrog::pobierzTyp() const
{
    return typ;
}

bool Wrog::stracZycie()
{
    if (zycia > 1)
    {
        zycia--;
        return true;
    }

    return false;
}

int Wrog::pobierzZycia()
{
    return zycia;
}

///////////////////// POCISK /////////////////////////

Pocisk::Pocisk(float x, float y, float kierunek) : predkosc(300.f), kierunek(kierunek)
{
    ksztalt.setSize(sf::Vector2f(5, 10));
    // jezeli kierunek < 0 to magenta, w innym przypadku yellow
    ksztalt.setFillColor(kierunek < 0 ? sf::Color::Magenta : sf::Color::Yellow);
    ksztalt.setPosition(x, y);
    ksztalt.setOutlineThickness(1);
    ksztalt.setOutlineColor(sf::Color::Black);
}

Pocisk::Pocisk(float x, float y, float kierunek, sf::Color kolor, int typWroga) : predkosc(300.f), kierunek(kierunek)
{
    ksztalt.setSize(sf::Vector2f(5, 10));
    ksztalt.setFillColor(kolor); // ustawienie koloru pocisku 
    ksztalt.setPosition(x, y);
    obrazenia = 1;
    ksztalt.setOutlineThickness(1);
    ksztalt.setOutlineColor(sf::Color::Black);
    //dla bosas wieksze obrazenia
    if (typWroga == 4)
        obrazenia = 2;
}

// ruch pociskow
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

int Pocisk::pobierzObrazenia() const
{
    return obrazenia;
}

///////////////////// RANKING /////////////////////////

Ranking::Ranking(const std::string &nazwaPliku, const sf::Font &czcionka) : nazwaPliku(nazwaPliku), czcionka(czcionka)
{
    zaladuj();
}

void Ranking::dodajWynik(const std::string &nazwaGracza, int punkty)
{
    wyniki.push_back({nazwaGracza, punkty});
    //sortowanie malejaco
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
    std::ifstream plik(nazwaPliku); //otwarcie pliku doo odczytu
    if (plik.is_open())
    {
        std::string nazwaGracza;
        int punkty;
        // plik czytany linia po linii, para string-int
        while (plik >> nazwaGracza >> punkty)
        {
            wyniki.push_back({nazwaGracza, punkty});
        }
        plik.close();
    }
}

void Ranking::zapisz()
{
    std::ofstream plik(nazwaPliku);  //otwarcie pliku do zapisu
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
        tekst += std::to_string(poz) + ". " + wynik.first + "  :   " + std::to_string(wynik.second) + " pkt\n";
        poz++;
    }
    tekstRanking.setFont(czcionka);
    tekstRanking.setString(tekst);
    tekstRanking.setCharacterSize(35);
    tekstRanking.setFillColor(sf::Color::White);
    tekstRanking.setOutlineThickness(3);
    tekstRanking.setOutlineColor(sf::Color::Black);
    tekstRanking.setPosition(100, 100);
    window.draw(tekstRanking);
}

///////////////////// KOMUNIKAT /////////////////////////

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

sf::Text Komunikat::pobierzTekst()
{
    return tekstKomunikatu;
}

///////////////////// USTAW TEKST /////////////////////////

UstawTekst::UstawTekst(sf::Font &czcionka, sf::Vector2f rozmiar, sf::Vector2f pozycja)
{
    tekst.setFont(czcionka);
    tekst.setCharacterSize(30);
    tekst.setFillColor(sf::Color::Cyan);

    tlo.setSize(rozmiar);
    tlo.setFillColor(sf::Color(50, 50, 50, 200)); //kolor szary
    tlo.setOutlineColor(sf::Color::White);
    tlo.setOutlineThickness(2);
    tlo.setPosition(pozycja);
    wysrodkujTekst();
}

void UstawTekst::wysrodkujTekst()
{
    sf::FloatRect granice = tekst.getLocalBounds();
    tekst.setOrigin(granice.width / 2, 0); //poczatkowy punkt transformacji
    tekst.setPosition(tlo.getPosition().x + tlo.getSize().x / 2, tlo.getPosition().y + tlo.getSize().y / 2 - 20);
}

void UstawTekst::pobierzZnak(sf::RenderWindow &window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        // wprowadzanie znakow przez użytkownika
        if (event.type == sf::Event::TextEntered)
        {
            if (event.text.unicode == '\b') // backspace
            {
                if (!wejscie.empty())
                {
                    wejscie.pop_back();
                }
            }
            else if (event.text.unicode >= 32 && event.text.unicode < 128 && wejscie.size() < 15)
            {
                // ignoruj niewidzialne znaki ponizej 32
                wejscie += static_cast<char>(event.text.unicode);
            }

            // aktualizacja wyswietlanego tekstu
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


/////////////////// USTAWIENIA ///////////////////////

Ustawienia::Ustawienia(const sf::Font &czcionka, Zasoby &zasoby)
    : zasoby(zasoby), wybranaOpcja(0)
{

    // konfiguracja ramki
    sf::Vector2f wymiarRamki(640.f, 400.f);
    ramka.setSize(wymiarRamki);
    ramka.setFillColor(sf::Color::Transparent);
    ramka.setOutlineThickness(15.f);
    ramka.setOutlineColor(sf::Color::Red);
    ramka.setPosition(160.f, 50.f);

    // poczatkowe ustawienie tla w oknie
    wybraneTlo.setTexture(zasoby.teksturaTlo1);
    wybraneTlo.setScale(
        wymiarRamki.x / zasoby.teksturaTlo1.getSize().x,
        wymiarRamki.y / zasoby.teksturaTlo1.getSize().y);
    wybraneTlo.setPosition(ramka.getPosition());

    // konfiguracja instrukcji
    instrukcja.setFont(czcionka);
    instrukcja.setCharacterSize(32);
    instrukcja.setFillColor(sf::Color::White);
    instrukcja.setString("uzyj strzalek lewo/prawo, aby zmienic tlo.\n\n            wcisnij Enter, aby zatwierdzic");
    instrukcja.setPosition((960.f - instrukcja.getLocalBounds().width) / 2, 480);

    // konfiguracja strzalek - nieregularny ksztalt
    strzalkaLewa.setPointCount(7);                       // strzalka zlozona z 7pkt
    strzalkaLewa.setPoint(0, sf::Vector2f(0.f, 25.f));   // grot
    strzalkaLewa.setPoint(1, sf::Vector2f(50.f, 0.f));   // dol grotu
    strzalkaLewa.setPoint(2, sf::Vector2f(50.f, 15.f));  // koniec linii - dol
    strzalkaLewa.setPoint(3, sf::Vector2f(100.f, 15.f)); // poczatek linii - dol
    strzalkaLewa.setPoint(4, sf::Vector2f(100.f, 35.f)); // poczatek linii - gora
    strzalkaLewa.setPoint(5, sf::Vector2f(50.f, 35.f));  // koniec linii - gora
    strzalkaLewa.setPoint(6, sf::Vector2f(50.f, 50.f));  // gora grotu
    strzalkaLewa.setOutlineThickness(3.f);
    strzalkaLewa.setOutlineColor(sf::Color::White);
    strzalkaLewa.setPosition(30.f, (600.f - strzalkaLewa.getLocalBounds().height) / 2);

    strzalkaPrawa.setPointCount(7);
    strzalkaPrawa.setPoint(0, sf::Vector2f(0.f, 15.f));   // poczatek linii dol
    strzalkaPrawa.setPoint(1, sf::Vector2f(50.f, 15.f));  // koniec linii dol
    strzalkaPrawa.setPoint(2, sf::Vector2f(50.f, 0.f));   // dol grotu
    strzalkaPrawa.setPoint(3, sf::Vector2f(100.f, 25.f)); // grot
    strzalkaPrawa.setPoint(4, sf::Vector2f(50.f, 50.f));  // gora grotu
    strzalkaPrawa.setPoint(5, sf::Vector2f(50.f, 35.f));  // koniec linii - gora
    strzalkaPrawa.setPoint(6, sf::Vector2f(0.f, 35.f));   // poczatek linii - gora
    strzalkaPrawa.setOutlineThickness(3.f);
    strzalkaPrawa.setOutlineColor(sf::Color::White);
    strzalkaPrawa.setPosition(830.f, (600.f - strzalkaPrawa.getLocalBounds().height) / 2);
}

void Ustawienia::rysuj(sf::RenderWindow &window)
{
    // sprawdzenie czy mozna przesunac w lewo, jesli tak-strzalka czerwona, nie-szara
    if (wybranaOpcja > 0)
        strzalkaLewa.setFillColor(sf::Color::Red);
    else
        strzalkaLewa.setFillColor(sf::Color(128, 128, 128)); // kolor szary

    // tak samo jak wyzej
    if (wybranaOpcja < 3)
        strzalkaPrawa.setFillColor(sf::Color::Red);
    else
        strzalkaPrawa.setFillColor(sf::Color(128, 128, 128));

    // rysowanie elementow
    window.draw(wybraneTlo);
    window.draw(ramka);
    window.draw(instrukcja);
    window.draw(strzalkaLewa);
    window.draw(strzalkaPrawa);
}

void Ustawienia::przesunWLewo()
{
    // wykonuje sie jesli wybrana opcja wieksza od 0
    if (wybranaOpcja > 0)
    {
        wybranaOpcja--;
        switch (wybranaOpcja)
        {
        case 0:
            wybraneTlo.setTexture(zasoby.teksturaTlo1);
            break;
        case 1:
            wybraneTlo.setTexture(zasoby.teksturaTlo2);
            break;
        case 2:
            wybraneTlo.setTexture(zasoby.teksturaTlo3);
            break;
        //"case 3:" bedzie potrzabne gdy bedzie potrzeba dodania nwoego tla
        // case 3:
        //     wybraneTlo.setTexture(zasoby.blackTexture); // przypisanie czarnej tekstury break;
        //     break;
        default:
            break;
        }
    }
    wybraneTlo.setScale(
        ramka.getSize().x / wybraneTlo.getTexture()->getSize().x,
        ramka.getSize().y / wybraneTlo.getTexture()->getSize().y);
    // -> poniewaz getTexture() zwraca wskaznik sf::Texture*
}

void Ustawienia::przesunWPrawo()
{
    if (wybranaOpcja < 3)
    {
        wybranaOpcja++;
        switch (wybranaOpcja)
        {
        // pominiete case 0, bo nie mozliwosci przejcia do 0 dodajac 1
        case 1:
            wybraneTlo.setTexture(zasoby.teksturaTlo2); // zmiana tla na wybrane
            break;
        case 2:
            wybraneTlo.setTexture(zasoby.teksturaTlo3);
            break;
        case 3:
            wybraneTlo.setTexture(zasoby.czarnaTekstura); // przypisanie czarnej tekstury
            break;
        default:
            break;
        }
        wybraneTlo.setScale(
            ramka.getSize().x / wybraneTlo.getTexture()->getSize().x,
            ramka.getSize().y / wybraneTlo.getTexture()->getSize().y);
        // -> poniewaz getTexture() zwraca wskaznik sf::Texture*
    }
}

int Ustawienia::pobierzWybranaOpcje() const
{
    return wybranaOpcja;
}
