#include "ShadowWorld.hpp"
#include <cmath>
#include <SFML/Window/Keyboard.hpp>

namespace ee {

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
    Line line;
    line.a = a;
    line.b = b;
    m_lines.push_back(line);
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

        const sf::Vector2f p = light->getPosition();
        const float rmul = 100.f * light->getRadius();

        for (const Line& line : m_lines)
        {
            const sf::Vector2f ad(setLength(line.a - p, rmul));
            const sf::Vector2f bd(setLength(line.b - p, rmul));

            std::vector<sf::Vector2f> sh;
            sh.push_back(line.a);
            sh.push_back(line.b);
            sh.push_back(p + bd);
            sh.push_back(p + ad);

            light->m_shadows.push_back(sh);
        }


    }

}

}
