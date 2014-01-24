/*
 * Copyright (c) 2009 Erin Catto http://www.box2d.org
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef B2_DYNAMIC_TREE_H
#define B2_DYNAMIC_TREE_H

#include <vector>
#include <cstring>
#include <cassert>
#include <cmath>
#include <limits>
#include "ShadowLine.hpp"
#include "Light.hpp"


const int b2_nullNode = -1;

/// This function is used to ensure that a floating point number is not a NaN or infinity.
inline bool b2IsValid(float x)
{
	int ix = *reinterpret_cast<int*>(&x);
	return (ix & 0x7f800000) != 0x7f800000;
}

/// A 2D column vector.
struct b2Vec2
{
	/// Default constructor does nothing (for performance).
	b2Vec2() {}

	/// Construct using coordinates.
	b2Vec2(float x, float y) : x(x), y(y) {}

	/// Set this vector to all zeros.
	void SetZero() { x = 0.0f; y = 0.0f; }

	/// Set this vector to some specified coordinates.
	void Set(float x_, float y_) { x = x_; y = y_; }

	/// Negate this vector.
	b2Vec2 operator -() const { b2Vec2 v; v.Set(-x, -y); return v; }
	
	/// Read from and indexed element.
	float operator () (int i) const
	{
		return (&x)[i];
	}

	/// Write to an indexed element.
	float& operator () (int i)
	{
		return (&x)[i];
	}

	/// Add a vector to this vector.
	void operator += (const b2Vec2& v)
	{
		x += v.x; y += v.y;
	}
	
	/// Subtract a vector from this vector.
	void operator -= (const b2Vec2& v)
	{
		x -= v.x; y -= v.y;
	}

	/// Multiply this vector by a scalar.
	void operator *= (float a)
	{
		x *= a; y *= a;
	}

	/// Get the length of this vector (the norm).
	float Length() const
	{
		return std::sqrt(x * x + y * y);
	}

	/// Get the length squared. For performance, use this instead of
	/// b2Vec2::Length (if possible).
	float LengthSquared() const
	{
		return x * x + y * y;
	}

	/// Convert this vector into a unit vector. Returns the length.
	float Normalize()
	{
		float length = Length();
		if (length < std::numeric_limits<float>::epsilon())
		{
			return 0.0f;
		}
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;

		return length;
	}

	/// Does this vector contain finite coordinates?
	bool IsValid() const
	{
		return b2IsValid(x) && b2IsValid(y);
	}

	float x, y;
};

/// Perform the dot product on two vectors.
inline float b2Dot(const b2Vec2& a, const b2Vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

/// Perform the cross product on two vectors. In 2D this produces a scalar.
inline float b2Cross(const b2Vec2& a, const b2Vec2& b)
{
	return a.x * b.y - a.y * b.x;
}

/// Perform the cross product on a scalar and a vector. In 2D this produces
/// a vector.
inline b2Vec2 b2Cross(float s, const b2Vec2& a)
{
	return b2Vec2(-s * a.y, s * a.x);
}

/// Multiply a matrix times a vector. If a rotation matrix is provided,
/// then this transforms the vector from one frame to another.

/// Add two vectors component-wise.
inline b2Vec2 operator + (const b2Vec2& a, const b2Vec2& b)
{
	return b2Vec2(a.x + b.x, a.y + b.y);
}

/// Subtract two vectors component-wise.
inline b2Vec2 operator - (const b2Vec2& a, const b2Vec2& b)
{
	return b2Vec2(a.x - b.x, a.y - b.y);
}

inline b2Vec2 operator * (float s, const b2Vec2& a)
{
	return b2Vec2(s * a.x, s * a.y);
}

inline bool operator == (const b2Vec2& a, const b2Vec2& b)
{
	return a.x == b.x && a.y == b.y;
}

inline b2Vec2 b2Min(const b2Vec2& a, const b2Vec2& b)
{
	return b2Vec2(std::min(a.x, b.x), std::min(a.y, b.y));
}

inline b2Vec2 b2Max(const b2Vec2& a, const b2Vec2& b)
{
	return b2Vec2(std::max(a.x, b.x), std::max(a.y, b.y));
}

/// This is a growable LIFO stack with an initial capacity of N.
/// If the stack size exceeds the initial capacity, the heap is used
/// to increase the size of the stack.

template <typename T, int N>
class b2GrowableStack
{

public:

    b2GrowableStack()
    {
        m_stack = m_array;
        m_count = 0;
        m_capacity = N;
    }

    ~b2GrowableStack()
    {
        if (m_stack != m_array)
        {
            delete [] m_stack;
            m_stack = NULL;
        }
    }

    void Push(const T& element)
    {
        if (m_count == m_capacity)
        {
            T * old = m_stack;
            m_capacity *= 2;
            m_stack = new T[m_capacity];
            std::memcpy(m_stack, old, m_count * sizeof (T));
            if (old != m_array)
            {
                delete [] old;
            }
        }

        m_stack[m_count] = element;
        ++m_count;
    }

    T Pop()
    {
        assert(m_count > 0);
        --m_count;
        return m_stack[m_count];
    }

    int GetCount()
    {
        return m_count;
    }

private:
    T * m_stack;
    T m_array[N];
    int m_count;
    int m_capacity;
};

/// Ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
struct b2RayCastInput
{
	b2Vec2 p1, p2;
	float maxFraction;
};

/// Ray-cast output data. The ray hits at p1 + fraction * (p2 - p1), where p1 and p2
/// come from b2RayCastInput.
struct b2RayCastOutput
{
	b2Vec2 normal;
	float fraction;
};

/// An axis aligned bounding box.
struct b2AABB
{
	/// Verify that the bounds are sorted.
	bool IsValid() const;

	/// Get the center of the AABB.
	b2Vec2 GetCenter() const
	{
		return 0.5f * (lowerBound + upperBound);
	}

	/// Get the extents of the AABB (half-widths).
	b2Vec2 GetExtents() const
	{
		return 0.5f * (upperBound - lowerBound);
	}

	/// Get the perimeter length
	float GetPerimeter() const
	{
		float wx = upperBound.x - lowerBound.x;
		float wy = upperBound.y - lowerBound.y;
		return 2.0f * (wx + wy);
	}

	/// Combine an AABB into this one.
	void Combine(const b2AABB& aabb)
	{
		lowerBound = b2Min(lowerBound, aabb.lowerBound);
		upperBound = b2Max(upperBound, aabb.upperBound);
	}

	/// Combine two AABBs into this one.
	void Combine(const b2AABB& aabb1, const b2AABB& aabb2)
	{
		lowerBound = b2Min(aabb1.lowerBound, aabb2.lowerBound);
		upperBound = b2Max(aabb1.upperBound, aabb2.upperBound);
	}

	/// Does this aabb contain the provided AABB.
	bool Contains(const b2AABB& aabb) const
	{
		bool result = true;
		result = result && lowerBound.x <= aabb.lowerBound.x;
		result = result && lowerBound.y <= aabb.lowerBound.y;
		result = result && aabb.upperBound.x <= upperBound.x;
		result = result && aabb.upperBound.y <= upperBound.y;
		return result;
	}

	bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input) const;

	b2Vec2 lowerBound;	///< the lower vertex
	b2Vec2 upperBound;	///< the upper vertex
};

// ---------------- Inline Functions ------------------------------------------

inline bool b2AABB::IsValid() const
{
	b2Vec2 d = upperBound - lowerBound;
	bool valid = d.x >= 0.0f && d.y >= 0.0f;
	valid = valid && lowerBound.IsValid() && upperBound.IsValid();
	return valid;
}

inline bool b2TestOverlap(const b2AABB& a, const b2AABB& b)
{
	b2Vec2 d1, d2;
	d1 = b.lowerBound - a.upperBound;
	d2 = a.lowerBound - b.upperBound;

	if (d1.x > 0.0f || d1.y > 0.0f)
		return false;

	if (d2.x > 0.0f || d2.y > 0.0f)
		return false;

	return true;
}

/// A node in the dynamic tree. The client does not interact with this directly.

struct b2TreeNode
{

    bool IsLeaf() const
    {
        return child1 == b2_nullNode;
    }

    /// Enlarged AABB
    b2AABB aabb;

    ee::ShadowLine line;

    union
    {

        int parent;
        int next;
    };

    int child1;
    int child2;

    // leaf = 0, free node = -1
    int height;
};

/// A dynamic AABB tree broad-phase, inspired by Nathanael Presson's btDbvt.
/// A dynamic tree arranges data in a binary tree to accelerate
/// queries such as volume queries and ray casts. Leafs are proxies
/// with an AABB. In the tree we expand the proxy AABB by b2_fatAABBFactor
/// so that the proxy AABB is bigger than the client object. This allows the client
/// object to move by small amounts without triggering a tree update.
///
/// Nodes are pooled and relocatable, so we use node indices rather than pointers.

class b2DynamicTree
{

public:
    /// Constructing the tree initializes the node pool.
    b2DynamicTree();

    /// Destroy the tree, freeing the node pool.
    ~b2DynamicTree();

    /// Create a proxy. Provide a tight fitting AABB and a userData pointer.
    int CreateProxy(const b2AABB& aabb, ee::ShadowLine line);

    /// Destroy a proxy. This asserts if the id is invalid.
    void DestroyProxy(int proxyId);

    /// Move a proxy with a swepted AABB. If the proxy has moved outside of its fattened AABB,
    /// then the proxy is removed from the tree and re-inserted. Otherwise
    /// the function returns immediately.
    /// @return true if the proxy was re-inserted.
    bool MoveProxy(int proxyId, const b2AABB& aabb1, const b2Vec2& displacement);

    /// Get proxy user data.
    /// @return the proxy user data or 0 if the id is invalid.
    ee::ShadowLine GetShadowLine(int proxyId) const;

    /// Get the fat AABB for a proxy.
    const b2AABB& GetFatAABB(int proxyId) const;

    /// Query an AABB for overlapping proxies. The callback class
    /// is called for each proxy that overlaps the supplied AABB.
    template <typename T>
    void Query(T* callback, const b2AABB& aabb) const;

    /// Ray-cast against the proxies in the tree. This relies on the callback
    /// to perform a exact ray-cast in the case were the proxy contains a shape.
    /// The callback also performs the any collision filtering. This has performance
    /// roughly equal to k * log(n), where k is the number of collisions and n is the
    /// number of proxies in the tree.
    /// @param input the ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
    /// @param callback a callback class that is called for each proxy that is hit by the ray.
    template <typename T>
    void RayCast(T* callback, const b2RayCastInput& input) const;

    /// Validate this tree. For testing.
    void Validate() const;

    /// Compute the height of the binary tree in O(N) time. Should not be
    /// called often.
    int GetHeight() const;

    /// Get the maximum balance of an node in the tree. The balance is the difference
    /// in height of the two children of a node.
    int GetMaxBalance() const;

    /// Get the ratio of the sum of the node areas to the root area.
    float GetAreaRatio() const;

    /// Build an optimal tree. Very expensive. For testing.
    void RebuildBottomUp();

    /// Shift the world origin. Useful for large worlds.
    /// The shift formula is: position -= newOrigin
    /// @param newOrigin the new origin with respect to the old origin
    void ShiftOrigin(const b2Vec2& newOrigin);

    int GetShadowLinesCount()const;

    ee::ShadowLine GetNthShadowLine(int index);
    
    void ClearAll();

private:

    int AllocateNode();
    void FreeNode(int node);

    void InsertLeaf(int node);
    void RemoveLeaf(int node);

    int Balance(int index);

    int ComputeHeight() const;
    int ComputeHeight(int nodeId) const;

    void ValidateStructure(int index) const;
    void ValidateMetrics(int index) const;

    int m_root;

    b2TreeNode * m_nodes;
    int m_nodeCount;
    int m_nodeCapacity;

    int m_freeList;

    /// This is used to incrementally traverse the tree for re-balancing.
    unsigned m_path;

    int m_insertionCount;

    std::vector<int> m_indices;
    
    float m_padding;

};

inline ee::ShadowLine b2DynamicTree::GetShadowLine(int proxyId) const
{
    assert(0 <= proxyId && proxyId < m_nodeCapacity);
    return m_nodes[proxyId].line;
}

inline const b2AABB& b2DynamicTree::GetFatAABB(int proxyId) const
{
    assert(0 <= proxyId && proxyId < m_nodeCapacity);
    return m_nodes[proxyId].aabb;
}

inline int b2DynamicTree::GetShadowLinesCount() const
{
    return m_indices.size();
}

template <typename T>
inline void b2DynamicTree::Query(T* callback, const b2AABB& aabb) const
{
    b2GrowableStack<int, 256> stack;
    stack.Push(m_root);

    while (stack.GetCount() > 0)
    {
        int nodeId = stack.Pop();
        if (nodeId == b2_nullNode)
        {
            continue;
        }

        const b2TreeNode* node = m_nodes + nodeId;

        if (b2TestOverlap(node->aabb, aabb))
        {
            if (node->IsLeaf())
            {
                bool proceed = callback->QueryCallback(nodeId);
                if (proceed == false)
                {
                    return;
                }
            }
            else
            {
                stack.Push(node->child1);
                stack.Push(node->child2);
            }
        }
    }
}

template <typename T>
inline void b2DynamicTree::RayCast(T* callback, const b2RayCastInput& input) const
{
    b2Vec2 p1 = input.p1;
    b2Vec2 p2 = input.p2;
    b2Vec2 r = p2 - p1;
    assert(r.LengthSquared() > 0.0f);
    r.Normalize();

    // v is perpendicular to the segment.
    b2Vec2 v = b2Cross(1.0f, r);
    b2Vec2 abs_v(std::abs(v.x),std::abs(v.y));

    // Separating axis for segment (Gino, p80).
    // |dot(v, p1 - c)| > dot(|v|, h)

    float maxFraction = input.maxFraction;

    // Build a bounding box for the segment.
    b2AABB segmentAABB;
    {
        b2Vec2 t = p1 + maxFraction * (p2 - p1);
        segmentAABB.lowerBound = b2Min(p1, t);
        segmentAABB.upperBound = b2Max(p1, t);
    }

    b2GrowableStack<int, 256> stack;
    stack.Push(m_root);

    while (stack.GetCount() > 0)
    {
        int nodeId = stack.Pop();
        if (nodeId == b2_nullNode)
        {
            continue;
        }

        const b2TreeNode* node = m_nodes + nodeId;

        if (b2TestOverlap(node->aabb, segmentAABB) == false)
        {
            continue;
        }

        // Separating axis for segment (Gino, p80).
        // |dot(v, p1 - c)| > dot(|v|, h)
        b2Vec2 c = node->aabb.GetCenter();
        b2Vec2 h = node->aabb.GetExtents();
        float separation = std::abs(b2Dot(v, p1 - c)) - b2Dot(abs_v, h);
        if (separation > 0.0f)
        {
            continue;
        }

        if (node->IsLeaf())
        {
            b2RayCastInput subInput;
            subInput.p1 = input.p1;
            subInput.p2 = input.p2;
            subInput.maxFraction = maxFraction;

            float value = callback->RayCastCallback(subInput, nodeId);

            if (value == 0.0f)
            {
                // The client has terminated the ray cast.
                return;
            }

            if (value > 0.0f)
            {
                // Update segment bounding box.
                maxFraction = value;
                b2Vec2 t = p1 + maxFraction * (p2 - p1);
                segmentAABB.lowerBound = b2Min(p1, t);
                segmentAABB.upperBound = b2Max(p1, t);
            }
        }
        else
        {
            stack.Push(node->child1);
            stack.Push(node->child2);
        }
    }
}

#endif
