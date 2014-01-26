/* 
 * File:   Light.hpp
 * Author: frex
 *
 * Created on January 18, 2014, 8:38 AM
 */

#ifndef LIGHT_HPP
#define	LIGHT_HPP

#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace ee {

class ShadowWorld;

class LightDef
{

public:
    LightDef();

    sf::Vector2f Position;
    float Radius;
    sf::Color Color;
    float Angle;
    float Spread;
};

class Shadow
{

public:
    sf::Vector2f vertices[4];
};

class Light
{

    friend class ShadowWorld;
public:

    sf::Vector2f getPosition()const;
    void setPosition(sf::Vector2f p);

    float getRadius()const;
    void setRadius(float r);

    sf::Color getColor()const;
    void setColor(sf::Color c);

    float getAngle()const;
    void setAngle(float a);

    float getSpread()const;
    void setSpread(float s);

    void markDirty();
    void syncWithDef(const LightDef& ld);

    //helpers to not dirty without need(normal setters always do) these compare
    //first, set later, no color setter - setting color doesnt dirty the light
    void setPositionClean(sf::Vector2f p);
    void setRadiusClean(float r);
    void setAngleClean(float a);
    void setSpreadClean(float s);

    const std::vector<sf::Vector2f>& getBakedLight()const;

    unsigned getShadowsCount()const;
    const Shadow& getShadow(unsigned i)const;

    void remove();

private:

    sf::Vector2f m_pos;
    float m_radius;
    sf::Color m_color;
    float m_angle;
    float m_spread;

    bool m_dirty;
    bool m_in;

    ShadowWorld * m_owner;

    std::vector<Shadow> m_shadows;
    std::vector<sf::Vector2f> m_cached;

};

inline void Light::setPositionClean(sf::Vector2f p)
{
    if (m_pos == p) return;
    setPosition(p);
}

inline void Light::setRadiusClean(float r)
{
    if (m_radius == r) return;
    setRadius(r);
}

inline void Light::setAngleClean(float a)
{
    if (m_angle == a) return;
    setAngle(a);
}

inline void Light::setSpreadClean(float s)
{
    if (m_spread == s) return;
    setSpread(s);
}


}

#endif	/* LIGHT_HPP */

