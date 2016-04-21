#ifndef OX_INGRCANNON_H
#define OX_INGRCANNON_H

namespace Ox
{
    class Game;

    class IngrCannon
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

    public:
        IngrCannon(Game& game, unsigned level);
        ~IngrCannon();
        void update();
        void draw();
        void refill(const std::string& ingr);
    };
}

#endif
