#ifndef OX_PIZZACLOCK_H
#define OX_PIZZACLOCK_H

namespace Ox
{
    class Game;

    class PizzaClock
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

    public:
        PizzaClock(Game& game);                                                             
        ~PizzaClock();
        void update();
        void draw(int left, int top);
        bool has_run_out();
        void reset();
        void add_ticks(unsigned ticks);
        void add_ingredient(const std::string& name);
    };
}

#endif