#include "precomp.h"
#include "game.h"
#include "gfxqueue.h"
#include "map.h"
#include "player.h"
#include "credits.h"
#include "instructions.h"
#include "titlescreen.h"
#include "customers.h"
#include "highscores.h"
#include "pathfinder.h"
#include "box.h"
#include "fatguy.h"
#include "ingrcannon.h"
#include "rats.h"
#include "sysutils.h"
#include "monsters.h"
#include "texts.h"
#include <sstream>
#include <memory>
#include <fstream>
using namespace std;

struct Ox::Game::Impl
{
    Game& self;
    unsigned level;
    bool loop;
    CL_Surface sidebar, try_again, game_over;
    CL_Surface reasons[REASON_NUM];
    Reason reason;
    unsigned try_again_alpha;
    CL_SoundBuffer player_death;
    Customers customers;
    GfxQueue gfx_queue;
    Texts texts;
    Map map;
    Player player;
    FatGuy fat_guy;
    IngrCannon ingr_cannon;
    Rats rats;
    Monsters monsters;
    unsigned score;
    unsigned lives;
    unsigned player_start_x, player_start_y;
    string song;

    Impl(Game& self, unsigned lev, bool loop)
    : self(self), level(lev), customers(self, level, !loop), texts(self), map(self),
        player(self), fat_guy(self), ingr_cannon(self, level), rats(self),
        monsters(self)
    {
    }

    void on_key_down(const CL_InputEvent& event)
    {
        if (event.id == CL_KEY_ESCAPE)
        {
            std::auto_ptr<SubApp> new_app(new HighScores(self.mgr(), score));
            self.mgr().set_new_app(new_app);
        }
    }

    void respawn_player()
    {
        player.box().set_x((player_start_x + 0.5) * Map::PHYS_TILE_SIZE);
        player.box().set_y((player_start_y) * Map::PHYS_TILE_SIZE);
        player.box().set_z(100);
    }
};

void Ox::Game::add_slots(CL_SlotContainer& slots)
{
    slots.connect(mgr().ic().get_keyboard().sig_key_down(),
        pimpl.get(), &Impl::on_key_down);
}

Ox::Game::Game(SubAppMgr& mgr, unsigned level, unsigned lives, unsigned score)
: SubApp(mgr)
{
    ostringstream next_file_name;
    next_file_name << "levels/" << level + 1 << "/level.txt";
    bool loop = !file_exists(next_file_name.str());

    pimpl.reset(new Impl(*this, level, loop));

    pimpl->loop = loop;
    pimpl->sidebar = CL_Surface("res/ui/sidebar.jpg");
    pimpl->try_again = CL_Surface("res/ui/try_again.png");
    pimpl->game_over = CL_Surface("res/ui/game_over.png");
    pimpl->reasons[REASON_TIMEOUT] = CL_Surface("res/ui/timeout.png");
    pimpl->reasons[REASON_DEATH] = CL_Surface("res/ui/death.png");
    pimpl->try_again_alpha = 0;
    pimpl->score = score;
    pimpl->lives = lives;
    pimpl->player_death = CL_SoundBuffer("res/player/death.ogg");

    vector<string> songs;
    CL_DirectoryScanner scanner;
    if (scanner.scan("res/music", "game*.ogg"))
        while (scanner.next())
            songs.push_back(scanner.get_name());
    if (!songs.empty())
        pimpl->song = songs[rand() % songs.size()];

    ostringstream filename;
    filename << "levels/" << level << "/level.txt";
    ifstream levfile(filename.str().c_str());

    // Reasonable default values
    pimpl->player_start_x = Map::TILES_X - 1;
    pimpl->player_start_y = Map::TILES_Y - 1;

    for (unsigned y = 0; y < Map::TILES_Y; ++y)
    {
        string line;
        if (!getline(levfile, line) || line.length() != Map::TILES_X)
            throw std::runtime_error("Bad level file: " + filename.str());

        for (unsigned x = 0; x < Map::TILES_X; ++x)
        {
            // Alphanumeric characters: These are ids of tiles.
            if (isalnum(line[x]))
            {
                char obs_id = isdigit(line[x]) ? line[x] : tolower(line[x]);
                map().add_tile(obs_id, x, y);
            }
            // # == fatty
            else if (line[x] == '#')
            {
                fat_guy().box().set_x((x + 0.5) * Map::PHYS_TILE_SIZE);
                fat_guy().box().set_y((y) * Map::PHYS_TILE_SIZE);
            }
            // @ == player
            else if (line[x] == '@')
            {
                pimpl->player_start_x = x;
                pimpl->player_start_y = y;
            }
            // * == rat
            else if (line[x] == '*')
            {
                rats().add_rat(x, y);
            }
            // - == HORZ monster
            else if (line[x] == '-')
            {
                monsters().add_monster(Monsters::TYPE_HORZ, x, y);
            }
            // | == VERT monster
            else if (line[x] == '|')
            {
                monsters().add_monster(Monsters::TYPE_VERT, x, y);
            }
        }
    }

    pimpl->respawn_player();
}

Ox::Game::~Game()
{
}

void Ox::Game::draw()
{
    map().draw();
    player().draw();

    fat_guy().draw();
    rats().draw();
    monsters().draw();

    ingr_cannon().draw();

    gfx_queue().draw();
    texts().draw();

    pimpl->sidebar.draw(650, 0);
    customers().draw();

    ostringstream lives;
    lives << "Lives: " << pimpl->lives;
    mgr().font().draw(660, 10, lives.str());

    ostringstream score;
    score << "Score: " << pimpl->score;
    mgr().font().draw(660, 40, score.str());

    if (pimpl->lives == 0)
    {
        pimpl->reasons[pimpl->reason].set_alpha(1.0);
        pimpl->reasons[pimpl->reason].draw(125, 200);
        pimpl->game_over.draw(125, 300);
    }
    else if (pimpl->try_again_alpha > 0)
    {
        pimpl->reasons[pimpl->reason].set_alpha(pimpl->try_again_alpha / 80.0);
        pimpl->reasons[pimpl->reason].draw(125, 200);
        pimpl->try_again.set_alpha(pimpl->try_again_alpha / 80.0);
        pimpl->try_again.draw(125, 300);
    }
}

void Ox::Game::update()
{
    if (pimpl->try_again_alpha > 0)
        --pimpl->try_again_alpha;

    if (pimpl->lives > 0)
    {
        ingr_cannon().update();
        player().update();

        fat_guy().update();
        rats().update();
        monsters().update();

        customers().update();
        texts().update();
    }
}

void Ox::Game::kill_player(Reason reason)
{
    do_score(-125);
    if (reason == REASON_DEATH)
        pimpl->player_death.play();
    pimpl->reason = reason;
    if (--pimpl->lives > 0)
    {
        pimpl->respawn_player();
        pimpl->try_again_alpha = 80;
        customers().player_died();
    }
}

void Ox::Game::do_score(int delta)
{
    if ((int)pimpl->score + delta < 0)
        pimpl->score = 0;
    else
        pimpl->score += delta;
}

void Ox::Game::win()
{
    do_score(pimpl->level * 250);
    std::auto_ptr<SubApp> new_app(new Instructions(mgr(), pimpl->level + 1,
        pimpl->lives, pimpl->score));
    mgr().set_new_app(new_app);
}

string Ox::Game::song_name() const
{
    return pimpl->song;
}

#define IMPLEMENT_ACCESSOR(Type, name) \
    const Ox::Type& Ox::Game::name() const { return pimpl->name; } \
    Ox::Type& Ox::Game::name() { return pimpl->name; }

IMPLEMENT_ACCESSOR(Customers, customers);
IMPLEMENT_ACCESSOR(GfxQueue, gfx_queue);
IMPLEMENT_ACCESSOR(Texts, texts);
IMPLEMENT_ACCESSOR(Map, map);
IMPLEMENT_ACCESSOR(Player, player);
IMPLEMENT_ACCESSOR(FatGuy, fat_guy);
IMPLEMENT_ACCESSOR(IngrCannon, ingr_cannon);
IMPLEMENT_ACCESSOR(Rats, rats);
IMPLEMENT_ACCESSOR(Monsters, monsters);
