#include "precomp.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "box.h"
#include "texts.h"
#include "gfxqueue.h"

struct Ox::Player::Impl
{
    Game* game;

    enum Frame
    {
        FRAME_WALK_0,
        FRAME_WALK_1,
        FRAME_WALK_2,
        FRAME_WALK_3,
        FRAME_WALK_4,
        FRAME_WALK_5,
        FRAME_WALK_NUM,
        FRAME_JUMP = FRAME_WALK_NUM,
        FRAME_NUM
    };

    enum Action
    {
        ACT_STAND,
        ACT_WALK,
        ACT_JUMP
    } action;

    enum Direction
    {
        DIR_DOWN,
        DIR_UP,
        DIR_LEFT,
        DIR_RIGHT,
        DIR_NUM
    } direction;

    CL_Surface surfaces[FRAME_NUM][DIR_NUM];
    Box box;
    int vz;

    CL_SoundBuffer jump, fall;

    Impl()
    : box(30, 30, 44)
    {
    }

    void load_surfaces(Direction dir, const std::string& filename_base)
    {
        surfaces[FRAME_WALK_0][dir] = 
            CL_Surface("res/player/" + filename_base + "-0.png");
        surfaces[FRAME_WALK_1][dir] = 
            CL_Surface("res/player/" + filename_base + "-1.png");
        surfaces[FRAME_WALK_2][dir] = 
            CL_Surface("res/player/" + filename_base + "-2.png");
        surfaces[FRAME_WALK_3][dir] = 
            surfaces[FRAME_WALK_0][dir];
        surfaces[FRAME_WALK_4][dir] = 
            CL_Surface("res/player/" + filename_base + "-3.png");
        surfaces[FRAME_WALK_5][dir] = 
            CL_Surface("res/player/" + filename_base + "-4.png");
        surfaces[FRAME_JUMP][dir] = 
            CL_Surface("res/player/" + filename_base + "-jump.png");
    }

    bool is_on_floor()
    {
        Box under_me = box;
        under_me.set_z(box.z() - 1);
        return (!game->map().is_box_free(under_me));
    }
};

Ox::Player::Player(Game& game)
: pimpl(new Impl)
{
    pimpl->game = &game;

    pimpl->load_surfaces(Impl::DIR_DOWN, "front");
    pimpl->load_surfaces(Impl::DIR_UP, "back");
    pimpl->load_surfaces(Impl::DIR_LEFT, "left");
    pimpl->load_surfaces(Impl::DIR_RIGHT, "right");

    pimpl->action = Impl::ACT_STAND;
    pimpl->direction = Impl::DIR_DOWN;
    pimpl->vz = 0;

    pimpl->jump = CL_SoundBuffer("res/player/jump.ogg");
    pimpl->fall = CL_SoundBuffer("res/player/fall.ogg");
}

Ox::Player::~Player()
{
}

const Ox::Box& Ox::Player::box() const
{
    return pimpl->box;
}

Ox::Box& Ox::Player::box()
{
    return pimpl->box;
}

void Ox::Player::draw()
{
    unsigned frame = Impl::FRAME_WALK_0;
    if (pimpl->action == Impl::ACT_WALK)
        frame = Impl::FRAME_WALK_0 + CL_System::get_time() / 60 % Impl::FRAME_WALK_NUM;
    else if (pimpl->action == Impl::ACT_JUMP)
        frame = Impl::FRAME_JUMP;
    CL_Surface& surf = pimpl->surfaces[frame][pimpl->direction];
    pimpl->game->gfx_queue().insert(surf, box(), true, -5);
}

void Ox::Player::update()
{
    bool left, right, up, down, jump;
    {
        CL_InputDevice& keyboard = pimpl->game->mgr().ic().get_keyboard();
        left = keyboard.get_keycode(CL_KEY_LEFT);
        right = keyboard.get_keycode(CL_KEY_RIGHT);
        up = keyboard.get_keycode(CL_KEY_UP);
        down = keyboard.get_keycode(CL_KEY_DOWN);
        jump = keyboard.get_keycode(CL_KEY_SPACE);

        if (pimpl->game->mgr().ic().get_joystick_count() > 0)
        {
            CL_InputDevice& gamepad = pimpl->game->mgr().ic().get_joystick();
            float x_axis = gamepad.get_axis(0);
            if (x_axis < 0)
                left = true;
            else if (x_axis > 0)
                right = true;
            float y_axis = gamepad.get_axis(1);
            if (y_axis < 0)
                up = true;
            else if (y_axis > 0)
                down = true;
            jump = jump || gamepad.get_keycode(0);
        }
    }

    if (jump && pimpl->is_on_floor())
    {
        pimpl->jump.play();
        pimpl->vz = 15;
    }

    pimpl->action = Impl::ACT_STAND;

    if (up)
    {
        box().try_move(pimpl->game->map(), 5, 0, -1, 0);
        pimpl->action = Impl::ACT_WALK;
        pimpl->direction = Impl::DIR_UP;
    }
    if (down)
    {
        box().try_move(pimpl->game->map(), 5, 0, +1, 0);
        pimpl->action = Impl::ACT_WALK;
        pimpl->direction = Impl::DIR_DOWN;
    }
    if (left)
    {
        box().try_move(pimpl->game->map(), 5, -1, 0, 0);
        pimpl->action = Impl::ACT_WALK;
        pimpl->direction = Impl::DIR_LEFT;
    }
    if (right)
    {
        box().try_move(pimpl->game->map(), 5, +1, 0, 0);
        pimpl->action = Impl::ACT_WALK;
        pimpl->direction = Impl::DIR_RIGHT;
    }

    if (!pimpl->is_on_floor())
        pimpl->action = Impl::ACT_JUMP;

    --pimpl->vz;

    if (pimpl->vz > 0)
        box().try_move(pimpl->game->map(), pimpl->vz, 0, 0, 1);
    else if (pimpl->vz < 0)
        // Fall - when hitting the floor, set vz to zero
        if (box().try_move(pimpl->game->map(), -pimpl->vz, 0, 0, -1) < -pimpl->vz)
        {
            if (pimpl->vz < -2)
                pimpl->fall.play();
            pimpl->vz = 0;
        }
}

void Ox::Player::show_message(const std::string& str)
{
    pimpl->game->texts().add(str, box().x(), box().y() / 2);
}