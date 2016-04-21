#ifndef OX_PLAYER_H
#define OX_PLAYER_H

namespace Ox
{
    class Game;
    class Map;
    class Box;

    class Player
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

    public:
        explicit Player(Game& game);
        ~Player();

        const Box& box() const;
        Box& box();

        void draw();
        void update();

        void show_message(const std::string& str);
    };
}

#endif
