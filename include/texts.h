#ifndef OX_TEXTS_H
#define OX_TEXTS_H

#include "game.h"

namespace Ox
{
    class Texts
    {
        Game& m_game;

        struct Text
        {
            std::string text;
            int x, y;
            float alpha;
        };
        std::vector<Text> m_texts;

    public:
        Texts(Game& game)
        : m_game(game)
        {
        }

        void draw()
        {
            for (unsigned i = 0; i < m_texts.size(); ++i)
            {
                m_game.mgr().font().set_alpha(m_texts[i].alpha);
                m_game.mgr().font().draw(m_texts[i].x, m_texts[i].y,
                    m_texts[i].text);
                m_game.mgr().font().set_alpha(1);
            }
        }

        void update()
        {
            for (unsigned i = 0; i < m_texts.size(); ++i)
            {
                m_texts[i].alpha -= 0.015;
                m_texts[i].y -= 1;
                if (m_texts[i].alpha <= 0)
                {
                    m_texts.erase(m_texts.begin() + i);
                    --i;
                }
            }
        }

        void add(const std::string& text, int x, int y)
        {
            Text new_text;
            new_text.text = text;
            new_text.x = x - m_game.mgr().font().get_width(text) / 2;
            new_text.y = y - 20;
            new_text.alpha = 1;
            m_texts.push_back(new_text);
        }
    };
}

#endif
