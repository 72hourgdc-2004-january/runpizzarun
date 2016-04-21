#include "precomp.h"
#include "map.h"
#include "game.h"
#include "box.h"
#include "gfxqueue.h"
#include "tile.h"
#include <string>
#include <map>
#include <cctype>
#include <fstream>
#include <exception>
#include <cstdlib>
using namespace std;

struct Ox::Map::Impl
{
    Game* game;

    unsigned background[TILES_X][TILES_Y];
    vector<CL_Surface> background_surfs;

    vector<TileDef> tile_defs;
    unsigned get_tile_def(char id)
    {
        for (unsigned i = 0; i < tile_defs.size(); ++i)
            if (tile_defs[i].id == id)
                return i;
        tile_defs.push_back(TileDef(id));
        return tile_defs.size() - 1;
    }

    bool is_blocked[TILES_X][TILES_Y];
    vector<Tile> tiles;
};

Ox::Map::Map(Game& game)
: pimpl(new Impl)
{
    pimpl->game = &game;

    CL_DirectoryScanner scanner;
    if (scanner.scan("res/background", "*.png"))
  	    while (scanner.next())
      		pimpl->background_surfs.push_back(CL_Surface(scanner.get_pathname()));
    if (pimpl->background_surfs.empty())
        throw runtime_error("No background tiles found!");

    for (unsigned y = 0; y < TILES_Y; ++y)
        for (unsigned x = 0; x < TILES_X; ++x)
        {
            pimpl->background[x][y] = rand() % pimpl->background_surfs.size();
            pimpl->is_blocked[x][y] = false;
        }
}

Ox::Map::~Map()
{
}

void Ox::Map::draw() const
{
    for (unsigned y = 0; y < TILES_Y; ++y)
        for (unsigned x = 0; x < TILES_X; ++x)
            pimpl->background_surfs[pimpl->background[x][y]].draw(x * TILE_GFX_WIDTH,
                y * TILE_GFX_HEIGHT);

    for (unsigned i = 0; i < pimpl->tiles.size(); ++i)
        pimpl->tiles[i].draw(pimpl->game->gfx_queue());
}

void Ox::Map::add_tile(char id, unsigned tile_x, unsigned tile_y)
{
    unsigned index = pimpl->get_tile_def(id);
    assert(tile_x < TILES_X && tile_y < TILES_Y);
    pimpl->is_blocked[tile_x][tile_y] = !pimpl->tile_defs[index].boxes.empty();
    pimpl->tiles.push_back(Tile(pimpl->tile_defs[index], tile_x, tile_y));
}

bool Ox::Map::is_box_free(const Box& box) const
{
    if (box.x() - box.sx() / 2 < 0 || box.x() + box.sx() / 2 > Map::MAX_X ||
        box.y() - box.sy() / 2 < 0 || box.y() + box.sy() / 2 > Map::MAX_Y ||
        box.z() - box.sz() / 2 < 0)
        return false;

    for (unsigned i = 0; i < pimpl->tiles.size(); ++i)
        if (pimpl->tiles[i].collides(box))
            return false;

    return true;
}

bool Ox::Map::is_tile_solid(int tile_x, int tile_y) const
{
    if (tile_x < 0 || tile_x >= TILES_X ||
        tile_y < 0 || tile_y >= TILES_Y)
        return true;
    else
        return pimpl->is_blocked[tile_x][tile_y];
}