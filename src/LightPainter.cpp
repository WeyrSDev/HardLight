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

clip::Polygon circl(sf::Vector2f v, float r, float angle, float spread)
{
    std::vector<sf::Vector2f> mid;

    if (spread + 1.f / 30.f * ee::pi2 < ee::pi2) mid.push_back(v); //fix for cones

    for (int i = 0; i < 30; ++i)
    {
        const float arg = angle - spread / 2.f + spread * i / 29.f;
        mid.push_back(v + r * sf::Vector2f(std::cos(arg), -std::sin(arg)));
    }

    return translate(mid);
}

void drawBlendedLight(sf::RenderTarget& t, sf::Shader * s, sf::Vector2f m,
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
    states.shader = s;

    if (s) s->setParameter("radius", radius);

    t.draw(arr, states);
}

}

void LightPainter::render(ShadowWorld& w)
{
    m_sumtex.clear();
    clip::Clipper clip;

    for (unsigned i = 0u; i < w.getLightCount(); ++i)
    {
        Light * l = w.getLight(i);

        if (l->m_cached.empty())
        {
            clip.Clear();
            clip::Polygons out(1u);
            out[0] = circl(l->getPosition(), l->getRadius(), l->getAngle(), l->getSpread());

            if (clip::Orientation(out[0])) clip::ReversePolygon(out[0]);

            clip.AddPolygon(out[0], clip::ptSubject);

            //DebugGeometryPainter gp(m_sumtex);
            //auto a = translate(out[0]);
            //gp.polygon(a.data(), a.size(), sf::Color::Blue);

            for (const auto& v : l->m_shadows)
            {
                auto pl = translate(v);
                clip::CleanPolygon(pl, 0.0); //to avoid segfaults from same points
                if (clip::Orientation(pl)) clip::ReversePolygon(pl);
                clip.AddPolygon(pl, clip::ptClip);
            }//for poly

            //do not clip if there are no shadows
            if (l->m_shadows.size() > 0u)
            {
                clip.Execute(clip::ctDifference, out, clip::pftNonZero, clip::pftNonZero);
            }

            if (out.size() > 0u) l->m_cached = translate(out[0]);
        }

        const auto& poly = l->m_cached;
        sf::Shader * frag = m_fragenabled?&m_frag:nullptr;
        drawBlendedLight(m_sumtex, frag, l->getPosition(), l->getRadius(), poly.data(), poly.size(), l->getColor());

    }//for light

    m_sumtex.display();
}

bool LightPainter::enableFragFromFile(const std::string& fn)
{
    m_fragenabled = m_frag.loadFromFile(fn, sf::Shader::Fragment);
    return isFragEnabled();
}

}
