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

    bool inWall()const;
private:

    sf::Vector2f m_pos;
    float m_radius;
    sf::Color m_color;
    float m_angle;
    float m_spread;
    bool m_inwall;

public://delme
    
    std::vector<std::vector<sf::Vector2f >> m_shadows;

};

}

#endif	/* LIGHT_HPP */

