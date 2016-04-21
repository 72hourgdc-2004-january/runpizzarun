#include "precomp.h"
#include "customers.h"
#include "game.h"
#include "recipe.h"
#include "box.h"
#include "ingrcannon.h"
#include "player.h"
#include "pizzaclock.h"
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <map>
#include <cstdlib>
#include <vector>
using namespace std;

struct Ox::Customers::Impl
{
    Game& game;

    vector<CL_Surface> images;
    unsigned cur_image;
    unsigned image_top;
    
    enum 
    {
        STATE_APPEARING,
        STATE_READY,
        STATE_DISAPPEARING
    } state;

    vector<Recipe> loaded_recipes, next_recipes;

    Recipe& cur_recipe()
    {
        if (next_recipes.empty())
        {
            next_recipes = loaded_recipes;
            random_shuffle(next_recipes.begin(), next_recipes.end());
        }
        return next_recipes.back();
    }

    void next_recipe()
    {
        next_recipes.pop_back();
    }

    map<string, unsigned> collected_ingrs;

    CL_SoundBuffer satisfied1, satisfied2;

    PizzaClock clock;

    unsigned happy_customers;
    bool can_win;

    Impl(Game& game_)
    : game(game_), clock(game)
    {
    }
};

Ox::Customers::Customers(Game& game, unsigned level, bool can_win)
: pimpl(new Impl(game))
{
    CL_DirectoryScanner scanner;
    if (scanner.scan("res/customers", "*.png"))
        while (scanner.next())
            pimpl->images.push_back(CL_Surface(scanner.get_pathname()));
    if (pimpl->images.empty())
        throw runtime_error("No customer images found!");
    pimpl->cur_image = rand() % pimpl->images.size();
    pimpl->image_top = 600;
    pimpl->state = Impl::STATE_APPEARING;

    ostringstream recipes_file;
    recipes_file << "levels/" << level << "/recipes.txt";
    ifstream recipes(recipes_file.str().c_str());
    for (string cur_recipe; getline(recipes, cur_recipe); )
        pimpl->loaded_recipes.push_back(Recipe(cur_recipe));
    if (pimpl->loaded_recipes.empty())
        throw runtime_error("Level contains no recipes");

    pimpl->satisfied1 = CL_SoundBuffer("res/misc/satisfied1.ogg");
    pimpl->satisfied2 = CL_SoundBuffer("res/misc/satisfied2.ogg");

    pimpl->happy_customers = 0;
    pimpl->can_win = can_win;
}

Ox::Customers::~Customers()
{
}

void Ox::Customers::collect_ingredient(const std::string& name)
{
    if (pimpl->state == Impl::STATE_READY)
    {
        ostringstream msgstream;

        // This is to beautiful code what goatse is to lesbian porn... :/

        if (pimpl->cur_recipe().ingredients.find(name) !=
                pimpl->cur_recipe().ingredients.end())
        {
            unsigned missing = 
                pimpl->cur_recipe().ingredients[name] -
                    pimpl->collected_ingrs[name];
            
            if (missing > 0)
            {
                pimpl->clock.add_ingredient(name);
                ++pimpl->collected_ingrs[name];
                pimpl->clock.add_ticks(50);
                pimpl->game.do_score(75);
                if (missing > 1)
                    msgstream << "Need " << missing - 1 << " more...";
                else
                    msgstream << "Enough " << name << "!";
            }
            else
            {
                pimpl->clock.add_ticks(100);
                pimpl->game.do_score(25);
                msgstream << "Already enough - Bonus!";
            }
        }
        else
        {
            msgstream << "Don't need that stuff!";
            pimpl->game.do_score(-5);
        }
        pimpl->game.player().show_message(msgstream.str());
    }
}

void Ox::Customers::player_died()
{
    pimpl->clock.reset();
    for (std::map<string, unsigned>::iterator i = pimpl->collected_ingrs.begin();
        i != pimpl->collected_ingrs.end(); ++i)
    {
        for (unsigned j = 0; j < i->second; ++j)
            pimpl->game.ingr_cannon().refill(i->first);
    }
    pimpl->collected_ingrs.clear();
}

void Ox::Customers::draw()
{
    pimpl->images[pimpl->cur_image].draw(650, pimpl->image_top);
    pimpl->clock.draw(pimpl->image_top + 250, 75);

    if (pimpl->state == Impl::STATE_READY)
    {
        CL_Font& font = pimpl->game.mgr().font();
        Recipe& rec = pimpl->cur_recipe();

        font.draw(660, 300, "I'd like a Pizza");
        font.draw(660, 320, rec.title + ":");

        unsigned counter = 0;
        for (map<string, unsigned>::iterator i = rec.ingredients.begin();
            i != rec.ingredients.end(); ++i)
        {
            ostringstream line;
            line << i->second << "x " << i->first;
            unsigned missing = i->second - pimpl->collected_ingrs[i->first];
            if (missing == 0)
                line << " - done!";
            else
                line << " - " << missing << " left";
            font.draw(660, 340 + counter++ * 20, line.str());
        }
    }
}

void Ox::Customers::update()
{
    switch (pimpl->state)
    {
        case Impl::STATE_APPEARING:
        {
            pimpl->image_top -= 5;
            if (pimpl->image_top == 400)
                pimpl->state = Impl::STATE_READY;
            break;
        }

        case Impl::STATE_READY:
        {
            pimpl->clock.update();
            if (pimpl->clock.has_run_out())
            {
                pimpl->game.kill_player(Game::REASON_TIMEOUT);
                return;
            }
            Recipe& rec = pimpl->cur_recipe();
            for (map<string, unsigned>::iterator i = rec.ingredients.begin();
                i != rec.ingredients.end(); ++i)
            {
                if (pimpl->collected_ingrs[i->first] < i->second)
                    return;
            }
            pimpl->state = Impl::STATE_DISAPPEARING;
            if (rand() % 2)
                pimpl->satisfied1.play();
            else
                pimpl->satisfied2.play();
            ++pimpl->happy_customers;

            break;
        }

        case Impl::STATE_DISAPPEARING:
        {
            pimpl->image_top += 5;
            if (pimpl->image_top == 600)
            {
                if (pimpl->happy_customers > 5 && pimpl->can_win)
                    return pimpl->game.win();
                pimpl->collected_ingrs.clear();
                pimpl->state = Impl::STATE_APPEARING;
                pimpl->clock.reset();
                pimpl->cur_image = rand() % pimpl->images.size();
                pimpl->next_recipe();
            }
            break;
        }
    }
}