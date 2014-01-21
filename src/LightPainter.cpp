#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <polyclipping/clipper.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "LightPainter.hpp"
#include "ShadowWorld.hpp"
#include "DebugGeometryPainter.hpp"

namespace ee {

namespace clip = ClipperLib;

void LightPainter::setSize(unsigned x, unsigned y)
{
    m_lighttex.create(x, y);
    m_sumtex.create(x, y);
}

void LightPainter::render(ShadowWorld& w)
{
    std::fprintf(stderr, "WARNING: naive 2x render tex impl used\n");
    m_sumtex.clear(sf::Color::Black);

    for (const std::unique_ptr<Light>& l : w.m_lights)
    {
        const bool b = false;

        if (b)
        {
            DebugGeometryPainter dgp(m_sumtex);
            dgp.getStates().blendMode = sf::BlendAdd;

            auto col = l->getColor();
            col.a = 0u;
            dgp.softCircle(l->getPosition(), l->getRadius(), l->getColor(), col, l->getAngle(), l->getSpread());

            //definitely context switching is the culprit
            //m_lighttex.clear();
            //m_lighttex.display();

        }
        else
        {
            m_lighttex.clear(sf::Color::Black);
            DebugGeometryPainter paint(m_lighttex);

            sf::Color edge = l->getColor();
            edge.a = 0u;
            paint.softCircle(l->getPosition(), l->getRadius(), l->getColor(), edge, l->getAngle(), l->getSpread());

            for (const std::vector<sf::Vector2f>& s : l->m_shadows)
            {
                //                paint.polygon(s.data(), s.size(), sf::Color::Black);

                DebugGeometryPainter p2(m_sumtex);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    paint.polygon(s.data(), s.size(), sf::Color::Blue);
                }
            }

            m_lighttex.display();
            m_sumtex.draw(sf::Sprite(m_lighttex.getTexture()), sf::BlendAdd);
        }
    }//for each light

    m_sumtex.display();
}

namespace {

const float scale = 1000.f * 1000.f;

clip::IntPoint point(sf::Vector2f v)
{
    return clip::IntPoint(v.x*scale, v.y * scale);
}

sf::Vector2f point(clip::IntPoint v)
{
    return sf::Vector2f(v.X / scale, v.Y / scale);
}

clip::Polygon translate(const std::vector<sf::Vector2f>& in)
{
    clip::Polygon out;

    for (const auto v : in)
    {
        out.push_back(point(v));
    }

    return out;
}

std::vector<sf::Vector2f> translate(const clip::Polygon& in)
{
    std::vector<sf::Vector2f> out;

    for (const auto v : in)
    {
        out.push_back(point(v));
    }

    return out;
}

clip::Polygon circl(sf::Vector2f v, float r)
{
    std::vector<sf::Vector2f> mid;

    for (int i = 0; i < 30; ++i)
    {
        const float arg = pi2 * i / 30.f;
        mid.push_back(v + r * sf::Vector2f(std::cos(arg), -std::sin(arg)));
    }

    return translate(mid);
}

}

void LightPainter::renderViaClipper(ShadowWorld& w)
{
    m_sumtex.clear();

    const bool dbug = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

    //    static_assert(false, "readme");
    //something is wrong with overlapping shadows in else branch
    //check it out
    //possibly it's PolygonFillType issue?

    if (dbug)
    {
        //something is wrong here, shadows are right but clip result is not?

        DebugGeometryPainter paint(m_sumtex);
        paint.getStates().blendMode = sf::BlendAdd;

        clip::Clipper clip;
        //    clip.AddPath(,,);
        for (const std::unique_ptr<Light>& l : w.m_lights)
        {
            clip::Polygons out(1);
            out[0] = circl(l->getPosition(), l->getRadius());
            clip.Clear();

            int count = 0;
            const sf::Color c[3] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};

            for (const auto& v : l->m_shadows)
            {
                if (out.size() > 0u)
                {
                    clip.Clear();
                    clip.AddPolygons(out, clip::ptSubject);
                    clip.AddPolygon(translate(v), clip::ptClip);
                    clip.Execute(clip::ctDifference, out);
                }
                if (out.size() > 0u && count < 3 && sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    auto poly = translate(out[0]);
                    paint.polygon(poly.data(), poly.size(), c[count]);
                }
                ++count;
            }//for poly

            light = translate(out[0]);

            if (out.size() > 0u)
            {
                //            clip::CleanPolygon(out[0]);
                auto poly = translate(out[0]);
                paint.polygonCenter(l->getPosition(), poly.data(), poly.size());
            }
        }//for light
    }
    else//test new approach
    {
        //something is wrong here, shadows are right but clip result is not?

        DebugGeometryPainter paint(m_sumtex);
        paint.getStates().blendMode = sf::BlendAdd;

        clip::Clipper clip;
        //    clip.AddPath(,,);
        for (const std::unique_ptr<Light>& l : w.m_lights)
        {
            clip.Clear();

            for (const auto& v : l->m_shadows)
            {
                clip.AddPolygon(translate(v), clip::ptClip);
            }//for poly

            clip.AddPolygon(circl(l->getPosition(), l->getRadius()), clip::ptSubject);


            clip::PolyFillType pft;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            {
                pft = clip::pftEvenOdd;
            }
            else
            {
                pft = clip::pftNonZero;
            }

            clip::Polygons out;
            clip.Execute(clip::ctDifference, out, pft, pft);

            if (out.size() > 0u)
            {
                const auto poly = translate(out[0]);
                paint.polygonCenter(l->getPosition(), poly.data(), poly.size());
            }
        }//for light
    }//end if

    m_sumtex.display();
}

//m_sumtex.clear();
//
////something is wrong here, shadows are right but clip result is not?
//
//DebugGeometryPainter paint(m_sumtex);
//paint.getStates().blendMode = sf::BlendAdd;
//
//clip::Clipper clip;
////    clip.AddPath(,,);
//for (const std::unique_ptr<Light>& l : w.m_lights)
//{
//    clip.Clear();
//
//    for (const auto& v : l->m_shadows)
//    {
//        clip.AddPolygon(translate(v), clip::ptClip);
//    }//for poly
//
//    clip.AddPolygon(circl(l->getPosition(), l->getRadius()), clip::ptSubject);
//
//    clip::Polygons out;
//    clip.Execute(clip::ctDifference, out);
//
//    if (out.size() > 0u)
//    {
//        const auto poly = translate(out[0]);
//        paint.polygonCenter(l->getPosition(), poly.data(), poly.size());
//    }
//}//for light
//
//m_sumtex.display();

}
