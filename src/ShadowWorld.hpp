/* 
 * File:   ShadowWorld.hpp
 * Author: frex
 *
 * Created on January 18, 2014, 8:28 AM
 */

#ifndef SHADOWWORLD_HPP
#define	SHADOWWORLD_HPP

#include "Light.hpp"
#include <vector>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "PI.hpp"

namespace ee {

class ShadowWorld
{

public:
    void addShadowCaster(sf::Vector2f * p, unsigned len);

    Light * addLight(sf::Vector2f p, float r);
    void removeLight(Light * ptr); //add ref overload too?

    void update();


private:
    typedef std::vector<sf::Vector2f> Polygon;
    
    void beginLight(Light * l);
    
    void calcMinMax(Light * l,const Polygon& p);
    
    bool insidePoly();
    
    
    
    void endLight(Light * l);
    
    std::vector<sf::Vector2f> m_minmax;
    
public://delme
    //private:
    std::vector<std::vector<sf::Vector2f> > m_polys;

    std::vector<std::unique_ptr<Light> > m_lights;

};

}

#endif	/* SHADOWWORLD_HPP */

