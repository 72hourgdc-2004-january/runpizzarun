

#include "precomp.h"
#include "direction.h"
#include "pathfinder.h"
#include <iostream>
#include <cstdio>

#define PATH_MAX 0xFFFFFF

namespace Ox {



	//////////////////////////////////////////////////////////////////////////
	PathFinder::PathFinder(Map &m) {
		int i,j;

		size_x = m.TILES_X;
		size_y = m.TILES_Y;
		for (j=0;j<size_y;j++) {
			for (i=0;i<size_x;i++) {
				map[i][j] = (m.is_tile_solid(i,j))?1:0;
			}
		}
		path_size = 0;
		cur_path = 0;
	}			

	
	//////////////////////////////////////////////////////////////////////////
	int PathFinder::ProcessPoint(int x, int y, int step ) {
        if ((x<0) || (x>=size_x)) return 0;
		if ((y<0) || (y>=size_y)) return 0;
        if (num_points>=3000) return 0;
        if (map[x][y]!=0) return 0;

		if (map[x][y]==1) return 0;
		
		if ((tmp[x][y]>=step) || (tmp[x][y]==0)) {
			tmp[x][y] = step;
			point[num_points].x = x;
			point[num_points].y = y;
			num_points ++;
			return step;
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
   	int PathFinder::RightPath(	int x, int y, int step  ) {
		if ((x<0) || (x>=size_x)) return 0;
		if ((y<0) || (y>=size_y)) return 0;

		if (tmp[x][y]<step && tmp[x][y]!=0) return 1;

		return 0;
	}


	//////////////////////////////////////////////////////////////////////////
	int PathFinder::update_path(int start_x, int start_y, int dest_x, int dest_y ) {
		m_Point ptmp[3000];
		int x,y,i,j;
		int done = 0;
		int step = 1;
		int np;

		num_points = 0;
		np = 1;
		ptmp[0].x = start_x;
		ptmp[0].y = start_y;

		if (start_x==dest_x && start_y==dest_y) done = 1;
		if (map[dest_x][dest_y]==1) {
			//
			//
			// Try to find paths to surrounding tiles ..
			//
			//
			int min=PATH_MAX,p,c = MAX_PATH;

			if (dest_y-1>=0)
			if (map[dest_x][dest_y-1]==0)
				c = update_path (start_x,start_y,dest_x+0,dest_y-1);
			if (c && c<min) { min = c; p=0; }

			if (dest_y+1<=size_y)
			if (map[dest_x][dest_y+1]==0)
				c = update_path (start_x,start_y,dest_x+0,dest_y+1);
			if (c && c<min) { min = c; p=1; }

			if (dest_x-1>=0)
			if (map[dest_x-1][dest_y]==0)
				c = update_path (start_x,start_y,dest_x-1,dest_y);
			if (c && c<min) { min = c; p=2; }

			if (dest_x+1<=size_x)
			if (map[dest_x+1][dest_y]==0)
				c = update_path (start_x,start_y,dest_x+1,dest_y);
			if (c && c<min) { min = c; p=3; }

			switch(p) {
				case 0:
					update_path (start_x,start_y,dest_x+0,dest_y-1);
					break;
				case 1:
					update_path (start_x,start_y,dest_x+0,dest_y+1);
					break;
				case 2:
					update_path (start_x,start_y,dest_x-1,dest_y);
					break;
				case 3:
					update_path (start_x,start_y,dest_x+1,dest_y);
					break;

				default:
					return 0;
			}
		}

		for (j=0;j<size_y;j++)
			for (i=0;i<size_x;i++)
				tmp[i][j] = 0;

		num_points = 0;
		tmp[start_x][start_y] = -1;

		// Process the field
		while (!done) {
			if (np==0) break;
			if (np>=3000) break;		// something's wrong :P

			for (i=0;i<np;i++) {
				x = ptmp[i].x;
				y = ptmp[i].y;
				ProcessPoint(x,y+1,step);
				ProcessPoint(x,y-1,step);
				ProcessPoint(x-1,y,step);
				ProcessPoint(x+1,y,step);
				
				if (tmp[dest_x][dest_y]) break;
			}
			
			// Copy to temp
			np = num_points;
			if (np>=3000) break;		// something's wrong :P
			for (i=0;i<num_points;i++) {
				ptmp[i].x = point[i].x;
				ptmp[i].y = point[i].y;
			}

			num_points = 0;
			done = 0;
			step ++;
		}

		// Build path
		path_size = tmp[dest_x][dest_y];
		
		// Get nearest matching path if possible + ...
		// (not working correctly ??)
/*		if (path_size==0) {
			int min=4,p=PATH_MAX;
			
			if (RightPath(dest_x,dest_y-1,PATH_MAX)&&tmp[dest_x][dest_y-1]<p) 
				{ min = 0; p=tmp[dest_x][dest_y-1]; }

			if (RightPath(dest_x,dest_y+1,PATH_MAX)&&tmp[dest_x][dest_y+1]<p) 
				{ min = 1; p=tmp[dest_x][dest_y+1]; }
			
			if (RightPath(dest_x-1,dest_y,PATH_MAX)&&tmp[dest_x-1][dest_y]<p) 
				{ min = 2; p=tmp[dest_x-1][dest_y]; }
			
			if (RightPath(dest_x+1,dest_y,PATH_MAX)&&tmp[dest_x+1][dest_y]<p) 
				{ min = 3; p=tmp[dest_x+1][dest_y]; }

			switch (min) {
				case 0:
					dest_y--;
					break;
				case 1:
					dest_y++;
					break;
				case 2:
					dest_x--;
					break;
				case 3:
					dest_x++;
					break;

				default:
					return 0;
			}
			path_size = tmp[dest_x][dest_y];
		}
*/
		x = dest_x;
		y = dest_y;

		for (path_size--;path_size>=0;path_size--) {
			path[path_size].x = x;
			path[path_size].y = y;

			if (RightPath(x,y-1,tmp[x][y]))	{ x = x+0;	y= y-1; continue;	}
			if (RightPath(x,y+1,tmp[x][y]))	{ x = x+0;	y= y+1; continue;	}
			if (RightPath(x-1,y,tmp[x][y]))	{ x = x-1;	y= y+0; continue;	}
			if (RightPath(x+1,y,tmp[x][y]))	{ x = x+1;	y= y+0; continue;	}
		}
		path_size = tmp[dest_x][dest_y]-1;
		cur_path = 0;

		return path_size;
	}

	//////////////////////////////////////////////////////////////////////////
	int PathFinder::tile_x () {
		return path[cur_path].x;
	}

	//////////////////////////////////////////////////////////////////////////
	int PathFinder::tile_y () {
		return path[cur_path].y;
	}

	//////////////////////////////////////////////////////////////////////////
	int PathFinder::tile_direction () {
		int dir;

		if (cur_path==0) return DIR_DOWN;
		
		if (path[cur_path-1].y > path[cur_path].y) dir = DIR_UP;
		if (path[cur_path-1].y < path[cur_path].y) dir = DIR_DOWN;

		if (path[cur_path-1].x > path[cur_path].x) dir = DIR_LEFT;
		if (path[cur_path-1].x < path[cur_path].x) dir = DIR_RIGHT;

		return dir;
	}

	//////////////////////////////////////////////////////////////////////////
	int PathFinder::next_tile () {
		if (cur_path >= path_size) 
			return -1;
		return ++cur_path;
	}
	
}