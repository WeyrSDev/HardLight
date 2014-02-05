#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Shader.hpp>
#include "LightPainter.hpp"
#include "ShadowWorld.hpp"
#include "DebugGeometryPainter.hpp"

namespace hlt {

void LightPainter::setSize(unsigned x, unsigned y)
{
    m_sumtex.create(x, y);
}

void drawBlendedLight(sf::RenderTarget& t, sf::Shader * s, sf::Vector2f m,
float radius, const sf::Vector2f * p, unsigned len, sf::Color c)
{
    sf::VertexArray arr(sf::TrianglesFan, len + 2u);
    arr[0].position = m;
    arr[0].color = c;

    for (unsigned i = 0; i < len; ++i)
    {
        arr[i + 1u].position = p[i];
        arr[i + 1u].color = c;
        arr[i + 1u].texCoords = p[i] - m;
    }

    arr[len + 1u] = arr[1];

    sf::RenderStates states;
    states.blendMode = sf::BlendAdd;
    states.shader = s;

    if (s) s->setParameter("radius", radius);

    t.draw(arr, states);
}

void LightPainter::render(ShadowWorld& w)
{
    m_sumtex.clear(m_ambient);
    m_sumtex.setView(sf::View(w.getViewRect()));

    for (int i = 0; i < w.getQueriedLightsCount(); ++i)
    {
        Light * l = w.getQueriedLight(i);
        const auto& poly = l->getBakedLight();
        sf::Shader * frag = m_fragenabled?&m_frag:nullptr;
        drawBlendedLight(m_sumtex, frag, l->getPosition(), l->getRadius(), poly.data(), poly.size(), l->getColor());
    }//for light

    m_sumtex.display();
}

bool LightPainter::enableFragFromFile(const std::string& fn)
{
    m_fragenabled = m_frag.loadFromFile(fn, sf::Shader::Fragment);
    return isFragEnabled();
}

void LightPainter::setAmbient(sf::Color ambient)
{
    m_ambient = ambient;
}

sf::Color LightPainter::getAmbient() const
{
    return m_ambient;
}


}
