/* 
 * File:   LightPainter.hpp
 * Author: frex
 *
 * Created on January 18, 2014, 9:37 PM
 */

#ifndef LIGHTPAINTER_HPP
#define	LIGHTPAINTER_HPP

#include <SFML/Graphics/RenderTexture.hpp>

namespace hlt {

class ShadowWorld;

class LightPainter
{

public:
    void setSize(unsigned x, unsigned y);
    void render(ShadowWorld& w);

    const sf::Texture& getCanvas()const;
    bool isFragEnabled()const;
    void reenableFrag(bool enabled);

    bool enableFragFromFile(const std::string& fn);

    void setAmbient(sf::Color ambient);
    sf::Color getAmbient() const;

private:
    sf::RenderTexture m_sumtex;

    sf::Shader m_frag;
    bool m_fragenabled;
    sf::Color m_ambient;
};

inline const sf::Texture& LightPainter::getCanvas() const
{
    return m_sumtex.getTexture();
}

inline void LightPainter::reenableFrag(bool enabled)
{
    m_fragenabled = enabled;
}

inline bool LightPainter::isFragEnabled() const
{
    return m_fragenabled;
}

}

#endif	/* LIGHTPAINTER_HPP */

