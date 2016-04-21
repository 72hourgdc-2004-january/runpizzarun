#ifndef OX_GAME_H
#define OX_GAME_H

#include "subapp.h"

namespace Ox
{
    class Customers;
    class GfxQueue;
    class Texts;
    class Map;
    class Player;
    class FatGuy;
    class IngrCannon;
    class Rats;
    class Monsters;

    class Game : public SubApp
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

        void add_slots(CL_SlotContainer& slots);

    public:
        Game(SubAppMgr& mgr, unsigned level, unsigned lives, unsigned score);
        ~Game();

        void draw();
        void update();

        enum Reason
        {
            REASON_TIMEOUT,
            REASON_DEATH,
            REASON_NUM
        };
        void kill_player(Reason reason);
        void do_score(int delta);
        void win();

        #define DECLARE_ACCESSOR(Type, name) const Type& name() const; Type& name();
        DECLARE_ACCESSOR(Customers, customers);
        DECLARE_ACCESSOR(GfxQueue, gfx_queue);
        DECLARE_ACCESSOR(Texts, texts);
        DECLARE_ACCESSOR(Map, map);
        DECLARE_ACCESSOR(Player, player);
        DECLARE_ACCESSOR(FatGuy, fat_guy);
        DECLARE_ACCESSOR(IngrCannon, ingr_cannon);
        DECLARE_ACCESSOR(Rats, rats);
        DECLARE_ACCESSOR(Monsters, monsters);
        #undef DECLARE_ACCESSOR

        std::string song_name() const;
    };
}

#endif
