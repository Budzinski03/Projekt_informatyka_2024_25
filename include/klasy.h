#include <SFML/Graphics.hpp>


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

class Wrog
{
public:
    Wrog(float x, float y);
    void rysuj(sf::RenderWindow &window) const;
    const sf::RectangleShape &pobierzKsztalt() const; // Zwraca referencję
    void ustawPozycje(float x, float y);
    void przesun(float dx, float dy);

private:
    sf::RectangleShape ksztalt;
};


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
    bool czyPusteWejscie();
};
