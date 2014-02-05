#include "ShadowWorld.hpp"
#include <cmath>
#include <polyclipping/clipper.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace hlt {

namespace clip = ClipperLib;

namespace {

const float scale = 1000.f * 1000.f;

clip::IntPoint point(sf::Vector2f v)
{
    return clip::IntPoint(v.x * scale, v.y * scale);
}

sf::Vector2f point(clip::IntPoint v)
{
    return sf::Vector2f(v.X / scale, v.Y / scale);
}

clip::Path translate(const std::vector<sf::Vector2f>& in)
{
    clip::Path out;

    for (const auto v : in)
    {
        out.push_back(point(v));
    }

    return out;
}

clip::Path translate(const Shadow& sh)
{
    clip::Path out;

    for (unsigned i = 0u; i < 4u; ++i) out.push_back(point(sh.vertices[i]));

    return out;
}

std::vector<sf::Vector2f> translate(const clip::Path& in)
{
    std::vector<sf::Vector2f> out;

    for (const auto v : in)
    {
        out.push_back(point(v));
    }

    return out;
}

std::vector<sf::Vector2f> circlS(sf::Vector2f v, float r, float angle, float spread)
{
    std::vector<sf::Vector2f> ret;

    if (spread + 1.f / 30.f * hlt::pi2 < hlt::pi2) ret.push_back(v); //fix for cones

    for (int i = 0; i < 30; ++i)
    {
        const float arg = angle - spread / 2.f + spread * i / 29.f;
        ret.push_back(v + r * sf::Vector2f(std::cos(arg), std::sin(arg)));
    }

    return ret;
}

inline clip::Path circlP(sf::Vector2f v, float r, float angle, float spread)
{
    return translate(circlS(v, r, angle, spread));
}

sf::Vector2f setLength(sf::Vector2f v, float l)
{
    const float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len == 0.f) return v;

    return l / len * v;
}

float lengthSquared(sf::Vector2f v)
{
    return v.x * v.x + v.y * v.y;
}

AABB getAABB(sf::Vector2f a, sf::Vector2f b)
{
    if (a.x == b.x) b.x += 1.f;
    if (a.y == b.y) b.y += 1.f;

    AABB ab;
    ab.lowerBound.Set(std::min(a.x, b.x), std::min(a.y, b.y));
    ab.upperBound.Set(std::max(a.x, b.x), std::max(a.y, b.y));
    return ab;
}

AABB getAABB(Light * light)
{
    const auto p = light->getPosition();
    const float r = light->getRadius();

    AABB ab;
    ab.lowerBound.Set(p.x - r, p.y - r);
    ab.upperBound.Set(p.x + r, p.y + r);
    return ab;
}

AABB get0AABB()
{
    AABB ab;
    ab.lowerBound.SetZero();
    ab.upperBound.SetZero();
    return ab;
}

sf::FloatRect toSF(const AABB& b)
{
    const auto l = b.lowerBound;
    const auto u = b.upperBound;
    return sf::FloatRect(l.x, l.y, u.x - l.x, u.y - l.y);
}

bool intersects(const AABB& a, const AABB& b)
{
    return toSF(a).intersects(toSF(b));
}

}

Light * ShadowWorld::addLight(const LightDef& ld)
{
    m_lights.emplace_back(new Light);
    Light * light = m_lights.back().get();
    light->syncWithDef(ld);
    light->m_owner = this;
    return light;
}

void ShadowWorld::removeLight(Light * ptr)
{
    for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
    {
        if (it->get() == ptr)
        {
            m_lights.erase(it);
            return;
        }
    }
}

void ShadowWorld::removeAllLights()
{
    m_lights.clear();
}

int ShadowWorld::putLineIntoBuffer(sf::Vector2f a, sf::Vector2f b, const AABB& ab)
{
    const int treeid = m_linetree.CreateProxy(ab, 0);
    const int lineid = m_linebuff.addLine(a, b, treeid);
    m_linetree.SetStoredValue(treeid, lineid);
    return lineid;
}

int ShadowWorld::addLine(sf::Vector2f a, sf::Vector2f b)
{
    const AABB dirty(getAABB(a, b));
    dirtyLights(dirty);
    return putLineIntoBuffer(a, b, dirty);
}

void ShadowWorld::addLines(const sf::Vector2f * v, unsigned len, int * ids)
{
    AABB dirty(get0AABB());
    const unsigned count = len / 2u;

    for (unsigned i = 0u; i < count; ++i)
    {
        const AABB ab(getAABB(v[2 * i], v[2 * i + 1u]));
        dirty.Combine(ab);
        const int lineid = putLineIntoBuffer(v[2 * i], v[2 * i + 1u], ab);
        if (ids) ids[i] = lineid;
    }

    dirtyLights(dirty);
}

void ShadowWorld::addLinesStrip(const sf::Vector2f * v, unsigned len, int * ids)
{
    AABB dirty(get0AABB());

    for (unsigned i = 0u; i < (len - 1u); ++i)
    {
        const AABB ab(getAABB(v[i], v[i + 1u]));
        dirty.Combine(ab);
        const int lineid = putLineIntoBuffer(v[i], v[i + 1u], ab);
        if (ids) ids[i] = lineid;
    }

    dirtyLights(dirty);
}

void ShadowWorld::removeLine(int lineid)
{
    const int treeid = m_linebuff.getLine(lineid).getTreeId();
    m_linebuff.removeLine(lineid);

    const AABB ab = m_linetree.GetFatAABB(treeid);
    m_linetree.DestroyProxy(treeid);

    dirtyLights(ab);
}

void ShadowWorld::removeLines(int * ids, unsigned len)
{
    AABB ab(get0AABB());

    for (unsigned i = 0u; i < len; ++i)
    {
        const int treeid = m_linebuff.getLine(ids[i]).getTreeId();
        m_linebuff.removeLine(ids[i]);

        ab.Combine(m_linetree.GetFatAABB(treeid));
        m_linetree.DestroyProxy(treeid);
    }

    dirtyLights(ab);
}

void ShadowWorld::dirtyLights(const AABB& ab)
{
    std::vector<Light*> swp;
    swp.swap(m_queriedlights);
    queryLights(ab);

    for (Light * light : m_queriedlights) light->markDirty();

    swp.swap(m_queriedlights);
}

void ShadowWorld::update()
{
    m_queriedlines.clear();
    m_linetree.Query(this, &ShadowWorld::queryLineCallback, m_viewrect);

    queryLights(m_viewrect);

    for (Light * light : m_queriedlights)//iter over selected lights
    {
        const sf::Vector2f p = light->getPosition();
        const float rmul = 100.f * light->getRadius();

        if (light->m_dirty)
        {
            light->m_cached.clear();
            light->m_shadows.clear();
            bool touchedline = false;

            for (const ShadowLine& line : m_queriedlines)
            {
                const sf::Vector2f ad(line.a - p);
                const sf::Vector2f bd(line.b - p);

                if (!intersects(getAABB(light), m_linetree.GetFatAABB(line.getTreeId())))
                {
                    continue;
                }

                Shadow sh;
                sh.vertices[0] = line.a;
                sh.vertices[1] = line.b;
                sh.vertices[2] = p + setLength(bd, rmul);
                sh.vertices[3] = p + setLength(ad, rmul);
                light->m_shadows.push_back(sh);
                touchedline = true;
            }

            //don't perform all clipper conversions and all if light didn't touch a light
            if (!touchedline)
            {
                light->m_cached = circlS(light->getPosition(), light->getRadius(), light->getAngle(), light->getSpread());
                continue;
            }

            clip::Clipper clip;

            clip::Paths out(1u);
            out[0] = circlP(light->getPosition(), light->getRadius(), light->getAngle(), light->getSpread());

            if (clip::Orientation(out[0])) clip::ReversePath(out[0]);

            clip.AddPath(out[0], clip::ptSubject, true);

            for (const auto& v : light->m_shadows)
            {
                auto pl = translate(v);
                clip::CleanPolygon(pl, 0.0); //to avoid segfaults from same points
                if (clip::Orientation(pl)) clip::ReversePath(pl);
                clip.AddPath(pl, clip::ptClip, true);
            }//for poly

            clip.Execute(clip::ctDifference, out, clip::pftNonZero, clip::pftNonZero);

            if (out.size() > 0u) light->m_cached = translate(out[0]);

            light->m_dirty = false;
        }//if dirty light


    }//for light
}

void ShadowWorld::rebuildLineTree()
{
    m_linetree.RebuildBottomUp();
}

unsigned ShadowWorld::getLightsCount() const
{
    return m_lights.size();
}

Light * ShadowWorld::getLight(unsigned i) const
{
    assert(i < m_lights.size());
    return m_lights[i].get();
}

unsigned ShadowWorld::getQueriedLightsCount() const
{
    return m_queriedlights.size();
}

Light * ShadowWorld::getQueriedLight(unsigned i) const
{
    assert(i < m_queriedlights.size());
    return m_queriedlights[i];
}

void ShadowWorld::removeAllLines()
{
    m_linetree.ClearAll();
    m_linebuff.removeAll();

    //its easier to dirty all lights than query which actually needed that
    for (const std::unique_ptr<Light>& light : m_lights)
    {
        light->markDirty();
    }
}

unsigned ShadowWorld::getLinesCount() const
{
    return m_linebuff.getCount();
}

const ShadowLine& ShadowWorld::getLineById(int id) const
{
    return m_linebuff.getLine(id);
}

unsigned ShadowWorld::getQueriedLinesCount() const
{
    return m_queriedlines.size();
}

const ShadowLine& ShadowWorld::getQueriedLine(unsigned i) const
{
    assert(i < m_queriedlines.size());
    return m_queriedlines[i];
}

void ShadowWorld::setViewRect(sf::FloatRect rect)
{
    //should be OK for 'teleporting' lights out of screen zone but be careful
    m_viewrect.lowerBound.Set(rect.left, rect.top);
    m_viewrect.upperBound.Set(rect.left + rect.width, rect.top + rect.height);
}

sf::FloatRect ShadowWorld::getViewRect() const
{
    const auto l = m_viewrect.lowerBound;
    const auto u = m_viewrect.upperBound;
    return sf::FloatRect(l.x, l.y, u.x - l.x, u.y - l.y);
}

bool ShadowWorld::queryLineCallback(int id)
{
    m_queriedlines.push_back(m_linebuff.getLine(m_linetree.GetStoredValue(id)));
    return true;
}

void ShadowWorld::queryLights(const AABB& ab)
{
    m_queriedlights.clear();

    for (const std::unique_ptr<Light>& light : m_lights)
    {
        const auto p = light->getPosition();
        const float r = light->getRadius();
        AABB lab;
        lab.lowerBound.Set(p.x - r, p.y - r);
        lab.upperBound.Set(p.x + r, p.y + r);

        const bool lastin = light->m_in;
        light->m_in = intersects(ab, lab);

        if (lastin || light->m_in)
        {
            m_queriedlights.push_back(light.get());
        }
    }
}



}
