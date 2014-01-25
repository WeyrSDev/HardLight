#include "ShadowLinesBuffer.hpp"
#include <cassert>

namespace ee {

const int InvalidFreeIndex = -1;

int ShadowLine::getTreeId() const
{
    assert(m_id < InvalidFreeIndex);
    return -m_id - 2;
}

ShadowLinesBuffer::ShadowLinesBuffer() :
m_firstfree(InvalidFreeIndex)
{

}

int ShadowLinesBuffer::addLine(sf::Vector2f a, sf::Vector2f b, int treeid)
{
    const int line = getNewLineId();

    m_lines[line].a = a;
    m_lines[line].b = b;
    m_lines[line].m_id = -treeid - 2;

    return line;
}

int ShadowLinesBuffer::removeLine(int lineid)
{
    assert(0 <= lineid && lineid < m_lines.size() && m_lines[lineid].m_id < InvalidFreeIndex);
    const int treeid = -m_lines[lineid].m_id - 2;

    m_lines[lineid].m_id = m_firstfree;
    m_firstfree = lineid;

    return treeid;
}

void ShadowLinesBuffer::removeAll()
{
    m_firstfree = 0;

    for (int i = 0; i < m_lines.size()-1; ++i)
    {
        m_lines[i].m_id = i + 1;
    }

    m_lines[m_lines.size() - 1].m_id = InvalidFreeIndex;
}

const ShadowLine& ShadowLinesBuffer::getLine(int lineid) const
{
    assert(0 <= lineid && lineid < m_lines.size() && m_lines[lineid].m_id < InvalidFreeIndex);
    return m_lines[lineid];
}

int ShadowLinesBuffer::getNewLineId()
{
    int ret;

    if (m_firstfree == InvalidFreeIndex)
    {
        //one past old array is new one
        ret = m_lines.size();

        //kick start from 4, then do 6, 9, 13, etc. growing by 50% each time
        m_lines.resize(std::max(4u, (3u * m_lines.size()) / 2u));

        //no need to check if ret+1<=size(), we always add at least 2 new lines

        m_lines[m_firstfree].m_id = ret + 1;

        for (int i = ret + 1; i < m_lines.size() -1 ; ++i)
        {
            m_lines[i].m_id = i + 1;
        }

        m_lines[m_lines.size() - 1].m_id = InvalidFreeIndex;
    }
    else
    {
        //pop one from free list
        ret = m_firstfree;
        m_firstfree = m_lines[m_firstfree].m_id;
    }

    return ret;
}

}

