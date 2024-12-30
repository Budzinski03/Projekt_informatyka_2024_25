#include <iostream>
#include <SFML\Graphics.hpp>
#include <vector>

class Gracz
{
public:
    Gracz();
    void steruj(float);
    void rysuj(sf::RenderWindow &);
    sf::Vector2f getPosition() const;

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

//obsługa wejścia
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

sf::Vector2f Gracz::getPosition() const
{
    return ksztalt.getPosition();
}

class Wrog
{
public:
    Wrog(float x, float y);
    void ruszaj(float deltaTime);
    void rysuj(sf::RenderWindow &window);
    sf::Vector2f getPosition();

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

//ruch wrogów
void Wrog::ruszaj(float deltaTime)
{
    ksztalt.move(0, predkosc * deltaTime);
}

void Wrog::rysuj(sf::RenderWindow &window)
{
    window.draw(ksztalt);
}

sf::Vector2f Wrog::getPosition()
{
    return ksztalt.getPosition();
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

//ruch pocisków
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

    // Dodanie kilku przeciwnikow
    wrogowie.emplace_back(100, 50);
    wrogowie.emplace_back(200, 50);
    wrogowie.emplace_back(300, 50);

    // czas do obliczania płynności ruchu
    sf::Clock clock;

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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                pociski.emplace_back(gracz.getPosition().x + 22.5f, gracz.getPosition().y);
        }
        // aktualizacja stanu gry
        sf::Time deltaTime = clock.restart();
        gracz.steruj(deltaTime.asSeconds());

        for (auto &wrog : wrogowie)
        {
            wrog.ruszaj(deltaTime.asSeconds());
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
                if (pociskIt->pobierzObszar().intersects(sf::FloatRect(wrogIt->getPosition(), sf::Vector2f(40, 20))))
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