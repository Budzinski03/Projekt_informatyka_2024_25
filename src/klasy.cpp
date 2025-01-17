#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include "../include/klasy.h"
#include <iostream>


///////////////////// MENU /////////////////////////

Menu::Menu(const sf::Font &czcionka) : wybranaOpcja(0)
{
    std::string tekstOpcji[] = {"Nowa gra", "Ranking", "Wyjscie"};

    for (int i = 0; i < 3; i++)
    {
        sf::Text tekst;
        tekst.setFont(czcionka);
        tekst.setCharacterSize(40);
        // jesli i = wybranaOpcja to kolor czerowny, jesli nie to bialy
        tekst.setFillColor(i == wybranaOpcja ? sf::Color::Red : sf::Color::White);
        tekst.setString(tekstOpcji[i]);
        tekst.setPosition(sf::Vector2f(960 / 8, 600 / (4) * (1 * i + 0.5)));
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
    else //jesli wiecej to wysweitla serce xilosc
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
    //ulozenie tekstu w zaleznosci do wymiarow - ilosci punktow
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
    std::cout << zycie << std::endl;
    for (int i = 0; i < zycie; i++)
    {
        std::cout << "dodano zycie\n";
        sf::Sprite serce;
        serce.setTexture(teksturaSerca);
        serce.setScale(0.05f, 0.05f);

        // Obliczenie pozycji nowego serca na podstawie istniejących serc
        float xOffset = 20 + serca.size() * 30; // Nowa pozycja zależna od liczby istniejących serc
        serce.setPosition(xOffset, 20);

        serca.push_back(serce);
    }

    // Zwiększenie liczby żyć
    liczbaZyc += zycie;
}

void Gracz::resetuj()
{
    // Resetuje pozycję gracza
    ksztalt.setPosition(480.f, 550.f); // Przykładowa pozycja startowa
}


///////////////////// WROG /////////////////////////

//przyejmuje 3 lub 4 argumenty, jesli 3 to zycia domyslne 1 ( wpliku nagłówkowym)
Wrog::Wrog(float x, float y, int typ, int zycia) : typ(typ), czyStrzelil(false), zycia(zycia)
{
    switch (typ)
    {
    case 1:
        ksztalt.setFillColor(sf::Color::Red);
        ksztalt.setSize(sf::Vector2f(50, 25));
        wartoscPunktow = 30;
        break;
    case 2:
        ksztalt.setFillColor(sf::Color::Green);
        ksztalt.setSize(sf::Vector2f(50, 25));
        wartoscPunktow = 20;
        break;
    case 3:
        ksztalt.setFillColor(sf::Color::Blue);
        ksztalt.setSize(sf::Vector2f(50, 25));
        wartoscPunktow = 10;
        break;
    case 4:
        ksztalt.setFillColor(sf::Color::Magenta);
        ksztalt.setSize(sf::Vector2f(80, 40));
        wartoscPunktow = 100;
        zycia = 5; // wieksza liczba zyc
        break;
    default:
        ksztalt.setFillColor(sf::Color::White);
        ksztalt.setSize(sf::Vector2f(80, 40));
        wartoscPunktow = 50;
        break;
    }
    ksztalt.setPosition(x, y);
    std::cout << zycia << '\n';
}

void Wrog::rysuj(sf::RenderWindow &window) const
{
    window.draw(ksztalt);
}

const sf::RectangleShape &Wrog::pobierzKsztalt() const
{
    return ksztalt; // Zwracamy referencję
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

int Wrog::pobierzTyp()
{
    return typ;
}


bool Wrog::stracZycie()
{
    if (zycia > 1)
    {
        zycia --;
        std::cout<<"odejmuje zycie  Zycia: "<<zycia<<std::endl;
        return true;
    }
    std::cout<<"Zwracam false\n";
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
}

Pocisk::Pocisk(float x, float y, float kierunek, sf::Color kolor, int typWroga) : predkosc(300.f), kierunek(kierunek)
{
    ksztalt.setSize(sf::Vector2f(5, 10));
    ksztalt.setFillColor(kolor); // ustawienie koloru pocsiku kolor pocisku
    ksztalt.setPosition(x, y);
    obrazenia = 1;
    if (typWroga == 4)
        obrazenia = 2;
}

// ruch pocisków
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
    tlo.setFillColor(sf::Color(50, 50, 50, 200));
    tlo.setOutlineColor(sf::Color::White);
    tlo.setOutlineThickness(2);
    tlo.setPosition(pozycja);
    wysrodkujTekst();
}

void UstawTekst::wysrodkujTekst()
{
    sf::FloatRect granice = tekst.getLocalBounds();
    tekst.setOrigin(granice.width / 2, 0);
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

        // Wprowadzanie znaków przez użytkownika
        if (event.type == sf::Event::TextEntered)
        {
            if (event.text.unicode == '\b') // Backspace
            {
                if (!wejscie.empty())
                {
                    wejscie.pop_back();
                }
            }
            else if (event.text.unicode >= 32 && event.text.unicode < 128 && wejscie.size() < 15)
            {
                // Ignoruj niewidzialne znaki poniżej 32 (np. tabulatory, nowa linia)
                wejscie += static_cast<char>(event.text.unicode);
            }

            // Aktualizacja wyświetlanego tekstu
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

// bool UstawTekst::czyPusteWejscie()
// {
//     return wejscie.empty();
// }