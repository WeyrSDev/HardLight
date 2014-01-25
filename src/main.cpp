#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <polyclipping/clipper.hpp>
#include "DebugGeometryPainter.hpp"
#include "ShadowWorld.hpp"
#include "LightPainter.hpp"

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
    shw.setViewRect(sf::FloatRect(0.f, 0.f, 640.f, 480.f));
    ee::LightPainter lp;
    lp.setSize(640u, 480u);
    //lp.enableFragFromFile("light.frag");

    ee::Light * lit = shw.addLight({0.f, 0.f}, 100.f);
    //lit->setColor(sf::Color::Red);
    lit->setSpread(ee::halfpi);
    //lit->setAngle(ee::halfpi);

    float angle = 0.f;

    std::vector<sf::Vector2f> pl;
    makeHex(pl, sf::Vector2f(300.f, 300.f));

    shw.addLinesStrip(pl.data(), pl.size());
    shw.addLine(pl[0], pl[pl.size() - 1u]);


    std::vector<ee::Light*> lights;


    sf::Vector2f a;
    bool adding = false;

    int count = 0;
    const int frames = 30;

    while (app.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) angle += ee::pi2 / 120.f;

        lit->setAngleClean(angle);

        sf::Event eve;
        while (app.pollEvent(eve))
        {
            if (eve.type == sf::Event::Closed) app.close();

            if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Left)
            {
                a = sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y);
                adding = true;
            }

            if (eve.type == sf::Event::MouseButtonReleased && eve.mouseButton.button == sf::Mouse::Left)
            {
                shw.addLine(a, sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y));
                adding = false;
            }

            if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Right)
            {
                auto l = shw.addLight(sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y), 100.f);
                l->setColor(sf::Color::Cyan);
                lights.push_back(l);
            }

            if (eve.type == sf::Event::KeyPressed && eve.key.code == sf::Keyboard::S)
            {
                lp.reenableFrag(!lp.isFragEnabled());
            }

            if (eve.type == sf::Event::KeyPressed && eve.key.code == sf::Keyboard::Z)
            {
                shw.removeAllLines();
            }

            if (eve.type == sf::Event::KeyPressed && eve.key.code == sf::Keyboard::D)
            {
                if (!lights.empty())
                {
                    shw.removeLight(lights.front());
                    lights.erase(lights.begin());
                }
            }

        }//while pollEvent

        lit->setPositionClean(sf::Vector2f(sf::Mouse::getPosition(app)));

        app.clear();

        shw.update();
        lp.render(shw);

        app.draw(payday);
        app.draw(sf::Sprite(lp.getCanvas()), sf::BlendMultiply);

        if (adding) gp.segment(a, sf::Vector2f(sf::Mouse::getPosition(app)));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            for (unsigned i = 0u; i < shw.getQueriedLinesCount(); ++i)
            {
                const ee::ShadowLine line = shw.getQueriedLine(i);
                gp.segment(line.a, line.b, sf::Color::Magenta);
            }

            //            for (const std::vector<sf::Vector2f>& v : lit->m_shadows)
            //            {
            //                gp.polygon(v.data(), v.size(), sf::Color::Blue);
            //            }

        }

        std::printf("total L : %u queried L : %u total L : %u queried L : %u\n", shw.getLightsCount(), shw.getQueriedLightsCount(), shw.getLinesCount(), shw.getQueriedLinesCount());

        app.display();
    }//while app is open
}

