#ifndef OX_SUBAPP_H
#define OX_SUBAPP_H

#include "sysutils.h"
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <list>
#include <memory>

extern CL_SoundOutput* GOUTPUT;

namespace Ox
{
    class SubAppMgr;

    class SubApp : boost::noncopyable
    {
        SubAppMgr& m_mgr;
        CL_SlotContainer m_slots;

    protected:
        virtual void add_slots(CL_SlotContainer& slots)
        {
        }

    public:
        explicit SubApp(SubAppMgr& mgr)
        : m_mgr(mgr)
        {
        }

        virtual ~SubApp()
        {
        }

        const SubAppMgr& mgr() const
        {
            return m_mgr;
        }

        SubAppMgr& mgr()
        {
            return m_mgr;
        }

        virtual void draw() // Not const, ClanLib's const correctness is annoying
        {
        }

        virtual void update()
        {
        }

        void connect_slots()
        {
            add_slots(m_slots);
        }

        void disconnect_slots()
        {
            m_slots = CL_SlotContainer();
        }

        virtual std::string song_name() const
        {
            return "";
        }
    };

    class SubAppMgr : boost::noncopyable
    {
        std::list<boost::shared_ptr<SubApp> > m_garbage;
        std::auto_ptr<SubApp> m_cur_app;
        bool m_cur_app_connected;
        CL_Font m_font, m_large_font;
        CL_InputContext& m_ic;
        std::string m_current_song_name;
        CL_SoundBuffer_Session m_session;
        CL_SoundBuffer m_current_song;

    public:
        SubAppMgr(CL_InputContext* ic)
        : m_font("Arial", 18, 0, true), m_large_font("Arial", 26, 0, true),
            m_ic(*ic)
        {
            assert(ic);
        }

        bool terminated() const
        {
            return m_cur_app.get() == 0;
        }

        void terminate()
        {
            if (!terminated())
            {
                m_cur_app->disconnect_slots();
                m_garbage.push_back(boost::shared_ptr<SubApp>(m_cur_app));
            }
        }

        void set_new_app(std::auto_ptr<SubApp> new_app)
        {
            terminate();
            m_cur_app = new_app;
            m_cur_app_connected = false;
        }

        void do_tick()
        {
            std::string new_song = m_cur_app->song_name();
            if (!terminated() && new_song != m_current_song_name &&
                (new_song.empty() || file_exists("res/music/" + new_song)))
            {
                if (!m_current_song_name.empty())
                {
                    m_current_song.stop();
                    m_session.stop();
                    m_current_song = CL_SoundBuffer();
                }

                m_current_song_name = new_song;

                if (!new_song.empty())
                {
                    m_current_song = CL_SoundBuffer("res/music/" + new_song);
                    m_session = m_current_song.play(true);
                }
            }

            m_garbage.clear();

            CL_Display::clear();

            if (!m_cur_app_connected)
            {
                m_cur_app->connect_slots();
                m_cur_app_connected = true;
            }

            assert(!terminated());
            m_cur_app->update();

            if (!terminated())
                m_cur_app->draw();

            CL_Display::flip();
            CL_System::keep_alive(20);
        }

        CL_Font& font()
        {
            return m_font;
        }

        CL_Font& large_font()
        {
            return m_large_font;
        }

        CL_InputContext& ic() const
        {
            return m_ic;
        }
    };
}

#endif
