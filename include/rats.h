#ifndef OX_RATS_H
#define OX_RATS_H

#include "box.h"
#include "game.h"
#include <cstdlib>

namespace Ox
{
    class Rats
    {
        Game& m_game;

        enum Action
        {
            ACT_WALK_UP,
            ACT_WALK_DOWN,
            ACT_WALK_LEFT,
            ACT_WALK_RIGHT,
            ACT_EAT,
            ACT_NUM
        };

        CL_Surface m_surfs[ACT_NUM][2];
        CL_SoundBuffer m_eat;

        struct Rat
        {
            Box box;
            Action action;
            unsigned ticks_to_eat;

            Rat(int tile_x, int tile_y)
            : box(16, 16, 16), action(ACT_WALK_DOWN)
            {
                box.set_x((tile_x + 0.5) * Map::PHYS_TILE_SIZE);
                box.set_y((tile_y) * Map::PHYS_TILE_SIZE);
                box.set_z(box.sz() / 2);
            }
        };
        std::vector<Rat> m_rats;

    public:
        explicit Rats(Game& game)
        : m_game(game)
        {
            m_surfs[ACT_WALK_UP][0] = CL_Surface("res/rat/back-0.png");
            m_surfs[ACT_WALK_UP][1] = CL_Surface("res/rat/back-1.png");
            m_surfs[ACT_WALK_DOWN][0] = CL_Surface("res/rat/front-0.png");
            m_surfs[ACT_WALK_DOWN][1] = CL_Surface("res/rat/front-1.png");
            m_surfs[ACT_WALK_LEFT][0] = CL_Surface("res/rat/left-0.png");
            m_surfs[ACT_WALK_LEFT][1] = CL_Surface("res/rat/left-1.png");
            m_surfs[ACT_WALK_RIGHT][0] = CL_Surface("res/rat/right-0.png");
            m_surfs[ACT_WALK_RIGHT][1] = CL_Surface("res/rat/right-1.png");
            m_surfs[ACT_EAT][0] = CL_Surface("res/rat/eat-0.png");
            m_surfs[ACT_EAT][1] = CL_Surface("res/rat/eat-1.png");
            m_eat = CL_SoundBuffer("res/rat/eat.ogg");
        }

        void add_rat(int tile_x, int tile_y)
        {
            m_rats.push_back(Rat(tile_x, tile_y));
        }

        void draw()
        {
            for (unsigned i = 0; i < m_rats.size(); ++i)
                m_game.gfx_queue().insert(
                    m_surfs[m_rats[i].action][CL_System::get_time() / 25 % 2],
                    m_rats[i].box, true);
        }

        void update()
        {
            for (unsigned i = 0; i < m_rats.size(); ++i)
            {
                const int speed = 8;
                int moved = 0;

                switch (m_rats[i].action)
                {
                    case ACT_EAT:
                        if (--m_rats[i].ticks_to_eat == 0)
                            m_rats[i].action = static_cast<Action>(rand() % 4);
                        if (m_rats[i].ticks_to_eat % 35 == 0)
                            m_eat.play();
                        continue;
                    case ACT_WALK_LEFT:   
                        moved = m_rats[i].box.try_move(m_game.map(), speed, -1, 0, 0);
                        break;
                    case ACT_WALK_RIGHT: 
                        moved = m_rats[i].box.try_move(m_game.map(), speed, +1, 0, 0);
                        break;
                    case ACT_WALK_UP: 
                        moved = m_rats[i].box.try_move(m_game.map(), speed, 0, -1, 0);
                        break;
                    case ACT_WALK_DOWN:
                        moved = m_rats[i].box.try_move(m_game.map(), speed, 0, +1, 0);
                        break;
                }

                if (std::abs(moved) < speed || std::rand() % 25 == 0)
                    m_rats[i].action = static_cast<Action>(rand() % 4);
            }
        }

        bool eat_ingredient(const Box& box)
        {
            for (unsigned i = 0; i < m_rats.size(); ++i)
                if (box.collides(m_rats[i].box))
                {
                    m_rats[i].action = ACT_EAT;
                    m_rats[i].ticks_to_eat = 150;
                    return true;
                }
            return false;
        }
    };
}

#endif
