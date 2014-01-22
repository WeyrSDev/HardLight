/* 
 * File:   ShadowCaster.hpp
 * Author: frex
 *
 * Created on January 18, 2014, 10:08 AM
 */

#ifndef SHADOWCASTER_HPP
#define	SHADOWCASTER_HPP

#include <vector>
#include <SFML/System/Vector2.hpp>

namespace ee {

class ShadowCaster
{
    friend class ShadowWorld;
public:

//private:
    std::vector<sf::Vector2f> m_vertices;
    int m_quadnode;
};

}

#endif	/* SHADOWCASTER_HPP */

