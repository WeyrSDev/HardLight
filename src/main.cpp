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
    sf::RenderWindow app(sf::VideoMode(640u, 480u), "lights");
    app.setFramerateLimit(60u);
    ee::DebugGeometryPainter gp(app);

    ee::ShadowWorld shw;
    ee::LightPainter lp;
    lp.setSize(640u, 480u);

    ee::Light * lit = shw.addLight({0.f, 0.f}, 200.f);

    lit->setColor(sf::Color::Yellow);

    const float off = 2.f;
    std::vector<sf::Vector2f> pl;
    pl.push_back(sf::Vector2f(300.f, 300.f));
    pl.push_back(sf::Vector2f(500.f, 300.f));
    pl.push_back(sf::Vector2f(500.f, 300.f - off));
    pl.push_back(sf::Vector2f(300.f, 300.f - off));

    shw.m_polys.push_back(pl);

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

            if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Middle)
            {
                std::vector<sf::Vector2f> poly;
                makeHex(poly, sf::Vector2f(sf::Mouse::getPosition(app)));
                shw.m_polys.push_back(poly);
            }

            if (eve.type == sf::Event::KeyPressed && eve.key.code == sf::Keyboard::A)
            {
                auto l = shw.addLight(sf::Vector2f(sf::Mouse::getPosition(app)), 100.f);
                l->setColor(sf::Color::Green);
            }

            if (eve.type == sf::Event::KeyPressed && eve.key.code == sf::Keyboard::Z)
            {
                shw.m_polys.clear();
            }

            if (eve.type == sf::Event::MouseWheelMoved)
            {
                lit->setRadius(lit->getRadius() + eve.mouseWheel.delta);
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
        app.draw(sf::Sprite(lp.getCanvas()));

        const sf::Vector2f mousepos(sf::Mouse::getPosition(app));

        for (const std::vector<sf::Vector2f>& v : shw.m_polys)
        {
            if (v.size() == 2u)
            {
                gp.segment(v[0], v[1], sf::Color(255u, 0u, 0u, 127u));
            }
            else
            {
                gp.polygon(v.data(), v.size(), sf::Color(255u, 0u, 0u, 127u));
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            for (const std::vector<sf::Vector2f>& v : lit->m_shadows)
            {
                gp.polygon(v.data(), v.size(), sf::Color::Blue);
            }
        }

        if (count % frames == 0)
        {
            std::printf("frame %f ", 60.f * clo.getElapsedTime().asSeconds());
            std::printf("lights:%u hulls:%u\n", shw.m_lights.size(), shw.m_polys.size());
        }

        app.display();
    }
}

