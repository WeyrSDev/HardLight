/* 
 * File:   LightPainter.hpp
 * Author: frex
 *
 * Created on January 18, 2014, 9:37 PM
 */

#ifndef LIGHTPAINTER_HPP
#define	LIGHTPAINTER_HPP

#include <SFML/Graphics/RenderTexture.hpp>

namespace ee {

class ShadowWorld;

class LightPainter
{

public:
    void setSize(unsigned x, unsigned y);
    void render(ShadowWorld& w);
    void renderViaClipper(ShadowWorld& w);


    //private:
    sf::RenderTexture m_lighttex;
    sf::RenderTexture m_sumtex;
    
    std::vector<sf::Vector2f> light;

};

}

#endif	/* LIGHTPAINTER_HPP */

