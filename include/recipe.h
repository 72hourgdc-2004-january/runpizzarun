#ifndef OX_RECIPE_H
#define OX_RECIPE_H

#include <fstream>
#include <stdexcept>

namespace Ox
{
    struct Recipe
    {
        std::string name;
        std::string title;
        std::map<std::string, unsigned> ingredients;

        explicit Recipe(const std::string& name)
        {
            this->name = name;
            std::ifstream recipe_file(("res/recipes/" + name + ".txt").c_str());
            getline(recipe_file, title);
            for (std::string str; getline(recipe_file, str); )
                ++ingredients[str];
            if (ingredients.empty())
                throw std::runtime_error("Invalid recipe: " + name);
        }
    };
}

#endif
