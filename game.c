// game.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include <string.h>
#include <errno.h>
#define DECLARE_GLOBALS
#include "egoboo.h"
const float mScale[16] = {
	1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,RESCALE
};
extern float mainVolume;

#define INITGUID
#define NAME "Boo"
#define TITLE "Boo"

#define RELEASE(x) if (x) {x->Release(); x=NULL;}
#ifdef _BIG_ENDIAN
int convert_int(int test)
{
	unsigned char *str = (char *)(&test);
	unsigned char tmp;
	int i;

	for (i = 0; i < sizeof(int) / 2; i++)
	{
		tmp = str[i];
		str[i] = str[sizeof(int) - 1 - i];
		str[sizeof(int) - 1 - i] = tmp;
	}
	return (*(int*)str);
}
float convert_float(float test)
{
	unsigned char *str = (char *)(&test);
	unsigned char tmp;
	int i;

	for (i = 0; i < sizeof(float) / 2; i++)
	{
		tmp = str[i];
		str[i] = str[sizeof(float) - 1 - i];
		str[sizeof(float) - 1 - i] = tmp;
	}
	return (*(float*)str);
}
#endif

//---------------------------------------------------------------------------------------------
char *os_cvrt_filename(char *name, char ch)
{
	// GAC - Convert filenames based on different operating systems
	static char newname[128];  // Should be enough
	char *p = newname;

	// Add a leading colon for the MacOS (if needed)
#ifdef _MACOS
	if ( name[0] != ':' )
	{
		*p = ':';
		p++;
	}
#endif

	while(*name){
		*p = *name != '/' ? *name : ch;
		p++;
		name++;
	}
	*p = '\0';
	return newname;
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
int what_action(char cTmp)
{
	// ZZ> This function changes a letter into an action code
	int action;
	action = ACTIONDA;
	if(cTmp=='U' || cTmp=='u')  action = ACTIONUA;
	if(cTmp=='T' || cTmp=='t')  action = ACTIONTA;
	if(cTmp=='S' || cTmp=='s')  action = ACTIONSA;
	if(cTmp=='C' || cTmp=='c')  action = ACTIONCA;
	if(cTmp=='B' || cTmp=='b')  action = ACTIONBA;
	if(cTmp=='L' || cTmp=='l')  action = ACTIONLA;
	if(cTmp=='X' || cTmp=='x')  action = ACTIONXA;
	if(cTmp=='F' || cTmp=='f')  action = ACTIONFA;
	if(cTmp=='P' || cTmp=='p')  action = ACTIONPA;
	if(cTmp=='Z' || cTmp=='z')  action = ACTIONZA;
	return action;
}

//--------------------------------------------------------------------------------------------
void general_error(int a, int b, char *szerrortext)
{
	// ZZ> This function displays an error message
	// Steinbach's Guideline for Systems Programming:
	//   Never test for an error condition you don't know how to handle.
	char                buf[256];
	FILE*               filewrite;
	snprintf(buf, sizeof(buf), "%d, %d... %s\n", 0, 0, szerrortext);

	fprintf(stderr,"ERROR: %s\n",szerrortext);

	filewrite = fopen("errorlog.txt", "w");
	if(filewrite)
	{
		fprintf(filewrite, "I'M MELTING\n");
		fprintf(filewrite, "%d, %d... %s\n", a, b, szerrortext);
		fclose(filewrite);
	}
	release_module();
	close_session();

	release_grfx();
	if (globalnetworkerr)
		fclose(globalnetworkerr);
	//DestroyWindow(hWnd);

	SDL_Quit ();
	exit(0);
}

//------------------------------------------------------------------------------
//Random Things-----------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void make_newloadname(char *modname, char *appendname, char *newloadname)
{
	// ZZ> This function takes some names and puts 'em together
	int cnt, tnc;
	char ctmp;

	cnt = 0;
	ctmp = modname[cnt];
	while(ctmp != 0)
	{
		newloadname[cnt] = ctmp;
		cnt++;
		ctmp = modname[cnt];
	}
	tnc = 0;
	ctmp = appendname[tnc];
	while(ctmp != 0)
	{
		newloadname[cnt] = ctmp;
		cnt++;
		tnc++;
		ctmp = appendname[tnc];
	}
	newloadname[cnt] = 0;
}

//--------------------------------------------------------------------------------------------
void load_global_waves(char *modname)
{
	// ZZ> This function loads the global waves
	char tmploadname[256];
	char newloadname[256];
	char wavename[256];
	int cnt;


	make_newloadname(modname, FILENAME("gamedat/"), tmploadname);
	cnt = 0;
	while(cnt < MAXWAVE)
	{
		snprintf(wavename, sizeof(wavename), "sound%d.wav", cnt);
		make_newloadname(tmploadname, wavename, newloadname);
		load_one_wave(newloadname);
		cnt++;
	}
}


//---------------------------------------------------------------------------------------------
void export_one_character(int character, int owner, int number)
{
	// ZZ> This function exports a character
	int tnc, profile;
	char letter;
	static char fromdir[MAX_STR_SIZE];
	static char todir[MAX_STR_SIZE];
	static char fromfile[MAX_STR_SIZE];
	static char tofile[MAX_STR_SIZE];
	static char todirname[MAX_STR_SIZE];
	static char todirfullname[MAX_STR_SIZE];

	// Don't export enchants
	disenchant_character(character);

	profile = chrmodel[character];
	if((capcancarrytonextmodule[profile] || capisitem[profile]==FALSE) && exportvalid)
	{
		// TWINK_BO.OBJ
		snprintf(todirname, sizeof(todirname), "badname.obj");//"BADNAME.OBJ");
		tnc = 0;
		letter = chrname[owner][tnc];
		while(tnc < 8 && letter != 0)
		{
			letter = chrname[owner][tnc];
			if(letter >= 'A' && letter <= 'Z')  letter -= 'A' - 'a';
			if(letter != 0)
			{
				if(letter < 'a' || letter > 'z')  letter = '_';
				todirname[tnc] = letter;
				tnc++;
			}
		}
		todirname[tnc] = '.'; tnc++;
		todirname[tnc] = 'o'; tnc++;
		todirname[tnc] = 'b'; tnc++;
		todirname[tnc] = 'j'; tnc++;
		todirname[tnc] = 0;



		// Is it a character or an item?
		if(owner != character)
		{
			// Item is a subdirectory of the owner directory...
			snprintf(todirfullname, sizeof(todirfullname), FILENAME("%s/%d.obj"), todirname, number);
		}
		else
		{
			// Character directory
			snprintf(todirfullname, sizeof(todirfullname), "%s", todirname);
		}


		// players/twink.obj or players/twink.obj/sword.obj
		snprintf(todir, sizeof(todir), "%s/players/%s", 
				get_import_path(), todirfullname);
		// modules/advent.mod/objects/advent.obj
		snprintf(fromdir, sizeof(fromdir), "%s", madname[profile]);


		// Delete all the old items
		if(owner == character)
		{
			tnc = 0;
			while(tnc < 8)
			{
				snprintf(tofile, sizeof(tofile), FILENAME("%s/%d.obj"), todir, tnc);	/*.OBJ*/
				delete_directory(tofile);
				tnc++;
			}
		}


		// Make the directory
		make_directory(todir);


		// Build the DATA.TXT file
		snprintf(tofile, sizeof(tofile), FILENAME("%s/data.txt"), todir);	/*DATA.TXT*/
		export_one_character_profile(tofile, character);


		// Build the SKIN.TXT file
		snprintf(tofile, sizeof(tofile), FILENAME("%s/skin.txt"), todir);	/*SKIN.TXT*/
		export_one_character_skin(tofile, character);


		// Build the NAMING.TXT file
		snprintf(tofile, sizeof(tofile), FILENAME("%s/naming.txt"), todir);	/*NAMING.TXT*/
		export_one_character_name(tofile, character);


		// Copy all of the misc. data files
		snprintf(fromfile, sizeof(fromfile), FILENAME("%s/message.txt"), fromdir);	/*MESSAGE.TXT*/
		snprintf(tofile,   sizeof(tofile), FILENAME("%s/message.txt"), todir);	/*MESSAGE.TXT*/
		copy_file(fromfile, tofile);
		snprintf(fromfile, sizeof(fromfile), FILENAME("%s/tris.md2"), fromdir);	/*TRIS.MD2*/
		snprintf(tofile,   sizeof(tofile), FILENAME("%s/tris.md2"), todir);	/*TRIS.MD2*/
		copy_file(fromfile, tofile);
		snprintf(fromfile, sizeof(fromfile), FILENAME("%s/copy.txt"), fromdir);	/*COPY.TXT*/
		snprintf(tofile,   sizeof(tofile), FILENAME("%s/copy.txt"), todir);	/*COPY.TXT*/
		copy_file(fromfile, tofile);
		snprintf(fromfile, sizeof(fromfile), FILENAME("%s/script.txt"), fromdir);
		snprintf(tofile,   sizeof(tofile), FILENAME("%s/script.txt"), todir);
		copy_file(fromfile, tofile);
		snprintf(fromfile, sizeof(fromfile), FILENAME("%s/enchant.txt"), fromdir);
		snprintf(tofile,   sizeof(tofile), FILENAME("%s/enchant.txt"), todir);
		copy_file(fromfile, tofile);
		snprintf(fromfile, sizeof(fromfile), FILENAME("%s/credits.txt"), fromdir);
		snprintf(tofile,   sizeof(tofile), FILENAME("%s/credits.txt"), todir);
		copy_file(fromfile, tofile);


		// Copy all of the particle files
		tnc = 0;
		while(tnc < MAXPRTPIPPEROBJECT)
		{
			snprintf(fromfile, sizeof(fromfile), FILENAME("%s/part%d.txt"), fromdir, tnc);
			snprintf(tofile,   sizeof(tofile), FILENAME("%s/part%d.txt"), todir,   tnc);
			copy_file(fromfile, tofile);
			tnc++;
		}


		// Copy all of the sound files
		tnc = 0;
		while(tnc < MAXWAVE)
		{
			snprintf(fromfile, sizeof(fromfile), FILENAME("%s/sound%d.wav"), fromdir, tnc);
			snprintf(tofile,   sizeof(tofile), FILENAME("%s/sound%d.wav"), todir,   tnc);
			copy_file(fromfile, tofile);
			tnc++;
		}


		// Copy all of the image files
		tnc = 0;
		while(tnc < 4)
		{
			snprintf(fromfile, sizeof(fromfile), FILENAME("%s/tris%d.bmp"), fromdir, tnc);
			snprintf(tofile,   sizeof(tofile), FILENAME("%s/tris%d.bmp"), todir,   tnc);
			copy_file(fromfile, tofile);
			snprintf(fromfile, sizeof(fromfile), FILENAME("%s/icon%d.bmp"), fromdir, tnc);
			snprintf(tofile,   sizeof(tofile), FILENAME("%s/icon%d.bmp"), todir,   tnc);
			copy_file(fromfile, tofile);
			tnc++;
		}
	}
}

//---------------------------------------------------------------------------------------------
void export_all_local_players(void)
{
	// ZZ> This function saves all the local players in the
	//     PLAYERS directory
	int cnt, character, item, number;

	// Check each player
	if(exportvalid)
	{
		cnt = 0;
		while(cnt < MAXPLAYER)
		{
			if(plavalid[cnt] && pladevice[cnt])
			{
				// Is it alive?
				character = plaindex[cnt];
				if(chron[character] && chralive[character])
				{
					// Export the character
					export_one_character(character, character, 0);


					// Export the left hand item
					item = chrholdingwhich[character][0];
					if(item != MAXCHR && chrisitem[item])  export_one_character(item, character, 0);

					// Export the right hand item
					item = chrholdingwhich[character][1];
					if(item != MAXCHR && chrisitem[item])  export_one_character(item, character, 1);

					// Export the inventory
					number = 2;
					item = chrnextinpack[character];
					while(item != MAXCHR)
					{
						if(chrisitem[item]) export_one_character(item, character, number);
						item = chrnextinpack[item];
						number++;
					}
				}
			}
			cnt++;
		}
	}
}

//---------------------------------------------------------------------------------------------
void quit_module(void)
{
	// ZZ> This function forces a return to the menu
	moduleactive = FALSE;
	hostactive = FALSE;
	export_all_local_players();
}

//--------------------------------------------------------------------------------------------
void quit_game(void)
{
	// ZZ> This function exits the game entirely
	if(gameactive)
	{
		/* PORT
			PostMessage(hGlobalWindow, WM_CLOSE, 0, 0);
		 */
		gameactive = FALSE;
	}
	if(moduleactive)
	{
		quit_module();
	}
	if(floatmemory != NULL)
	{
		free(floatmemory);
		floatmemory = NULL;
	}
}

/* ORIGINAL, UNOPTIMIZED VERSION
//--------------------------------------------------------------------------------------------
void goto_colon(FILE* fileread)
{
// ZZ> This function moves a file read pointer to the next colon
char cTmp;


fscanf(fileread, "%c", &cTmp);
while(cTmp != ':')
{
if(fscanf(fileread, "%c", &cTmp)==EOF)
{
if(globalname==NULL)
{
general_error(0, 0, "NOT ENOUGH COLONS IN FILE!!!");
}
else
{
general_error(0, 0, globalname);
}
}
}
}
 */

//--------------------------------------------------------------------------------------------
void goto_colon(FILE* fileread)
{
	// ZZ> This function moves a file read pointer to the next colon
	//    char cTmp;
	unsigned int ch = fgetc(fileread);

	//    fscanf(fileread, "%c", &cTmp);
	while(ch != ':')
	{
		if (ch == EOF)
		{
			// not enough colons in file!
			general_error(0, 0, globalname);
		}

		ch = fgetc(fileread);
	}
}

//--------------------------------------------------------------------------------------------
unsigned char goto_colon_yesno(FILE* fileread)
{
	// ZZ> This function moves a file read pointer to the next colon, or it returns
	//     FALSE if there are no more
	char cTmp;

	fscanf(fileread, "%c", &cTmp);
	while(cTmp != ':')
	{
		if(fscanf(fileread, "%c", &cTmp)==EOF)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------
char get_first_letter(FILE* fileread)
{
	// ZZ> This function returns the next non-whitespace character
	char cTmp;
	fscanf(fileread, "%c", &cTmp);
	while(isspace(cTmp))
	{
		fscanf(fileread, "%c", &cTmp);
	}
	return cTmp;
}

//--------------------------------------------------------------------------------------------
//Tag Reading---------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void reset_tags()
{
	// ZZ> This function resets the tags
	numscantag = 0;
}

//--------------------------------------------------------------------------------------------
int read_tag(FILE *fileread)
{
	// ZZ> This function finds the next tag, returning TRUE if it found one
	if(goto_colon_yesno(fileread))
	{
		if(numscantag < MAXTAG)
		{
			fscanf(fileread, "%s%d", tagname[numscantag], &tagvalue[numscantag]);
			numscantag++;
			return TRUE;
		}
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
void read_all_tags(char *szFilename)
{
	// ZZ> This function reads the scancode.txt file
	FILE* fileread;


	reset_tags();
	fileread = fopen(FILENAME(szFilename), "r");
	if(fileread)
	{
		while(read_tag(fileread));
		fclose(fileread);
	}
}

//--------------------------------------------------------------------------------------------
int tag_value(char *string)
{
	// ZZ> This function matches the string with its tag, and returns the value...
	//     It will return 255 if there are no matches.
	int cnt;

	cnt = 0;
	while(cnt < numscantag)
	{
		if(strcmp(string, tagname[cnt])==0)
		{
			// They match
			return tagvalue[cnt];
		}
		cnt++;
	}
	// No matches
	return 255;
}

//--------------------------------------------------------------------------------------------
void read_controls(char *szFilename)
{
	// ZZ> This function reads the controls.txt file
	FILE* fileread;
	char currenttag[TAGSIZE];
	int cnt;


	fileread = fopen(FILENAME(szFilename), "r");
	if(fileread)
	{
		cnt = 0;
		while(goto_colon_yesno(fileread) && cnt < MAXCONTROL)
		{
			fscanf(fileread, "%s", currenttag);
			controlvalue[cnt] = tag_value(currenttag);
			//printf("CTRL: %i, %s\n", controlvalue[cnt], currenttag);
			controliskey[cnt] = (currenttag[0] == 'K');
			cnt++;
		}
		fclose(fileread);
	}
}

//--------------------------------------------------------------------------------------------
void undo_idsz(int idsz)
{
	// ZZ> This function takes an integer and makes an text IDSZ out of it.
	//     It will set valueidsz to "NONE" if the idsz is 0
	if(idsz == IDSZNONE)
	{
		snprintf(valueidsz, sizeof(valueidsz), "NONE");
	}
	else
	{
		valueidsz[0] = ((idsz>>15)&31) + 'A';
		valueidsz[1] = ((idsz>>10)&31) + 'A';
		valueidsz[2] = ((idsz>>5)&31) + 'A';
		valueidsz[3] = ((idsz)&31) + 'A';
		valueidsz[4] = 0;
	}
	return;
}

//--------------------------------------------------------------------------------------------
int get_idsz(FILE* fileread)
{
	// ZZ> This function reads and returns an IDSZ tag, or IDSZNONE if there wasn't one
	int test;

	int idsz = IDSZNONE;
	char cTmp = get_first_letter(fileread);
	if(cTmp == '[')
	{
		fscanf(fileread, "%c", &cTmp);  cTmp = cTmp-'A';  idsz=idsz|(cTmp<<15);
		fscanf(fileread, "%c", &cTmp);  cTmp = cTmp-'A';  idsz=idsz|(cTmp<<10);
		fscanf(fileread, "%c", &cTmp);  cTmp = cTmp-'A';  idsz=idsz|(cTmp<<5);
		fscanf(fileread, "%c", &cTmp);  cTmp = cTmp-'A';  idsz=idsz|(cTmp);
	}

	test = (('N'-'A')<<15)|(('O'-'A')<<10)|(('N'-'A')<<5)|('E'-'A');  // [NONE]

	if(idsz == test)
		idsz = IDSZNONE;

	return idsz;
}

//--------------------------------------------------------------------------------------------
int get_free_message(void)
{
	// This function finds the best message to use
	// Pick the first one
	int tnc = msgstart;
	msgstart++;
	msgstart = msgstart % maxmessage;
	return tnc;
}

//--------------------------------------------------------------------------------------------
void display_message(int message, unsigned short character)
{
	// ZZ> This function sticks a message in the display queue and sets its timer
	int slot, read, write, cnt;
	char *eread;
	char szTmp[256];
	char cTmp, lTmp;

	unsigned short target = chraitarget[character];
	unsigned short owner = chraiowner[character];
	if (message < msgtotal)
	{
		slot = get_free_message();
		msgtime[slot] = MESSAGETIME;
		// Copy the message
		read = msgindex[message];
		cnt=0;
		write = 0;
		cTmp = msgtext[read];  read++;
		while(cTmp != 0)
		{
			if(cTmp == '%')
			{
				// Escape sequence
				eread = szTmp;
				szTmp[0] = 0;
				cTmp = msgtext[read];  read++;
				if(cTmp == 'n')  // Name
				{
					if(chrnameknown[character])
						snprintf(szTmp, sizeof(szTmp), "%s", chrname[character]);
					else
					{
						lTmp = capclassname[chrmodel[character]][0];
						if(lTmp == 'A' || lTmp == 'E' || lTmp == 'I' || lTmp == 'O' || lTmp == 'U')
							snprintf(szTmp, sizeof(szTmp), "an %s", capclassname[chrmodel[character]]);
						else
							snprintf(szTmp, sizeof(szTmp), "a %s", capclassname[chrmodel[character]]);
					}
					if(cnt == 0 && szTmp[0] == 'a')  szTmp[0] = 'A';
				}
				if(cTmp == 'c')  // Class name
				{
					eread = capclassname[chrmodel[character]];
				}
				if(cTmp == 't')  // Target name
				{
					if(chrnameknown[target])
						snprintf(szTmp, sizeof(szTmp), "%s", chrname[target]);
					else
					{
						lTmp = capclassname[chrmodel[target]][0];
						if(lTmp == 'A' || lTmp == 'E' || lTmp == 'I' || lTmp == 'O' || lTmp == 'U')
							snprintf(szTmp, sizeof(szTmp), "an %s", capclassname[chrmodel[target]]);
						else
							snprintf(szTmp, sizeof(szTmp), "a %s", capclassname[chrmodel[target]]);
					}
					if(cnt == 0 && szTmp[0] == 'a')  szTmp[0] = 'A';
				}
				if(cTmp == 'o')  // Owner name
				{
					if(chrnameknown[owner])
						snprintf(szTmp, sizeof(szTmp), "%s", chrname[owner]);
					else
					{
						lTmp = capclassname[chrmodel[owner]][0];
						if(lTmp == 'A' || lTmp == 'E' || lTmp == 'I' || lTmp == 'O' || lTmp == 'U')
							snprintf(szTmp, sizeof(szTmp), "an %s", capclassname[chrmodel[owner]]);
						else
							snprintf(szTmp, sizeof(szTmp), "a %s", capclassname[chrmodel[owner]]);
					}
					if(cnt == 0 && szTmp[0] == 'a')  szTmp[0] = 'A';
				}
				if(cTmp == 's')  // Target class name
				{
					eread = capclassname[chrmodel[target]];
				}
				if(cTmp >= '0' && cTmp <= '3')  // Target's skin name
				{
					eread = capskinname[chrmodel[target]][cTmp-'0'];
				}
				if(cTmp == 'd')  // tmpdistance value
				{
					snprintf(szTmp, sizeof(szTmp), "%d", valuetmpdistance);
				}
				if(cTmp == 'x')  // tmpx value
				{
					snprintf(szTmp, sizeof(szTmp), "%d", valuetmpx);
				}
				if(cTmp == 'y')  // tmpy value
				{
					snprintf(szTmp, sizeof(szTmp), "%d", valuetmpy);
				}
				if(cTmp == 'D')  // tmpdistance value
				{
					snprintf(szTmp, sizeof(szTmp), "%2d", valuetmpdistance);
				}
				if(cTmp == 'X')  // tmpx value
				{
					snprintf(szTmp, sizeof(szTmp), "%2d", valuetmpx);
				}
				if(cTmp == 'Y')  // tmpy value
				{
					snprintf(szTmp, sizeof(szTmp), "%2d", valuetmpy);
				}
				if(cTmp == 'a')  // Character's ammo
				{
					if(chrammoknown[character])
						snprintf(szTmp, sizeof(szTmp), "%d", chrammo[character]);
					else
						snprintf(szTmp, sizeof(szTmp), "?");
				}
				if(cTmp == 'k')  // Kurse state
				{
					if(chriskursed[character])
						snprintf(szTmp, sizeof(szTmp), "kursed");
					else
						snprintf(szTmp, sizeof(szTmp), "unkursed");
				}
				if(cTmp == 'p')  // Character's possessive
				{
					if(chrgender[character] == GENFEMALE)
					{
						snprintf(szTmp, sizeof(szTmp), "her");
					}
					else
					{
						if(chrgender[character] == GENMALE)
						{
							snprintf(szTmp, sizeof(szTmp), "his");
						}
						else
						{
							snprintf(szTmp, sizeof(szTmp), "its");
						}
					}
				}
				if(cTmp == 'm')  // Character's gender
				{
					if(chrgender[character] == GENFEMALE)
					{
						snprintf(szTmp, sizeof(szTmp), "female ");
					}
					else
					{
						if(chrgender[character] == GENMALE)
						{
							snprintf(szTmp, sizeof(szTmp), "male ");
						}
						else
						{
							snprintf(szTmp, sizeof(szTmp), " ");
						}
					}
				}
				if(cTmp == 'g')  // Target's possessive
				{
					if(chrgender[target] == GENFEMALE)
					{
						sprintf(szTmp, "her");
					}
					else
					{
						if(chrgender[target] == GENMALE)
						{
							snprintf(szTmp, sizeof(szTmp), "his");
						}
						else
						{
							snprintf(szTmp, sizeof(szTmp), "its");
						}
					}
				}
				cTmp = *eread;  eread++;
				while(cTmp != 0 && write < MESSAGESIZE-1)
				{
					msgtextdisplay[slot][write] = cTmp;
					cTmp = *eread;  eread++;
					write++;
				}
			}
			else
			{
				// Copy the letter
				if(write < MESSAGESIZE-1)
				{
					msgtextdisplay[slot][write] = cTmp;
					write++;
				}
			}
			cTmp = msgtext[read];  read++;
			cnt++;
		}
		msgtextdisplay[slot][write] = 0;
	}
}

//--------------------------------------------------------------------------------------------
void remove_enchant(unsigned short enchantindex)
{
	// ZZ> This function removes a specific enchantment and adds it to the unused list
	unsigned short character, overlay;
	unsigned short lastenchant, currentenchant;
	int add;
	int distance, volume;


	if(enchantindex < MAXENCHANT)
	{
		if(encon[enchantindex])
		{
			// Unsparkle the spellbook
			character = encspawner[enchantindex];
			if(character < MAXCHR)
			{
				chrsparkle[character] = NOSPARKLE;
				// Make the spawner unable to undo the enchantment
				if(chrundoenchant[character] == enchantindex)
				{
					chrundoenchant[character] = MAXENCHANT;
				}
			}


			// Play the end sound
			character = enctarget[enchantindex];
			distance = ABS(camtrackx-chroldx[character])+ABS(camtracky-chroldy[character]);
			volume = -distance;
			volume = volume<<VOLSHIFT;
			if(volume > VOLMIN && moduleactive)
			{

				play_sound_pvf(evewaveindex[enceve[enchantindex]], PANMID, volume, evefrequency[enceve[enchantindex]]);

			}



			// Unset enchant values, doing morph last
			unset_enchant_value(enchantindex, SETDAMAGETYPE);
			unset_enchant_value(enchantindex, SETNUMBEROFJUMPS);
			unset_enchant_value(enchantindex, SETLIFEBARCOLOR);
			unset_enchant_value(enchantindex, SETMANABARCOLOR);
			unset_enchant_value(enchantindex, SETSLASHMODIFIER);
			unset_enchant_value(enchantindex, SETCRUSHMODIFIER);
			unset_enchant_value(enchantindex, SETPOKEMODIFIER);
			unset_enchant_value(enchantindex, SETHOLYMODIFIER);
			unset_enchant_value(enchantindex, SETEVILMODIFIER);
			unset_enchant_value(enchantindex, SETFIREMODIFIER);
			unset_enchant_value(enchantindex, SETICEMODIFIER);
			unset_enchant_value(enchantindex, SETZAPMODIFIER);
			unset_enchant_value(enchantindex, SETFLASHINGAND);
			unset_enchant_value(enchantindex, SETLIGHTBLEND);
			unset_enchant_value(enchantindex, SETALPHABLEND);
			unset_enchant_value(enchantindex, SETSHEEN);
			unset_enchant_value(enchantindex, SETFLYTOHEIGHT);
			unset_enchant_value(enchantindex, SETWALKONWATER);
			unset_enchant_value(enchantindex, SETCANSEEINVISIBLE);
			unset_enchant_value(enchantindex, SETMISSILETREATMENT);
			unset_enchant_value(enchantindex, SETCOSTFOREACHMISSILE);
			unset_enchant_value(enchantindex, SETCHANNEL);
			unset_enchant_value(enchantindex, SETMORPH);


			// Remove all of the cumulative values
			add = 0;
			while(add < MAXEVEADDVALUE)
			{
				remove_enchant_value(enchantindex, add);
				add++;
			}


			// Unlink it
			if(chrfirstenchant[character] == enchantindex)
			{
				// It was the first in the list
				chrfirstenchant[character] = encnextenchant[enchantindex];
			}
			else
			{
				// Search until we find it
				currentenchant = chrfirstenchant[character];
				while(currentenchant != enchantindex)
				{
					lastenchant = currentenchant;
					currentenchant = encnextenchant[currentenchant];
				}
				// Relink the last enchantment
				encnextenchant[lastenchant] = encnextenchant[enchantindex];
			}



			// See if we spit out an end message
			if(eveendmessage[enceve[enchantindex]] >= 0)
			{
				display_message(madmsgstart[enceve[enchantindex]]+eveendmessage[enceve[enchantindex]], enctarget[enchantindex]);
			}
			// Check to see if we spawn a poof
			if(evepoofonend[enceve[enchantindex]])
			{
				spawn_poof(enctarget[enchantindex], enceve[enchantindex]);
			}
			// Check to see if the character dies
			if(evekillonend[enceve[enchantindex]])
			{
				if(chrinvictus[character])  teammorale[chrbaseteam[character]]++;
				chrinvictus[character] = FALSE;
				kill_character(character, MAXCHR);
			}
			// Kill overlay too...
			overlay = encoverlay[enchantindex];
			if(overlay<MAXCHR)
			{
				if(chrinvictus[overlay])  teammorale[chrbaseteam[overlay]]++;
				chrinvictus[overlay] = FALSE;
				kill_character(overlay, MAXCHR);
			}





			// Now get rid of it
			encon[enchantindex] = FALSE;
			freeenchant[numfreeenchant] = enchantindex;
			numfreeenchant++;


			// Now fix dem weapons
			reset_character_alpha(chrholdingwhich[character][0]);
			reset_character_alpha(chrholdingwhich[character][1]);
		}
	}
}

//--------------------------------------------------------------------------------------------
unsigned short enchant_value_filled(unsigned short enchantindex, unsigned char valueindex)
{
	// ZZ> This function returns MAXENCHANT if the enchantment's target has no conflicting
	//     set values in its other enchantments.  Otherwise it returns the enchantindex
	//     of the conflicting enchantment
	unsigned short character, currenchant;

	character = enctarget[enchantindex];
	currenchant = chrfirstenchant[character];
	while(currenchant != MAXENCHANT)
	{
		if(encsetyesno[currenchant][valueindex]==TRUE)
		{
			return currenchant;
		}
		currenchant = encnextenchant[currenchant];
	}
	return MAXENCHANT;
}

//--------------------------------------------------------------------------------------------
void set_enchant_value(unsigned short enchantindex, unsigned char valueindex,
		unsigned short enchanttype)
{
	// ZZ> This function sets and saves one of the character's stats
	unsigned short conflict, character;


	encsetyesno[enchantindex][valueindex] = FALSE;
	if(evesetyesno[enchanttype][valueindex])
	{
		conflict = enchant_value_filled(enchantindex, valueindex);
		if(conflict == MAXENCHANT || eveoverride[enchanttype])
		{
			// Check for multiple enchantments
			if(conflict < MAXENCHANT)
			{
				// Multiple enchantments aren't allowed for sets
				if(everemoveoverridden[enchanttype])
				{
					// Kill the old enchantment
					remove_enchant(conflict);
				}
				else
				{
					// Just unset the old enchantment's value
					unset_enchant_value(conflict, valueindex);
				}
			}
			// Set the value, and save the character's real stat
			character = enctarget[enchantindex];
			encsetyesno[enchantindex][valueindex] = TRUE;
			switch(valueindex)
			{
				case SETDAMAGETYPE:
					encsetsave[enchantindex][valueindex] = chrdamagetargettype[character];
					chrdamagetargettype[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETNUMBEROFJUMPS:
					encsetsave[enchantindex][valueindex] = chrjumpnumberreset[character];
					chrjumpnumberreset[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETLIFEBARCOLOR:
					encsetsave[enchantindex][valueindex] = chrlifecolor[character];
					chrlifecolor[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETMANABARCOLOR:
					encsetsave[enchantindex][valueindex] = chrmanacolor[character];
					chrmanacolor[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETSLASHMODIFIER:
					encsetsave[enchantindex][valueindex] = chrdamagemodifier[character][DAMAGESLASH];
					chrdamagemodifier[character][DAMAGESLASH] = evesetvalue[enchanttype][valueindex];
					break;
				case SETCRUSHMODIFIER:
					encsetsave[enchantindex][valueindex] = chrdamagemodifier[character][DAMAGECRUSH];
					chrdamagemodifier[character][DAMAGECRUSH] = evesetvalue[enchanttype][valueindex];
					break;
				case SETPOKEMODIFIER:
					encsetsave[enchantindex][valueindex] = chrdamagemodifier[character][DAMAGEPOKE];
					chrdamagemodifier[character][DAMAGEPOKE] = evesetvalue[enchanttype][valueindex];
					break;
				case SETHOLYMODIFIER:
					encsetsave[enchantindex][valueindex] = chrdamagemodifier[character][DAMAGEHOLY];
					chrdamagemodifier[character][DAMAGEHOLY] = evesetvalue[enchanttype][valueindex];
					break;
				case SETEVILMODIFIER:
					encsetsave[enchantindex][valueindex] = chrdamagemodifier[character][DAMAGEEVIL];
					chrdamagemodifier[character][DAMAGEEVIL] = evesetvalue[enchanttype][valueindex];
					break;
				case SETFIREMODIFIER:
					encsetsave[enchantindex][valueindex] = chrdamagemodifier[character][DAMAGEFIRE];
					chrdamagemodifier[character][DAMAGEFIRE] = evesetvalue[enchanttype][valueindex];
					break;
				case SETICEMODIFIER:
					encsetsave[enchantindex][valueindex] = chrdamagemodifier[character][DAMAGEICE];
					chrdamagemodifier[character][DAMAGEICE] = evesetvalue[enchanttype][valueindex];
					break;
				case SETZAPMODIFIER:
					encsetsave[enchantindex][valueindex] = chrdamagemodifier[character][DAMAGEZAP];
					chrdamagemodifier[character][DAMAGEZAP] = evesetvalue[enchanttype][valueindex];
					break;
				case SETFLASHINGAND:
					encsetsave[enchantindex][valueindex] = chrflashand[character];
					chrflashand[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETLIGHTBLEND:
					encsetsave[enchantindex][valueindex] = chrlight[character];
					chrlight[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETALPHABLEND:
					encsetsave[enchantindex][valueindex] = chralpha[character];
					chralpha[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETSHEEN:
					encsetsave[enchantindex][valueindex] = chrsheen[character];
					chrsheen[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETFLYTOHEIGHT:
					encsetsave[enchantindex][valueindex] = chrflyheight[character];
					if(chrflyheight[character]==0 && chrzpos[character] > -2)
					{
						chrflyheight[character] = evesetvalue[enchanttype][valueindex];
					}
					break;
				case SETWALKONWATER:
					encsetsave[enchantindex][valueindex] = chrwaterwalk[character];
					if(chrwaterwalk[character]==FALSE)
					{
						chrwaterwalk[character] = evesetvalue[enchanttype][valueindex];
					}
					break;
				case SETCANSEEINVISIBLE:
					encsetsave[enchantindex][valueindex] = chrcanseeinvisible[character];
					chrcanseeinvisible[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETMISSILETREATMENT:
					encsetsave[enchantindex][valueindex] = chrmissiletreatment[character];
					chrmissiletreatment[character] = evesetvalue[enchanttype][valueindex];
					break;
				case SETCOSTFOREACHMISSILE:
					encsetsave[enchantindex][valueindex] = chrmissilecost[character];
					chrmissilecost[character] = evesetvalue[enchanttype][valueindex];
					chrmissilehandler[character] = encowner[enchantindex];
					break;
				case SETMORPH:
					encsetsave[enchantindex][valueindex] = chrtexture[character] - madskinstart[chrmodel[character]];
					// Special handler for morph
					change_character(character, enchanttype, 0, LEAVEALL); // LEAVEFIRST);
					chralert[character]|=ALERTIFCHANGED;
					break;
				case SETCHANNEL:
					encsetsave[enchantindex][valueindex] = chrcanchannel[character];
					chrcanchannel[character] = evesetvalue[enchanttype][valueindex];
					break;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
void getadd(int min, int value, int max, int* valuetoadd)
{
	// ZZ> This function figures out what value to add should be in order
	//     to not overflow the min and max bounds
	int newvalue;

	newvalue = value+(*valuetoadd);
	if(newvalue < min)
	{
		// Increase valuetoadd to fit
		*valuetoadd = min-value;
		if(*valuetoadd > 0)  *valuetoadd=0;
		return;
	}


	if(newvalue > max)
	{
		// Decrease valuetoadd to fit
		*valuetoadd = max-value;
		if(*valuetoadd < 0)  *valuetoadd=0;
	}
}

//--------------------------------------------------------------------------------------------
void fgetadd(float min, float value, float max, float* valuetoadd)
{
	// ZZ> This function figures out what value to add should be in order
	//     to not overflow the min and max bounds
	float newvalue;


	newvalue = value+(*valuetoadd);
	if(newvalue < min)
	{
		// Increase valuetoadd to fit
		*valuetoadd = min-value;
		if(*valuetoadd > 0)  *valuetoadd=0;
		return;
	}


	if(newvalue > max)
	{
		// Decrease valuetoadd to fit
		*valuetoadd = max-value;
		if(*valuetoadd < 0)  *valuetoadd=0;
	}
}

//--------------------------------------------------------------------------------------------
void add_enchant_value(unsigned short enchantindex, unsigned char valueindex,
		unsigned short enchanttype)
{
	// ZZ> This function does cumulative modification to character stats
	int valuetoadd, newvalue;
	float fvaluetoadd, fnewvalue;
	unsigned short character;


	character = enctarget[enchantindex];
	switch(valueindex)
	{
		case ADDJUMPPOWER:
			fnewvalue = chrjump[character];
			fvaluetoadd = eveaddvalue[enchanttype][valueindex]/16.0;
			fgetadd(0, fnewvalue, 20.0, &fvaluetoadd);
			valuetoadd = fvaluetoadd*16.0; // Get save value
			fvaluetoadd = valuetoadd/16.0;
			chrjump[character]+=fvaluetoadd;
			break;
		case ADDBUMPDAMPEN:
			fnewvalue = chrbumpdampen[character];
			fvaluetoadd = eveaddvalue[enchanttype][valueindex]/128.0;
			fgetadd(0, fnewvalue, 1.0, &fvaluetoadd);
			valuetoadd = fvaluetoadd*128.0; // Get save value
			fvaluetoadd = valuetoadd/128.0;
			chrbumpdampen[character]+=fvaluetoadd;
			break;
		case ADDBOUNCINESS:
			fnewvalue = chrdampen[character];
			fvaluetoadd = eveaddvalue[enchanttype][valueindex]/128.0;
			fgetadd(0, fnewvalue, 0.95, &fvaluetoadd);
			valuetoadd = fvaluetoadd*128.0; // Get save value
			fvaluetoadd = valuetoadd/128.0;
			chrdampen[character]+=fvaluetoadd;
			break;
		case ADDDAMAGE:
			newvalue = chrdamageboost[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex] << 6;
			getadd(0, newvalue, 4096, &valuetoadd);
			chrdamageboost[character]+=valuetoadd;
			break;
		case ADDSIZE:
			fnewvalue = chrsizegoto[character];
			fvaluetoadd = eveaddvalue[enchanttype][valueindex]/128.0;
			fgetadd(0.5, fnewvalue, 2.0, &fvaluetoadd);
			valuetoadd = fvaluetoadd*128.0; // Get save value
			fvaluetoadd = valuetoadd/128.0;
			chrsizegoto[character]+=fvaluetoadd;
			chrsizegototime[character] = SIZETIME;
			break;
		case ADDACCEL:
			fnewvalue = chrmaxaccel[character];
			fvaluetoadd = eveaddvalue[enchanttype][valueindex]/25.0;
			fgetadd(0, fnewvalue, 1.5, &fvaluetoadd);
			valuetoadd = fvaluetoadd*1000.0; // Get save value
			fvaluetoadd = valuetoadd/1000.0;
			chrmaxaccel[character]+=fvaluetoadd;
			break;
		case ADDRED:
			newvalue = chrredshift[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex];
			getadd(0, newvalue, 6, &valuetoadd);
			chrredshift[character]+=valuetoadd;
			break;
		case ADDGRN:
			newvalue = chrgrnshift[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex];
			getadd(0, newvalue, 6, &valuetoadd);
			chrgrnshift[character]+=valuetoadd;
			break;
		case ADDBLU:
			newvalue = chrblushift[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex];
			getadd(0, newvalue, 6, &valuetoadd);
			chrblushift[character]+=valuetoadd;
			break;
		case ADDDEFENSE:
			newvalue = chrdefense[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex];
			getadd(55, newvalue, 255, &valuetoadd);  // Don't fix again!
			chrdefense[character]+=valuetoadd;
			break;
		case ADDMANA:
			newvalue = chrmanamax[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex] << 6;
			getadd(0, newvalue, HIGHSTAT, &valuetoadd);
			chrmanamax[character]+=valuetoadd;
			chrmana[character]+=valuetoadd;
			if(chrmana[character] < 0)  chrmana[character] = 0;
			break;
		case ADDLIFE:
			newvalue = chrlifemax[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex] << 6;
			getadd(LOWSTAT, newvalue, HIGHSTAT, &valuetoadd);
			chrlifemax[character]+=valuetoadd;
			chrlife[character]+=valuetoadd;
			if(chrlife[character] < 1)  chrlife[character] = 1;
			break;
		case ADDSTRENGTH:
			newvalue = chrstrength[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex] << 6;
			getadd(0, newvalue, PERFECTSTAT, &valuetoadd);
			chrstrength[character]+=valuetoadd;
			break;
		case ADDWISDOM:
			newvalue = chrwisdom[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex] << 6;
			getadd(0, newvalue, PERFECTSTAT, &valuetoadd);
			chrwisdom[character]+=valuetoadd;
			break;
		case ADDINTELLIGENCE:
			newvalue = chrintelligence[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex] << 6;
			getadd(0, newvalue, PERFECTSTAT, &valuetoadd);
			chrintelligence[character]+=valuetoadd;
			break;
		case ADDDEXTERITY:
			newvalue = chrdexterity[character];
			valuetoadd = eveaddvalue[enchanttype][valueindex] << 6;
			getadd(0, newvalue, PERFECTSTAT, &valuetoadd);
			chrdexterity[character]+=valuetoadd;
			break;
	}
	encaddsave[enchantindex][valueindex] = valuetoadd;  // Save the value for undo
}


//--------------------------------------------------------------------------------------------
unsigned short spawn_enchant(unsigned short owner, unsigned short target,
		unsigned short spawner, unsigned short enchantindex, unsigned short modeloptional)
{
	// ZZ> This function enchants a target, returning the enchantment index or MAXENCHANT
	//     if failed
	unsigned short enchanttype, overlay;
	int add;


	if(modeloptional < MAXMODEL)
	{
		// The enchantment type is given explicitly
		enchanttype = modeloptional;
	}
	else
	{
		// The enchantment type is given by the spawner
		enchanttype = chrmodel[spawner];
	}


	// Target and owner must both be alive and on and valid
	if(target < MAXCHR)
	{
		if(!chron[target] || !chralive[target])
			return MAXENCHANT;
	}
	else
	{
		// Invalid target
		return MAXENCHANT;
	}
	if(owner < MAXCHR)
	{
		if(!chron[owner] || !chralive[owner])
			return MAXENCHANT;
	}
	else
	{
		// Invalid target
		return MAXENCHANT;
	}


	if(evevalid[enchanttype])
	{
		if(enchantindex == MAXENCHANT)
		{
			// Should it choose an inhand item?
			if(everetarget[enchanttype])
			{
				// Is at least one valid?
				if(chrholdingwhich[target][0] == MAXCHR && chrholdingwhich[target][1] == MAXCHR)
				{
					// No weapons to pick
					return MAXENCHANT;
				}
				// Left, right, or both are valid
				if(chrholdingwhich[target][0] == MAXCHR)
				{
					// Only right hand is valid
					target = chrholdingwhich[target][1];
				}
				else
				{
					// Pick left hand
					target = chrholdingwhich[target][0];
				}
			}


			// Make sure it's valid
			if(evedontdamagetype[enchanttype] != DAMAGENULL)
			{
				if((chrdamagemodifier[target][evedontdamagetype[enchanttype]]&7)>=3)  // Invert | Shift = 7
				{
					return MAXENCHANT;
				}
			}
			if(eveonlydamagetype[enchanttype] != DAMAGENULL)
			{
				if(chrdamagetargettype[target] != eveonlydamagetype[enchanttype])
				{
					return MAXENCHANT;
				}
			}


			// Find one to use
			enchantindex = get_free_enchant();
		}
		else
		{
			numfreeenchant--;  // To keep it in order
		}
		if(enchantindex < MAXENCHANT)
		{
			// Make a new one
			encon[enchantindex] = TRUE;
			enctarget[enchantindex] = target;
			encowner[enchantindex] = owner;
			encspawner[enchantindex] = spawner;
			if(spawner < MAXCHR)
			{
				chrundoenchant[spawner] = enchantindex;
			}
			enceve[enchantindex] = enchanttype;
			enctime[enchantindex] = evetime[enchanttype];
			encspawntime[enchantindex] = 1;
			encownermana[enchantindex] = eveownermana[enchanttype];
			encownerlife[enchantindex] = eveownerlife[enchanttype];
			enctargetmana[enchantindex] = evetargetmana[enchanttype];
			enctargetlife[enchantindex] = evetargetlife[enchanttype];



			// Add it as first in the list
			encnextenchant[enchantindex] = chrfirstenchant[target];
			chrfirstenchant[target] = enchantindex;


			// Now set all of the specific values, morph first
			set_enchant_value(enchantindex, SETMORPH, enchanttype);
			set_enchant_value(enchantindex, SETDAMAGETYPE, enchanttype);
			set_enchant_value(enchantindex, SETNUMBEROFJUMPS, enchanttype);
			set_enchant_value(enchantindex, SETLIFEBARCOLOR, enchanttype);
			set_enchant_value(enchantindex, SETMANABARCOLOR, enchanttype);
			set_enchant_value(enchantindex, SETSLASHMODIFIER, enchanttype);
			set_enchant_value(enchantindex, SETCRUSHMODIFIER, enchanttype);
			set_enchant_value(enchantindex, SETPOKEMODIFIER, enchanttype);
			set_enchant_value(enchantindex, SETHOLYMODIFIER, enchanttype);
			set_enchant_value(enchantindex, SETEVILMODIFIER, enchanttype);
			set_enchant_value(enchantindex, SETFIREMODIFIER, enchanttype);
			set_enchant_value(enchantindex, SETICEMODIFIER, enchanttype);
			set_enchant_value(enchantindex, SETZAPMODIFIER, enchanttype);
			set_enchant_value(enchantindex, SETFLASHINGAND, enchanttype);
			set_enchant_value(enchantindex, SETLIGHTBLEND, enchanttype);
			set_enchant_value(enchantindex, SETALPHABLEND, enchanttype);
			set_enchant_value(enchantindex, SETSHEEN, enchanttype);
			set_enchant_value(enchantindex, SETFLYTOHEIGHT, enchanttype);
			set_enchant_value(enchantindex, SETWALKONWATER, enchanttype);
			set_enchant_value(enchantindex, SETCANSEEINVISIBLE, enchanttype);
			set_enchant_value(enchantindex, SETMISSILETREATMENT, enchanttype);
			set_enchant_value(enchantindex, SETCOSTFOREACHMISSILE, enchanttype);
			set_enchant_value(enchantindex, SETCHANNEL, enchanttype);


			// Now do all of the stat adds
			add = 0;
			while(add < MAXEVEADDVALUE)
			{
				add_enchant_value(enchantindex, add, enchanttype);
				add++;
			}


			// Create an overlay character?
			encoverlay[enchantindex] = MAXCHR;
			if(eveoverlay[enchanttype])
			{
				overlay = spawn_one_character(chrxpos[target], chrypos[target], chrzpos[target],
						enchanttype, chrteam[target], 0, chrturnleftright[target],
						NULL, MAXCHR);
				if(overlay < MAXCHR)
				{
					encoverlay[enchantindex] = overlay;  // Kill this character on end...
					chraitarget[overlay] = target;
					chraistate[overlay] = eveoverlay[enchanttype];
					chroverlay[overlay] = TRUE;


					// Start out with ActionMJ...  Object activated
					if(madactionvalid[chrmodel[overlay]][ACTIONMJ])
					{
						chraction[overlay] = ACTIONMJ;
						chrlip[overlay] = 0;
						chrframe[overlay] = madactionstart[chrmodel[overlay]][ACTIONMJ];
						chrlastframe[overlay] = chrframe[overlay];
						chractionready[overlay] = FALSE;
					}
					chrlight[overlay] = 254;  // Assume it's transparent...
				}
			}
		}
		return enchantindex;
	}
	return MAXENCHANT;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void load_action_names(char* loadname)
{
	// ZZ> This function loads all of the 2 letter action names
	FILE* fileread;
	int cnt;
	char first, second;

	fileread = fopen(FILENAME(loadname), "r");
	if(fileread)
	{
		cnt = 0;
		while(cnt < MAXACTION)
		{
			goto_colon(fileread);
			fscanf(fileread, "%c%c", &first, &second);
			cActionName[cnt][0] = first;
			cActionName[cnt][1] = second;
			cnt++;
		}
		fclose(fileread);
	}
}

//--------------------------------------------------------------------------------------------
void get_name(FILE* fileread, char *szName)
{
	// ZZ> This function loads a string of up to MAXCAPNAMESIZE characters, parsing
	//     it for underscores.  The szName argument is rewritten with the null terminated
	//     string
	int cnt;
	char cTmp;
	char szTmp[256];


	fscanf(fileread, "%s", szTmp);
	cnt = 0;
	while(cnt < MAXCAPNAMESIZE-1)
	{
		cTmp = szTmp[cnt];
		if(cTmp=='_')  cTmp=' ';
		szName[cnt] = cTmp;
		cnt++;
	}
	szName[cnt] = 0;
}

//--------------------------------------------------------------------------------------------
void read_setup(char* filename)
{
	// ZZ> This function loads the setup file

	ConfigFilePtr lConfigSetup;
	char lCurSectionName[64];
	BOOL lTempBool;
	Sint32 lTmpInt;
	char lTmpStr[24];


	lConfigSetup = OpenConfigFile( filename );
	if ( lConfigSetup == NULL )
	{
		//Major Error
		general_error(0,0,"Could not find Setup.txt\n");
	}
	else
	{
		globalname = filename; // heu!?

		/*********************************************

		  GRAPHIC Section

		 *********************************************/

		strcpy( lCurSectionName, "GRAPHIC" );
		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  zreflect = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  zreflect = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "Z_REFLECTION", &zreflect ) == 0 )
		{
			zreflect = FALSE; // default
		}

		/*
			goto_colon(fileread); fscanf(fileread, "%d", &iTmp);
			maxtotalmeshvertices = iTmp*1024;  // For malloc
		 */
		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "MAX_NUMBER_VERTICES", &lTmpInt ) == 0 )
		{
			lTmpInt = 25; // default
		}
		maxtotalmeshvertices = lTmpInt * 1024;

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  fullscreen = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  fullscreen = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "FULLSCREEN", &fullscreen ) == 0 )
		{
			fullscreen = FALSE; // default
		}
		/*
			goto_colon(fileread); fscanf(fileread, "%d", &iTmp);
			scrx = iTmp;  cTmp = get_first_letter(fileread);  fscanf(fileread, "%d", &iTmp);
			scry = iTmp;
		 */
		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "SCREENSIZE_X", &lTmpInt ) == 0 )
		{
			lTmpInt = 640; // default
		}
		scrx = lTmpInt;

		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "SCREENSIZE_Y", &lTmpInt ) == 0 )
		{
			lTmpInt = 480; // default
		}
		scry = lTmpInt;

		/*
			goto_colon(fileread); fscanf(fileread, "%d", &iTmp);
			scrd = iTmp;
		 */
		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "COLOR_DEPTH", &lTmpInt ) == 0 )
		{
			lTmpInt = 16; // default
		}
		scrd = lTmpInt;

		/*
			goto_colon(fileread); fscanf(fileread, "%d", &iTmp);
			scrz = iTmp;
		 */
		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "Z_DEPTH", &lTmpInt ) == 0 )
		{
			lTmpInt = 16; // default
		}
		scrz = lTmpInt;

		/*
			goto_colon(fileread); fscanf(fileread, "%d", &iTmp);
			messageon = TRUE;
			maxmessage = iTmp;
			if(maxmessage < 1)  { maxmessage = 1;  messageon = FALSE; }
			if(maxmessage > MAXMESSAGE)  { maxmessage = MAXMESSAGE; }
		 */
		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "MAX_TEXT_MESSAGE", &lTmpInt ) == 0 )
		{
			lTmpInt = 1; // default
		}
		messageon = TRUE;
		maxmessage = lTmpInt;
		if(maxmessage < 1)  { maxmessage = 1;  messageon = FALSE; }
		if(maxmessage > MAXMESSAGE)  { maxmessage = MAXMESSAGE; }

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  { staton = TRUE; wraptolerance = 90; }
			if(cTmp == 'F' || cTmp == 'f')  { staton = FALSE; wraptolerance = 32; }
		 */

		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "STATUS_BAR", &staton ) == 0 )
		{
			staton = FALSE; // default
		}
		wraptolerance = 32;
		if ( staton == TRUE )
		{
			wraptolerance = 90;
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  perspective = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  perspective = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "PERSPECTIVE_CORRECT", &perspective ) == 0 )
		{
			perspective = FALSE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  dither = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  dither = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "DITHERING", &dither ) == 0 )
		{
			dither = FALSE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  reffadeor = 0;
			if(cTmp == 'F' || cTmp == 'f')  reffadeor = 255;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "FLOOR_REFLECTION_FADEOUT", &lTempBool ) == 0 )
		{
			lTempBool = FALSE; // default
		}
		if ( lTempBool )
		{
			reffadeor = 0;
		}
		else
		{
			reffadeor = 255;
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  refon = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  refon = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "REFLECTION", &refon ) == 0 )
		{
			refon = FALSE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  shaon = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  shaon = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "SHADOWS", &shaon ) == 0 )
		{
			shaon = FALSE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  shasprite = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  shasprite = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "SHADOW_AS_SPRITE", &shasprite ) == 0 )
		{
			shasprite = TRUE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  phongon = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  phongon = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "PHONG", &phongon ) == 0 )
		{
			phongon = TRUE; // default
		}


		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  twolayerwateron = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  twolayerwateron = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "MULTI_LAYER_WATER", &twolayerwateron ) == 0 )
		{
			twolayerwateron = FALSE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  overlayvalid = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  overlayvalid = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "OVERLAY", &overlayvalid ) == 0 )
		{
			overlayvalid = FALSE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  backgroundvalid = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  backgroundvalid = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "BACKGROUND", &backgroundvalid ) == 0 )
		{
			backgroundvalid = FALSE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  fogallowed = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  fogallowed = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "FOG", &lTempBool ) == 0 )
		{
			lTempBool = FALSE; // default
		}
		fogallowed = lTempBool;

		/*********************************************

		  SOUND Section

		 *********************************************/

		strcpy( lCurSectionName, "SOUND" );
		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  soundon = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  soundon = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "SOUND", &soundon ) == 0 )
		{
			soundon = FALSE; // default
		}

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  musicvalid = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  musicvalid = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "MUSIC", &musicvalid ) == 0 )
		{
			musicvalid = FALSE; // default
		}


		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "OUTPUT_SPS", &lTmpInt ) == 0 )
		{
			lTmpInt = 22050; // default
		}
		gSoundOutputSPS = lTmpInt;
		if ( gSoundOutputSPS != 22050 || gSoundOutputSPS != 44100)
		{
			gSoundOutputSPS = 22050;
		}

		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "MAX_SOUND_CHANNEL", &lTmpInt ) == 0 )
		{
			lTmpInt = 12; // default
		}
		gSoundMaxSoundChannel = lTmpInt;
		if ( gSoundMaxSoundChannel < 8 ) gSoundMaxSoundChannel = 8;
		if ( gSoundMaxSoundChannel > 32 ) gSoundMaxSoundChannel = 32;

		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "OUPUT_BUFFER_SIZE", &lTmpInt ) == 0 )
		{
			lTmpInt = 2048; // default
		}
		gSoundOutputMixBufferSize = lTmpInt;
		if ( gSoundOutputMixBufferSize < 512 ) gSoundOutputMixBufferSize = 512;
		if ( gSoundOutputMixBufferSize > 8196 ) gSoundOutputMixBufferSize = 8196;

		/*********************************************

		  CONTROL Section

		 *********************************************/

		strcpy( lCurSectionName, "CONTROL" );

		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'G' || cTmp == 'g')  autoturncamera = 255;
			if(cTmp == 'T' || cTmp == 't')  autoturncamera = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  autoturncamera = FALSE;
		 */
		if ( GetConfigValue( lConfigSetup, lCurSectionName, "AUTOTURN_CAMERA", lTmpStr, 24) == 0 )
		{
			strcpy( lTmpStr, "GOOD" ); // default
		}


		if(lTmpStr[0] == 'G' || lTmpStr[0] == 'g')  autoturncamera = 255;
		if(lTmpStr[0] == 'T' || lTmpStr[0] == 't')  autoturncamera = TRUE;
		if(lTmpStr[0] == 'F' || lTmpStr[0] == 'f')  autoturncamera = FALSE;

		//[claforte] Force autoturncamera to FALSE, or else it doesn't move right.
		autoturncamera = FALSE;


		/*********************************************

		  NETWORK Section

		 *********************************************/

		strcpy( lCurSectionName, "NETWORK" );

		networkon = FALSE; // default

		/*
			goto_colon(fileread); fscanf(fileread, "%d", &lag);
		 */
		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "LAG_TOLERANCE", &lTmpInt ) == 0 )
		{
			lTmpInt = 2; // default
		}
		lag = lTmpInt;

		/*
			goto_colon(fileread); fscanf(fileread, "%d", &orderlag);
		 */
		if ( GetConfigIntValue( lConfigSetup, lCurSectionName, "RTS_LAG_TOLERANCE", &lTmpInt ) == 0 )
		{
			lTmpInt = 25; // default
		}
		orderlag = lTmpInt;

		/*
			goto_colon(fileread);  fscanf(fileread, "%s", nethostname);
		 */
		if ( GetConfigValue( lConfigSetup, lCurSectionName, "HOST_NAME", nethostname, 64) == 0 )
		{
			strcpy( nethostname, "no host" ); // default
		}

		/*
			goto_colon(fileread);  fscanf(fileread, "%s", netmessagename);
		 */
		if ( GetConfigValue( lConfigSetup, lCurSectionName, "MULTIPLAYER_NAME", netmessagename, 64) == 0 )
		{
			strcpy( netmessagename, "little Raoul" ); // default
		}

		/*********************************************

		  DEBUG Section

		 *********************************************/

		strcpy( lCurSectionName, "DEBUG" );
		/*
			goto_colon(fileread); cTmp = get_first_letter(fileread);
			if(cTmp == 'T' || cTmp == 't')  fpson = TRUE;
			if(cTmp == 'F' || cTmp == 'f')  fpson = FALSE;
		 */
		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "DISPLAY_FPS", &lTempBool ) == 0 )
		{
			lTempBool = TRUE; // default
		}
		fpson = lTempBool;

		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "HIDE_MOUSE", &gHideMouse ) == 0 )
		{
			gHideMouse = TRUE; // default
		}

		if ( GetConfigBooleanValue( lConfigSetup, lCurSectionName, "GRAB_MOUSE", &gGrabMouse ) == 0 )
		{
			gGrabMouse = TRUE; // default
		}

		CloseConfigFile( lConfigSetup );
	}

	/* PORT        
		goto_colon(fileread); cTmp = get_first_letter(fileread);

		if(cTmp == 'T' || cTmp == 't')  filter = D3DFILTER_LINEAR;
		if(cTmp == 'F' || cTmp == 'f')  filter = D3DFILTER_NEAREST;

		goto_colon(fileread); cTmp = get_first_letter(fileread);
		if(cTmp == 'T' || cTmp == 't')  shading = D3DSHADE_GOURAUD;
		if(cTmp == 'F' || cTmp == 'f')  shading = D3DSHADE_FLAT;

		goto_colon(fileread); cTmp = get_first_letter(fileread);
		if(cTmp == 'T' || cTmp == 't')  antialias = D3DANTIALIAS_SORTINDEPENDENT;
		if(cTmp == 'F' || cTmp == 'f')  antialias = D3DANTIALIAS_NONE;
	 */
}
//--------------------------------------------------------------------------------------------
void log_madused(char *savename)
{
	// ZZ> This is a debug function for checking model loads
	FILE* hFileWrite;
	int cnt;

	hFileWrite = fopen(FILENAME(savename), "w");
	if(hFileWrite)
	{
		fprintf(hFileWrite, "Slot usage for objects in last module loaded...\n");
		fprintf(hFileWrite, "%d of %d frames used...\n", madloadframe, MAXFRAME);
		cnt = 0;
		while(cnt < MAXMODEL)
		{
			fprintf(hFileWrite, "%3d %32s %s\n", cnt, capclassname[cnt], madname[cnt]);
			cnt++;
		}
		fclose(hFileWrite);
	}
}

//---------------------------------------------------------------------------------------------
void make_lightdirectionlookup()
{
	// ZZ> This function builds the lighting direction table
	//     The table is used to find which direction the light is coming
	//     from, based on the four corner vertices of a mesh tile.
	unsigned int cnt;
	unsigned short tl, tr, br, bl;
	int x, y;

	for (cnt = 0; cnt < 65536; cnt++)
	{
		tl = (cnt&0xf000)>>12;
		tr = (cnt&0x0f00)>>8;
		br = (cnt&0x00f0)>>4;
		bl = (cnt&0x000f);
		x = br+tr-bl-tl;
		y = br+bl-tl-tr;
		lightdirectionlookup[cnt] = (atan2(-y, x)+PI)*256/(2*PI);
	}
}

float sinlut[MAXLIGHTROTATION];
float coslut[MAXLIGHTROTATION];

//---------------------------------------------------------------------------------------------
float light_for_normal(int rotation, int normal, float lx, float ly, float lz, float ambi)
{
	// ZZ> This function helps make_lighttable
	float fTmp;
	float nx, ny, nz;
	float sinrot, cosrot;

	nx = md2normals[normal][0];
	ny = md2normals[normal][1];
	nz = md2normals[normal][2];
	sinrot = sinlut[rotation];
	cosrot = coslut[rotation];
	fTmp = cosrot*nx + sinrot*ny;
	ny = cosrot*ny - sinrot*nx;
	nx = fTmp;
	fTmp = nx*lx + ny*ly + nz*lz + ambi;
	if(fTmp < ambi) fTmp = ambi;
	return fTmp;
}


//---------------------------------------------------------------------------------------------
void make_lighttable(float lx, float ly, float lz, float ambi)
{
	// ZZ> This function makes a light table to fake directional lighting
	int lev, cnt, tnc;
	int itmp, itmptwo;

	// Build a lookup table for sin/cos
	for (cnt = 0; cnt < MAXLIGHTROTATION; cnt++)
	{
		sinlut[cnt] = sin(2*PI*cnt/MAXLIGHTROTATION);
		coslut[cnt] = cos(2*PI*cnt/MAXLIGHTROTATION);
	}

	for (cnt = 0; cnt < MD2LIGHTINDICES-1; cnt++)  // Spikey mace
	{
		for (tnc = 0; tnc < MAXLIGHTROTATION; tnc++)
		{
			lev = MAXLIGHTLEVEL-1;
			itmp = (255*light_for_normal(tnc,
						cnt,
						lx*lev/MAXLIGHTLEVEL,
						ly*lev/MAXLIGHTLEVEL,
						lz*lev/MAXLIGHTLEVEL,
						ambi));
			// This creates the light value for each level entry
			while(lev >= 0)
			{
				itmptwo = (((lev*itmp/(MAXLIGHTLEVEL-1))));
				if(itmptwo > 255)  itmptwo = 255;
				lighttable[lev][tnc][cnt] = (unsigned char) itmptwo;
				lev--;
			}
		}
	}
	// Fill in index number 162 for the spike mace
	for (tnc = 0; tnc < MAXLIGHTROTATION; tnc++)
	{
		lev = MAXLIGHTLEVEL-1;
		itmp = 255;
		// This creates the light value for each level entry
		while(lev >= 0)
		{
			itmptwo = (((lev*itmp/(MAXLIGHTLEVEL-1))));
			if(itmptwo > 255)  itmptwo = 255;
			lighttable[lev][tnc][cnt] = (unsigned char) itmptwo;
			lev--;
		}
	}
}

//---------------------------------------------------------------------------------------------
void make_lighttospek(void)
{
	// ZZ> This function makes a light table to fake directional lighting
	int cnt, tnc;
	unsigned char spek;
	float fTmp, fPow;


	// New routine
	for (cnt = 0; cnt < MAXSPEKLEVEL; cnt++)
	{
		for (tnc = 0; tnc < 256; tnc++)
		{
			fTmp = tnc/256.0;
			fPow = (fTmp*4.0)+1;
			fTmp = pow(fTmp, fPow);
			fTmp = fTmp*cnt*255.0/MAXSPEKLEVEL;
			if(fTmp<0) fTmp=0;
			if(fTmp>255) fTmp=255;
			spek = fTmp;
			spek = spek>>1;
			lighttospek[cnt][tnc] = (0xff000000)|(spek<<16)|(spek<<8)|(spek);
		}
	}
}

//---------------------------------------------------------------------------------------------
int vertexconnected(int modelindex, int vertex)
{
	// ZZ> This function returns 1 if the model vertex is connected, 0 otherwise
	int cnt, tnc, entry;

	entry = 0;
	for (cnt = 0; cnt < madcommands[modelindex]; cnt++)
	{
		for (tnc = 0; tnc < madcommandsize[modelindex][cnt]; tnc++)
		{
			if(madcommandvrt[modelindex][entry] == vertex)
			{
				// The vertex is used
				return 1;
			}
			entry++;
		}
	}

	// The vertex is not used
	return 0;
}

//---------------------------------------------------------------------------------------------
void get_madtransvertices(int modelindex)
{
	// ZZ> This function gets the number of vertices to transform for a model...
	//     That means every one except the grip ( unconnected ) vertices
	int cnt, trans = 0;

	for (cnt = 0; cnt < madvertices[modelindex]; cnt++)
		trans += vertexconnected(modelindex, cnt);

	madtransvertices[modelindex] = trans;
}

//---------------------------------------------------------------------------------------------
int rip_md2_header(void)
{
	// ZZ> This function makes sure an md2 is really an md2
	int iTmp;
	int* ipIntPointer;

	// Check the file type
	ipIntPointer = (int*) cLoadBuffer;
#ifdef _LITTLE_ENDIAN
	iTmp = ipIntPointer[0];
#else
	iTmp = convert_int( ipIntPointer[0] );
#endif
	if(iTmp != MD2START ) return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------------
void fix_md2_normals(unsigned short modelindex)
{
	// ZZ> This function helps light not flicker so much
	int cnt, tnc;
	unsigned char indexofcurrent, indexofnext, indexofnextnext, indexofnextnextnext;
	unsigned char indexofnextnextnextnext;
	unsigned int frame;

	frame = madframestart[modelindex];
	cnt = 0;
	while(cnt < madvertices[modelindex])
	{
		tnc = 0;
		while(tnc < madframes[modelindex])
		{
			indexofcurrent = madvrta[frame][cnt];
			indexofnext = madvrta[frame+1][cnt];
			indexofnextnext = madvrta[frame+2][cnt];
			indexofnextnextnext = madvrta[frame+3][cnt];
			indexofnextnextnextnext = madvrta[frame+4][cnt];
			if(indexofcurrent == indexofnextnext && indexofnext != indexofcurrent)
			{
				madvrta[frame+1][cnt] = indexofcurrent;
			}
			if(indexofcurrent == indexofnextnextnext)
			{
				if(indexofnext != indexofcurrent)
				{
					madvrta[frame+1][cnt] = indexofcurrent;
				}
				if(indexofnextnext != indexofcurrent)
				{
					madvrta[frame+2][cnt] = indexofcurrent;
				}
			}
			if(indexofcurrent == indexofnextnextnextnext)
			{
				if(indexofnext != indexofcurrent)
				{
					madvrta[frame+1][cnt] = indexofcurrent;
				}
				if(indexofnextnext != indexofcurrent)
				{
					madvrta[frame+2][cnt] = indexofcurrent;
				}
				if(indexofnextnextnext != indexofcurrent)
				{
					madvrta[frame+3][cnt] = indexofcurrent;
				}
			}
			tnc++;
		}
		cnt++;
	}
}

//---------------------------------------------------------------------------------------------
void rip_md2_commands(unsigned short modelindex)
{
	// ZZ> This function converts an md2's GL commands into our little command list thing
	int iTmp;
	float fTmpu, fTmpv;
	int iNumVertices;
	int tnc;

	//    char* cpCharPointer = (char*) cLoadBuffer;
	int* ipIntPointer = (int*) cLoadBuffer;
	float* fpFloatPointer = (float*) cLoadBuffer;

	// Number of GL commands in the MD2
#ifdef _LITTLE_ENDIAN
	int iNumCommands = ipIntPointer[9];
#else
	int iNumCommands = convert_int( ipIntPointer[9] );
#endif

	// Offset (in DWORDS) from the start of the file to the gl command list.
#ifdef _LITTLE_ENDIAN
	int iCommandOffset = ipIntPointer[15]>>2;	
#else
	int iCommandOffset = convert_int( ipIntPointer[15] )>>2;
#endif

	// Read in each command
	// iNumCommands isn't the number of commands, rather the number of dwords in
	// the command list...  Use iCommandCount to figure out how many we use
	int iCommandCount = 0;
	int entry = 0;

	int cnt = 0;
	while(cnt < iNumCommands)
	{
#ifdef _LITTLE_ENDIAN
		iNumVertices = ipIntPointer[iCommandOffset]; iCommandOffset++; cnt++;
#else
		iNumVertices = convert_int( ipIntPointer[iCommandOffset] );  iCommandOffset++;  cnt++;
#endif
		if(iNumVertices != 0)
		{
			if(iNumVertices < 0)
			{
				// Fans start with a negative
				iNumVertices = -iNumVertices;
				// PORT: madcommandtype[modelindex][iCommandCount] = (unsigned char) D3DPT_TRIANGLEFAN;
				madcommandtype[modelindex][iCommandCount] = GL_TRIANGLE_FAN;
				madcommandsize[modelindex][iCommandCount] = (unsigned char) iNumVertices;
			}
			else
			{
				// Strips start with a positive
				// PORT: madcommandtype[modelindex][iCommandCount] = (unsigned char) D3DPT_TRIANGLESTRIP;
				madcommandtype[modelindex][iCommandCount] = GL_TRIANGLE_STRIP;
				madcommandsize[modelindex][iCommandCount] = (unsigned char) iNumVertices;
			}

			// Read in vertices for each command
			tnc = 0;
			while(tnc < iNumVertices)
			{
#ifdef _LITTLE_ENDIAN
				fTmpu = fpFloatPointer[iCommandOffset];  iCommandOffset++;  cnt++;
				fTmpv = fpFloatPointer[iCommandOffset];  iCommandOffset++;  cnt++;
				iTmp = ipIntPointer[iCommandOffset];  iCommandOffset++;  cnt++;
#else
				fTmpu = convert_float( fpFloatPointer[iCommandOffset] );  iCommandOffset++;  cnt++;
				fTmpv = convert_float( fpFloatPointer[iCommandOffset] );  iCommandOffset++;  cnt++;
				iTmp = convert_int( ipIntPointer[iCommandOffset] );  iCommandOffset++;  cnt++;
#endif
				madcommandu[modelindex][entry] = fTmpu-(.5/64); // GL doesn't align correctly
				madcommandv[modelindex][entry] = fTmpv-(.5/64); // with D3D
				madcommandvrt[modelindex][entry] = (unsigned short) iTmp;
				entry++;
				tnc++;
			}
			iCommandCount++;
		}
	}
	madcommands[modelindex] = iCommandCount;
}

//---------------------------------------------------------------------------------------------
int rip_md2_frame_name(int frame)
{
	// ZZ> This function gets frame names from the load buffer, it returns
	//     TRUE if the name in cFrameName[] is valid
	int iFrameOffset;
	int iNumVertices;
	int iNumFrames;
	int cnt;
	int* ipNamePointer;
	int* ipIntPointer;
	int foundname;

	// Jump to the Frames section of the md2 data
	ipNamePointer = (int*) cFrameName;
	ipIntPointer = (int*) cLoadBuffer;

#ifdef _LITTLE_ENDIAN
	iNumVertices = ipIntPointer[6];
	iNumFrames = ipIntPointer[10];
	iFrameOffset = ipIntPointer[14]>>2;
#else
	iNumVertices = convert_int( ipIntPointer[6] );
	iNumFrames = convert_int( ipIntPointer[10] );
	iFrameOffset = convert_int( ipIntPointer[14] )>>2;
#endif


	// Chug through each frame
	foundname = FALSE;
	cnt = 0;
	while(cnt < iNumFrames && !foundname)
	{
		iFrameOffset+=6;
		if(cnt == frame)
		{
			ipNamePointer[0] = ipIntPointer[iFrameOffset]; iFrameOffset++;
			ipNamePointer[1] = ipIntPointer[iFrameOffset]; iFrameOffset++;
			ipNamePointer[2] = ipIntPointer[iFrameOffset]; iFrameOffset++;
			ipNamePointer[3] = ipIntPointer[iFrameOffset]; iFrameOffset++;
			foundname = TRUE;
		}
		else
		{
			iFrameOffset+=4;
		}
		iFrameOffset+=iNumVertices;
		cnt++;
	}
	cFrameName[15] = 0;  // Make sure it's null terminated
	return foundname;
}

//---------------------------------------------------------------------------------------------
void rip_md2_frames(unsigned short modelindex)
{
	// ZZ> This function gets frames from the load buffer and adds them to
	//     the indexed model
	unsigned char cTmpx, cTmpy, cTmpz;
	unsigned char cTmpNormalIndex;
	float fRealx, fRealy, fRealz;
	float fScalex, fScaley, fScalez;
	float fTranslatex, fTranslatey, fTranslatez;
	int iFrameOffset;
	int iNumVertices;
	int iNumFrames;
	int cnt, tnc;
	char* cpCharPointer;
	int* ipIntPointer;
	float* fpFloatPointer;

	// Jump to the Frames section of the md2 data
	cpCharPointer = (char*) cLoadBuffer;
	ipIntPointer = (int*) cLoadBuffer;
	fpFloatPointer = (float*) cLoadBuffer;

#ifdef _LITTLE_ENDIAN
	iNumVertices = ipIntPointer[6];
	iNumFrames = ipIntPointer[10];
	iFrameOffset = ipIntPointer[14]>>2;
#else
	iNumVertices = convert_int( ipIntPointer[6] );
	iNumFrames = convert_int( ipIntPointer[10] );
	iFrameOffset = convert_int( ipIntPointer[14] )>>2;
#endif


	// Read in each frame
	madframestart[modelindex] = madloadframe;
	madframes[modelindex] = iNumFrames;
	madvertices[modelindex] = iNumVertices;
	madscale[modelindex] = (float) (1.0/320.0);  // Scale each vertex float to fit it in a short
	cnt = 0;
	while(cnt < iNumFrames && madloadframe < MAXFRAME)
	{
#ifdef _LITTLE_ENDIAN
		fScalex = fpFloatPointer[iFrameOffset]; iFrameOffset++;
		fScaley = fpFloatPointer[iFrameOffset]; iFrameOffset++;
		fScalez = fpFloatPointer[iFrameOffset]; iFrameOffset++;
		fTranslatex = fpFloatPointer[iFrameOffset]; iFrameOffset++;
		fTranslatey = fpFloatPointer[iFrameOffset]; iFrameOffset++;
		fTranslatez = fpFloatPointer[iFrameOffset]; iFrameOffset++;
#else
		fScalex = convert_float( fpFloatPointer[iFrameOffset] ); iFrameOffset++;
		fScaley = convert_float( fpFloatPointer[iFrameOffset] ); iFrameOffset++;
		fScalez = convert_float( fpFloatPointer[iFrameOffset] ); iFrameOffset++;
		fTranslatex = convert_float( fpFloatPointer[iFrameOffset] ); iFrameOffset++;
		fTranslatey = convert_float( fpFloatPointer[iFrameOffset] ); iFrameOffset++;
		fTranslatez = convert_float( fpFloatPointer[iFrameOffset] ); iFrameOffset++;
#endif


		iFrameOffset+=4;
		tnc = 0;
		while(tnc < iNumVertices)
		{
			// This should work because it's reading a single character
			cTmpx = cpCharPointer[(iFrameOffset<<2)];
			cTmpy = cpCharPointer[(iFrameOffset<<2)+1];
			cTmpz = cpCharPointer[(iFrameOffset<<2)+2];
			cTmpNormalIndex = cpCharPointer[(iFrameOffset<<2)+3];
			fRealx = (cTmpx*fScalex)+fTranslatex;
			fRealy = (cTmpy*fScaley)+fTranslatey;
			fRealz = (cTmpz*fScalez)+fTranslatez;
			madvrtx[madloadframe][tnc] = (signed short) (-fRealx*256);
			madvrty[madloadframe][tnc] = (signed short) (fRealy*256);
			madvrtz[madloadframe][tnc] = (signed short) (fRealz*256);
			madvrta[madloadframe][tnc] = cTmpNormalIndex;
			iFrameOffset++;
			tnc++;
		}
		madloadframe++;
		cnt++;
	}
}

//---------------------------------------------------------------------------------------------
int load_one_md2(char* szLoadname, unsigned short modelindex)
{
	// ZZ> This function loads an id md2 file, storing the converted data in the indexed model
	//    int iFileHandleRead;
	int iBytesRead = 0;
	int iReturnValue;

	// Read the input file
	printf("load one md2 '%s'\n", szLoadname);
	FILE *file = fopen(FILENAME(szLoadname),"rb");
	if (!file) {
		printf("  cannot open : %s\n", strerror(errno));
		return FALSE;
	}

	// Read up to MD2MAXLOADSIZE bytes from the file into the cLoadBuffer array.
	iBytesRead = fread(cLoadBuffer, 1, MD2MAXLOADSIZE, file);
	if (iBytesRead == 0) {
		printf("  cannot read : %s\n", strerror(errno));
		return FALSE;
	}

	// Check the header
	// TODO: Verify that the header's filesize correspond to iBytesRead.
	iReturnValue = rip_md2_header();
	if (iReturnValue == FALSE) 
		return FALSE;

	// Get the frame vertices
	rip_md2_frames(modelindex);
	// Get the commands
	rip_md2_commands(modelindex);
	// Fix them normals
	fix_md2_normals(modelindex);
	// Figure out how many vertices to transform
	get_madtransvertices(modelindex);

	return TRUE;
}

//--------------------------------------------------------------------------------------------
void make_enviro(void)
{
	// ZZ> This function sets up the environment mapping table
	int cnt;
	float z;
	float x, y;

	// Find the environment map positions
	for (cnt = 0; cnt < MD2LIGHTINDICES; cnt++)
	{
		x = md2normals[cnt][0];
		y = md2normals[cnt][1];
		x = (atan2(y, x)+PI)/(PI);
		x--; 

		if(x < 0) 
			x--;

		indextoenvirox[cnt] = x;
	}

	for (cnt = 0; cnt < 256; cnt++)
	{
		z = cnt / 256.0;  // Z is between 0 and 1
		lighttoenviroy[cnt] = z;
	}
}

//--------------------------------------------------------------------------------------------
void add_stat(unsigned short character)
{
	// ZZ> This function adds a status display to the do list
	if(numstat < MAXSTAT)
	{
		statlist[numstat] = character;
		chrstaton[character] = TRUE;
		numstat++;
	}
}

//--------------------------------------------------------------------------------------------
void move_to_top(unsigned short character)
{
	// ZZ> This function puts the character on top of the statlist
	int cnt, oldloc;


	// Find where it is
	oldloc = numstat;

	for (cnt = 0; cnt < numstat; cnt++)
		if(statlist[cnt] == character)
		{
			oldloc = cnt;
			cnt = numstat;
		}

	// Change position
	if(oldloc < numstat)
	{
		// Move all the lower ones up
		while(oldloc > 0)
		{
			oldloc--;
			statlist[oldloc+1] = statlist[oldloc];
		}
		// Put the character in the top slot
		statlist[0] = character;
	}
}

//--------------------------------------------------------------------------------------------
void sort_stat()
{
	// ZZ> This function puts all of the local players on top of the statlist
	int cnt;

	for (cnt = 0; cnt < numpla; cnt++)
		if(plavalid[cnt] && pladevice[cnt] != INPUTNONE)
		{
			move_to_top(plaindex[cnt]);
		}
}

//--------------------------------------------------------------------------------------------
void move_water(void)
{
	// ZZ> This function animates the water overlays
	int layer;

	for (layer = 0; layer < MAXWATERLAYER; layer++)
	{
		waterlayeru[layer] += waterlayeruadd[layer];
		waterlayerv[layer] += waterlayervadd[layer];
		if (waterlayeru[layer]>1.0)  waterlayeru[layer]-=1.0;
		if (waterlayerv[layer]>1.0)  waterlayerv[layer]-=1.0;
		if (waterlayeru[layer]<-1.0)  waterlayeru[layer]+=1.0;
		if (waterlayerv[layer]<-1.0)  waterlayerv[layer]+=1.0;
		waterlayerframe[layer]=(waterlayerframe[layer]+waterlayerframeadd[layer])&WATERFRAMEAND;
	}
}

//--------------------------------------------------------------------------------------------
void play_action(unsigned short character, unsigned short action, unsigned char actionready)
{
	// ZZ> This function starts a generic action for a character
	if(madactionvalid[chrmodel[character]][action])
	{
		chrnextaction[character] = ACTIONDA;
		chraction[character] = action;
		chrlip[character] = 0;
		chrlastframe[character] = chrframe[character];
		chrframe[character] = madactionstart[chrmodel[character]][chraction[character]];
		chractionready[character] = actionready;
	}
}

//--------------------------------------------------------------------------------------------
void set_frame(unsigned short character, unsigned short frame, unsigned char lip)
{
	// ZZ> This function sets the frame for a character explicitly...  This is used to
	//     rotate Tank turrets
	chrnextaction[character] = ACTIONDA;
	chraction[character] = ACTIONDA;
	chrlip[character] = (lip<<6);
	chrlastframe[character] = madactionstart[chrmodel[character]][ACTIONDA] + frame;
	chrframe[character] = madactionstart[chrmodel[character]][ACTIONDA] + frame + 1;
	chractionready[character] = TRUE;
}

//--------------------------------------------------------------------------------------------
void reset_character_alpha(unsigned short character)
{
	// ZZ> This function fixes an item's transparency
	unsigned short enchant, mount;

	if(character != MAXCHR)
	{
		mount = chrattachedto[character];
		if(chron[character] && mount != MAXCHR && chrisitem[character] && chrtransferblend[mount])
		{
			// Okay, reset transparency
			enchant = chrfirstenchant[character];
			while(enchant < MAXENCHANT)
			{
				unset_enchant_value(enchant, SETALPHABLEND);
				unset_enchant_value(enchant, SETLIGHTBLEND);
				enchant = encnextenchant[enchant];
			}
			chralpha[character] = capalpha[chrmodel[character]];
			chrlight[character] = caplight[chrmodel[character]];
			enchant = chrfirstenchant[character];
			while(enchant < MAXENCHANT)
			{
				set_enchant_value(enchant, SETALPHABLEND, enceve[enchant]);
				set_enchant_value(enchant, SETLIGHTBLEND, enceve[enchant]);
				enchant = encnextenchant[enchant];
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
int generate_number(int numbase, int numrand)
{
	int tmp;

	// ZZ> This function generates a random number
	if(numrand<=0)
	{
		general_error(numbase, numrand, "ONE OF THE DATA PAIRS IS WRONG");
	}
	tmp=(rand()%numrand)+numbase;
	return (tmp);
}

//--------------------------------------------------------------------------------------------
void drop_money(unsigned short character, unsigned short money)
{
	// ZZ> This function drops some of a character's money
	unsigned short huns, tfives, fives, ones, cnt;

	if(money > chrmoney[character])  money = chrmoney[character];
	if(money>0 && chrzpos[character] > -2)
	{
		chrmoney[character] = chrmoney[character]-money;
		huns = money/100;  money -= (huns<<7)-(huns<<5)+(huns<<2);
		tfives = money/25;  money -= (tfives<<5)-(tfives<<3)+tfives;
		fives = money/5;  money -= (fives<<2)+fives;
		ones = money;

		for (cnt = 0; cnt < ones; cnt++)
			spawn_one_particle(chrxpos[character], chrypos[character],  chrzpos[character], 0, MAXMODEL, COIN1, MAXCHR, SPAWNLAST, NULLTEAM, MAXCHR, cnt, MAXCHR);

		for (cnt = 0; cnt < fives; cnt++)
			spawn_one_particle(chrxpos[character], chrypos[character],  chrzpos[character], 0, MAXMODEL, COIN5, MAXCHR, SPAWNLAST, NULLTEAM, MAXCHR, cnt, MAXCHR);

		for (cnt = 0; cnt < tfives; cnt++)
			spawn_one_particle(chrxpos[character], chrypos[character],  chrzpos[character], 0, MAXMODEL, COIN25, MAXCHR, SPAWNLAST, NULLTEAM, MAXCHR, cnt, MAXCHR);

		for (cnt = 0; cnt < huns; cnt++)
			spawn_one_particle(chrxpos[character], chrypos[character],  chrzpos[character], 0, MAXMODEL, COIN100, MAXCHR, SPAWNLAST, NULLTEAM, MAXCHR, cnt, MAXCHR);

		chrdamagetime[character] = DAMAGETIME;  // So it doesn't grab it again
	}
}

//--------------------------------------------------------------------------------------------
void call_for_help(unsigned short character)
{
	// ZZ> This function issues a call for help to all allies
	unsigned char team;
	unsigned short cnt;

	team = chrteam[character];
	teamsissy[team] = character;

	for (cnt = 0; cnt < MAXCHR; cnt++)
		if(chron[cnt] && cnt!=character && teamhatesteam[chrteam[cnt]][team] == FALSE)
			chralert[cnt]=chralert[cnt]|ALERTIFCALLEDFORHELP;
}

//--------------------------------------------------------------------------------------------
void give_experience(int character, int amount, unsigned char xptype)
{
	// ZZ> This function gives a character experience, and pawns off level gains to
	//     another function
	int newamount;
	unsigned char curlevel;
	int number;
	int profile;
	char text[128];


	if(chrinvictus[character]==FALSE)
	{
		// Figure out how much experience to give
		profile = chrmodel[character];
		newamount = amount;
		if(xptype < MAXEXPERIENCETYPE)
		{
			newamount = amount*capexperiencerate[profile][xptype];
		}
		newamount+=chrexperience[character];
		if(newamount > MAXXP)  newamount = MAXXP;
		chrexperience[character]=newamount;


		// Do level ups and stat changes
		curlevel = chrexperiencelevel[character];
		if(curlevel < MAXLEVEL-1)
		{
			if(chrexperience[character] >= capexperienceforlevel[profile][curlevel+1])
			{
				// The character is ready to advance...
				if(chrisplayer[character])
				{
					sprintf(text, "%s gained a level!!!", chrname[character]);
					debug_message(text);
				}
				chrexperiencelevel[character]++;
				chrexperience[character] = capexperienceforlevel[profile][curlevel+1];
				// Size
				chrsizegoto[character]+=capsizeperlevel[profile];  // Limit this?
				chrsizegototime[character] = SIZETIME;
				// Strength
				number = generate_number(capstrengthperlevelbase[profile], capstrengthperlevelrand[profile]);
				number = number+chrstrength[character];
				if(number > PERFECTSTAT) number = PERFECTSTAT;
				chrstrength[character] = number;
				// Wisdom
				number = generate_number(capwisdomperlevelbase[profile], capwisdomperlevelrand[profile]);
				number = number+chrwisdom[character];
				if(number > PERFECTSTAT) number = PERFECTSTAT;
				chrwisdom[character] = number;
				// Intelligence
				number = generate_number(capintelligenceperlevelbase[profile], capintelligenceperlevelrand[profile]);
				number = number+chrintelligence[character];
				if(number > PERFECTSTAT) number = PERFECTSTAT;
				chrintelligence[character] = number;
				// Dexterity
				number = generate_number(capdexterityperlevelbase[profile], capdexterityperlevelrand[profile]);
				number = number+chrdexterity[character];
				if(number > PERFECTSTAT) number = PERFECTSTAT;
				chrdexterity[character] = number;
				// Life
				number = generate_number(caplifeperlevelbase[profile], caplifeperlevelrand[profile]);
				number = number+chrlifemax[character];
				if(number > PERFECTBIG) number = PERFECTBIG;
				chrlife[character]+=(number-chrlifemax[character]);
				chrlifemax[character] = number;
				// Mana
				number = generate_number(capmanaperlevelbase[profile], capmanaperlevelrand[profile]);
				number = number+chrmanamax[character];
				if(number > PERFECTBIG) number = PERFECTBIG;
				chrmana[character]+=(number-chrmanamax[character]);
				chrmanamax[character] = number;
				// Mana Return
				number = generate_number(capmanareturnperlevelbase[profile], capmanareturnperlevelrand[profile]);
				number = number+chrmanareturn[character];
				if(number > PERFECTSTAT) number = PERFECTSTAT;
				chrmanareturn[character] = number;
				// Mana Flow
				number = generate_number(capmanaflowperlevelbase[profile], capmanaflowperlevelrand[profile]);
				number = number+chrmanaflow[character];
				if(number > PERFECTSTAT) number = PERFECTSTAT;
				chrmanaflow[character] = number;
			}
		}
	}
}


//--------------------------------------------------------------------------------------------
void give_team_experience(unsigned char team, int amount, unsigned char xptype)
{
	// ZZ> This function gives a character experience, and pawns off level gains to
	//     another function
	int cnt;

	for (cnt = 0; cnt < MAXCHR; cnt++)
		if(chrteam[cnt] == team && chron[cnt])
			give_experience(cnt, amount, xptype);
}


//--------------------------------------------------------------------------------------------
void setup_alliances(char *modname)
{
	// ZZ> This function reads the alliance file
	char newloadname[256];
	char szTemp[256];
	unsigned char teama, teamb;
	FILE *fileread;


	// Load the file
	make_newloadname(modname, FILENAME("gamedat/alliance.txt"), newloadname);
	fileread = fopen(newloadname, "r");
	if(fileread)
	{
		while(goto_colon_yesno(fileread))
		{
			fscanf(fileread, "%s", szTemp);
			teama = (szTemp[0]-'A')%MAXTEAM;
			fscanf(fileread, "%s", szTemp);
			teamb = (szTemp[0]-'A')%MAXTEAM;
			teamhatesteam[teama][teamb] = FALSE;
		}
		fclose(fileread);
	}
}

//grfx.c
//--------------------------------------------------------------------------------------------
void load_mesh_fans()
{
	// ZZ> This function loads fan types for the terrain
	int cnt, entry;
	int numfantype, fantype, bigfantype, vertices;
	int numcommand, command, commandsize;
	int itmp;
	float ftmp;
	FILE* fileread;
	float offx, offy;


	// Initialize all mesh types to 0
	entry = 0;
	while(entry < MAXMESHTYPE)
	{
		meshcommandnumvertices[entry] = 0;
		meshcommands[entry] = 0;
		entry++;
	}


	// Open the file and go to it
	fileread = fopen(FILENAME(DATA_PATH"basicdat/fans.txt"), "r");
	if(fileread)
	{
		goto_colon(fileread);
		fscanf(fileread, "%d", &numfantype);
		fantype = 0;
		bigfantype = MAXMESHTYPE/2; // Duplicate for 64x64 tiles
		while(fantype < numfantype)
		{
			goto_colon(fileread);
			fscanf(fileread, "%d", &vertices);
			meshcommandnumvertices[fantype] = vertices;
			meshcommandnumvertices[bigfantype] = vertices;  // Dupe
			cnt = 0;
			while(cnt < vertices)
			{
				goto_colon(fileread);
				fscanf(fileread, "%d", &itmp);
				goto_colon(fileread);
				fscanf(fileread, "%f", &ftmp);
				meshcommandu[fantype][cnt] = ftmp;
				meshcommandu[bigfantype][cnt] = ftmp;  // Dupe
				goto_colon(fileread);
				fscanf(fileread, "%f", &ftmp);
				meshcommandv[fantype][cnt] = ftmp;
				meshcommandv[bigfantype][cnt] = ftmp;  // Dupe
				cnt++;
			}


			goto_colon(fileread);
			fscanf(fileread, "%d", &numcommand);
			meshcommands[fantype] = numcommand;
			meshcommands[bigfantype] = numcommand;  // Dupe
			entry = 0;
			command = 0;
			while(command < numcommand)
			{
				goto_colon(fileread);
				fscanf(fileread, "%d", &commandsize);
				meshcommandsize[fantype][command] = commandsize;
				meshcommandsize[bigfantype][command] = commandsize;  // Dupe
				cnt = 0;
				while(cnt < commandsize)
				{
					goto_colon(fileread);
					fscanf(fileread, "%d", &itmp);
					meshcommandvrt[fantype][entry] = itmp;
					meshcommandvrt[bigfantype][entry] = itmp;  // Dupe
					entry++;
					cnt++;
				}
				command++;
			}
			fantype++;
			bigfantype++;  // Dupe
		}
		fclose(fileread);
	}


	// Correct all of them silly texture positions for seamless tiling
	entry = 0;
	while(entry < MAXMESHTYPE/2)
	{
		cnt = 0;
		while(cnt < meshcommandnumvertices[entry])
		{
			//            meshcommandu[entry][cnt] = ((.5/32)+(meshcommandu[entry][cnt]*31/32))/8;
			//            meshcommandv[entry][cnt] = ((.5/32)+(meshcommandv[entry][cnt]*31/32))/8;
			meshcommandu[entry][cnt] = ((.6/32)+(meshcommandu[entry][cnt]*30.8/32))/8;
			meshcommandv[entry][cnt] = ((.6/32)+(meshcommandv[entry][cnt]*30.8/32))/8;
			cnt++;
		}
		entry++;
	}
	// Do for big tiles too
	while(entry < MAXMESHTYPE)
	{
		cnt = 0;
		while(cnt < meshcommandnumvertices[entry])
		{
			//            meshcommandu[entry][cnt] = ((.5/64)+(meshcommandu[entry][cnt]*63/64))/4;
			//            meshcommandv[entry][cnt] = ((.5/64)+(meshcommandv[entry][cnt]*63/64))/4;
			meshcommandu[entry][cnt] = ((.6/64)+(meshcommandu[entry][cnt]*62.8/64))/4;
			meshcommandv[entry][cnt] = ((.6/64)+(meshcommandv[entry][cnt]*62.8/64))/4;
			cnt++;
		}
		entry++;
	}


	// Make tile texture offsets
	entry = 0;
	while(entry < MAXTILETYPE)
	{
		offx = (entry&7)/8.0;
		offy = (entry>>3)/8.0;
		meshtileoffu[entry] = offx;
		meshtileoffv[entry] = offy;
		entry++;
	}
}

//--------------------------------------------------------------------------------------------
void make_fanstart()
{
	// ZZ> This function builds a look up table to ease calculating the
	//     fan number given an x,y pair
	int cnt;


	cnt = 0;
	while(cnt < meshsizey)
	{
		meshfanstart[cnt] = meshsizex*cnt;
		cnt++;
	}
	cnt = 0;
	while(cnt < (meshsizey>>2))
	{
		meshblockstart[cnt] = (meshsizex>>2)*cnt;
		cnt++;
	}
}

//--------------------------------------------------------------------------------------------
void make_twist()
{
	// ZZ> This function precomputes surface normals and steep hill acceleration for
	//     the mesh
	int cnt;
	int x, y;
	float xslide, yslide;

	cnt = 0;
	while(cnt < 256)
	{
		y = cnt>>4;
		x = cnt&15;
		y = y-7;  // -7 to 8
		x = x-7;  // -7 to 8
		mapudtwist[cnt] = 32768+y*SLOPE;
		maplrtwist[cnt] = 32768+x*SLOPE;
		if(ABS(y) >=7 ) y=y<<1;
		if(ABS(x) >=7 ) x=x<<1;
		xslide = x*SLIDE;
		yslide = y*SLIDE;
		if(xslide < 0)
		{
			xslide+=SLIDEFIX;
			if(xslide > 0)
				xslide=0;
		}
		else
		{
			xslide-=SLIDEFIX;
			if(xslide < 0)
				xslide=0;
		}
		if(yslide < 0)
		{
			yslide+=SLIDEFIX;
			if(yslide > 0)
				yslide=0;
		}
		else
		{
			yslide-=SLIDEFIX;
			if(yslide < 0)
				yslide=0;
		}
		veludtwist[cnt] = -yslide*hillslide;
		vellrtwist[cnt] = xslide*hillslide;
		flattwist[cnt] = FALSE;
		if(ABS(veludtwist[cnt]) + ABS(vellrtwist[cnt]) < SLIDEFIX*4)
		{
			flattwist[cnt] = TRUE;
		}
		cnt++;
	}
}

//--------------------------------------------------------------------------------------------
int load_mesh(char *modname)
{
	// ZZ> This function loads the level.mpd file
	FILE* fileread;
	char newloadname[256];
	int itmp, cnt;
	float ftmp;
	int fan;
	int numvert, numfan;
	int x, y, vert;

	make_newloadname(modname, "gamedat/level.mpd", newloadname);
	fileread = fopen(FILENAME(newloadname), "rb");
	if(fileread)
	{
#ifdef _LITTLE_ENDIAN
		fread(&itmp, 4, 1, fileread);  if(itmp != MAPID) return FALSE;
		fread(&itmp, 4, 1, fileread);  numvert = itmp;
		fread(&itmp, 4, 1, fileread);  meshsizex = itmp;
		fread(&itmp, 4, 1, fileread);  meshsizey = itmp;
#else
		fread(&itmp, 4, 1, fileread);  if( ( int )convert_int( itmp ) != MAPID) return FALSE;
		fread(&itmp, 4, 1, fileread);  numvert = ( int )convert_int( itmp );
		fread(&itmp, 4, 1, fileread);  meshsizex = ( int )convert_int( itmp );
		fread(&itmp, 4, 1, fileread);  meshsizey = ( int )convert_int( itmp );
#endif

		numfan = meshsizex*meshsizey;
		meshedgex = meshsizex*128;
		meshedgey = meshsizey*128;
		numfanblock = ((meshsizex>>2))*((meshsizey>>2));  // MESHSIZEX MUST BE MULTIPLE OF 4
		watershift = 3;
		if(meshsizex > 16)  watershift++;
		if(meshsizex > 32)  watershift++;
		if(meshsizex > 64)  watershift++;
		if(meshsizex > 128)  watershift++;
		if(meshsizex > 256)  watershift++;


		// Load fan data
		fan = 0;
		while(fan < numfan)
		{
			fread(&itmp, 4, 1, fileread);

#ifdef _LITTLE_ENDIAN
			meshtype[fan] = itmp>>24;
			meshfx[fan] = itmp>>16;
			meshtile[fan] = itmp;
#else
			meshtype[fan] = convert_int( itmp )>>24;
			meshfx[fan] = convert_int( itmp )>>16;
			meshtile[fan] = convert_int( itmp );
#endif

			fan++;
		}
		// Load fan data
		fan = 0;
		while(fan < numfan)
		{
			fread(&itmp, 1, 1, fileread);

#ifdef _LITTLE_ENDIAN
			meshtwist[fan] = itmp;
#else
			meshtwist[fan] = convert_int( itmp );
#endif

			fan++;
		}


		// Load vertex x data
		cnt = 0;
		while(cnt < numvert)
		{
			fread(&ftmp, 4, 1, fileread);

#ifdef _LITTLE_ENDIAN
			meshvrtx[cnt] = ftmp;
#else
			meshvrtx[cnt] = convert_float( ftmp );
#endif

			cnt++;
		}
		// Load vertex y data
		cnt = 0;
		while(cnt < numvert)
		{
			fread(&ftmp, 4, 1, fileread);

#ifdef _LITTLE_ENDIAN
			meshvrty[cnt] = ftmp;
#else
			meshvrty[cnt] = convert_float( ftmp );
#endif

			cnt++;
		}
		// Load vertex z data
		cnt = 0;
		while(cnt < numvert)
		{
			fread(&ftmp, 4, 1, fileread);

#ifdef _LITTLE_ENDIAN
			meshvrtz[cnt] = ftmp/16.0;  // Cartman uses 4 bit fixed point for Z
#else
			meshvrtz[cnt] = (convert_float( ftmp ))/16.0;  // Cartman uses 4 bit fixed point for Z
#endif

			cnt++;
		}
		// GS - set to if(1) to disable lighting!!!!
		if(0)    //(shading == D3DSHADE_FLAT && rtscontrol==FALSE)
		{
			// Assume fullbright
			cnt = 0;
			while(cnt < numvert)
			{
				meshvrta[cnt] = 255;
				meshvrtl[cnt] = 255;
				cnt++;
			}
		}
		else
		{
			// Load vertex a data
			cnt = 0;
			while(cnt < numvert)
			{
				fread(&itmp, 1, 1, fileread);

#ifdef _LITTLE_ENDIAN
				meshvrta[cnt] = itmp;
#else
				meshvrta[cnt] = convert_int( itmp );
#endif
				meshvrtl[cnt] = 0;

				cnt++;
			}
		}
		fclose(fileread);


		make_fanstart();


		vert = 0;
		y = 0;
		while(y < meshsizey)
		{
			x = 0;
			while(x < meshsizex)
			{
				fan = meshfanstart[y]+x;
				meshvrtstart[fan] = vert;
				vert+=meshcommandnumvertices[meshtype[fan]];
				x++;
			}
			y++;
		}

		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
void clear_orders()
{
	// ZZ> This function clears the order list
	int cnt;

	cnt = 0;
	while(cnt < MAXORDER)
	{
		ordervalid[cnt] = FALSE;
		cnt++;
	}
}

//--------------------------------------------------------------------------------------------
unsigned short get_empty_order()
{
	// ZZ> This function looks for an unused order
	int cnt;

	cnt = 0;
	while(cnt < MAXORDER)
	{
		// Find an empty slot
		if(ordervalid[cnt]==FALSE)
		{
			ordervalid[cnt] = TRUE;
			return cnt;
		}
		cnt++;
	}
	return MAXORDER;
}

//--------------------------------------------------------------------------------------------
void update_game()
{
	// ZZ> This function does several iterations of character movements and such
	//     to keep the game in sync.
	int cnt, numdead;

	// Check for all local players being dead
	alllocalpladead = FALSE;
	localseeinvisible = FALSE;
	localseekurse = FALSE;
	cnt = 0;
	numdead = 0;
	while(cnt < MAXPLAYER)
	{
		if(plavalid[cnt] && pladevice[cnt] != INPUTNONE)
		{
			if(chralive[plaindex[cnt]] == FALSE)
			{
				numdead++;
				if (key_set_once(GP2X_START)) {
					respawn_character(plaindex[cnt]);
				}
			}
			else
			{
				if(chrcanseeinvisible[plaindex[cnt]])
				{
					localseeinvisible = TRUE;
				}
				if(chrcanseekurse[plaindex[cnt]])
				{
					localseekurse = TRUE;
				}
			}
		}
		cnt++;
	}
	if(numdead >= numlocalpla)
	{
		alllocalpladead = TRUE;
	}

	// This is the main game loop
	msgtimechange = 0;

	// [claforte Jan 6th 2001]
	// TODO: Put that back in place once networking is functional.
	while(wldclock < allclock && (numplatimes > 0 || rtscontrol))
	{
		// Important stuff to keep in sync
		srand(randsave);
		talk_to_remotes();
		resize_characters();
		keep_weapons_with_holders();
		let_ai_think();
		do_weather_spawn();
		do_enchant_spawn();
		unbuffer_player_latches();
		move_characters();
		move_particles();
		make_character_matrices();
		attach_particles();
		make_onwhichfan();
		bump_characters();
		stat_return();
		pit_kill();
		chug_orders();
		// Generate the new seed
		randsave += *((unsigned int*) &md2normals[wldframe&127][0]);
		randsave += *((unsigned int*) &md2normals[randsave&127][1]);

		// Stuff for which sync doesn't matter
		check_ig_passage();
		flash_select();
		animate_tiles();
		move_water();
		play_next_music_sound();

		// Timers
		wldclock+=FRAMESKIP;
		wldframe++;
		msgtimechange++;
		statclock++;
	}
	if(rtscontrol == FALSE)
	{
		if(numplatimes == 0)
		{
			// The remote ran out of messages, and is now twiddling its thumbs...
			// Make it go slower so it doesn't happen again
			wldclock+=25;
		}
		if(numplatimes > 3 && hostactive == FALSE)
		{
			// The host has too many messages, and is probably experiencing control
			// lag...  Speed it up so it gets closer to sync
			wldclock-=5;
		}
	}

	// Do interactive music ( Keeps beat better... )
	check_ig_music();
}

//--------------------------------------------------------------------------------------------
void update_timers()
{
	// ZZ> This function updates the game timers
	lstclock = allclock;
	allclock = ClockGetTick()-sttclock;
	fpsclock+=allclock-lstclock;
	if(fpsclock >= CLOCKS_PER_SEC)//1000000)
	{
		create_szfpstext(fpsframe);
		fpsclock = 0;
		fpsframe = 0;
	}
}

//--------------------------------------------------------------------------------------------
void read_pair(FILE* fileread)
{
	// ZZ> This function reads a damage/stat pair ( eg. 5-9 )
	char cTmp;
	float  fBase, fRand;

	fscanf(fileread, "%f", &fBase);  // The first number
	pairbase = fBase*256;
	cTmp = get_first_letter(fileread);  // The hyphen
	if(cTmp!='-')
	{
		// Not in correct format, so fail
		pairrand = 1;
		return;
	}
	fscanf(fileread, "%f", &fRand);  // The second number
	pairrand = fRand*256;
	pairrand = pairrand-pairbase;
	if(pairrand<1)
		pairrand = 1;
}

//--------------------------------------------------------------------------------------------
void undo_pair(int base, int rand)
{
	// ZZ> This function generates a damage/stat pair ( eg. 3-6.5 )
	//     from the base and random values.  It set pairfrom and
	//     pairto
	pairfrom = base/256.0;
	pairto = rand/256.0;
	if(pairfrom < 0.0)  pairfrom = 0.0;
	if(pairto < 0.0)  pairto = 0.0;
	pairto += pairfrom;
}

//--------------------------------------------------------------------------------------------
void ftruthf(FILE* filewrite, char* text, unsigned char truth)
{
	// ZZ> This function kinda mimics fprintf for the output of
	//     TRUE FALSE statements

	fprintf(filewrite, text);
	if(truth)
	{
		fprintf(filewrite, "TRUE\n");
	}
	else
	{
		fprintf(filewrite, "FALSE\n");
	}
}

//--------------------------------------------------------------------------------------------
void fdamagf(FILE* filewrite, char* text, unsigned char damagetype)
{
	// ZZ> This function kinda mimics fprintf for the output of
	//     SLASH CRUSH POKE HOLY EVIL FIRE ICE ZAP statements
	fprintf(filewrite, text);
	if(damagetype == DAMAGESLASH)
		fprintf(filewrite, "SLASH\n");
	if(damagetype == DAMAGECRUSH)
		fprintf(filewrite, "CRUSH\n");
	if(damagetype == DAMAGEPOKE)
		fprintf(filewrite, "POKE\n");
	if(damagetype == DAMAGEHOLY)
		fprintf(filewrite, "HOLY\n");
	if(damagetype == DAMAGEEVIL)
		fprintf(filewrite, "EVIL\n");
	if(damagetype == DAMAGEFIRE)
		fprintf(filewrite, "FIRE\n");
	if(damagetype == DAMAGEICE)
		fprintf(filewrite, "ICE\n");
	if(damagetype == DAMAGEZAP)
		fprintf(filewrite, "ZAP\n");
	if(damagetype == DAMAGENULL)
		fprintf(filewrite, "NONE\n");
}

//--------------------------------------------------------------------------------------------
void factiof(FILE* filewrite, char* text, unsigned char action)
{
	// ZZ> This function kinda mimics fprintf for the output of
	//     SLASH CRUSH POKE HOLY EVIL FIRE ICE ZAP statements
	fprintf(filewrite, text);
	if(action == ACTIONDA)
		fprintf(filewrite, "WALK\n");
	if(action == ACTIONUA)
		fprintf(filewrite, "UNARMED\n");
	if(action == ACTIONTA)
		fprintf(filewrite, "THRUST\n");
	if(action == ACTIONSA)
		fprintf(filewrite, "SLASH\n");
	if(action == ACTIONCA)
		fprintf(filewrite, "CHOP\n");
	if(action == ACTIONBA)
		fprintf(filewrite, "BASH\n");
	if(action == ACTIONLA)
		fprintf(filewrite, "LONGBOW\n");
	if(action == ACTIONXA)
		fprintf(filewrite, "XBOW\n");
	if(action == ACTIONFA)
		fprintf(filewrite, "FLING\n");
	if(action == ACTIONPA)
		fprintf(filewrite, "PARRY\n");
	if(action == ACTIONZA)
		fprintf(filewrite, "ZAP\n");
}

//--------------------------------------------------------------------------------------------
void fgendef(FILE* filewrite, char* text, unsigned char gender)
{
	// ZZ> This function kinda mimics fprintf for the output of
	//     MALE FEMALE OTHER statements

	fprintf(filewrite, text);
	if(gender == GENMALE)
		fprintf(filewrite, "MALE\n");
	if(gender == GENFEMALE)
		fprintf(filewrite, "FEMALE\n");
	if(gender == GENOTHER)
		fprintf(filewrite, "OTHER\n");
}

//--------------------------------------------------------------------------------------------
void fpairof(FILE* filewrite, char* text, int base, int rand)
{
	// ZZ> This function mimics fprintf in spitting out
	//     damage/stat pairs
	undo_pair(base, rand);
	fprintf(filewrite, text);
	fprintf(filewrite, "%4.2f-%4.2f\n", pairfrom, pairto);
}

//--------------------------------------------------------------------------------------------
void funderf(FILE* filewrite, char* text, char* usename)
{
	// ZZ> This function mimics fprintf in spitting out
	//     a name with underscore spaces
	char cTmp;
	int cnt;


	fprintf(filewrite, text);
	cnt = 0;
	cTmp = usename[0];
	cnt++;
	while(cTmp != 0)
	{
		if(cTmp == ' ')
		{
			fprintf(filewrite, "_");
		}
		else
		{
			fprintf(filewrite, "%c", cTmp);
		}
		cTmp = usename[cnt];
		cnt++;
	}
	fprintf(filewrite, "\n");
}

//--------------------------------------------------------------------------------------------
void get_message(FILE* fileread)
{
	// ZZ> This function loads a string into the message buffer, making sure it
	//     is null terminated.
	int cnt;
	char cTmp;
	char szTmp[256];


	if(msgtotal<MAXTOTALMESSAGE)
	{
		if(msgtotalindex>=MESSAGEBUFFERSIZE)
		{
			msgtotalindex = MESSAGEBUFFERSIZE-1;
		}
		msgindex[msgtotal]=msgtotalindex;
		fscanf(fileread, "%s", szTmp);
		szTmp[255] = 0;
		cTmp = szTmp[0];
		cnt = 1;
		while(cTmp!=0 && msgtotalindex<MESSAGEBUFFERSIZE-1)
		{
			if(cTmp=='_')  cTmp=' ';
			msgtext[msgtotalindex] = cTmp;
			msgtotalindex++;
			cTmp = szTmp[cnt];
			cnt++;
		}
		msgtext[msgtotalindex]=0;  msgtotalindex++;
		msgtotal++;
	}
}

//--------------------------------------------------------------------------------------------
void load_all_messages(char *loadname, int object)
{
	// ZZ> This function loads all of an objects messages
	FILE *fileread;


	madmsgstart[object] = 0;
	fileread = fopen(FILENAME(loadname), "r");
	if(fileread)
	{
		madmsgstart[object] = msgtotal;
		while(goto_colon_yesno(fileread))
		{
			get_message(fileread);
		}
		fclose(fileread);
	}
}


//--------------------------------------------------------------------------------------------
void reset_teams()
{
	// ZZ> This function makes everyone hate everyone else
	int teama, teamb;


	teama = 0;
	while(teama < MAXTEAM)
	{
		// Make the team hate everyone
		teamb = 0;
		while(teamb < MAXTEAM)
		{
			teamhatesteam[teama][teamb] = TRUE;
			teamb++;
		}
		// Make the team like itself
		teamhatesteam[teama][teama] = FALSE;
		// Set defaults
		teamleader[teama] = NOLEADER;
		teamsissy[teama] = 0;
		teammorale[teama] = 0;
		teama++;
	}


	// Keep the null team neutral
	teama = 0;
	while(teama < MAXTEAM)
	{
		teamhatesteam[teama][NULLTEAM] = FALSE;
		teamhatesteam[NULLTEAM][teama] = FALSE;
		teama++;
	}
}

//--------------------------------------------------------------------------------------------
void reset_messages()
{
	// ZZ> This makes messages safe to use
	int cnt;

	msgtotal=0;
	msgtotalindex=0;
	msgtimechange=0;
	msgstart=0;
	cnt = 0;
	while(cnt < MAXMESSAGE)
	{
		msgtime[cnt] = 0;
		cnt++;
	}
	cnt = 0;
	while(cnt < MAXTOTALMESSAGE)
	{
		msgindex[cnt] = 0;
		cnt++;
	}
	msgtext[0] = 0;
}

//--------------------------------------------------------------------------------------------
void make_randie()
{
	// ZZ> This function makes the random number table
	int tnc, cnt;


	// Fill in the basic values
	cnt = 0;
	while(cnt < MAXRAND)
	{
		randie[cnt] = rand()<<1;
		cnt++;
	}


	// Keep adjusting those values
	tnc = 0;
	while(tnc < 20)
	{
		cnt = 0;
		while(cnt < MAXRAND)
		{
			randie[cnt] += rand();
			cnt++;
		}
		tnc++;
	}

	// All done
	randindex = 0;
}

//--------------------------------------------------------------------------------------------
void reset_timers()
{
	// ZZ> This function resets the timers...
	sttclock = ClockGetTick();
	allclock = 0;
	lstclock = 0;
	wldclock = 0;
	statclock = 0;
	pitclock = 0;  pitskill = FALSE;
	wldframe = 0;
	allframe = 0;
	fpsframe = 0;
	outofsync = FALSE;
}

//--------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	// ZZ> This is where the program starts and all the high level stuff happens
	struct glvector t1={0,0,0}; 
	struct glvector t2={0,0,-1}; 
	struct glvector t3={0,1,0}; 
	static char conffile[MAX_STR_SIZE];
	int	sdlinit_retval;
	FILE *conff;

	// Set up all of the data
	printf("Egoboo, Copyright (C) 2000 Aaron Bishop\n");

	globalnetworkerr = fopen(FILENAME(DATA_PATH"basicdat/netlog.txt"), "w");
	//Check in ~/.egoboo before
	snprintf(conffile, sizeof(conffile), "%s/setup.txt", get_import_path());
	conff = fopen(conffile, "r");
	if (conff)
	{
		fclose(conff);
		read_setup(conffile);
	}
	else
		read_setup(FILENAME(CONF_PATH"setup.txt"));
	read_all_tags(DATA_PATH"basicdat/scancode.txt");
	//Check in ~/.egoboo before
	snprintf(conffile, sizeof(conffile), "%s/controls.txt", get_import_path());
	conff = fopen(conffile, "r");
	if (conff)
	{
		fclose(conff);
		read_controls(conffile);
	}
	else
		read_controls(CONF_PATH"controls.txt");
	reset_ai_script();
	load_ai_codes(DATA_PATH"basicdat/aicodes.txt");
	load_action_names(DATA_PATH"basicdat/actions.txt");

	sdlinit_retval = sdlinit(argc,argv);
	glinit(argc,argv);

	if ( soundon && sdlinit_retval)
	{
		InitSound( gSoundOutputMixBufferSize, gSoundOutputSPS, gSoundMaxSoundChannel );
	}

	if(!get_mesh_memory())
	{
		fprintf(stderr,"Reduce the maximum number of vertices!!!  See SETUP.TXT");
		return FALSE;
	}

	// Matrix init stuff (from remove.c)
	rotmeshtopside = ((float)scrx/scry)*ROTMESHTOPSIDE/(1.33333);
	rotmeshbottomside = ((float)scrx/scry)*ROTMESHBOTTOMSIDE/(1.33333);
	rotmeshup = ((float)scrx/scry)*ROTMESHUP/(1.33333);
	rotmeshdown = ((float)scrx/scry)*ROTMESHDOWN/(1.33333);
	mWorld = IdentityMatrix();
	mViewSave = ViewMatrix( t1,t2,t3,0 );
	mProjection = ProjectionMatrix(.001f, 2000.0f, (float)(FOV*PI/180)); // 60 degree FOV
	mProjection = MatrixMult(Translate(0, 0, -.999996), mProjection); // Fix Z value...
	mProjection = MatrixMult(ScaleXYZ(-1, -1, 100000), mProjection);  // HUK // ...'cause it needs it

	//[claforte] Fudge the values.
	mProjection.v[10] /= 2.0;
	mProjection.v[11] /= 2.0;

	prime_icons();
	prime_titleimage();
	make_textureoffset();  // THIS SHOULD WORK
	make_lightdirectionlookup(); // THIS SHOULD WORK 
	make_turntosin();  // THIS SHOULD WORK
	make_enviro(); // THIS SHOULD WORK
	load_mesh_fans(); // THIS SHOULD WORK
	load_menu_trim(); // THIS SHOULD WORK (PORTED)

	gameactive = TRUE;
	while(gameactive)
	{
		// Give us a nice little menu
		load_menu(); // Working (may be part of following problem)
		fiddle_with_menu(); // Working (network stuff not in) 

		release_menu();
		fprintf(stderr, "released menu\n");

		// Did we get through all the menus?
		if(gameactive)
		{
			int in_pause = 0;
			//printf("MENU: game is now active\n");
			// Start a new module
			srand(seed);
			clear_select();

			fprintf(stderr, "loading_module\n");
			load_module(pickedmodule);  // :TODO: Seems to be the next part to fix
			netmessagemode = FALSE;
			netmessagedelay = 20;
			pressed = FALSE;
			fprintf(stderr, "making onwhichfan\n");
			make_onwhichfan();
			reset_camera();
			reset_timers();
			fprintf(stderr, "figuring what to draw\n");
			figure_out_what_to_draw();
			fprintf(stderr, "making char matrices\n");
			make_character_matrices();
			fprintf(stderr, "attaching particules\n");
			attach_particles();

			// Let the game go
			moduleactive = TRUE;
			randsave = 0;
			srand(0);
			fprintf(stderr, "module is active\n");
			while(moduleactive)
			{
				// This is the control loop
				read_input();
				if (key_set_once(GP2X_VOLM) && mainVolume > 0) {
					mainVolume -= .1f;
				} else if (key_set_once(GP2X_VOLP) && mainVolume < 1) {
					mainVolume += .1f;
				}

				// Do important things
				if(!in_pause)
				{
					set_local_latches();
					update_timers();
					update_game();
					wldclock = allclock;
				}
				else
				{
					update_timers();
					wldclock = allclock;
				}

				// Do the display stuff
				move_camera();
				figure_out_what_to_draw();
				draw_main();

				// Check for quitters
				// :TODO: nolocalplayers is not set correctly
				if(
					(key_set(GP2X_START) && key_set(GP2X_SELECT)) ||
					(
						(alllocalpladead || respawnanytime || beatmodule) &&
						key_set_once(GP2X_START)
					))
				{
					quit_module();
				}
				else if (key_set_once(GP2X_SELECT))
				{
					in_pause = !in_pause;
				}
			}
			release_module();
			close_session();
			nextmenu = MENUH;
		}
	}
	quit_game();
	ShutdownSound();
	release_menu_trim();
	release_grfx();

	return TRUE;
}

//--------------------------------------------------------------------------------------------
