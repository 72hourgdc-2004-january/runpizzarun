#include "precomp.h"
#include "ingrcannon.h"
#include "game.h"
#include "map.h"
#include "gfxqueue.h"
#include "ingredient.h"
#include "box.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
using namespace std;

struct Ox::IngrCannon::Impl
{
    Game* game;
    CL_Surface ufo_surf;
    CL_SoundBuffer ufo_fire;
    Box cannon_box;

    struct IngrDef
    {
        std::string name;
        CL_Surface surface;
    };
    vector<IngrDef> ingr_defs;
    unsigned get_ingr_def(const std::string& name)
    {
        for (unsigned i = 0; i < ingr_defs.size(); ++i)
            if (ingr_defs[i].name == name)
                return i;
        ingr_defs.push_back(IngrDef());
        ingr_defs.back().name = name;
        ingr_defs.back().surface =
            CL_Surface("res/ingredients/" + name + "/ingame.png");
        return ingr_defs.size() - 1;
    }

    vector<unsigned> load_ingrs, next_ingrs;
    unsigned ticks_to_next_ingr;

    void reload()
    {
        next_ingrs = load_ingrs;
        random_shuffle(next_ingrs.begin(), next_ingrs.end());
    }

    vector<Ingredient> ingrs;

    Impl()
    : cannon_box(50, 50, 50)
    {
    }
};

Ox::IngrCannon::IngrCannon(Game& game, unsigned level)
: pimpl(new Impl)
{
    pimpl->game = &game;

    pimpl->ufo_surf = CL_Surface("res/misc/ufo.png");
    pimpl->ufo_fire = CL_SoundBuffer("res/misc/ufo_fire.ogg");

    ostringstream recipe_path;
    recipe_path << "levels/" << level << "/recipes.txt";
    ifstream recipe_list(recipe_path.str().c_str());
    for (string recipe; getline(recipe_list, recipe); )
    {
        ifstream ingr_list(("res/recipes/" + recipe + ".txt").c_str());
        
        // Skip recipe title.
        getline(ingr_list, string() = string());

        for (string ingr; getline(ingr_list, ingr); )
            pimpl->load_ingrs.push_back(pimpl->get_ingr_def(ingr));
    }
    if (pimpl->load_ingrs.empty())
        throw runtime_error("Couldn't find any mathing ingredients for the level");

    pimpl->ticks_to_next_ingr = 80;
}

Ox::IngrCannon::~IngrCannon()
{
}

void Ox::IngrCannon::draw()
{
    pimpl->ufo_surf.set_angle(sin(CL_System::get_time() / 200.0) * 20);
    pimpl->game->gfx_queue().insert(pimpl->ufo_surf, pimpl->cannon_box, true);

    for (unsigned i = 0; i < pimpl->ingrs.size(); ++i)
        pimpl->ingrs[i].draw();
}

void Ox::IngrCannon::update()
{
    // Fly a weird route...I knew how these functions were called once, but
    // I lost that math book...was it Lissajous curves? :P
    int center_x = Map::MAX_X / 2;
    int center_y = Map::MAX_Y / 2;
    int center_z = 150;
    double factor_x = Map::MAX_X * 0.3;
    double factor_y = Map::MAX_Y * 0.3;
    double factor_z = 25;
    unsigned time = CL_System::get_time();
    pimpl->cannon_box.set_x(center_x + sin(time / 735.0) * factor_x);
    pimpl->cannon_box.set_y(center_y + cos(time / 923.0) * factor_y);
    pimpl->cannon_box.set_z(150 + sin(time / 643.0) * factor_z);

    // From time to time, drop a new ingredient (but don't drop too many)
    if (pimpl->ingrs.size() < 45 && --pimpl->ticks_to_next_ingr == 0)
    {
        if (pimpl->next_ingrs.empty())
            pimpl->reload();

        pimpl->ufo_fire.play();

        Impl::IngrDef& def = pimpl->ingr_defs[pimpl->next_ingrs.back()];
        pimpl->next_ingrs.pop_back();

        pimpl->ingrs.push_back(Ingredient(*pimpl->game, def.name, def.surface,
            pimpl->cannon_box.x(), pimpl->cannon_box.y(),
            pimpl->cannon_box.z() + 20));

        pimpl->ticks_to_next_ingr = 45;
    }

    // Let all ingredients check for collision with player (and move)
    for (unsigned i = 0; i < pimpl->ingrs.size(); ++i)
        pimpl->ingrs[i].update();

    // Remove all ingredients that were collected
    for (unsigned i = 0; i < pimpl->ingrs.size(); ++i)
        if (pimpl->ingrs[i].collected())
            pimpl->ingrs.erase(pimpl->ingrs.begin() + i--);
}

void Ox::IngrCannon::refill(const std::string& ingr)
{
    pimpl->next_ingrs.push_back(pimpl->get_ingr_def(ingr));
}