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
#include "ShadowCaster.hpp"
#include "DebugGeometryPainter.hpp"
#include <Box2D/Collision/b2DynamicTree.h>

namespace ee {

class Line
{

public:
    sf::Vector2f a, b;
};

class ShadowWorld
{

public:

    Light * addLight(sf::Vector2f p, float r);
    void removeLight(Light * ptr); //add ref overload too?

    void addLine(sf::Vector2f a, sf::Vector2f b);
    void addLines(const sf::Vector2f * v, unsigned len);
    void addLinesStrip(const sf::Vector2f * v, unsigned len);

    void update();

private:


public://delme
    //private:

    b2DynamicTree m_tree;

    std::vector<std::unique_ptr<Line> > m_lines;


    std::vector<std::unique_ptr<Light> > m_lights;

};

}

#endif	/* SHADOWWORLD_HPP */

