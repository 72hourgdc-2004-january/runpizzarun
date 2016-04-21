

#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__


#include "map.h"
#include "direction.h"


namespace Ox {

	//////////////////////////////////////////////////////////////////////////
	//
	//
	//
	//
	//////////////////////////////////////////////////////////////////////////
	class PathFinder {
	private:
		//////////////////////////////////////////////////////////////////////////
		typedef struct {
			int x,y;
		} m_Point;

		//////////////////////////////////////////////////////////////////////////
		int map[50][50];
		int cur_path;
		int path_size;
        m_Point path[3000];
		int size_x,size_y;
		
		// temp vars
		int num_points;
		m_Point point[3000];

		int ProcessPoint ( int x, int y, int step );
		int RightPath ( int x, int y, int step );

        int tmp[50][50];

	public:
		//////////////////////////////////////////////////////////////////////////
		explicit PathFinder ( Map &m );

		int update_path ( int start_x, int start_y,  int dest_x, int dest_y );
		int tile_x () ;
		int tile_y () ;
		int tile_direction () ;
		int next_tile ();		
	};


}










#endif //__PATHFINDER_H__