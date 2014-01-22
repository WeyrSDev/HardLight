/* 
 * File:   DebugGeometryPainter.hpp
 * Author: frex
 *
 * Created on January 18, 2014, 7:37 AM
 */

#ifndef DEBUGGEOMETRYPAINTER_HPP
#define	DEBUGGEOMETRYPAINTER_HPP

#include <cassert>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include "PI.hpp"

namespace sf {
class RenderTarget;
}

namespace ee {

class DebugGeometryPainter
{

public:
    DebugGeometryPainter();
    DebugGeometryPainter(sf::RenderTarget& t);

    void segment(sf::Vector2f p1, sf::Vector2f p2, sf::Color c = sf::Color::White)const;
    void halfLine(sf::Vector2f p1, sf::Vector2f p2, sf::Color c = sf::Color::White)const;
    void line(sf::Vector2f p1, sf::Vector2f p2, sf::Color c = sf::Color::White)const;

    void polygonCenter(sf::Vector2f m, const sf::Vector2f * p, unsigned len, sf::Color c = sf::Color::White)const;

    void circle(sf::Vector2f p, float r, sf::Color c = sf::Color::White)const;
    void polygon(const sf::Vector2f * p, unsigned len, sf::Color c = sf::Color::White)const;
    void curve(const sf::Vector2f * p, unsigned len, unsigned s, unsigned e, sf::Color c = sf::Color::White, bool loop = false)const;
    void circleOutline(sf::Vector2f p, float r, sf::Color c = sf::Color::White)const;
    void softCircle(sf::Vector2f p, float r, sf::Color c = sf::Color::White, sf::Color o = sf::Color::Black, float angle = 0.f, float spread = pi2)const;

    void rectangle(float x1, float y1, float x2, float y2, sf::Color c = sf::Color::White)const;

    sf::RenderStates& getStates();
    sf::RenderStates getStates()const;

private:
    sf::RenderTarget& target()const;

    sf::RenderStates m_states;
    sf::RenderTarget * m_targ = nullptr;

};

inline sf::RenderTarget& DebugGeometryPainter::target()const
{
    assert(m_targ);
    return *m_targ;
}

}


#endif	/* DEBUGGEOMETRYPAINTER_HPP */

