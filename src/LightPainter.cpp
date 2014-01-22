#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <polyclipping/clipper.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Shader.hpp>
#include "LightPainter.hpp"
#include "ShadowWorld.hpp"
#include "DebugGeometryPainter.hpp"

namespace ee {

namespace clip = ClipperLib;

void LightPainter::setSize(unsigned x, unsigned y)
{
    m_sumtex.create(x, y);

    //almost surely it has to be done per pixel..
    m_frag.loadFromFile("light.frag", sf::Shader::Fragment);
}

namespace {

const float scale = 1000.f * 1000.f;

clip::IntPoint point(sf::Vector2f v)
{
    return clip::IntPoint(v.x * scale, v.y * scale);
}

sf::Vector2f point(clip::IntPoint v)
{
    return sf::Vector2f(v.X / scale, v.Y / scale);
}

clip::Polygon translate(const std::vector<sf::Vector2f>& in)
{
    clip::Polygon out;

    for (const auto v : in)
    {
        out.push_back(point(v));
    }

    return out;
}

std::vector<sf::Vector2f> translate(const clip::Polygon& in)
{
    std::vector<sf::Vector2f> out;

    for (const auto v : in)
    {
        out.push_back(point(v));
    }

    return out;
}

clip::Polygon circl(sf::Vector2f v, float r)
{
    std::vector<sf::Vector2f> mid;

    for (int i = 0; i < 30; ++i)
    {
        const float arg = pi2 * i / 30.f;
        mid.push_back(v + r * sf::Vector2f(std::cos(arg), -std::sin(arg)));
    }

    return translate(mid);
}

void drawBlendedLight(sf::RenderTarget& t, sf::Shader& s, sf::Vector2f m,
float radius, const sf::Vector2f * p, unsigned len, sf::Color c)
{
    sf::VertexArray arr(sf::TrianglesFan, len + 2u);
    arr[0].position = m;
    arr[0].color = c;

    for (unsigned i = 0; i < len; ++i)
    {
        arr[i + 1u].position = p[i];
        arr[i + 1u].color = c;
        arr[i + 1u].texCoords = p[i] - m;
    }

    arr[len + 1u] = arr[1];

    sf::RenderStates states;
    states.blendMode = sf::BlendAdd;
    states.shader = &s;

    s.setParameter("radius", radius);

    t.draw(arr, states);
}

}

void LightPainter::render(ShadowWorld& w)
{
    m_sumtex.clear();

    clip::Clipper clip;
    for (const std::unique_ptr<Light>& l : w.m_lights)
    {
        clip.Clear();

        clip::Polygons out(1u);
        out[0] = circl(l->getPosition(), l->getRadius());

        int count = -1;
        sf::Color color[3] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};
        for (int i = 0; i < 3; ++i) color[i].a = 127u;

        const bool rgb[3] = {sf::Keyboard::isKeyPressed(sf::Keyboard::R), sf::Keyboard::isKeyPressed(sf::Keyboard::G), sf::Keyboard::isKeyPressed(sf::Keyboard::B)};

        static int change = 0;

        const auto type1 = clip::pftEvenOdd;

        const auto type2 = clip::pftEvenOdd;

        const bool qdown = rgb[0] || rgb[1] || rgb[2];

        for (const auto& v : l->m_shadows)
        {
            ++count;

            clip.Clear();

            auto pl = translate(v);
            clip::CleanPolygon(pl, 0.0); //to avoid segfaults from same points

            if (pl.size() != 0u)
            {
                clip.AddPolygon(out[0], clip::ptSubject);
                clip.AddPolygon(pl, clip::ptClip);
                clip.Execute(clip::ctDifference, out, type1, type2);
            }

            std::printf("%d\n", (int) clip::Orientation(translate(v)));


            if (count < 3 && rgb[count] && out.size() > 0u && qdown)
            {
                const auto poly = translate(out[0]);

                DebugGeometryPainter paint(m_sumtex);
                paint.polygon(poly.data(), poly.size(), color[count]);

            }

        }//for poly




        if (!qdown && out.size() > 0u)
        {
            const auto poly = translate(out[0]);
            //paint.polygonCenter(l->getPosition(), poly.data(), poly.size());
            sf::Color out = l->getColor();
            out.a = 0u;
            drawBlendedLight(m_sumtex, m_frag, l->getPosition(), l->getRadius(), poly.data(), poly.size(), l->getColor());

        }
    }//for light

    m_sumtex.display();
}

}
