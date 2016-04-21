#include "precomp.h"
#include "titlescreen.h"
#include "instructions.h"
#include "credits.h"
#include "highscores.h"

struct Ox::TitleScreen::Impl
{
    TitleScreen& self;
    CL_Surface screen, cursor;
    CL_SoundBuffer click;

    Impl(TitleScreen& owner)
    : self(owner)
    {
    }

    void on_click(const CL_InputEvent& event)
    {
        if (event.id != CL_MOUSE_LEFT || event.mouse_pos.x < 204)
            return;

        if (event.mouse_pos.x <= 444 &&
            event.mouse_pos.y >= 252 &&
            event.mouse_pos.y <= 311)
        {
            click.play();
            self.mgr().set_new_app(std::auto_ptr<SubApp>(new Instructions(self.mgr(), 0, 7, 0)));
        }
        else if (event.mouse_pos.x <= 496 &&
            event.mouse_pos.y >= 333 &&
            event.mouse_pos.y <= 387)
        {
            click.play();
            self.mgr().set_new_app(std::auto_ptr<SubApp>(new HighScores(self.mgr(), 0)));
        }
        else if (event.mouse_pos.x <= 399 &&
            event.mouse_pos.y >= 406 &&
            event.mouse_pos.y <= 460)
        {
            click.play();
            self.mgr().set_new_app(std::auto_ptr<SubApp>(new Credits(self.mgr())));
        }
        else if (event.mouse_pos.x <= 328 &&
            event.mouse_pos.y >= 476 &&
            event.mouse_pos.y <= 531)
        {
            click.play();
            self.mgr().terminate();
        }
    }

    void on_key_down(const CL_InputEvent& event)
    {
        if (event.id == CL_KEY_ESCAPE)
            self.mgr().terminate();
        /*else if (event.id == CL_KEY_RETURN)
            self.mgr().set_new_app(std::auto_ptr<SubApp>(new Instructions(self.mgr(), 0)));*/
    }

    /*void on_button_down(const CL_InputEvent& event)
    {
        if (event.id == 0)
            self.mgr().set_new_app(std::auto_ptr<SubApp>(new Instructions(self.mgr(), 0)));
    }*/
};

void Ox::TitleScreen::add_slots(CL_SlotContainer& slots)
{
    slots.connect(mgr().ic().get_mouse().sig_key_down(),
        pimpl.get(), &Impl::on_click);
    slots.connect(mgr().ic().get_keyboard().sig_key_down(),
        pimpl.get(), &Impl::on_key_down);
    /*if (mgr().ic().get_joystick_count() > 0)
        slots.connect(mgr().ic().get_joystick().sig_key_down(),
            pimpl.get(), &Impl::on_button_down);*/
}

Ox::TitleScreen::TitleScreen(SubAppMgr& mgr)
: SubApp(mgr), pimpl(new Impl(*this))
{
    pimpl->screen = CL_Surface("res/ui/title.jpg");
    pimpl->cursor = CL_Surface("res/ui/cursor.png");
    pimpl->click = CL_SoundBuffer("res/ui/click.ogg");
}

Ox::TitleScreen::~TitleScreen()
{
}

void Ox::TitleScreen::draw()
{
    pimpl->screen.draw(0, 0);
    pimpl->cursor.draw(mgr().ic().get_mouse().get_x(),
        mgr().ic().get_mouse().get_y());
}

std::string Ox::TitleScreen::song_name() const
{
    return "menu.ogg";
}