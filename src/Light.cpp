#include "Light.hpp"

namespace ee {

sf::Vector2f Light::getPosition() const
{
    return m_pos;
}

void Light::setPosition(sf::Vector2f p)
{
    m_pos=p;
}

float Light::getRadius() const
{
    return m_radius;
}

void Light::setRadius(float r)
{
    m_radius=r;
}

sf::Color Light::getColor() const
{
    return m_color;
}

void Light::setColor(sf::Color c)
{
    m_color=c;
}

float Light::getAngle() const
{
    return m_angle;
}

void Light::setAngle(float a)
{
    m_angle=a;
}

float Light::getSpread() const
{
    return m_spread;
}

void Light::setSpread(float s)
{
    m_spread=s;
}

}
