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
#include "DebugGeometryPainter.hpp"
#include "b2DynamicTree.h"
#include "ShadowLinesBuffer.hpp"

namespace ee {

class ShadowWorld
{

public:

    //lights:
    Light * addLight(sf::Vector2f p, float r);//change this to good init struct with named param idiom?
    void removeLight(Light * ptr); //add ref overload too?
    unsigned getLightCount()const;
    Light * getLight(unsigned i)const;
    
    //lines:
    void addLine(sf::Vector2f a, sf::Vector2f b);
    void addLines(const sf::Vector2f * v, unsigned len);
    void addLinesStrip(const sf::Vector2f * v, unsigned len);
    //remove line

    //handling:
    void update();

    
    
    

    
    //private:
    
    //bool queryLight(int id);
    bool queryLine(int id);
    
    std::vector<ShadowLine> m_queriedlines;



    std::vector<std::unique_ptr<Light> > m_lights;
    //array for indices here, sorted maybe?
    b2DynamicTree m_lighttree;

    ShadowLinesBuffer m_linebuff;
    b2DynamicTree m_linetree;
    
    
};

}

#endif	/* SHADOWWORLD_HPP */

