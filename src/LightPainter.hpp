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

    const sf::Texture& getCanvas()const;
    
private:
    sf::RenderTexture m_sumtex;
    
    sf::Shader m_frag;
};

inline const sf::Texture& LightPainter::getCanvas() const
{
    return m_sumtex.getTexture();
}

}

#endif	/* LIGHTPAINTER_HPP */

