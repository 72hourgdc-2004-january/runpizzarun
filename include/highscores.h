#ifndef OX_HIGHSCORES_H
#define OX_HIGHSCORES_H

#include "subapp.h"
#include "titlescreen.h"
#include <fstream>
#include <sstream>

namespace Ox 
{
    class HighScores : public SubApp
    {
        CL_Surface m_screen;
        bool m_enter_name;
        unsigned m_score;
        std::string m_entered_name;
        struct HighScore
        {
            std::string name, score;
            unsigned score_int;

            HighScore() : score_int(1) {}
        };
        std::vector<HighScore> m_scores;

        void on_key_down(const CL_InputEvent& event)
        {
            if (m_enter_name)
            {
                if (event.id == CL_KEY_ESCAPE)
                {
                    m_enter_name = false;
                }
                else if (event.id == CL_KEY_ENTER)
                {
                    // Stop entering name
                    m_enter_name = false;

                    // Put name into list
                    for (unsigned i = 0; i < m_scores.size(); ++i)
                    {
                        HighScore& score = m_scores[i];
                        if (m_scores[i].score_int < m_score)
                        {
                            HighScore my_score;
                            my_score.name = m_entered_name;
                            my_score.score_int = m_score;
                            std::ostringstream score_conv;
                            score_conv << m_score;
                            my_score.score = score_conv.str();

                            m_scores.insert(m_scores.begin() + i, my_score);
                            break;
                        }
                    }
                    m_scores.resize(10);

                    // Save new highscore list
                    std::ofstream highscores("res/dontopenitsavirus/format_c.xyz",
                        std::ios::trunc);
                    for (unsigned i = 0; i < m_scores.size(); ++i)
                        highscores << m_scores[i].name << std::endl
                            << m_scores[i].score_int << std::endl;
                }
                else if (event.id == CL_KEY_BACKSPACE)
                {
                    if (!m_entered_name.empty())
                        m_entered_name.resize(m_entered_name.size() - 1);
                }
                else if (!event.str.empty())
                {
                    m_entered_name += event.str;
                }
            }
            else if (event.id == CL_KEY_ESCAPE)
                mgr().set_new_app(std::auto_ptr<SubApp>(new TitleScreen(mgr())));
        }

        void add_slots(CL_SlotContainer& slots)
        {
            slots.connect(mgr().ic().get_keyboard().sig_key_down(),
                this, &HighScores::on_key_down);
        }

    public:
        HighScores(SubAppMgr& mgr, unsigned score)
        : SubApp(mgr)
        {
            m_screen = CL_Surface("res/ui/highscores.jpg");

            std::ifstream highscores("res/dontopenitsavirus/format_c.xyz");
            std::string cur_name, cur_score;
            while (getline(highscores, cur_name) && getline(highscores, cur_score))
            {
                HighScore new_score;
                new_score.name = cur_name;
                new_score.score = cur_score;
                new_score.score_int = atoi(cur_score.c_str());
                m_scores.push_back(new_score);
            }
            m_scores.resize(10);

            m_enter_name = score != 0 && score > m_scores.back().score_int;
            m_score = score;
        }

        void draw()
        {
            m_screen.draw();
            if (m_enter_name)
            {
                std::string str = "New highscore! Enter your name:";
                mgr().large_font().draw(400 - mgr().large_font().get_width(str) / 2,
                    250, str);

                str = m_entered_name + '_';
                mgr().large_font().draw(400 - mgr().large_font().get_width(str) / 2,
                    320, str);
            }
            else
            {
                for (unsigned i = 0; i < m_scores.size(); ++i)
                {
                    HighScore& hs = m_scores[i];
                    mgr().large_font().draw(200, 190 + i * 40, hs.name);
                    mgr().large_font().draw(750 - mgr().large_font().get_width(hs.score),
                        190 + i * 40, hs.score);
                }
            }
        }

        std::string song_name() const { return "menu.ogg"; }
    };
}

#endif
