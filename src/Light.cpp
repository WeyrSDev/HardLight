#include "Light.hpp"
#include <cassert>

namespace ee {

sf::Vector2f Light::getPosition() const
{
    return m_pos;
}

void Light::setPosition(sf::Vector2f p)
{
    m_pos = p;
    markDirty();
}

float Light::getRadius() const
{
    return m_radius;
}

void Light::setRadius(float r)
{
    m_radius = r;
    markDirty();
}

sf::Color Light::getColor() const
{
    return m_color;
}

void Light::setColor(sf::Color c)
{
    m_color = c;
}

float Light::getAngle() const
{
    return m_angle;
}

void Light::setAngle(float a)
{
    m_angle = a;
    markDirty();
}

float Light::getSpread() const
{
    return m_spread;
}

void Light::setSpread(float s)
{
    m_spread = s;
    markDirty();
}

void Light::markDirty()
{
    m_dirty = true;
}

const std::vector<sf::Vector2f>& Light::getBakedLight() const
{
    return m_cached;
}

unsigned Light::getShadowsCount() const
{
    return m_shadows.size();
}

const Shadow& Light::getShadow(unsigned i) const
{
    assert(i < m_shadows.size());
    return m_shadows[i];
}

}
