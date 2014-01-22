#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <polyclipping/clipper.hpp>
#include "DebugGeometryPainter.hpp"
#include "ShadowWorld.hpp"
#include "LightPainter.hpp"

//const float polyscale = 1000.f;

namespace clip = ClipperLib;

/*
 sort out weird crap with -y faking too?
 */

void makeHex(std::vector<sf::Vector2f>& poly, sf::Vector2f p)
{
    sf::CircleShape sha;
    sha.setRadius(60.f);
    sha.setPointCount(9u);
    poly.clear();

    for (int i = 0; i < sha.getPointCount(); ++i)
    {
        poly.push_back(p + sha.getPoint(i));
    }
}

int main()
{
    sf::Texture tex;
    tex.loadFromFile("payday.jpg");
    sf::Sprite payday(tex);
    payday.setScale(640.f / tex.getSize().x, 480.f / tex.getSize().y);

    sf::RenderWindow app(sf::VideoMode(640u, 480u), "lights");
    app.setFramerateLimit(60u);
    ee::DebugGeometryPainter gp(app);

    ee::ShadowWorld shw;
    ee::LightPainter lp;
    lp.setSize(640u, 480u);
    lp.enableFragFromFile("light.frag");

    ee::Light * lit = shw.addLight({0.f, 0.f}, 300.f);
    lit->setColor(sf::Color::Red);

    std::vector<sf::Vector2f> pl;
    makeHex(pl, sf::Vector2f(300.f, 300.f));

    shw.addLines(pl.data(), pl.size());
    shw.addLine(pl[0], pl[pl.size() - 1u]);


    sf::Vector2f a;

    int count = 0;
    const int frames = 30;

    while (app.isOpen())
    {
        ++count;
        sf::Clock clo;

        sf::Event eve;
        while (app.pollEvent(eve))
        {
            if (eve.type == sf::Event::Closed) app.close();

            if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Left)
            {
                a = sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y);
            }

            if (eve.type == sf::Event::MouseButtonReleased && eve.mouseButton.button == sf::Mouse::Left)
            {
                shw.addLine(a, sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y));
            }

            if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Right)
            {
                shw.addLight(sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y), 250.f);
            }

            if (eve.type == sf::Event::KeyPressed && eve.key.code == sf::Keyboard::Z)
            {
                shw.m_lines.clear();
            }

        }//while pollEvent

        if (true || sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            lit->setPosition(sf::Vector2f(sf::Mouse::getPosition(app)));
        }

        shw.update();

        sf::Clock clo2;
        lp.render(shw);
        if (count % frames == 0) std::printf("shadow %f ", 60.f * clo2.getElapsedTime().asSeconds());

        app.clear();
        app.draw(payday);
        app.draw(sf::Sprite(lp.getCanvas()), sf::BlendMultiply);

        const sf::Vector2f mousepos(sf::Mouse::getPosition(app));

        if (count % frames == 0)
        {
            std::printf("frame %f ", 60.f * clo.getElapsedTime().asSeconds());
            std::printf("lights:%u lines:%u\n", shw.m_lights.size(), shw.m_lines.size());
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            for (const ee::Line& line : shw.m_lines)
            {
                gp.segment(line.a, line.b, sf::Color::Magenta);
            }
        }

        app.display();
    }//while app is open
}

