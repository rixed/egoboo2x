// camera.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"

//--------------------------------------------------------------------------------------------
void camera_look_at(float x, float y)
{
	// ZZ> This function makes the camera turn to face the character
	camzgoto = camzadd;
	if(doturntime != 0)
	{
		camturnleftright = (1.5*PI)-atan2(y-camy, x-camx);  // xgg
	}
}

void dump_matrix(GLMATRIX a)
{
	int i; int j;

	for(j=0;j<4;j++)
	{
		printf("  ");
		for(i=0;i<4;i++)
			printf("%f ",(a)_CNV(i,j));
		printf("\n");
	}
}

//--------------------------------------------------------------------------------------------
void project_view()
{
	// ZZ> This function figures out where the corners of the view area
	//     go when projected onto the plane of the mesh.  Used later for
	//     determining which mesh fans need to be rendered

	int cnt, tnc, extra[2];
	float ztemp;
	float numstep;
	float zproject;
	float xfin, yfin, zfin;
	GLMATRIX mTemp;

	// Range
	ztemp = (camz);

	// Topleft
	//printf("DIAG: In project_view\n");
	//printf("DIAG: dumping mView\n"); dump_matrix(mView);
	//printf("cam xyz,zoom = %f %f %f %f\n",camx,camy,camz,camzoom);

	mTemp = MatrixMult(RotateY(-rotmeshtopside*PI/360), mView);
	mTemp = MatrixMult(RotateX(rotmeshup*PI/360), mTemp);
	zproject = (mTemp)_CNV(2,2);									//2,2
	// Camera must look down
	if(zproject < 0)
	{
		numstep = -ztemp/zproject;
		xfin = camx+(numstep*(mTemp)_CNV(0,2));  // xgg			//0,2
		yfin = camy+(numstep*(mTemp)_CNV(1,2));					//1,2
		zfin = 0;
		cornerx[0] = xfin;
		cornery[0] = yfin;
		//printf("Camera TL: %f %f\n",xfin,yfin);
		//dump_matrix(mTemp);
	}

	// Topright
	mTemp = MatrixMult(RotateY(rotmeshtopside*PI/360), mView);
	mTemp = MatrixMult(RotateX(rotmeshup*PI/360), mTemp);
	zproject = (mTemp)_CNV(2,2);									//2,2
	// Camera must look down
	if(zproject < 0)
	{
		numstep = -ztemp/zproject;
		xfin = camx+(numstep*(mTemp)_CNV(0,2));  // xgg			//0,2
		yfin = camy+(numstep*(mTemp)_CNV(1,2));					//1,2
		zfin = 0;
		cornerx[1] = xfin;
		cornery[1] = yfin;
		//printf("Camera TR: %f %f\n",xfin,yfin);
		//dump_matrix(mTemp);
	}

	// Bottomright
	mTemp = MatrixMult(RotateY(rotmeshbottomside*PI/360), mView);
	mTemp = MatrixMult(RotateX(-rotmeshdown*PI/360), mTemp);
	zproject = (mTemp)_CNV(2,2);									//2,2
	// Camera must look down
	if(zproject < 0)
	{
		numstep = -ztemp/zproject;
		xfin = camx+(numstep*(mTemp)_CNV(0,2));  // xgg			//0,2
		yfin = camy+(numstep*(mTemp)_CNV(1,2));					//1,2
		zfin = 0;
		cornerx[2] = xfin;
		cornery[2] = yfin;
		//printf("Camera BR: %f %f\n",xfin,yfin);
		//dump_matrix(mTemp);
	}

	// Bottomleft
	mTemp = MatrixMult(RotateY(-rotmeshbottomside*PI/360), mView);
	mTemp = MatrixMult(RotateX(-rotmeshdown*PI/360), mTemp);
	zproject = (mTemp)_CNV(2,2);									//2,2
	// Camera must look down
	if(zproject < 0)
	{
		numstep = -ztemp/zproject;
		xfin = camx+(numstep*(mTemp)_CNV(0,2));  // xgg			//0,2
		yfin = camy+(numstep*(mTemp)_CNV(1,2));					//1,2
		zfin = 0;
		cornerx[3] = xfin;
		cornery[3] = yfin;
		//printf("Camera BL: %f %f\n",xfin,yfin);
		//dump_matrix(mTemp);
	}

	// Get the extreme values
	cornerlowx = cornerx[0];
	cornerlowy = cornery[0];
	cornerhighx = cornerx[0];
	cornerhighy = cornery[0];
	cornerlistlowtohighy[0] = 0;
	cornerlistlowtohighy[3] = 0;

	for (cnt = 0; cnt < 4; cnt++)
	{
		if(cornerx[cnt] < cornerlowx)
			cornerlowx=cornerx[cnt];
		if(cornery[cnt] < cornerlowy)
		{
			cornerlowy=cornery[cnt];
			cornerlistlowtohighy[0] = cnt;
		}
		if(cornerx[cnt] > cornerhighx)
			cornerhighx=cornerx[cnt];
		if(cornery[cnt] > cornerhighy)
		{
			cornerhighy=cornery[cnt];
			cornerlistlowtohighy[3] = cnt;
		}
	}

	// Figure out the order of points
	tnc = 0;
	for (cnt = 0; cnt < 4; cnt++)
	{
		if(cnt != cornerlistlowtohighy[0] && cnt != cornerlistlowtohighy[3])
		{
			extra[tnc] = cnt;
			tnc++;
		}
	}
	cornerlistlowtohighy[1] = extra[1];
	cornerlistlowtohighy[2] = extra[0];
	if(cornery[extra[0]] < cornery[extra[1]])
	{
		cornerlistlowtohighy[1] = extra[0];
		cornerlistlowtohighy[2] = extra[1];
	}

	// BAD: exit here
	//printf("Corners:\n");
	//printf("x: %d %d\n",cornerlowx,cornerhighx);
	//printf("y: %d %d\n",cornerlowy,cornerhighy);
	/*printf("Exiting, camera code is broken\n");
	  exit(0);*/
}

//--------------------------------------------------------------------------------------------
void make_camera_matrix()
{
	// ZZ> This function sets mView to the camera's location and rotation
	mView = mViewSave;
	mView = MatrixMult(Translate(camx, -camy, camz), mView);  // xgg
	if(camswingamp > .001)
	{
		camroll = turntosin[camswing]*camswingamp;
		mView = MatrixMult(RotateY(camroll), mView);
	}
	mView = MatrixMult(RotateZ(camturnleftright), mView);
	mView = MatrixMult(RotateX(camturnupdown), mView);
	//lpD3DDDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &mView);
	//        glMatrixMode(GL_MODELVIEW);
	///        glLoadMatrixf(mView.v);
}

//--------------------------------------------------------------------------------------------
void bound_camera()
{
	// ZZ> This function stops the camera from moving off the mesh
	if(camx < EDGE)  camx = EDGE;
	if(camx > meshedgex-EDGE)  camx = meshedgex-EDGE;
	if(camy < EDGE)  camy = EDGE;
	if(camy > meshedgey-EDGE)  camy = meshedgey-EDGE;
}

//--------------------------------------------------------------------------------------------
void bound_camtrack()
{
	// ZZ> This function stops the camera target from moving off the mesh
	if(usefaredge)
	{
		if(camtrackx < FARTRACK)  camtrackx = FARTRACK;
		if(camtrackx > meshedgex-FARTRACK)  camtrackx = meshedgex-FARTRACK;
		if(camtracky < FARTRACK)  camtracky = FARTRACK;
		if(camtracky > meshedgey-FARTRACK)  camtracky = meshedgey-FARTRACK;
	}
	else
	{
		if(camtrackx < EDGETRACK)  camtrackx = EDGETRACK;
		if(camtrackx > meshedgex-EDGETRACK)  camtrackx = meshedgex-EDGETRACK;
		if(camtracky < EDGETRACK)  camtracky = EDGETRACK;
		if(camtracky > meshedgey-EDGETRACK)  camtracky = meshedgey-EDGETRACK;
	}
}

//--------------------------------------------------------------------------------------------
void adjust_camera_angle(int height)
{
	// ZZ> This function makes the camera look downwards as it is raised up
	float percentmin, percentmax;


	if(height < MINZADD)  height = MINZADD;
	percentmax = (height-MINZADD)/(float)(MAXZADD-MINZADD);
	percentmin = 1.0-percentmax;

	camturnupdown = ((MINUPDOWN*percentmin)+(MAXUPDOWN*percentmax));
	camzoom = (MINZOOM*percentmin)+(MAXZOOM*percentmax);
}

//--------------------------------------------------------------------------------------------
void move_camera()
{
	// ZZ> This function moves the camera
	int cnt, locoalive, band, movex, movey;
	float x, y, z, level, newx, newy;
	unsigned short character, turnsin, turncos;

	if(autoturncamera)
		doturntime = 255;
	else if(doturntime != 0)  
		doturntime--;

	x = 0;
	y = 0;
	z = 0;
	level = 0;
	locoalive = 0;

	for (cnt = 0; cnt < MAXPLAYER; cnt++)
	{
		if(plavalid[cnt] && pladevice[cnt] != INPUTNONE)
		{
			character = plaindex[cnt];
			if(chralive[character])
			{
				if(chrattachedto[character]==MAXCHR)
				{
					// The character is on foot
					x+=chrxpos[character];
					y+=chrypos[character];
					z+=chrzpos[character];
					level+=chrlevel[character];
				}
				else
				{
					// The character is mounted
					x+=chrxpos[chrattachedto[character]];
					y+=chrypos[chrattachedto[character]];
					z+=chrzpos[chrattachedto[character]];
					level+=chrlevel[chrattachedto[character]];
				}
				locoalive++;
			}
		}
	}

	if(locoalive>0)
	{
		x=x/locoalive;
		y=y/locoalive;
		z=z/locoalive;
		level=level/locoalive;
	}
	else
	{
		x = camtrackx;
		y = camtracky;
		z = camtrackz;
	}

	if(rtscontrol)
	{
		if(mousebutton[0])
		{
			x = camtrackx;
			y = camtracky;
		}
		else
		{
			band = 50;
			movex = 0;
			movey = 0;
			if(cursorx < band+6)
				movex += -(band+6-cursorx);
			if(cursorx > scrx-band-16)
				movex += cursorx+16-scrx+band;
			if(cursory < band+8)
				movey += -(band+8-cursory);
			if(cursory > scry-band-24)
				movey += cursory+24-scry+band;
			turnsin = (camturnleftrightone*16383);
			turnsin = turnsin&16383;
			turncos = (turnsin+4096)&16383;
			x = (movex*turntosin[turncos]+movey*turntosin[turnsin])*rtsscrollrate;
			y = (-movex*turntosin[turnsin]+movey*turntosin[turncos])*rtsscrollrate;
			camx = (camx + camx + x)/2.0;
			camy = (camy + camy + y)/2.0;
			x = camtrackx+x;
			y = camtracky+y;
		}
		if(rtssetcamera)
		{
			x = rtssetcamerax;
			y = rtssetcameray;
		}
		z = camtrackz;
	}
	camtrackxvel = -camtrackx;
	camtrackyvel = -camtracky;
	camtrackzvel = -camtrackz;
	camtrackx = (camtrackx+x)/2.0;
	camtracky = (camtracky+y)/2.0;
	camtrackz = (camtrackz+z)/2.0;
	camtracklevel = (camtracklevel+level)/2.0;


	camturnadd=camturnadd*camsustain;
	camzadd = (camzadd*3.0 + camzaddgoto)/4.0;
	camz = (camz*3.0 + camzgoto)/4.0;
	// Camera controls
	if(autoturncamera == 255 && numlocalpla==1)
	{
		if(keyon)
			camturnadd+=(key_left()-key_right())*(CAMKEYTURN);
	}
	// Keyboard camera controls
	if(keyon && key_set(GP2X_R))
	{
		if(key_left())
		{
			camturnadd+=CAMKEYTURN;
			doturntime = TURNTIME;  // Sticky turn...
		} else if(key_right())
		{
			camturnadd-=CAMKEYTURN;
			doturntime = TURNTIME;  // Sticky turn...
		}
		if(key_up()||key_down())
		{
			camzaddgoto+=(key_down()-key_up())*CAMKEYTURN*8;
			if(camzaddgoto < MINZADD)  camzaddgoto = MINZADD;
			if(camzaddgoto > MAXZADD)  camzaddgoto = MAXZADD;
		}
	}
	camx-=(float) ((mView)_CNV(0,0))*camturnadd;  // xgg
	camy+=(float) ((mView)_CNV(1,0))*-camturnadd;

	// Make it not break...
	bound_camtrack();
	bound_camera();

	// Do distance effects for overlay and background
	camtrackxvel += camtrackx;
	camtrackyvel += camtracky;
	camtrackzvel += camtrackz;
	if(overlayon)
	{
		// Do fg distance effect
		waterlayeru[0] += camtrackxvel*waterlayerdistx[0];
		waterlayerv[0] += camtrackyvel*waterlayerdisty[0];
	}
	if(clearson==FALSE)
	{
		// Do bg distance effect
		waterlayeru[1] += camtrackxvel*waterlayerdistx[1];
		waterlayerv[1] += camtrackyvel*waterlayerdisty[1];
	}

	// Center on target for doing rotation...
	if(doturntime != 0)
	{
		camcenterx = camcenterx*.9 + camtrackx*.1;
		camcentery = camcentery*.9 + camtracky*.1;
	}

	// Create a tolerance area for walking without camera movement
	x = camtrackx - camx;
	y = camtracky - camy;
	newx = -((mView)_CNV(0,0) * x + (mView)_CNV(1,0) * y); //newx = -(mView(0,0) * x + mView(1,0) * y);
	newy = -((mView)_CNV(0,1) * x + (mView)_CNV(1,1) * y);   //newy = -(mView(0,1) * x + mView(1,1) * y);


	// Debug information

	// Get ready to scroll...
	movex = 0;
	movey = 0;

	// Adjust for camera height...
	z = (TRACKXAREALOW  * (MAXZADD - camzadd)) +
		(TRACKXAREAHIGH * (camzadd - MINZADD));
	z = z / (MAXZADD - MINZADD);
	if(newx < -z)
	{
		// Scroll left
		movex += (newx + z);
	}
	if(newx > z)
	{
		// Scroll right
		movex += (newx - z);
	}

	// Adjust for camera height...
	z = (TRACKYAREAMINLOW  * (MAXZADD - camzadd)) +
		(TRACKYAREAMINHIGH * (camzadd - MINZADD));
	z = z / (MAXZADD - MINZADD);

	if(newy < z)
	{
		// Scroll down
		movey -= (newy - z);
	}
	else
	{
		// Adjust for camera height...
		z = (TRACKYAREAMAXLOW  * (MAXZADD - camzadd)) +
			(TRACKYAREAMAXHIGH * (camzadd - MINZADD));
		z = z / (MAXZADD - MINZADD);
		if(newy > z)
		{
			// Scroll up
			movey -= (newy - z);
		}
	}

	turnsin = (camturnleftrightone*16383);
	turnsin = turnsin&16383;
	turncos = (turnsin+4096)&16383;
	camcenterx += (movex*turntosin[turncos]+movey*turntosin[turnsin]);
	camcentery += (-movex*turntosin[turnsin]+movey*turntosin[turncos]);

	// Finish up the camera
	camera_look_at(camcenterx,camcentery);
	camx=(float) camcenterx+(camzoom*sin(camturnleftright));
	camy=(float) camcentery+(camzoom*cos(camturnleftright));
	bound_camera();
	adjust_camera_angle(camz);

	make_camera_matrix();
}

//--------------------------------------------------------------------------------------------
void reset_camera()
{
	// ZZ> This function makes sure the camera starts in a suitable position
	int cnt, save;
	//    int mi;


	camswing = 0;
	camx = meshedgex/2;
	camy = meshedgey/2;
	camz = 800;
	camzoom = 1000;
	rtsx = 0;
	rtsy = 0;
	camtrackxvel = 0;
	camtrackyvel = 0;
	camtrackzvel = 0;
	camcenterx = camx;
	camcentery = camy;
	camtrackx = camx;
	camtracky = camy;
	camtrackz = 0;
	camturnadd = 0;
	camtracklevel = 0;
	camzadd = 800;
	camzaddgoto = 800;
	camzgoto = 800;
	camturnleftright = (float) (-PI/4);
	camturnleftrightone = (float) (-PI/4)/(2*PI);
	camturnleftrightshort = 0;
	camturnupdown = (float) (PI/4);
	camroll = 0;
	rtssetcamera = TRUE;
	if(rtscontrol)
	{
		rtssetcamerax = meshedgex/2;
		rtssetcameray = meshedgey/2;

		for (cnt = 0; cnt < MAXCHR; cnt++)
			if(chron[cnt] && chrteam[cnt] == rtslocalteam)
			{
				rtssetcamerax = chrxpos[cnt];
				rtssetcameray = chrypos[cnt];
			}
	}

	// Now move the camera towards the players
	mView = ZeroMatrix();

	save = autoturncamera;
	autoturncamera = TRUE;

	for (cnt = 0; cnt < 32; cnt++)
	{
		move_camera();
		camcenterx = camtrackx;
		camcentery = camtracky;
	}

	autoturncamera = save;
	doturntime = 0;
	rtssetcamera = FALSE;
	rtsx = camtrackx;
	rtsy = camtracky;
}

