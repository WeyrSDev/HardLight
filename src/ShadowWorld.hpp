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
    Light * addLight(sf::Vector2f p, float r); //change this to good init struct with named param idiom?
    void removeLight(Light * ptr); //add ref overload too?
    void removeAllLights();
    unsigned getLightsCount()const;
    Light * getLight(unsigned i)const;
    unsigned getQueriedLightsCount()const;
    Light * getQueriedLight(unsigned i)const;


    //lines:
    void addLine(sf::Vector2f a, sf::Vector2f b);
    void addLines(const sf::Vector2f * v, unsigned len);
    void addLinesStrip(const sf::Vector2f * v, unsigned len);
    //void removeLine(int lineid);
    void removeAllLines();
    unsigned getLinesCount()const;
    const ShadowLine& getLineById(int id)const;
    unsigned getQueriedLinesCount()const;
    const ShadowLine& getQueriedLine(unsigned i)const;


    //handling:
    void update();
    void setViewRect(sf::FloatRect rect);
    sf::FloatRect getViewRect()const;




private:

    void queryLights(const b2AABB& ab);
    bool queryLineCallback(int id);

    ShadowLinesBuffer m_linebuff;
    b2DynamicTree m_linetree;
    std::vector<ShadowLine> m_queriedlines;

    std::vector<std::unique_ptr<Light> > m_lights;
    std::vector<Light*> m_queriedlights;

    b2AABB m_viewrect;

};

}

#endif	/* SHADOWWORLD_HPP */

