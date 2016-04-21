#ifndef OX_INGREDIENT_H
#define OX_INGREDIENT_H

#include "box.h"
#include "player.h"
#include "customers.h"
#include "rats.h"
#include <cstdlib>
#include <cmath>

namespace Ox
{
    class Ingredient
    {
        Game* m_game;
        std::string m_name;
        CL_Surface m_surface;
        Box m_box;
        int m_vx, m_vy, m_vz;
        bool m_hit_floor;
        bool m_collected;

    public:
        Ingredient(Game& game, const std::string& name, const CL_Surface& surface,
            int x, int y, int z)
        : m_game(&game), m_name(name), m_surface(surface), m_box(10, 10, 6),
            m_hit_floor(false), m_collected(false)
        {
            m_box.set_x(x);
            m_box.set_y(y);
            m_box.set_z(z);

            double angle = (std::rand() % 360) * 0.01745329252;
            m_vx = std::cos(angle) * 5;
            m_vy = std::sin(angle) * 5;
            m_vz = std::rand() % 5 + 15;
        }

        void update()
        {
            if (collected())
                return;

            // First check for collision with the player
            if (m_box.collides(m_game->player().box()))
            {
                // Notify the customer of our collection...
                m_game->customers().collect_ingredient(m_name);
                // ...and disappear
                m_collected = true;
                return;
            }

            // Then check if the evil rats eat us
            if (m_game->rats().eat_ingredient(m_box))
            {
                m_collected = true;
                return;
            }

            // Now on to the movement...

            if (m_hit_floor)
                return;

            // Still flying!
            m_box.try_move(m_game->map(), m_vx, 1, 0, 0);
            m_box.try_move(m_game->map(), m_vy, 0, 1, 0);
            if (m_box.try_move(m_game->map(), m_vz, 0, 0, 1) != m_vz)
                // Ouch, hit the floor :(
                m_hit_floor = true;
            else
                // Yaaay!
                --m_vz;
        }

        bool collected() const
        {
            return m_collected;
        }

        void draw()
        {
            if (m_collected)
                return;
            m_game->gfx_queue().insert(m_surface, m_box, true);
        }
    };
}

#endif
