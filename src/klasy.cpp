#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include "../include/klasy.h"
#include <iostream>



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
    //  Rysowanie punktów
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



Pocisk::Pocisk(float x, float y, float kierunek) : predkosc(300.f), kierunek(kierunek)
{
    ksztalt.setSize(sf::Vector2f(5, 10));
    // jezeli kierunek < 0 to magenta, w innym przypadku yellow
    ksztalt.setFillColor(kierunek < 0 ? sf::Color::Magenta : sf::Color::Yellow);
    ksztalt.setPosition(x, y);
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

bool UstawTekst::czyPusteWejscie()
{
    return wejscie.empty();
}