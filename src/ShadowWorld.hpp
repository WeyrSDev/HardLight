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
#include "DynamicTree.hpp"
#include "ShadowLinesBuffer.hpp"

namespace ee {

class LightDef;

class ShadowWorld
{

public:

    //lights:
    Light * addLight(const LightDef& ld = LightDef());
    void removeLight(Light * ptr);
    void removeAllLights();
    unsigned getLightsCount()const;
    Light * getLight(unsigned i)const;
    unsigned getQueriedLightsCount()const;
    Light * getQueriedLight(unsigned i)const;


    //lines:
    int addLine(sf::Vector2f a, sf::Vector2f b);
    void addLines(const sf::Vector2f * v, unsigned len, int * ids = nullptr);
    void addLinesStrip(const sf::Vector2f * v, unsigned len, int * ids = nullptr);
    void removeLine(int lineid);
    void removeLines(int * ids, unsigned len);
    void removeAllLines();
    unsigned getLinesCount()const;
    const ShadowLine& getLineById(int id)const;
    unsigned getQueriedLinesCount()const;
    const ShadowLine& getQueriedLine(unsigned i)const;


    //handling:
    void update();
    void rebuildLineTree();
    void setViewRect(sf::FloatRect rect);
    sf::FloatRect getViewRect()const;




private:

    void queryLights(const AABB& ab);

    bool queryLineCallback(int id);

    int putLineIntoBuffer(sf::Vector2f a, sf::Vector2f b, const AABB& ab);

    void dirtyLights(const AABB& ab);

    ShadowLinesBuffer m_linebuff;
    DynamicTree m_linetree;
    std::vector<ShadowLine> m_queriedlines;

    std::vector<std::unique_ptr<Light> > m_lights;
    std::vector<Light*> m_queriedlights;

    AABB m_viewrect;

};

}

#endif	/* SHADOWWORLD_HPP */

