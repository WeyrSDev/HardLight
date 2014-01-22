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
    l->m_inwall = false;

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
    std::unique_ptr<ShadowCaster> sc(new ShadowCaster);

    sc->m_vertices.assign(p, p + len);

    b2AABB ab;

    for (unsigned i = 0u; i < len; ++i)
    {
        ab.lowerBound.x = std::min(ab.lowerBound.x, p[i].x);
        ab.upperBound.x = std::max(ab.upperBound.x, p[i].x);

        ab.lowerBound.y = std::min(ab.lowerBound.y, p[i].y);
        ab.upperBound.y = std::max(ab.upperBound.y, p[i].y);
    }

    sc->m_quadnode = m_tree.CreateProxy(ab, sc.get());

    m_polys.push_back(std::move(sc));
}

void ShadowWorld::update()
{
    for (const std::unique_ptr<Light>& light : m_lights)
    {
        beginLight(light.get());

        collectPolys();
        collectAngles();

        if (inWall())
        {
            light->m_inwall = true;
        }
        else
        {
            buildShadows();
        }
    }//for light

    //make 'shadow'

}

void ShadowWorld::beginLight(Light * light)
{
    m_clight = light;
    m_inwall = false;
    light->m_inwall = false;
    light->m_shadows.clear();
    m_collected.clear();
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

void ShadowWorld::collectPolys()
{
    b2AABB ab;

    ab.lowerBound.x = m_clight->getPosition().x - m_clight->getRadius();
    ab.lowerBound.y = m_clight->getPosition().y - m_clight->getRadius();

    ab.upperBound.x = m_clight->getPosition().x + m_clight->getRadius();
    ab.upperBound.y = m_clight->getPosition().y + m_clight->getRadius();

    Query qr;
    m_tree.Query(&qr, ab);

    for (const int id : qr.ids)
    {
        CPoly cp;
        auto sc = static_cast<ShadowCaster*> (m_tree.GetUserData(id));
        cp.vert = &sc->m_vertices;
        m_collected.push_back(cp);
    }
}

void ShadowWorld::collectAngles()
{
    for (CPoly& cp : m_collected)
    {
        const PolyData& poly = *cp.vert;

        const sf::Vector2f mid = getMid(poly);

        const float ref = getAngle2(mid - m_clight->m_pos, pi);

        float min = getAngle2(poly[0] - m_clight->m_pos, ref);
        float max = min;
        unsigned i1 = 0u, i2 = 0u;

        for (int i = 0; i < poly.size(); ++i)
        {
            const sf::Vector2f d(poly[i] - m_clight->getPosition());

            const float b = getAngle2(poly[i] - m_clight -> m_pos, ref);
            const auto v = poly[i] - m_clight->m_pos;

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

        cp.i1 = i1;
        cp.i2 = i2;

        if ((max - min) > pi) //is that right? -not right for very thin shapes
        {
            m_inwall = true; //light is in wall
            return;
        }
    }//for each collected poly
}

bool ShadowWorld::inWall()
{
    return m_inwall;
}

void ShadowWorld::buildShadows()
{
    for (const CPoly& cp : m_collected)
    {
        m_clight->m_shadows.push_back(PolyData());
        PolyData& shadow = m_clight->m_shadows.back();
        const PolyData& poly = *cp.vert;
        const unsigned i1 = cp.i1;
        const unsigned i2 = cp.i2;

        unsigned i = i1;
        shadow.push_back(poly[i]);

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

        const sf::Vector2f fly1 = setLength(poly[i1] - m_clight->m_pos, rmul * m_clight->m_radius);
        const sf::Vector2f fly2 = setLength(poly[i2] - m_clight->m_pos, rmul * m_clight->m_radius);

        shadow.push_back(poly[i2] + fly2);
        shadow.push_back(poly[i1] + fly1);

    }//for each collected poly
}

void ShadowWorld::endLight(Light * light)
{
    m_clight = nullptr;
}


}
