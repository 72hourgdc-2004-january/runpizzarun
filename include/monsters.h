#ifndef OX_MONSTER_H
#define OX_MONSTER_H

#include "game.h"
#include "box.h"

namespace Ox
{
    class Monsters
    {
    public:
        enum Type
        {
            TYPE_VERT,
            TYPE_HORZ
        };

    private:
        Game& m_game;

        enum Direction
        {
            DIR_LEFT,
            DIR_RIGHT,
            DIR_UP,
            DIR_DOWN,
            DIR_NUM
        };
        enum Frame
        {
            FRAME_WALK_0,
            FRAME_WALK_1,
            FRAME_WALK_2,
            FRAME_WALK_3,
            FRAME_WALK_NUM,
            FRAME_NUM = FRAME_WALK_NUM
        };
        CL_Surface m_surfaces[FRAME_NUM][DIR_NUM];

        struct Monster
        {
            Box box;
            Type type;
            Direction direction;

            Monster(Type t, unsigned tile_x, unsigned tile_y)
            : box(34, 34, 40), type(type)
            {
                box.set_x((tile_x + 0.5) * Map::PHYS_TILE_SIZE);
                box.set_y((tile_y) * Map::PHYS_TILE_SIZE);
                box.set_z(box.sz() / 2);

                if (t == TYPE_HORZ)
                    direction = DIR_RIGHT;
                else
                    direction = DIR_DOWN;
            }
        };
        std::vector<Monster> m_monsters;

        void load_images(Direction dir, const std::string& name)
        {
            m_surfaces[FRAME_WALK_0][dir] = CL_Surface("res/monster/" + name + "-0.png");
            m_surfaces[FRAME_WALK_1][dir] = CL_Surface("res/monster/" + name + "-1.png");
            m_surfaces[FRAME_WALK_2][dir] = m_surfaces[FRAME_WALK_0][dir];
            m_surfaces[FRAME_WALK_3][dir] = CL_Surface("res/monster/" + name + "-2.png");
        }

    public:
        explicit Monsters(Game& game)
        : m_game(game)
        {
            load_images(DIR_LEFT, "left");
            load_images(DIR_RIGHT, "right");
            load_images(DIR_UP, "back");
            load_images(DIR_DOWN, "front");
        }

        void draw()
        {
            for (unsigned i = 0; i < m_monsters.size(); ++i)
            {
                unsigned frame = CL_System::get_time() / 100 % FRAME_WALK_NUM;
                m_game.gfx_queue().insert(m_surfaces[frame][m_monsters[i].direction],
                    m_monsters[i].box, true);
            }
        }

        void update()
        {
            for (unsigned i = 0; i < m_monsters.size(); ++i)
            {
                const int speed = 5;

                switch (m_monsters[i].direction)
                {
                case DIR_LEFT:
                    if (m_monsters[i].box.try_move(m_game.map(), speed, -1, 0, 0) < speed)
                        m_monsters[i].direction = DIR_RIGHT;
                    break;
                case DIR_RIGHT:
                    if (+m_monsters[i].box.try_move(m_game.map(), speed, +1, 0, 0) < speed)
                        m_monsters[i].direction = DIR_LEFT;
                    break;
                case DIR_UP:
                    if (m_monsters[i].box.try_move(m_game.map(), speed, 0, -1, 0) < speed)
                        m_monsters[i].direction = DIR_DOWN;
                    break;
                case DIR_DOWN:
                    if (+m_monsters[i].box.try_move(m_game.map(), speed, 0, +1, 0) < speed)
                        m_monsters[i].direction = DIR_UP;
                    break;
                }

                if (m_monsters[i].box.collides(m_game.player().box()))
                    m_game.kill_player(Game::REASON_DEATH);
            }
        }

        void add_monster(Type type, int tile_x, int tile_y)
        {
            m_monsters.push_back(Monster(type, tile_x, tile_y));
        }
    };
}

#endif
