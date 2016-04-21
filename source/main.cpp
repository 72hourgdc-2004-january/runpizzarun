#include "precomp.h"
#include "titlescreen.h"
#include <ClanLib/application.h>
#include <boost/utility.hpp>
#include <exception>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <sstream>
using namespace std;

namespace Ox
{
    template<typename Setup>
    struct ClanLib : boost::noncopyable
    {
        ClanLib()
        {
            Setup::init();
        }

        ~ClanLib()
        {
            Setup::deinit();
        }
    };

    class App : CL_ClanApplication
    {
        SubAppMgr* mgr_marker;

        int main(int argc, char** argv)
        {
            srand(CL_System::get_time());

            // Hacky unportable stuff :(
            #ifdef WIN32
            char path_buf[MAX_PATH * 2];
            GetModuleFileName(0, path_buf, sizeof(path_buf));
            string path = path_buf;
            string::reverse_iterator last_slash = 
                find(path.rbegin(), path.rend(), '\\');
            assert(last_slash != path.rend());
            string dir = string(path.begin(), last_slash.base());
            SetCurrentDirectory(dir.c_str());
            #endif

            try
            {
                ClanLib<CL_SetupCore> core;
                ClanLib<CL_SetupDisplay> display;
                ClanLib<CL_SetupGL> gl;
                ClanLib<CL_SetupSound> sound;
                ClanLib<CL_SetupVorbis> vorbis;

                #ifdef NDEBUG
                CL_DisplayWindow window("Run Pizza Run!", 800, 600/*, true*/);
                #else
                CL_DisplayWindow window("Run Pizza Run!", 800, 600);
                #endif

                CL_SoundOutput output(44100);

                CL_Slot slot_quit = window.sig_window_close().connect(this,
                    &App::on_window_close);

                CL_Mouse::hide();

                Ox::SubAppMgr mgr(window.get_ic());
                mgr_marker = &mgr;
                mgr.set_new_app(std::auto_ptr<SubApp>(new TitleScreen(mgr)));

                unsigned last_second = 0;
                unsigned accum_frames = 0;

                while (!mgr.terminated())
                {
                    unsigned time = CL_System::get_time();
                    if (time / 1000 == last_second)
                        ++accum_frames;
                    else
                    {
                        ostringstream caption;
                        caption << "Run Pizza Run! - " << accum_frames << " FPS";
                        window.set_title(caption.str());
                        accum_frames = 1;
                        last_second = time / 1000;
                    }
                    mgr.do_tick();
                }
            }
            #ifdef WIN32
            catch (const exception& ex)
            {
                ::MessageBox(0, ex.what(), "Error",
                    MB_OK | MB_ICONERROR);
                return 1;
            }
            catch (const CL_Error& err)
            {
                ::MessageBox(0, err.message.c_str(), "ClanLib Error",
                    MB_OK | MB_ICONERROR);
                return 1;
            }
            #endif

            return 0;
        }

        void on_window_close()
        {
            assert(mgr_marker);
            mgr_marker->terminate();
        }
    } app;
}