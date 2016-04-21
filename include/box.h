#ifndef OX_BOX_H
#define OX_BOX_H

#include "map.h"

namespace Ox
{
    class Box
    {
        int m_x, m_y, m_z;
        int m_sx, m_sy, m_sz;

    public:
        Box(int sx, int sy, int sz)
        : m_sx(sx), m_sy(sy), m_sz(sz)
        {
            // unsigned values cause too much trouble when not being careful,
            // so use ints and check manually :)
            assert(sx >= 0);
            assert(sy >= 0);
            assert(sz >= 0);

            /*// Probably not necessary, but you never know
            assert(sx % 2 == 0);
            assert(sy % 2 ==0);
            assert(sz % 2 ==0);*/
        }

        int x() const { return m_x; }
        int y() const { return m_y; }
        int z() const { return m_z; }

        void set_x(int value) { m_x = value; }
        void set_y(int value) { m_y = value; }
        void set_z(int value) { m_z = value; }

        int sx() const { return m_sx; }
        int sy() const { return m_sy; }
        int sz() const { return m_sz; }

        bool collides(const Box& other) const
        {
            return x() - sx() / 2 < other.x() + other.sx() / 2 &&
                   x() + sx() / 2 > other.x() - other.sx() / 2 &&
                   y() - sy() / 2 < other.y() + other.sy() / 2 &&
                   y() + sy() / 2 > other.y() - other.sy() / 2 &&
                   z() - sz() / 2 < other.z() + other.sz() / 2 &&
                   z() + sz() / 2 > other.z() - other.sz() / 2;
        }

        // Returns the number of steps that were successful
        int try_move(Map& map, int times, int step_x, int step_y, int step_z)
        {
            if (times > 0)
                for (int i = 0; i < times; ++i)
                {
                    Box backup = *this;
                    set_x(x() + step_x);
                    set_y(y() + step_y);
                    set_z(z() + step_z);
                    if (!map.is_box_free(*this))
                    {
                        *this = backup;
                        return i;
                    }
                }
            else if (times < 0)
                for (int i = 0; i < -times; ++i)
                {
                    Box backup = *this;
                    set_x(x() - step_x);
                    set_y(y() - step_y);
                    set_z(z() - step_z);
                    if (!map.is_box_free(*this))
                    {
                        *this = backup;
                        return -i;
                    }
                }

            return times;
        }
    };
}

#endif
