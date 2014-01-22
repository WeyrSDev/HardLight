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
#include <Box2D/Collision/b2DynamicTree.h>

namespace ee {

class ShadowWorld
{

public:
    void addShadowCaster(sf::Vector2f * p, unsigned len);

    Light * addLight(sf::Vector2f p, float r);
    void removeLight(Light * ptr); //add ref overload too?

    void update();


private:
    typedef std::vector<sf::Vector2f> PolyData;

    void beginLight(Light * light);

    void collectPolys();
    void collectAngles();
    bool inWall();
    void buildShadows();
    void endLight(Light * light);


    bool m_inwall;

    class CPoly
    {

    public:
        const PolyData * vert;
        unsigned i1, i2;
    };

    std::vector<CPoly> m_collected;
    
    Light * m_clight;

    
    b2DynamicTree m_tree;
    
public://delme
    //private:
    std::vector<std::unique_ptr<ShadowCaster>> m_polys;

    std::vector<std::unique_ptr<Light> > m_lights;

};

}

#endif	/* SHADOWWORLD_HPP */

