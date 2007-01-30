// menu.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"

char*	get_import_path()
{
#ifdef GP2X
	return ".";
#else
	static char path[1024] = "";
	if (!path[0])
		snprintf(path, 1024, "%s/.egoboo", getenv("HOME"));
	return path;
#endif
}

//--------------------------------------------------------------------------------------------
void draw_module_tag(int module, int y, int tagsz)
{
	// ZZ> This function draws a module tag
	char text[256];
	draw_trim_box(0, y, tagsz+2, y+tagsz+2);
	draw_trim_box(tagsz+4, y, scrx, y+tagsz+2);
	if(module < globalnummodule)
	{
		draw_titleimage(module, 4, y+4, tagsz-8);
		y+=6;
		tagsz += 12;
		draw_string(modlongname[module], tagsz, y);  y+=fontyspacing;
		draw_string(modrank[module], tagsz, y);  y+=fontyspacing;
		if(modmaxplayers[module] > 1)
		{
			if(modminplayers[module]==modmaxplayers[module])
			{
				sprintf(text, "%d players", modminplayers[module]);
			}
			else
			{
				sprintf(text, "%d-%d players", modminplayers[module], modmaxplayers[module]);
			}
		}
		else
		{
			sprintf(text, "1 player");
		}
		draw_string(text, tagsz, y);  y+=fontyspacing;
		if(modimportamount[module] == 0 && modallowexport[module]==FALSE)
		{
			draw_string("No Import/Export", tagsz, y);  y+=fontyspacing;
		}
		else
		{
			if(modimportamount[module] == 0)
			{
				draw_string("No Import", tagsz, y);  y+=fontyspacing;
			}
			if(modallowexport[module]==FALSE)
			{
				draw_string("No Export", tagsz, y);  y+=fontyspacing;
			}
		}
		if(modrespawnvalid[module] == FALSE)
		{
			draw_string("No Respawn", tagsz, y);  y+=fontyspacing;
		}
		if(modrtscontrol[module] == TRUE)
		{
			draw_string("RTS", tagsz, y);  y+=fontyspacing;
		}
		if(modrtscontrol[module] == ALLSELECT)
		{
			draw_string("Diaboo RTS", tagsz, y);  y+=fontyspacing;
		}
	}
}

//--------------------------------------------------------------------------------------------
void menu_pick_player(int module)
{
	// ZZ> This function handles the display for picking players to import
	int x, y;
	float open;
	int cnt, tnc, start, numshow;
	int stillchoosing;
	int import;
	unsigned char control, sparkle;
	static char fromdir[MAX_STR_SIZE];
	static char todir[MAX_STR_SIZE];
	static char basefromdir[MAX_STR_SIZE];
	enum { rootid_user, rootid_global };

	// Set the important flags
	respawnvalid = FALSE;
	respawnanytime = FALSE;
	if(modrespawnvalid[module])  respawnvalid = TRUE;
	if(modrespawnvalid[module]==ANYTIME)  respawnanytime = TRUE;
	rtscontrol = FALSE;
	if(modrtscontrol[module] != FALSE)
	{
		rtscontrol = TRUE;
		allselect = FALSE;
		if(modrtscontrol[module] == ALLSELECT)
			allselect = TRUE;
	}
	exportvalid = modallowexport[module];
	importvalid = (modimportamount[module] > 0);
	importamount = modimportamount[module];
	playeramount = modmaxplayers[module];
	make_directory(get_import_path());  // Just in case...

	start = 0;
	if(importvalid)
	{
		// Figure out which characters are available. First from the
		// user's home directory, and then from the global directory.
		check_player_import_begin();
		snprintf(fromdir, sizeof fromdir, "%s/players", get_import_path());
		check_player_import(rootid_user, fromdir);
		check_player_import(rootid_global, DATA_PATH"players");
		check_player_import_end();
		numshow = (scry-80-fontyspacing-fontyspacing)>>5;

		// Open some windows
		y = fontyspacing + 8;
		open = 0;
		while(open < 1.0)
		{
			//clear_surface(lpDDSBack);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			draw_trim_box_opening(0, 0, scrx, scry, open);
			draw_trim_box_opening(0, 0, scrx, 40, open);
			draw_trim_box_opening(0, scry-40, scrx, scry, open);
			flip_pages();
			open += .030;
		}

		wldframe = 0;  // For sparkle
		stillchoosing = TRUE;
		while(stillchoosing)
		{
			// Draw the windows
			//clear_surface(lpDDSBack);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			draw_trim_box(0, 0, scrx, scry);
			draw_trim_box(0, 40, scrx, scry-40);

			// Draw the Up/Down buttons
			if(start == 0)
			{
				// Show the instructions
				x = (scrx-270)>>1;
				draw_string("Choose your character", x, 10);
			}
			else
			{
				x = (scrx-40)>>1;
				draw_string("Up", x, 10);
			}
			x = (scrx-80)>>1;
			draw_string("Down", x, scry-fontyspacing-20);

			// Draw each import character
			y = 40+fontyspacing;
			cnt = 0;
			while(cnt < numshow && cnt + start < numloadplayer)
			{
				sparkle = NOSPARKLE;
				if(keybplayer == (cnt+start))
				{
					draw_one_icon(keybicon, 32, y, NOSPARKLE);
					sparkle = 0;  // White
				}
				else
					draw_one_icon(nullicon, 32, y, NOSPARKLE);
				draw_one_icon((cnt+start), 64, y, sparkle);
				draw_string(loadplayername[cnt+start], 102, y+6);
				y+=32;
				cnt++;
			}
			wldframe++;  // For sparkle

			// Handle other stuff...
			read_input();
			if(pending_click)
			{
				pending_click=FALSE;
				stillchoosing = FALSE;
			}
			if(key_set_once(GP2X_UP) && keybplayer > 0)
			{
				if (--keybplayer < start) start--;
			}
			if(key_set_once(GP2X_DOWN) && keybplayer < numloadplayer-1)
			{
				if (++keybplayer >= (start + numshow)) start++;
			}
			flip_pages();
		}
		wldframe = 0;  // For sparkle

		// Tell the user we're loading
		y = fontyspacing + 8;
		open = 0;
		while(open < 1.0)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			draw_trim_box_opening(0, 0, scrx, scry, open);
			flip_pages();
			open += .030;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		draw_trim_box(0, 0, scrx, scry);
		draw_string("Copying the imports...", y, y);
		flip_pages();

		// Now build the import directory...
		empty_import_directory(get_import_path());
		cnt = 0;
		numimport = 0;
		while(cnt < numloadplayer)
		{
			if((cnt == keybplayer && keyon)   ||
					(cnt == mousplayer && mouseon) ||
					(cnt == joyaplayer && joyaon)  ||
					(cnt == joybplayer && joybon))
			{
				// This character has been selected
				control = INPUTNONE;
				if(cnt == keybplayer)  control = control | INPUTKEY;
				if(cnt == mousplayer)  control = control | INPUTMOUSE;
				if(cnt == joyaplayer)  control = control | INPUTJOYA;
				if(cnt == joybplayer)  control = control | INPUTJOYB;
				localcontrol[numimport] = control;
				localslot[numimport] = (numimport+(localmachine*4))*9;

				// Copy the character to the import directory
				switch (loadplayerrootid[cnt])
				{
					case rootid_user:
						snprintf(basefromdir, sizeof(basefromdir),
								FILENAME("%s/players/%s"), 
								get_import_path(), loadplayerdir[cnt]);
						break;
					case rootid_global:
						snprintf(basefromdir, sizeof(basefromdir), 
								FILENAME(DATA_PATH"players/%s"), 
								loadplayerdir[cnt]);
						break;
					default:
						general_error(cnt, loadplayerrootid[cnt], "rootid");
						// won't return
				}
				snprintf(todir, sizeof(todir), FILENAME("%s/temp%04d.obj"), 
						get_import_path(), localslot[numimport]);

				// change back to copy_directory_to_host once networking is
				// working!
				copy_directory(basefromdir, todir);
				//                copy_directory_to_host(basefromdir, todir);

				// Copy all of the character's items to the import directory
				tnc = 0;
				while(tnc < 8)
				{
					sprintf(fromdir, FILENAME("%s/%d.obj"), basefromdir, tnc);
					sprintf(todir, FILENAME("%s/temp%04d.obj"), 
							get_import_path(), localslot[numimport]+tnc+1);

					// change back to copy_directory_to_host once networking is
					// working!
					copy_directory(fromdir, todir);
					//                    copy_directory_to_host(fromdir, todir);
					tnc++;
				}


				numimport++;
			}
			cnt++;
		}

	}
	nextmenu = MENUG;
}

//--------------------------------------------------------------------------------------------
void menu_module_loading(int module)
{
	// ZZ> This function handles the display for when a module is loading
	static char text[MAX_STR_SIZE];
	int y;
	float open;
	int cnt;
	int const tagsz = 100;

	// Open some windows
	y = fontyspacing + 8;
	open = 0;
	while(open < 1.0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		draw_trim_box_opening(0, y, tagsz, y+tagsz, open);
		draw_trim_box_opening(tagsz+8, y, scrx, y+tagsz, open);
		draw_trim_box_opening(0, y+tagsz+8, scrx, scry, open);
		flip_pages();
		open += .030;
	}

	// Put the stuff in the windows
	//clear_surface(lpDDSBack);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	y = 0;
	draw_string("Loading...  Wait!!!", 0, y);  y+=fontyspacing;
	y+=8;
	draw_trim_box(0, y+tagsz+4, scrx, scry);
	draw_module_tag(module, y, tagsz);

	// Show the summary
	sprintf(text, FILENAME(DATA_PATH"modules/%s/gamedat/menu.txt"), modloadname[module]);
	get_module_summary(text);
	y = fontyspacing+tagsz+12;
	cnt = 0;
	while(cnt < SUMMARYLINES)
	{
		draw_string(modsummary[cnt], 14, y); y+=fontyspacing;
		cnt++;
	}
	flip_pages();
	nextmenu = MENUD;
	menuactive = FALSE;
}

//--------------------------------------------------------------------------------------------
void menu_choose_module()
{
	// ZZ> This function lets the host choose a module
	int numtag;
	char text[256];
	int x, y, ystt;
	float open;
	int cnt;
	int module;
	int stillchoosing;
	int const tagsz = 140;
	// Figure out how many tags to display
	numtag = 1;
	ystt = (scry - tagsz)/2;
	

	// Open the tag windows
	open = 0;
	while(open < 1.0)
	{
		//clear_surface(lpDDSBack);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		draw_trim_box_opening(0, 0, scrx, scry, open);
		y = ystt;
		cnt = 0;
		draw_trim_box_opening(0, y, tagsz, scry-ystt, open);
		draw_trim_box_opening(tagsz+2, y, scrx, scry-ystt, open);
		flip_pages();
		open += .030;
	}

	// Let the user pick a module
	module = 0;
	stillchoosing = TRUE;
	while(stillchoosing)
	{
		// Draw the tags
		//clear_surface(lpDDSBack);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		draw_trim_box(0, 0, scrx, scry);
		draw_module_tag(module, ystt, tagsz);

		// Draw the Up/Down buttons
		x = (scrx-40)>>1;
		draw_string("Up", x, ystt/2);
		x = (scrx-60)>>1;
		draw_string("Down", x, scry-ystt/2-fontyspacing);

		// Handle the mouse
		read_input();
		if(key_set_once(GP2X_UP) && module > 0) {
			module--;
		} else if (key_set_once(GP2X_DOWN) && module < globalnummodule-1) {
			module++;
		} else if (pending_click) {
			pending_click=FALSE;
			printf("playersready\n");
			// Set start info
			playersready = 1;
			seed = time(0);
			pickedindex = module;
			sprintf(pickedmodule, "%s", modloadname[module]);
			readytostart = TRUE;
			stillchoosing = FALSE;
			printf("pickedmodule = '%s'\n", pickedmodule);
		} else if (key_set_once(GP2X_START)) {
			nextmenu = MENUD;
			menuactive = FALSE;
			stillchoosing = FALSE;
			gameactive = FALSE;
		}
		flip_pages();
	}
	nextmenu = MENUF;
}

//--------------------------------------------------------------------------------------------
void menu_end_text()
{
	// ZZ> This function gives the player the ending text
	float open;
	int stillchoosing;

	// Open the text window
	open = 0;
	while(open < 1.0)
	{
		//clear_surface(lpDDSBack);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		draw_trim_box_opening(0, 0, scrx, scry, open);
		flip_pages();
		open += .030;
	}

	// Wait for input
	stillchoosing = TRUE;
	while(stillchoosing)
	{
		// Show the text
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		draw_trim_box(0, 0, scrx, scry);
		draw_wrap_string(endtext, 14, 8, scrx-40);
		
		// Handle the mouse
		read_input();
		if(pending_click)
		{
			pending_click = FALSE;
			stillchoosing = FALSE;
		}
		flip_pages();
	}
	nextmenu = MENUD;
}

//--------------------------------------------------------------------------------------------
void menu_initial_text()
{
	// ZZ> This function gives the player the initial title screen
	float open;
	char text[1024];
	int stillchoosing;

	// Open the text window
	open = 0;
	while(open < 1.0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		// clear_surface(lpDDSBack); PORT!
		draw_trim_box_opening(0, 0, scrx, scry, open);
		flip_pages();
		open += .030;
	}

	// Wait for input
	stillchoosing = TRUE;
	while(stillchoosing)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		// Show the text
		draw_trim_box(0, 0, scrx, scry);
		int y = 20;
		int x = 20;
		draw_string("Egoboo v2.22", x, y); y += fontyspacing;
		draw_string("http://egoboo.sourceforge.net", x, y); y += fontyspacing;
		y += fontyspacing;
		draw_string("GP2X port using GPU940 openGl, 2006-12-26", x, y); y += fontyspacing;
		draw_string("http://gna.org/projects/gpu940", x, y); y += fontyspacing;
		y += fontyspacing*2;
		draw_string("       Instructions :", x, y); y += fontyspacing*2;
		draw_string("Move with stick, select with click or X", x, y); y += fontyspacing;
		draw_string("Move camera with Right + stick", x, y); y += fontyspacing;
		draw_string("Jump with Y", x, y); y += fontyspacing;
		draw_string("Left hand Use with Left + A", x,y); y += fontyspacing;
		draw_string("Right hand Use with A", x,y); y += fontyspacing;
		draw_string("Left hand Get/Drop with Left + X", x,y); y += fontyspacing;
		draw_string("Right hand Get/Drop with X", x,y); y += fontyspacing;
		draw_string("Left hand Inventory with Left + B", x,y); y += fontyspacing;
		draw_string("Right hand Inventory with B", x,y); y += fontyspacing;
		draw_string("Pause with Select", x,y); y += fontyspacing;
		draw_string("Quit with Start + Select", x,y); y += fontyspacing;
		y += fontyspacing;
		draw_string("       The rest is up to you !", x,y);
		flip_pages();
		
		// Handle the mouse
		read_input();
		if ( pending_click )
		{
			pending_click = FALSE;
			stillchoosing = FALSE;
		}
	}
	nextmenu = MENUD;
}

//--------------------------------------------------------------------------------------------
void fiddle_with_menu()
{
	// ZZ> This function gives a nice little menu to play around in.

	menuactive = TRUE;
	readytostart = FALSE;
	playersready = 0;
	localmachine = 0;
	rtslocalteam = 0;
	numfile = 0;
	numfilesent = 0;
	numfileexpected = 0;
	hostactive = TRUE;
	numplayer = 1;
	while(menuactive)
	{
		printf("menu = MENU%c\n", 'A'+nextmenu);
		switch(nextmenu)
		{
			case MENUD:
				// MENUD...  Choose a module to run
				//printf("MENUD\n");
				menu_choose_module();
				break;
			case MENUF:
				// MENUF...  Let the players choose characters
				//printf("MENUF\n");
				menu_pick_player(pickedindex);
				break;
			case MENUG:
				// MENUG...  Let the user read while it loads
				//printf("MENUG\n");
				menu_module_loading(pickedindex);
				break;
			case MENUH:
				// MENUH...  Show the end text
				//printf("MENUH\n");
				menu_end_text();
				break;
			case MENUI:
				// MENUI...  Show the initial text
				//printf("MENUI\n");
				menu_initial_text();
				break;
		}
	}
	fprintf(stderr, "Left menu system\n");
}

//--------------------------------------------------------------------------------------------
void release_menu_trim()
{
	// ZZ> This function frees the menu trim memory
	//GLTexture_Release( &TxTrimX );		//RELEASE(lpDDSTrimX);
	//GLTexture_Release( &TxTrimY );		//RELEASE(lpDDSTrimY);
	GLTexture_Release( &TxBlip );		//RELEASE(lpDDSBlip);
	GLTexture_Release( &TxTrim );

}

//--------------------------------------------------------------------------------------------
void release_menu()
{
	// ZZ> This function releases all the menu images
	fprintf(stderr, "release_menu\n");
	GLTexture_Release( &TxFont );		//RELEASE(lpDDSFont);
	release_all_titleimages();
	release_all_icons();

}

