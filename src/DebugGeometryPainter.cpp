#include "DebugGeometryPainter.hpp"
#include <SFML/Graphics.hpp>

namespace hlt {

DebugGeometryPainter::DebugGeometryPainter()
{
    //default
}

DebugGeometryPainter::DebugGeometryPainter(sf::RenderTarget& t) : m_targ(&t)
{

}

void DebugGeometryPainter::segment(sf::Vector2f p1, sf::Vector2f p2, sf::Color c) const
{
    sf::Vertex ver[2];
    ver[0].color = ver[1].color = c;
    ver[0].position = p1;
    ver[1].position = p2;
    ver[0].color.a = ver[1].color.a = 127u;
    target().draw(ver, 2u, sf::Lines, m_states);
}

void DebugGeometryPainter::halfLine(sf::Vector2f p1, sf::Vector2f p2, sf::Color c) const
{
    const sf::Vector2f d(p2 - p1);
    segment(p1, p2 + 10000.f * d, c);
}

void DebugGeometryPainter::line(sf::Vector2f p1, sf::Vector2f p2, sf::Color c) const
{
    const sf::Vector2f d(p2 - p1);
    segment(p1 - 10000.f * d, p2 + 10000.f * d, c);
}

void DebugGeometryPainter::polygonCenter(sf::Vector2f m, const sf::Vector2f* p, unsigned len, sf::Color c) const
{
    sf::VertexArray arr(sf::TrianglesFan, len + 2u);
    arr[0].position = m;
    arr[0].color = c;

    for (unsigned i = 0; i < len; ++i)
    {
        arr[i + 1u].position = p[i];
        arr[i + 1u].color = c;
    }

    arr[len + 1u] = arr[1];
    target().draw(arr, m_states);
}

void DebugGeometryPainter::circle(sf::Vector2f p, float r, sf::Color c)const
{
    sf::CircleShape sha;
    sha.setRadius(r);
    sha.setOrigin(r, r);
    sha.setPosition(p);
    sha.setFillColor(c);
    target().draw(sha, m_states);
}

void DebugGeometryPainter::polygon(const sf::Vector2f * p, unsigned len, sf::Color c) const
{
    //    sf::Vector2f avg;
    //
    //    for (unsigned i = 0; i < len; ++i) avg += p[i];
    //
    //    avg /= static_cast<float> (len);
    //
    //    sf::VertexArray arr(sf::TrianglesFan, len + 2u);
    //    arr[0].position = avg;
    //    arr[0].color = c;
    //
    //    for (unsigned i = 0; i < len; ++i)
    //    {
    //        arr[i + 1u].position = p[i];
    //        arr[i + 1u].color = c;
    //    }
    //
    //    arr[len + 1u] = arr[1];
    //
    //
    //    target().draw(arr, m_states);

    sf::ConvexShape sha;
    sha.setPointCount(len);

    for (unsigned i = 0; i < len; ++i) sha.setPoint(i, p[i]);

    sha.setFillColor(c);
    target().draw(sha, m_states);
}

void DebugGeometryPainter::curve(const sf::Vector2f * p, unsigned len, unsigned s, unsigned e, sf::Color c, bool loop) const
{
    if (s >= len || e >= len || len == 0 || s == e) return;
    sf::VertexArray arr(sf::LinesStrip);
    arr.append(sf::Vertex(p[s], c));

    while (s != e)
    {
        s = (s + 1u) % len;
        arr.append(sf::Vertex(p[s], c));
    }

    if (loop) arr.append(arr[0]);

    target().draw(arr, m_states);
}

void DebugGeometryPainter::circleOutline(sf::Vector2f p, float r, sf::Color c) const
{
    sf::CircleShape sha;
    sha.setRadius(r);
    sha.setOrigin(r, r);
    sha.setPosition(p);
    sha.setOutlineColor(c);
    sha.setOutlineThickness(-1.f);
    sha.setFillColor(sf::Color::Transparent);
    target().draw(sha, m_states);
}

void DebugGeometryPainter::softCircle(sf::Vector2f p, float r, sf::Color c, sf::Color o, float angle, float spread) const
{
    sf::VertexArray arr(sf::TrianglesFan, 31u);
    arr[0] = p;
    arr[0].color = c;

    const float from = angle - spread / 2.f;

    for (int i = 0; i < 30; ++i)
    {
        const float arg = from + spread * i / 29.f;
        arr[i + 1].position = p + r * sf::Vector2f(std::cos(arg), -std::sin(arg));
        arr[i + 1].color = o;
    }

    target().draw(arr, m_states);
}

void DebugGeometryPainter::rectangle(float x1, float y1, float x2, float y2, sf::Color c) const
{
    sf::RectangleShape sha;
    sha.setPosition(x1, y1);
    sha.setSize(sf::Vector2f(x2 - x1, y2 - y1));
    sha.setFillColor(c);
    target().draw(sha, m_states);
}

sf::RenderStates& DebugGeometryPainter::getStates()
{
    return m_states;
}

sf::RenderStates DebugGeometryPainter::getStates() const
{
    return m_states;
}

}

