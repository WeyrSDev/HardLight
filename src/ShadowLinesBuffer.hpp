/* 
 * File:   ShadowLinesBuffer.hpp
 * Author: frex
 *
 * Created on January 24, 2014, 7:42 PM
 */

#ifndef SHADOWLINESBUFFER_HPP
#define	SHADOWLINESBUFFER_HPP

#include <vector>
#include <SFML/System/Vector2.hpp>

namespace hlt {

class ShadowLine
{

    friend class ShadowLinesBuffer;
public:
    sf::Vector2f a, b;
    int getTreeId() const;
private:
    int m_id; //-1, last empty, 0+ lineids, less than -1 = -treeid-2
};

class ShadowLinesBuffer
{

public:
    ShadowLinesBuffer();

    //returns lineid
    int addLine(sf::Vector2f a, sf::Vector2f b, int treeid);

    //returns treeid that this line had
    int removeLine(int lineid);

    //remove all lines
    void removeAll();

    //get line by lineid to inspect a,b and treeid
    const ShadowLine& getLine(int lineid) const;

    int getCount()const;

private:
    int getNewLineId();

    int m_firstfree;

    int m_count;

    std::vector<ShadowLine> m_lines;

};

inline int ShadowLinesBuffer::getCount() const
{
    return m_count;
}

}

#endif	/* SHADOWLINESBUFFER_HPP */

