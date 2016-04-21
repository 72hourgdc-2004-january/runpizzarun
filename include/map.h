#ifndef OX_MAP_H
#define OX_MAP_H

#include <cassert>

namespace Ox
{
    class Game;
    class Box;

    class Map
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

    public:
        const static unsigned TILES_X = 13;
        const static unsigned TILES_Y = 24;
        const static unsigned TILE_GFX_WIDTH = 50;
        const static unsigned TILE_GFX_HEIGHT = 25;
        const static unsigned PHYS_TILE_SIZE = 50;
        const static unsigned MAX_X = TILES_X * PHYS_TILE_SIZE;
        const static unsigned MAX_Y = TILES_Y * PHYS_TILE_SIZE;

        Map(Game& game);
        ~Map();

        void draw() const;
        void add_tile(char id, unsigned tile_x, unsigned tile_y);
        bool is_box_free(const Box& box) const;
        bool is_tile_solid(int tile_x, int tile_y) const;
    };
}

#endif
