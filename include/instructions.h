#ifndef OX_INSTRUCTIONS_H
#define OX_INSTRUCTIONS_H

#include "subapp.h"
#include "game.h"
#include "titlescreen.h"
#include <fstream>
#include <sstream>

namespace Ox
{
    class Instructions : public SubApp
    {
        CL_Surface m_screen;
        unsigned m_level, m_lives, m_score;
        std::vector<std::string> m_lines;

        void add_slots(CL_SlotContainer& slots)
        {
            slots.connect(mgr().ic().get_keyboard().sig_key_down(),
                this, &Instructions::on_key_down);
            if (mgr().ic().get_joystick_count() > 0)
                slots.connect(mgr().ic().get_joystick().sig_key_down(),
                    this, &Instructions::on_button_down);
        }

        void on_key_down(const CL_InputEvent& event)
        {
            if (event.id == CL_KEY_ESCAPE)
                mgr().set_new_app(std::auto_ptr<SubApp>(new TitleScreen(mgr())));
            else if (event.id == CL_KEY_RETURN)
                mgr().set_new_app(std::auto_ptr<SubApp>(new Game(mgr(), m_level, m_lives, m_score)));
        }

        void on_button_down(const CL_InputEvent& event)
        {
            if (event.id == 0)
                mgr().set_new_app(std::auto_ptr<SubApp>(new Game(mgr(), m_level, m_lives, m_score)));
        }

    public:
        Instructions(SubAppMgr& mgr, unsigned level, unsigned lives, unsigned score)
        : SubApp(mgr), m_level(level), m_lives(lives), m_score(score)
        {
            m_screen = CL_Surface("res/ui/instructions.jpg");
            std::ostringstream instr_path;
            instr_path << "levels/" << level << "/instructions.txt";
            std::fstream instr_file(instr_path.str().c_str());
            for (std::string cur_line; getline(instr_file, cur_line); )
                m_lines.push_back(cur_line);
        }

        void draw()
        {
            m_screen.draw();
            for (unsigned i = 0; i < m_lines.size(); ++i)
                mgr().large_font().draw(35, 35 + i * 40, m_lines[i]);
        }

        std::string song_name() const { return "menu.ogg"; }
    };
}

#endif
