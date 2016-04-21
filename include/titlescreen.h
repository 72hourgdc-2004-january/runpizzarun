#ifndef OX_TITLESCREEN_H
#define OX_TITLESCREEN_H

#include "subapp.h"

namespace Ox
{
    class TitleScreen : public SubApp
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

        void add_slots(CL_SlotContainer& slots);

    public:
        explicit TitleScreen(SubAppMgr& mgr);
        ~TitleScreen();

        void draw();
        std::string song_name() const;
    };
}

#endif
