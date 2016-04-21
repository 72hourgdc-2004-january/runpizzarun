#include "precomp.h"
#include "fatguy.h"
#include "game.h"
#include "pathfinder.h"
#include "gfxqueue.h"
#include "direction.h"
#include "player.h"
#include "box.h"
#include <cmath>
#include <vector>
using namespace std;

struct Ox::FatGuy::Impl
{
    Game* game;
    Box box;

    enum Frame
    {
        FRAME_WALK_0,
        FRAME_WALK_1,
        FRAME_WALK_2,
        FRAME_WALK_3,
        FRAME_WALK_4,
        FRAME_WALK_5,
        FRAME_WALK_6,
        FRAME_WALK_7,
        FRAME_WALK_NUM,
        FRAME_NUM = FRAME_WALK_NUM
    };

    CL_Surface surfaces[FRAME_NUM][DIR_NUM];
    CL_SoundBuffer eat;

    enum Action
    {
        ACT_IDLE,
        ACT_WALK
    } action;
    Direction direction;

    boost::scoped_ptr<PathFinder> path_finder;
    int old_player_x, old_player_y;

    vector<CL_SoundBuffer> sounds;
    unsigned ticks_silent;

    Impl()
    : box(30, 30, 40)
    {
    }

    void load_surfaces(Direction dir, const std::string& prefix)
    {
        surfaces[FRAME_WALK_0][dir] = CL_Surface("res/fatguy/" + prefix + "-0.png");
        surfaces[FRAME_WALK_1][dir] = CL_Surface("res/fatguy/" + prefix + "-1.png");
        surfaces[FRAME_WALK_2][dir] = CL_Surface("res/fatguy/" + prefix + "-2.png");
        surfaces[FRAME_WALK_3][dir] = surfaces[FRAME_WALK_1][dir];
        surfaces[FRAME_WALK_4][dir] = surfaces[FRAME_WALK_0][dir];
        surfaces[FRAME_WALK_5][dir] = CL_Surface("res/fatguy/" + prefix + "-3.png");
        surfaces[FRAME_WALK_6][dir] = CL_Surface("res/fatguy/" + prefix + "-4.png");
        surfaces[FRAME_WALK_7][dir] = surfaces[FRAME_WALK_5][dir];
    }
};

Ox::FatGuy::FatGuy(Game& game)
: pimpl(new Impl)
{
    pimpl->game = &game;

    pimpl->action = Impl::ACT_IDLE;
    pimpl->direction = DIR_DOWN;

    pimpl->load_surfaces(DIR_LEFT, "left");
    pimpl->load_surfaces(DIR_RIGHT, "right");
    pimpl->load_surfaces(DIR_UP, "back");
    pimpl->load_surfaces(DIR_DOWN, "front");
    pimpl->eat = CL_SoundBuffer("res/fatguy/eat.ogg");

    pimpl->old_player_x = -1;
    pimpl->old_player_y = -1;

    box().set_x(Map::PHYS_TILE_SIZE / 0.5);
    box().set_y(Map::PHYS_TILE_SIZE / 0.5);
    box().set_z(box().sz() / 2);

    CL_DirectoryScanner scanner;
    if (scanner.scan("res/fatguy", "random-*.ogg"))
        while (scanner.next())
            pimpl->sounds.push_back(CL_SoundBuffer(scanner.get_pathname()));

    pimpl->ticks_silent = 0;
}

Ox::FatGuy::~FatGuy()
{
}

const Ox::Box& Ox::FatGuy::box() const
{
    return pimpl->box;
}

Ox::Box& Ox::FatGuy::box()
{
    return pimpl->box;
}

void Ox::FatGuy::draw()
{
    unsigned frame = Impl::FRAME_WALK_0;
    if (pimpl->action == Impl::ACT_WALK)
        frame = Impl::FRAME_WALK_0 + CL_System::get_time() / 70 % Impl::FRAME_WALK_NUM;
    pimpl->game->gfx_queue().insert(pimpl->surfaces[frame][pimpl->direction], box(), true);
}

void Ox::FatGuy::update()
{
    // Create path_finder on first update() - otherwise, we get in trouble
    // concerning order of creation :|
    if (!pimpl->path_finder)
        pimpl->path_finder.reset(new PathFinder(pimpl->game->map()));

    Player& plr = pimpl->game->player();

    // Hey, nice fatty pizza!
    if (box().collides(plr.box()))
    {
        pimpl->game->kill_player(Game::REASON_DEATH);
        pimpl->eat.play();
        return;
    }

    // Calculate the position of both characters in map coordinates
    int self_x = box().x() / Map::PHYS_TILE_SIZE;
    int self_y = box().y() / Map::PHYS_TILE_SIZE;
    int plr_x = plr.box().x() / Map::PHYS_TILE_SIZE;
    int plr_y = plr.box().y() / Map::PHYS_TILE_SIZE;

    // Have we arrived at the player?
    if (self_x == plr_x && self_y == plr_y)
    {
        // Yep: Go idle;
        pimpl->action = Impl::ACT_IDLE;
        return;
    }

    // Player has moved - time to update the path.
    if (pimpl->old_player_x != plr_x || pimpl->old_player_y != plr_y)
    {
        // Remember current coordinates to check next frame.
        pimpl->old_player_x = plr_x;
        pimpl->old_player_y = plr_y;

        // Try to update the path. If no path was found, become idle.
        if (pimpl->path_finder->update_path(self_x, self_y, plr_x, plr_y) == 0)
            pimpl->action = Impl::ACT_IDLE;
        else
            pimpl->action = Impl::ACT_WALK;
    }

    if (pimpl->action == Impl::ACT_WALK)
    {
        // Sometimes play a sound so the player gets horribly frightened.
        if (!pimpl->sounds.empty() && ++pimpl->ticks_silent > 250 && rand() % 100 == 0)
        {
            pimpl->sounds[rand() % pimpl->sounds.size()].play();
            pimpl->ticks_silent = 0;
        }


        // We're not idle, so we have to move towards the player.

        int target_x = (pimpl->path_finder->tile_x() + 0.5) * Map::PHYS_TILE_SIZE;
        int target_y = (pimpl->path_finder->tile_y()) * Map::PHYS_TILE_SIZE;
        if (abs(self_x - plr_x) + abs(self_y - plr_y) < 2)
            target_x = plr.box().x(), target_y = plr.box().y();

        int dx = target_x - box().x();
        int dy = target_y - box().y();
        if (abs(dx) > abs(dy))
        {
            // Display the horizontal movement animation
            pimpl->direction = dx < 0 ? DIR_LEFT : DIR_RIGHT;
        }
        else
        {
            // Display the vertical movement animation
            pimpl->direction = dy < 0 ? DIR_UP : DIR_DOWN;
        }

        for (unsigned i = 0; i < 2; ++i)
        {
            if (box().x() > target_x)
                // PathFinder says: To the left
                box().try_move(pimpl->game->map(), 1, -1, 0, 0);
            else if (box().x() < target_x)
                // PathFinder says:: To the right
                box().try_move(pimpl->game->map(), 1, +1, 0, 0);
        }
    
        for (unsigned i = 0; i < 2; ++i)
        {
            if (box().y() > target_y)
                // PathFinder says: Up
                box().try_move(pimpl->game->map(), 1, 0, -1, 0);
            else if (box().y() < target_y)
                // PathFinder says: Down
                box().try_move(pimpl->game->map(), 1, 0, +1, 0);
        }

        if (box().x() / Map::PHYS_TILE_SIZE == pimpl->path_finder->tile_x() &&
            box().y() / Map::PHYS_TILE_SIZE == pimpl->path_finder->tile_y())
            // Followed PathFinder, ask him for next tile:
            if (pimpl->path_finder->next_tile() == 0)
                // He says we're there - go idle
                pimpl->action = Impl::ACT_IDLE;
    }
}