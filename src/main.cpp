#include <SFML/Graphics.hpp>
#include "ShadowWorld.hpp"
#include "LightPainter.hpp"

int main()
{
    sf::RenderWindow app(sf::VideoMode(640u, 480u), "lights");
    app.setFramerateLimit(60u);

    //prepare shadow world and set its view
    ee::ShadowWorld sw;
    sw.setViewRect(sf::FloatRect(0.f, 0.f, 640.f, 480.f));

    //prepare light painter and set internal texture size
    ee::LightPainter lp;
    lp.setSize(640u, 480u);
    lp.enableFragFromFile("light.frag");

    ee::LightDef ldef;
    ldef.Color = sf::Color::Green;
    ldef.Radius = 300.f;
    ee::Light * lit = sw.addLight(ldef);

    bool adding = false;
    sf::Vector2f p1;

    while (app.isOpen())
    {
        sf::Event eve;
        while (app.pollEvent(eve))
        {
            switch (eve.type)
            {
                case sf::Event::Closed:
                    app.close();
                    break;
                case sf::Event::MouseMoved:
                    lit->setPosition(sf::Vector2f(eve.mouseMove.x, eve.mouseMove.y));
                    break;
                case sf::Event::MouseButtonPressed:
                    if (eve.mouseButton.button == sf::Mouse::Left)
                    {
                        adding = true;
                        p1 = sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y);
                    }
                    else if (eve.mouseButton.button == sf::Mouse::Right)
                    {
                        ldef = ee::LightDef();
                        ldef.Position = sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y);
                        sw.addLight(ldef);
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if (eve.mouseButton.button == sf::Mouse::Left)
                    {
                        sw.addLine(p1, sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y));
                        adding = false;
                    }
                    break;
            }//switch eve.type
        }//while pollEvent

        app.clear(sf::Color::White);

        //update shadow world and then render that
        sw.update();
        lp.render(sw);

        //multiply texture of painters render texture with scene
        app.draw(sf::Sprite(lp.getCanvas()), sf::BlendMultiply);

        //if space pressed draw all lines that participated in last update
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            for (unsigned i = 0u; i < sw.getQueriedLinesCount(); ++i)
            {
                sf::Vertex vert[2];
                vert[0].position = sw.getQueriedLine(i).a;
                vert[1].position = sw.getQueriedLine(i).b;
                app.draw(vert, 2u, sf::Lines);
            }
        }

        if (adding)
        {
            sf::Vertex vert[2];
            vert[0].position = p1;
            vert[1].position = sf::Vector2f(sf::Mouse::getPosition(app));
            vert[0].color = vert[1].color = sf::Color::Yellow;
            app.draw(vert, 2u, sf::Lines);
        }

        app.display();
    }//while app is open
}
