#ifndef OX_TILE_H
#define OX_TILE_H

#include "box.h"
#include "map.h"
#include "gfxqueue.h"
#include <fstream>
#include <sstream>

namespace Ox
{
    struct TileDef
    {
        char id;
        CL_Surface surface;
        Box virt_box;
        std::vector<Box> boxes;

        TileDef(char id)
        : virt_box(666, 666, 666) // dummy
        {
            this->id = id;
            surface = CL_Surface(std::string("res/tiles/") + id + ".png");

            int max_z = 0;

            std::ifstream bounds((std::string("res/tiles/") + id + "_bounds.txt").c_str());
            for (std::string box_str; getline(bounds, box_str); )
            {
                int x, y, z, sx, sy, sz;
                if (std::istringstream(box_str) >> x >> y >> z >> sx >> sy >> sz)
                {
                    if (z + sz / 2 > max_z)
                        max_z = z + sz / 2;
                    Box new_box(sx, sy, sz);
                    new_box.set_x(x);
                    new_box.set_y(y);
                    new_box.set_z(z);
                    boxes.push_back(new_box);
                }
            }

            virt_box = Box(Map::PHYS_TILE_SIZE, Map::PHYS_TILE_SIZE, max_z * 2);
            virt_box.set_z(max_z);
        }
    };

    class Tile
    {
        CL_Surface m_surface;
        Box m_virt_box;
        std::vector<Box> m_boxes;

    public:
        Tile(const TileDef& def, unsigned x, unsigned y)
        : m_virt_box(def.virt_box), m_surface(def.surface), m_boxes(def.boxes)
        {
            int offs_x = (x + 0.5) * Map::PHYS_TILE_SIZE;
            int offs_y = (y) * Map::PHYS_TILE_SIZE;

            m_virt_box.set_x(offs_x);
            m_virt_box.set_y(offs_y);
            m_virt_box.set_z(m_virt_box.sz() / 2);

            for (unsigned i = 0; i < m_boxes.size(); ++i)
            {
                m_boxes[i].set_x(m_boxes[i].x() + offs_x);
                m_boxes[i].set_y(m_boxes[i].y() + offs_y);
            }
        }

        void draw(GfxQueue& queue)
        {
            queue.insert(m_surface, m_virt_box, false);
        }

        bool collides(const Box& box) const
        {
            for (unsigned i = 0; i < m_boxes.size(); ++i)
                if (m_boxes[i].collides(box))
                    return true;
            return false;
        }
    };
}

#endif
