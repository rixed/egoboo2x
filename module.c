// module.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"

//--------------------------------------------------------------------------------------------
void release_module( void )
{
    // ZZ> This function frees up memory used by the module
    release_all_textures();
    release_all_icons();
    release_map();
    reset_sounds();
}

//--------------------------------------------------------------------------------------------
int module_reference_matches(char *szLoadName, unsigned int idsz)
{
    // ZZ> This function returns TRUE if the named module has the required IDSZ
    FILE *fileread;
    static char newloadname[MAX_STR_SIZE];
    unsigned int newidsz;
    int foundidsz;
    int cnt;


    if(szLoadName[0] == 'N' && szLoadName[1] == 'O' && szLoadName[2] == 'N' && szLoadName[3] == 'E' && szLoadName[4] == 0)
        return TRUE;
    if(idsz == IDSZNONE)
        return TRUE;


    foundidsz = FALSE;
    sprintf(newloadname, FILENAME(DATA_PATH"modules/%s/gamedat/menu.txt"), szLoadName);
    fileread = fopen(newloadname, "r"); 
    if(fileread)
    {
        // Read basic data
        globalname = szLoadName;
        goto_colon(fileread);  // Name of module...  Doesn't matter
        goto_colon(fileread);  // Reference directory...
        goto_colon(fileread);  // Reference IDSZ...
        goto_colon(fileread);  // Import...
        goto_colon(fileread);  // Export...
        goto_colon(fileread);  // Min players...
        goto_colon(fileread);  // Max players...
        goto_colon(fileread);  // Respawn...
        goto_colon(fileread);  // RTS...
        goto_colon(fileread);  // Rank...


        // Summary...
        cnt = 0;
        while(cnt < SUMMARYLINES)
        {
            goto_colon(fileread);
            cnt++;
        }


        // Now check expansions
        while(goto_colon_yesno(fileread) && foundidsz == FALSE)
        {
            newidsz = get_idsz(fileread);
            if(newidsz == idsz)
            {
                foundidsz = TRUE;
            }
        }


        fclose(fileread);
    }
    return foundidsz;
}

//--------------------------------------------------------------------------------------------
void add_module_idsz(char *szLoadName, unsigned int idsz)
{
    // ZZ> This function appends an IDSZ to the module's menu.txt file
    FILE *filewrite;
    static char newloadname[MAX_STR_SIZE];
    char chara, charb, charc, chard;

    // Only add if there isn't one already
    if(module_reference_matches(szLoadName, idsz) == FALSE)
    {
        // Try to open the file in append mode
        sprintf(newloadname, FILENAME(DATA_PATH"modules/%s/gamedat/enu.txt"), szLoadName);
        filewrite = fopen(newloadname, "a"); 
        if(filewrite)
        {
            chara = ((idsz>>15)&31)+'A';
            charb = ((idsz>>10)&31)+'A';
            charc = ((idsz>>5)&31)+'A';
            chard = ((idsz)&31)+'A';
            fprintf(filewrite, ":[%c%c%c%c]\n", chara, charb, charc, chard);
            fclose(filewrite);
        }
    }
}

//--------------------------------------------------------------------------------------------
int find_module(char *smallname)
{
    // ZZ> This function returns -1 if the module does not exist locally, the module
    //     index otherwise

    int cnt, index;
    cnt = 0;
    index = -1;
    while(cnt < globalnummodule)
    {
        if(strcmp(smallname, modloadname[cnt]) == 0)
        {
            index = cnt;
            cnt = globalnummodule;
        }
        cnt++;
    }
    return index;
}

//--------------------------------------------------------------------------------------------
void load_module(char *smallname)
{
    // ZZ> This function loads a module
    static char modname[MAX_STR_SIZE];
    static char musicdirectory[MAX_STR_SIZE];
    FILE* musicfile;

    //printf("in load_module\n");

    beatmodule = FALSE;
    timeron = FALSE;
    sprintf(modname, FILENAME(DATA_PATH"modules/%s/"), smallname);
    make_randie();	// This should work
    printf("randie done\n");
    reset_teams();      // This should work
    printf("reset_teams done\n");
    load_one_icon(DATA_PATH"basicdat/nullicon.bmp");  // This works (without transparency)
    printf("load_one_icon done\n");
    reset_particles(modname);  // This should work
    printf("reset_particles done\n");
    read_wawalite(modname);   // This should work
    printf("read_wawa done\n");
    make_twist();             // This should work
    printf("make_twist done\n");
    reset_messages();         // This should work
    printf("reset messages done\n");
    prime_names();            // This should work
    printf("prime_names done\n");
    load_basic_textures(modname);  // This should work (without colorkey stuff)
    printf("load_basic_tex done\n");
    reset_ai_script();        // This should work
    printf("reset_ai_script done\n");
    load_ai_script(DATA_PATH"basicdat/script.txt");  // This should work
    printf("load_ai_script done\n");
    reset_sounds();           // This doesn't do ANYTHING
    printf("reset_sounds done\n");
    release_all_models();     // This should work
    printf("release_all_models done\n");
    free_all_enchants();      // This should work
    printf("free_all_enchants done\n");

    // Load sound files
    sprintf(musicdirectory, "%smusic", modname);
    igloaded = FALSE;
    load_global_waves(modname);  // This is not fully implemented
    printf("load_global_waves done\n");
    load_all_music_sounds(musicdirectory); // This is not fully implemented
    printf("load_all_music_sounds done\n");
    if(instrumentsloaded == FALSE)
    {
        sprintf(musicdirectory, FILENAME("%sgamedat/igmusic.txt"), modname);  // The file to load
        musicfile = fopen(musicdirectory, "r");
        sprintf(musicdirectory, FILENAME(DATA_PATH"basicdat/musica"));  // The default directory
        if(musicfile)
        {
            fscanf(musicfile, "%s", musicdirectory);  // Read in a new directory
            fclose(musicfile);
        }
        load_all_music_sounds(musicdirectory);
        igloaded = instrumentsloaded;
    }

    printf("Got to load_all_objects\n");

// [claforte Jan 6th 2001]
// The following will crash on _BIG_ENDIAN architectures at the moment,
// since the MD2 loading functions (ie: rip_md2_*) all assume that the machine is
// little-endian.

//#ifdef _LITTLE_ENDIAN
    load_all_objects(modname); // This is broken and needs to be fixed
	
		printf("Got to load mesh\n");
    if(!load_mesh(modname))
    {
        general_error(0, 0, "LOAD PROBLEMS");
    }
    printf("Got to setup_particles\n");
    setup_particles();
    printf("Got to setup_passage\n");
    setup_passage(modname);
    printf("Got to reset_players\n");
    
	reset_players();
    printf("Got to setup_characters\n");
    
	setup_characters(modname);
//#endif //_LITTLE_ENDIAN
	
		printf("Got to reset_end_text\n");
    reset_end_text();
    printf("Got to reset_press\n");
    reset_press();
    printf("Got to setup_alliances\n");
    setup_alliances(modname);

    // Load fonts and bars after other images, as not to hog videomem
    printf("Got to load_font\n");
    load_font(DATA_PATH"basicdat/font.bmp", DATA_PATH"basicdat/font.txt", FALSE);
    printf("Got to load_bars\n");
    load_bars(DATA_PATH"basicdat/bars.bmp");
    printf("Got to load_map\n");
    load_map(modname, FALSE);
    printf("Got to log_madused\n");
    // GS - log_madused(FILENAME(DATA_PATH"basicdat/slotused.txt"));


    // Now setup the music
    printf("Got to load_all_music_tracks\n");
    // GS - load_all_music_tracks(smallname);
    // GS - load_all_music_loops(smallname);
    // GS - load_music_sequence(smallname);
    play_next_track();
    if(igloaded)
    {
        // Normal music failed, so default to interactive music
        sprintf(generictext, FILENAME("%s/length.txt"), musicdirectory);
        // GS - load_ig_length(generictext);
        // GS - create_ig_trackmaster();
    }

    // RTS stuff
    clear_orders();


    // Start playing the damage tile sound silently...
/*PORT
    play_sound_pvf_looped(damagetilesound, PANMID, VOLMIN, FRQDEFAULT);
*/
}

//--------------------------------------------------------------------------------------------
int get_module_data(int modnumber, char *szLoadName)
{
    // ZZ> This function loads the module data file
    FILE *fileread;
    char reference[128];
    unsigned int idsz;
    char cTmp;
    int iTmp;

    fileread = fopen( FILENAME(szLoadName), "r"); 
    if(fileread)
    {
        // Read basic data
        globalname = szLoadName;
        goto_colon(fileread);  get_name(fileread, modlongname[modnumber]);
        goto_colon(fileread);  fscanf(fileread, "%s", reference);
        goto_colon(fileread);  idsz = get_idsz(fileread);
        if(module_reference_matches(reference, idsz))
        {
            globalname = szLoadName;
            goto_colon(fileread);  fscanf(fileread, "%d", &iTmp);
                modimportamount[modnumber] = iTmp;
            goto_colon(fileread);  cTmp = get_first_letter(fileread);
                modallowexport[modnumber] = FALSE;
                if(cTmp == 'T' || cTmp == 't')  modallowexport[modnumber] = TRUE;
            goto_colon(fileread);  fscanf(fileread, "%d", &iTmp);  modminplayers[modnumber] = iTmp;
            goto_colon(fileread);  fscanf(fileread, "%d", &iTmp);  modmaxplayers[modnumber] = iTmp;
            goto_colon(fileread);  cTmp = get_first_letter(fileread);
                modrespawnvalid[modnumber] = FALSE;
                if(cTmp == 'T' || cTmp == 't')  modrespawnvalid[modnumber] = TRUE;
                if(cTmp == 'A' || cTmp == 'a')  modrespawnvalid[modnumber] = ANYTIME;
            goto_colon(fileread);  cTmp = get_first_letter(fileread);
                modrtscontrol[modnumber] = FALSE;
                if(cTmp == 'T' || cTmp == 't')  modrtscontrol[modnumber] = TRUE;
                if(cTmp == 'A' || cTmp == 'a')  modrtscontrol[modnumber] = ALLSELECT;
            goto_colon(fileread);  fscanf(fileread, "%s", generictext);
                iTmp = 0;
                while(iTmp < RANKSIZE-1)
                {
                    modrank[modnumber][iTmp] = generictext[iTmp];
                    iTmp++;
                }
                modrank[modnumber][iTmp] = 0;



            // Read the expansions
            return TRUE;
        }
    }
    return FALSE;
}

//--------------------------------------------------------------------------------------------
int get_module_summary(char *szLoadName)
{
    // ZZ> This function gets the quest description out of the module's menu file
    FILE *fileread;
    char cTmp;
    char szLine[160];
    int cnt;
    int tnc;

    fileread = fopen(szLoadName, "r"); 
    if(fileread)
    {
        // Skip over basic data
        globalname = szLoadName;
        goto_colon(fileread);  // Name...
        goto_colon(fileread);  // Reference...
        goto_colon(fileread);  // IDSZ...
        goto_colon(fileread);  // Import...
        goto_colon(fileread);  // Export...
        goto_colon(fileread);  // Min players...
        goto_colon(fileread);  // Max players...
        goto_colon(fileread);  // Respawn...
        goto_colon(fileread);  // RTS control...
        goto_colon(fileread);  // Rank...


        // Read the summary
        cnt = 0;
        while(cnt < SUMMARYLINES)
        {
            goto_colon(fileread);  fscanf(fileread, "%s", szLine);
            tnc = 0;
            cTmp = szLine[tnc];  if(cTmp == '_')  cTmp = ' ';
            while(tnc < SUMMARYSIZE-1 && cTmp != 0)
            {
                modsummary[cnt][tnc] = cTmp;
                tnc++;
                cTmp = szLine[tnc];  if(cTmp == '_')  cTmp = ' ';
            }
            modsummary[cnt][tnc] = 0;
            cnt++;
        }
        return TRUE;
    }
    return FALSE;
}

