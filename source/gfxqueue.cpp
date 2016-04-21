#include "precomp.h"
#include "gfxqueue.h"
#include "box.h"
#include <functional>
#include <queue>
#include <vector>
using namespace std;

struct Ox::GfxQueue::Impl
{
    struct WaitingSurface
    {
        CL_Surface surface;
        int x, y;
        int display_z;

        void draw()
        {
            surface.draw(x, y);
        }
    };

    struct DisplayZCompare : 
        binary_function<const WaitingSurface&, const WaitingSurface&, bool>
    {
        bool operator()(const WaitingSurface& l, const WaitingSurface& r) const
        {
            if (l.display_z > r.display_z)
                return true;
            else if (l.display_z < r.display_z)
                return false;
            else return l.y < r.y;
        }
    };

    priority_queue<WaitingSurface, vector<WaitingSurface>, DisplayZCompare> 
        waiting_surfaces;
    vector<WaitingSurface> waiting_background_surfaces;

    struct WaitingShadow
    {
        CL_Surface surface;
        int x, y;

        void draw()
        {
            float old_r, old_g, old_b, old_a;
            surface.get_color(old_r, old_g, old_b, old_a);
            surface.set_color(0, 0, 0, 0.4);
            surface.set_scale(1, 0.5);
            surface.draw(x, y + surface.get_height() / 2);
            surface.set_scale(1, 1);
            surface.set_color(old_r, old_g, old_b, old_a);
        }
    };
    vector<WaitingShadow> waiting_shadows;
};

Ox::GfxQueue::GfxQueue()
: pimpl(new Impl)
{
}

Ox::GfxQueue::~GfxQueue()
{
}

void Ox::GfxQueue::insert(const CL_Surface& surface, const Box& box, bool with_shadow,
    int offs_y)
{
    Impl::WaitingSurface new_surf;
    new_surf.surface = surface;
    new_surf.x = box.x() - surface.get_width() / 2;
    new_surf.y = (box.y() - box.z() - surface.get_height()) / 2 + offs_y;
    new_surf.display_z = box.y() + box.z() - box.sy() / 2;

    if (box.z() == 0 && box.sz() == 0)
        return pimpl->waiting_background_surfaces.push_back(new_surf);

    pimpl->waiting_surfaces.push(new_surf);

    if (with_shadow)
    {
        Impl::WaitingShadow new_shad;
        new_shad.surface = surface;
        new_shad.x = box.x() - surface.get_width() / 2;
        new_shad.y = (box.y() - box.sz() / 2 - surface.get_height()) / 2 + offs_y;
        pimpl->waiting_shadows.push_back(new_shad);
    }
}

void Ox::GfxQueue::draw()
{
    for (unsigned i = 0; i < pimpl->waiting_background_surfaces.size(); ++i)
        pimpl->waiting_background_surfaces[i].draw();
    pimpl->waiting_background_surfaces.clear();

    for (unsigned i = 0; i < pimpl->waiting_shadows.size(); ++i)
        pimpl->waiting_shadows[i].draw();
    pimpl->waiting_shadows.clear();

    while (!pimpl->waiting_surfaces.empty())
    {
        pimpl->waiting_surfaces.top().draw();
        pimpl->waiting_surfaces.pop();
    }
}