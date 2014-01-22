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
    m_lines.emplace_back(new ShadowLine);
    ShadowLine * line = m_lines.back().get();
    line->a = a;
    line->b = b;

    b2AABB ab;
    ab.lowerBound.x = std::min(a.x, b.x);
    ab.lowerBound.y = std::min(a.y, b.y);

    ab.upperBound.x = std::max(a.x, b.x);
    ab.upperBound.y = std::max(a.y, b.y);

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

class Query
{

public:

    bool QueryCallback(int id)
    {
        ids.push_back(id);
        return true;
    }

    std::vector<int> ids;
};

void ShadowWorld::update()
{
    for (const std::unique_ptr<Light>& light : m_lights)
    {
        light->m_shadows.clear();

        const sf::Vector2f p = light->getPosition();
        const float rmul = 100.f * light->getRadius();

        b2AABB ab;
        ab.lowerBound.x = p.x - light->getRadius();
        ab.lowerBound.y = p.y - light->getRadius();
        ab.upperBound.x = p.x + light->getRadius();
        ab.upperBound.y = p.y + light->getRadius();
        Query qr;
        m_tree.Query(&qr, ab);

        for (const int id : qr.ids)
        {
            const ShadowLine& line = *static_cast<ShadowLine*> (m_tree.GetUserData(id));

            const sf::Vector2f ad(setLength(line.a - p, rmul));
            const sf::Vector2f bd(setLength(line.b - p, rmul));

            std::vector<sf::Vector2f> sh;
            sh.push_back(line.a);
            sh.push_back(line.b);
            sh.push_back(p + bd);
            sh.push_back(p + ad);

            light->m_shadows.push_back(sh);
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

}
