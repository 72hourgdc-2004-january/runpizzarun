#ifndef OX_CREDITS_H
#define OX_CREDITS_H

#include "subapp.h"
#include "game.h"
#include "titlescreen.h"

namespace Ox
{
    class Credits : public SubApp
    {
        CL_Surface m_screen;

        void add_slots(CL_SlotContainer& slots)
        {
            slots.connect(mgr().ic().get_keyboard().sig_key_down(),
                this, &Credits::on_key_down);
            if (mgr().ic().get_joystick_count() > 0)
                slots.connect(mgr().ic().get_joystick().sig_key_down(),
                    this, &Credits::on_button_down);
        }

        void on_key_down(const CL_InputEvent& event)
        {
            if (event.id == CL_KEY_ESCAPE || event.id == CL_KEY_ESCAPE)
                mgr().set_new_app(std::auto_ptr<SubApp>(new TitleScreen(mgr())));
        }

        void on_button_down(const CL_InputEvent& event)
        {
            if (event.id == 0)
                mgr().set_new_app(std::auto_ptr<SubApp>(new TitleScreen(mgr())));
        }

    public:
        explicit Credits(SubAppMgr& mgr)
        : SubApp(mgr), m_screen("res/ui/credits.jpg")
        {
        }

        void draw()
        {
            m_screen.draw(0, 0);
        }

        std::string song_name() const
        {
            return "menu.ogg";
        }
    };
}

#endif
