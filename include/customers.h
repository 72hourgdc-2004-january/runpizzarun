#ifndef OX_CUSTOMERS_H
#define OX_CUSTOMERS_H

namespace Ox
{
    class Game;

    class Customers
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

    public:
        Customers(Game& game, unsigned level, bool can_win);
        ~Customers();
        void collect_ingredient(const std::string& name);
        void player_died();
        void draw();
        void update();
    };
}

#endif
