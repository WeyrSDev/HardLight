#include "ShadowWorld.hpp"
#include <cmath>
#include <SFML/Window/Keyboard.hpp>

namespace ee {

namespace {

float getAngle(sf::Vector2f v)
{
    float tn = std::atan2(-v.y, v.x);
    if (tn < 0.f) tn += 2.f * pi;
    return tn;
    //    return (180.f / (pi))*(tn);
}

float getAngle2(sf::Vector2f v, float ref)
{
    return std::fmod(pi2 - ref + getAngle(v), pi2);
}

sf::Vector2f getMid(const std::vector<sf::Vector2f>& poly)
{
    sf::Vector2f ret;
    for (const auto v : poly) ret += v;
    return ret / static_cast<float> (poly.size()+(!poly.size())); //at least 1, no div 0
}

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

void ShadowWorld::addShadowCaster(sf::Vector2f * p, unsigned len)
{

}

void ShadowWorld::update()
{
    for (const std::unique_ptr<Light>& light : m_lights)
    {
        light->m_shadows.clear();

        for (const std::vector<sf::Vector2f>& poly : m_polys)
        {
            const sf::Vector2f mid = getMid(poly);

            const float ref = getAngle2(mid - light->m_pos, pi);

            float min = getAngle2(poly[0] - light->m_pos, ref);
            float max = min;
            unsigned i1 = 0u, i2 = 0u;

            refangle = ref;

            for (int i = 0; i < poly.size(); ++i)
            {
                const float b = getAngle2(poly[i] - light -> m_pos, ref);
                const auto v = poly[i] - light->m_pos;

                if (min > b)
                {
                    i1 = i;
                    min = b;
                }

                if (max < b)
                {
                    i2 = i;
                    max = b;
                }
            }

            //making shadow for that occluder:
            light->m_shadows.push_back(std::vector<sf::Vector2f>());

            std::vector<sf::Vector2f>& shadow = light->m_shadows.back();

            shadow.push_back(poly[i1]);

            unsigned i = i1;

            while (i != i2)
            {
                i = (i + 1u) % poly.size();
                shadow.push_back(poly[i]);
            }

            const sf::Vector2f z1 = poly[i1];
            const sf::Vector2f z2 = poly[i2];

            //rmul must be large enough not to cause too short triangle to be
            //created but it also must be
            // small enough to not cause floating precision loss(?)
            const float rmul = 1000.f;

            const sf::Vector2f fly1 = setLength(poly[i1] - light->m_pos, rmul * light->m_radius);
            const sf::Vector2f fly2 = setLength(poly[i2] - light->m_pos, rmul * light->m_radius);

            shadow.push_back(poly[i2] + fly2);
            shadow.push_back(poly[i1] + fly1);


        }//for poly
    }//for light

    //make 'shadow'

}


}
