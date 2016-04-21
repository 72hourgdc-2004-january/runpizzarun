#ifndef OX_GFXQUEUE_H
#define OX_GFXQUEUE_H

namespace Ox
{
    class Box;

    // Manages drawing objects in the pseudo-3D space, including Z-ordering.
    // Note that moving all the in-game graphics to a different place on the
    // screen is most easily achieved by changing this class.
    class GfxQueue
    {
        struct Impl;
        boost::scoped_ptr<Impl> pimpl;

    public:
        GfxQueue();
        ~GfxQueue();

        // Queues a surface to be drawn with the next call to draw().
        void insert(const CL_Surface& surface, const Box& box,
            bool with_shadow, int offs_y = 0);

        // Draws all the objects in the queue, then flushes it.
        void draw();
    };
}

#endif
