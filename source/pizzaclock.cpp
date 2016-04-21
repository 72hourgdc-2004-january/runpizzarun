#include "precomp.h"
#include "pizzaclock.h"
#include "game.h"
#include <cstdlib>

struct Ox::PizzaClock::Impl
{
    Game* game;
    CL_Surface bg, finger;
    CL_SoundBuffer put_ingr;
    unsigned ticks_left;
    static const int MAX_TICKS = 1500;

    struct IngrDef
    {
        std::string name;
        CL_Surface surface;

        explicit IngrDef(const std::string& name)
        {
            this->name = name;
            surface = CL_Surface("res/ingredients/" + name + "/clock.png");
        }
    };
    std::vector<IngrDef> ingr_defs;
    struct DisplayedIngr
    {
        CL_Surface surface;
        double angle;
    };
    std::vector<DisplayedIngr> ingrs;
};

Ox::PizzaClock::PizzaClock(Game& game)
: pimpl(new Impl)
{
    pimpl->game = &game;
    pimpl->bg = CL_Surface("res/ui/pizzaclock_background.png");
    pimpl->finger = CL_Surface("res/ui/pizzaclock_hand.png");
    pimpl->put_ingr = CL_SoundBuffer("res/misc/pizzaclock_put.ogg");
    pimpl->ticks_left = Impl::MAX_TICKS;
}
                                                        
Ox::PizzaClock::~PizzaClock()
{
}

void Ox::PizzaClock::update()
{
    if (pimpl->ticks_left)
        --pimpl->ticks_left;
}

void Ox::PizzaClock::draw(int left, int top)
{
    pimpl->bg.draw(left, top);
    for (unsigned i = 0; i < pimpl->ingrs.size(); ++i)
    {
        pimpl->ingrs[i].surface.set_angle(pimpl->ingrs[i].angle);
        pimpl->ingrs[i].surface.draw(left, top);
    }
    pimpl->finger.set_angle(pimpl->ticks_left * 360.0 / Impl::MAX_TICKS);
    pimpl->finger.draw(left, top);
}

bool Ox::PizzaClock::has_run_out()
{
    return pimpl->ticks_left == 0;
}

void Ox::PizzaClock::reset()
{
    pimpl->ticks_left = Impl::MAX_TICKS;
    pimpl->ingrs.clear();
}

void Ox::PizzaClock::add_ticks(unsigned ticks)
{
    pimpl->ticks_left += ticks;
    if (pimpl->ticks_left > Impl::MAX_TICKS)
    {
        pimpl->game->do_score((pimpl->ticks_left - Impl::MAX_TICKS) * 25);
        pimpl->ticks_left = Impl::MAX_TICKS;
    }
}

void Ox::PizzaClock::add_ingredient(const std::string& name)
{
    Impl::DisplayedIngr new_ingr;

    bool found = false;
    for (unsigned i = 0; i < pimpl->ingr_defs.size() && !found; ++i)
        if (pimpl->ingr_defs[i].name == name)
            new_ingr.surface = pimpl->ingr_defs[i].surface, found = true;
    if (!found)
    {
        pimpl->ingr_defs.push_back(Impl::IngrDef(name));
        new_ingr.surface = pimpl->ingr_defs.back().surface;
    }
            
    new_ingr.angle = std::rand() % 360;
    pimpl->ingrs.push_back(new_ingr);

    pimpl->put_ingr.play();
}