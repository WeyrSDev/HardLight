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

sf::Color getCol()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
    {
        return sf::Color::Red;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
    {
        return sf::Color::Green;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
    {
        return sf::Color::Blue;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
    {
        return sf::Color::Black;
    }
    else
    {
        return sf::Color::White;
    }
}

sf::Vector2f getLaurentianCenter(const std::vector<sf::Vector2f>& v)
{
    if (v.size() == 0) return sf::Vector2f();

    float minx = v[0].x;
    float maxx = v[0].x;

    float miny = v[0].y;
    float maxy = v[0].y;

    for (const sf::Vector2f p : v)
    {
        minx = std::min(p.x, minx);
        maxx = std::max(p.x, maxx);

        miny = std::min(p.y, miny);
        maxy = std::max(p.y, maxy);
    }

    return sf::Vector2f((minx + maxx) / 2.f, (miny + maxy) / 2.f);
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

    lit->setColor(sf::Color::Red);

    std::vector<sf::Vector2f> pl;
    pl.push_back(sf::Vector2f(300.f, 300.f));
    pl.push_back(sf::Vector2f(500.f, 300.f));
    //    pl.push_back(sf::Vector2f(500.f, 300.f - off));
    //    pl.push_back(sf::Vector2f(300.f, 300.f - off));


    shw.m_polys.push_back(pl);
    //#warning "ReadMe"
    //FOR LINES IT STILL FAILS AT TIMES, MAKE SEPARATE ALGO?

    //    static_assert(false, "10+ lights lag the algo, perf it");

    int count = 0;

    const int frames = 30;

    float angle = 0.f;

    while (app.isOpen())
    {
        angle += ee::pi2 / 360.f;


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
        }

        if (true || sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            //200.f * sf::Vector2f(std::cos(angle), -std::sin(angle)) +
            lit->setPosition(sf::Vector2f(sf::Mouse::getPosition(app)));
        }

        shw.update();

        sf::Clock clo2;

        //        lp.render(shw);
        lp.renderViaClipper(shw);

        ++count;

        if (count % frames == 0)
        {
            std::printf("shadow %f ", 60.f * clo2.getElapsedTime().asSeconds());
        }


        app.clear();

        app.draw(sf::Sprite(lp.m_sumtex.getTexture()));



        sf::RectangleShape sha;
        sha.setSize({1000.f, 1000.f});
        sha.setFillColor(getCol());
        app.draw(sha, sf::BlendMode::BlendMultiply);




        const sf::Vector2f mousepos(sf::Mouse::getPosition(app));

        const sf::Vector2f vz2 = mousepos +
        100.f * sf::Vector2f(std::cos(shw.refangle), -std::sin(shw.refangle));

        //        gp.halfLine(mousepos, vz2, sf::Color::Red);

        //static_assert(false,"scanline is wrong");
        const auto laurent = getLaurentianCenter(lp.light);

        for (const auto v : lp.light)
        {
            //gp.halfLine(laurent, v, sf::Color::Red);
            //gp.segment(lit->getPosition(), v, sf::Color::Red);
        }

        for (const std::vector<sf::Vector2f>& v : shw.m_polys)
        {
            gp.polygon(v.data(), v.size(), sf::Color(255u, 0u, 0u, 127u));

            for (const sf::Vector2f p : v)
            {
                //gp.halfLine(sf::Vector2f(sf::Mouse::getPosition(app)), p);
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
            std::printf("frame %f\n", 60.f * clo.getElapsedTime().asSeconds());
        }


        app.display();
    }
    // static_assert(false,"read!");
    //make shadows based on the idea i had wheen seeing pic
    //we can make polygons using 'infinite' lines projected onto shapes
    //so like a 2x2 box occludes
    //and then we project from one side a light, determine visible from light
    //surfaces and then using clipper of light projected thru surface points
    //+ surface itself in middle clipped with entire world, and we transmute
    //that into vertices, triangles or smt, with right lerping of
    //colors and shit and draw! and boom!

    //use something outside of range as a point of reference for scanline 
    //rotator?
}

