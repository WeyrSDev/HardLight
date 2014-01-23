#include "ShadowWorld.hpp"
#include <cmath>
#include <polyclipping/clipper.hpp>

namespace ee {

namespace clip = ClipperLib;

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

}

namespace {

sf::Vector2f setLength(sf::Vector2f v, float l)
{
    const float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len == 0.f) return v;

    return l / len * v;
}

}

Light * ShadowWorld::addLight(sf::Vector2f p, float r)
{
    m_lights.emplace_back(new Light);
    Light * l = m_lights.back().get();

    l->m_pos = p;
    l->m_radius = r;
    l->m_spread = pi2;
    l->m_angle = 0.f;
    l->m_color = sf::Color::White;

    return l;
}

void ShadowWorld::removeLight(Light * ptr)
{
    for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
    {
        if (it->get() == ptr)
        {
            m_lights.erase(it);
            return;
        }
    }
}

void ShadowWorld::addLine(sf::Vector2f a, sf::Vector2f b)
{
    b2AABB ab;
    ab.lowerBound.x = std::min(a.x, b.x);
    ab.lowerBound.y = std::min(a.y, b.y);

    ab.upperBound.x = std::max(a.x, b.x);
    ab.upperBound.y = std::max(a.y, b.y);

    ee::ShadowLine line;
    line.a = a;
    line.b = b;

    m_tree.CreateProxy(ab, line);
}

void ShadowWorld::addLines(const sf::Vector2f* v, unsigned len)
{
    for (unsigned i = 0u; i < (len - 1u); ++i)
    {
        addLine(v[i], v[i + 1u]);
    }
}

void ShadowWorld::addLinesStrip(const sf::Vector2f* v, unsigned len)
{
    const unsigned count = len / 2u;

    for (unsigned i = 0u; i < count; ++i)
    {
        addLine(v[2 * i], v[2 * i + 1]);
    }
}

void ShadowWorld::update()
{
    for (const std::unique_ptr<Light>& light : m_lights)
    {
        light->m_shadows.clear();
        light->m_cached.clear();

        m_currentlight = light.get();

        const sf::Vector2f p = light->getPosition();
        const float rmul = 100.f * light->getRadius();

        b2AABB ab;
        ab.lowerBound.x = p.x - light->getRadius();
        ab.lowerBound.y = p.y - light->getRadius();
        ab.upperBound.x = p.x + light->getRadius();
        ab.upperBound.y = p.y + light->getRadius();

        m_tree.Query(this, ab); //query the line tree and build shadows

        if (light->m_cached.empty())
        {
            clip::Clipper clip;

            clip::Polygons out(1u);
            out[0] = circl(light->getPosition(), light->getRadius(), light->getAngle(), light->getSpread());

            if (clip::Orientation(out[0])) clip::ReversePolygon(out[0]);

            clip.AddPolygon(out[0], clip::ptSubject);

            for (const auto& v : light->m_shadows)
            {
                auto pl = translate(v);
                clip::CleanPolygon(pl, 0.0); //to avoid segfaults from same points
                if (clip::Orientation(pl)) clip::ReversePolygon(pl);
                clip.AddPolygon(pl, clip::ptClip);
            }//for poly

            //do not clip if there are no shadows
            if (light->m_shadows.size() > 0u)
            {
                clip.Execute(clip::ctDifference, out, clip::pftNonZero, clip::pftNonZero);
            }

            if (out.size() > 0u) light->m_cached = translate(out[0]);
        }

    }//for light
}

unsigned ShadowWorld::getLightCount() const
{
    return m_lights.size();
}

Light * ShadowWorld::getLight(unsigned i) const
{
    return i < m_lights.size()?m_lights[i].get():nullptr;
}

void ShadowWorld::rebuildLinesTree()
{
    m_tree.RebuildBottomUp();
}

bool ShadowWorld::QueryCallback(int id)
{
    const ShadowLine line = m_tree.GetShadowLine(id);

    const sf::Vector2f p = m_currentlight->getPosition();
    const float rmul = 100.f * m_currentlight->getRadius();
    const sf::Vector2f ad(setLength(line.a - p, rmul));
    const sf::Vector2f bd(setLength(line.b - p, rmul));

    std::vector<sf::Vector2f> sh;
    sh.push_back(line.a);
    sh.push_back(line.b);
    sh.push_back(p + bd);
    sh.push_back(p + ad);

    m_currentlight->m_shadows.push_back(sh);

    return true;
}

unsigned ShadowWorld::getLinesCounts() const
{
    m_tree.GetShadowLinesCount();
}

void ShadowWorld::removeAllLines()
{
    m_tree.ClearAll();
}


}
