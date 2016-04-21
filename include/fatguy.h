#ifndef OX_FATGUY_H
#define OX_FATGUY_H

namespace Ox
{
    class Game;
    class Box;

    class FatGuy
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

    public:
        explicit FatGuy(Game& game);
        ~FatGuy();

        const Box& box() const;
        Box& box();

        void draw();
        void update();
    };
}

#endif
