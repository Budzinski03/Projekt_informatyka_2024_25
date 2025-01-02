#include <iostream>
#include <SFML\Graphics.hpp>
#include <vector>

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

// obsługa wejścia
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
    //void ruszaj(float deltaTime);
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

// // ruch wrogów
// void Wrog::ruszaj(float deltaTime)
// {
//     ksztalt.move(0, predkosc * deltaTime);
// }

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

// ruch pocisków
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

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space invaders");

    Gracz gracz;
    std::vector<Wrog> wrogowie;
    std::vector<Pocisk> pociski;

    // Dodanie przeciwnikow
    int kolumny = 10;
    int wiersze = 4;
    float odstepK = 60.f;
    float odstepW = 40.f;

    // kierunek i szybkość ruchu przeciwników
    bool ruchWPrawo = true;     // czy grupa porusza sie w prawo
    float predkoscRuchu = 20.f; // predkosc ruchu w poziomie
    float odstepPoziomy = 10.f; // odleglosc ruchu w dol

    for (int rzad = 0; rzad < wiersze; ++rzad)
    {
        for (int kolumna = 0; kolumna < kolumny; ++kolumna)
        {
            float x = kolumna * odstepK + 20; // odtsep miedzy kolumnami
            float y = rzad * odstepW + 20;    // odstep miedzy rzedami
            wrogowie.emplace_back(x, y);
        }
    }

    // czas do obliczania płynności ruchu
    sf::Clock clock;

    float czasOdOstatniegoStrzalu = 0.f;
    float minimalnyCzasStrzalu = 0.2f;

    // pętla gry
    while (window.isOpen())
    {
        // obsługa zdarzeń
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

            // po nacisnieciu spacji wystrzal oraz czas miedzy strzalami = minimalnyCzasStrzalu
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && czasOdOstatniegoStrzalu >= minimalnyCzasStrzalu)
            {
                pociski.emplace_back(gracz.pobierzPozycje().x + 22.5f, gracz.pobierzPozycje().y);
                czasOdOstatniegoStrzalu = 0.0f;
            }
        }
        // aktualizacja stanu gry
        sf::Time deltaTime = clock.restart();
        gracz.steruj(deltaTime.asSeconds());

        czasOdOstatniegoStrzalu += deltaTime.asSeconds();


        
        bool dotknietoKrawedzi = false;
        for (auto &wrog : wrogowie)
        {
            // ruch w lewo lub w prawo
            if (ruchWPrawo)
            {
                wrog.przesun(predkoscRuchu * deltaTime.asSeconds(), 0);
            }
            else
            {
                wrog.przesun(-predkoscRuchu * deltaTime.asSeconds(), 0);
            }

            // sprawdzenie czy wrog dotknal krawedzi ekranu
            if (wrog.pobierzPozycje().x <= 0 || wrog.pobierzPozycje().x + 40 >= 800)
            {
                dotknietoKrawedzi = true; // zmiana kierunku
            }
        }

        // jesli dotykają krawędzi to zmiana kierunku
        if (dotknietoKrawedzi)
        {
            ruchWPrawo = !ruchWPrawo; // zmiana kierunku
            for (auto &wrog : wrogowie)
            {
                wrog.przesun(0, odstepPoziomy); // przesun wrogow w dol
            }
            dotknietoKrawedzi = false; 
        }

        for (const auto &wrog : wrogowie)
        {
            std::cout << "Wrog: (" << wrog.pobierzPozycje().x << ", " << wrog.pobierzPozycje().y << ")\n";
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

        // Sprawdzanie kolizji między pociskami a przeciwnikami
        for (auto wrogIt = wrogowie.begin(); wrogIt != wrogowie.end();)
        {
            bool wrogZniszczony = false;

            for (auto pociskIt = pociski.begin(); pociskIt != pociski.end();)
            {
                if (pociskIt->pobierzObszar().intersects(sf::FloatRect(wrogIt->pobierzPozycje(), sf::Vector2f(40, 20))))
                {
                    // Jeśli kolizja, usuń pocisk i oznacz przeciwnika do usunięcia
                    pociskIt = pociski.erase(pociskIt);
                    wrogZniszczony = true;
                    break;
                }
                else
                {
                    ++pociskIt;
                }
            }

            // Usuń przeciwnika, jeśli został trafiony
            if (wrogZniszczony)
            {
                wrogIt = wrogowie.erase(wrogIt);
            }
            else
            {
                ++wrogIt;
            }
        }

        // renderowanie
        window.clear();
        gracz.rysuj(window);

        for (auto &wrog : wrogowie)
        {
            wrog.rysuj(window);
        }

        for (auto &pocisk : pociski)
        {
            pocisk.rysuj(window);
        }

        window.display();
    }

    return 0;
}