// passage.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
int open_passage(int passage)
{
    // ZZ> This function makes a passage passable
    int x, y;
    unsigned int fan;
    int useful;


    useful = FALSE;
    if(passage < numpassage)
    {
        useful = (passopen[passage]==FALSE);
        passopen[passage] = TRUE;
        y = passtly[passage];
        while(y <= passbry[passage])
        {
            x = passtlx[passage];
            while(x <= passbrx[passage])
            {
                fan = meshfanstart[y]+x;
                meshfx[fan] = meshfx[fan]&(255-MESHFXWALL-MESHFXIMPASS-MESHFXSLIPPY);
                x++;
            }
            y++;
        }
    }
    return useful;
}

//--------------------------------------------------------------------------------------------
int break_passage(int passage, unsigned short starttile, unsigned short frames,
    unsigned short become, unsigned char meshfxor)
{
    // ZZ> This function breaks the tiles of a passage if there is a character standing
    //     on 'em...  Turns the tiles into damage terrain if it reaches last frame.
    int x, y;
    unsigned short tile, endtile;
    unsigned int fan;
    int useful, character;


    endtile = starttile+frames-1;
    useful = FALSE;
    if(passage < numpassage)
    {
        character = 0;
        while(character < MAXCHR)
        {
            if(chron[character] && !chrinpack[character])
            {
                if(chrweight[character] > 20 && chrflyheight[character] == 0 && chrzpos[character] < (chrlevel[character]+20) && chrattachedto[character]==MAXCHR)
                {
                    x = chrxpos[character];  x = x>>7;
                    if(x>=passtlx[passage] && x<=passbrx[passage])
                    {
                        y = chrypos[character];  y = y>>7;
                        if(y>=passtly[passage] && y<=passbry[passage])
                        {
                            // The character is in the passage, so might need to break...
                            fan = meshfanstart[y]+x;
                            tile = meshtile[fan];
                            if(tile >= starttile && tile < endtile)
                            {
                                // Remember where the hit occured...
                                valuetmpx = chrxpos[character];
                                valuetmpy = chrypos[character];
                                useful = TRUE;
                                // Change the tile
                                tile++;
                                if(tile == endtile)
                                {
                                    meshfx[fan]|=meshfxor;
                                    if(become != 0)
                                    {
                                        tile = become;
                                    }
                                }
                                meshtile[fan] = tile;
                            }
                        }
                    }
                }
            }
            character++;
        }
    }
    return useful;
}

//--------------------------------------------------------------------------------------------
void flash_passage(int passage, unsigned char color)
{
    // ZZ> This function makes a passage flash white
    int x, y, cnt, numvert;
    unsigned int fan, vert;


    if(passage < numpassage)
    {
        y = passtly[passage];
        while(y <= passbry[passage])
        {
            x = passtlx[passage];
            while(x <= passbrx[passage])
            {
                fan = meshfanstart[y]+x;
                vert = meshvrtstart[fan];
                cnt = 0;
                numvert = meshcommandnumvertices[meshtype[fan]];
                while(cnt < numvert)
                {
                    meshvrta[vert] = color;
                    vert++;
                    cnt++;
                }
                x++;
            }
            y++;
        }
    }
}

//--------------------------------------------------------------------------------------------
unsigned char find_tile_in_passage(int passage, int tiletype)
{
    // ZZ> This function finds the next tile in the passage, valuetmpx and valuetmpy
    //     must be set first, and are set on a find...  Returns TRUE or FALSE
    //     depending on if it finds one or not
    int x, y;
    unsigned int fan;


    if(passage < numpassage)
    {
        // Do the first row
        x = valuetmpx >> 7;
        y = valuetmpy >> 7;
        if(x < passtlx[passage])  x = passtlx[passage];
        if(y < passtly[passage])  y = passtly[passage];
        if(y < passbry[passage])
        {
            while(x <= passbrx[passage])
            {
                fan = meshfanstart[y]+x;
                if(meshtile[fan] == tiletype)
                {
                    valuetmpx = (x << 7) + 64;
                    valuetmpy = (y << 7) + 64;
                    return TRUE;
                }
                x++;
            }
            y++;
        }


        // Do all remaining rows
        while(y <= passbry[passage])
        {
            x = passtlx[passage];
            while(x <= passbrx[passage])
            {
                fan = meshfanstart[y]+x;
                if(meshtile[fan] == tiletype)
                {
                    valuetmpx = (x << 7) + 64;
                    valuetmpy = (y << 7) + 64;
                    return TRUE;
                }
                x++;
            }
            y++;
        }
    }
    return FALSE;
}

//--------------------------------------------------------------------------------------------
unsigned short who_is_blocking_passage(int passage)
{
    // ZZ> This function returns MAXCHR if there is no character in the passage,
    //     otherwise the index of the first character found is returned...
    //     Finds living ones, then items and corpses
    float tlx, tly, brx, bry;
    unsigned short character, foundother;
    float bumpsize;


    // Passage area
    tlx = (passtlx[passage]<<7)-CLOSETOLERANCE;
    tly = (passtly[passage]<<7)-CLOSETOLERANCE;
    brx = ((passbrx[passage]+1)<<7)+CLOSETOLERANCE;
    bry = ((passbry[passage]+1)<<7)+CLOSETOLERANCE;


    // Look at each character
    foundother = MAXCHR;
    character = 0;
    while(character < MAXCHR)
    {
        if(chron[character])
        {
            bumpsize = chrbumpsize[character];
            if((!chrinpack[character]) && chrattachedto[character]==MAXCHR && bumpsize!=0)
            {
                if(chrxpos[character]>tlx-bumpsize && chrxpos[character]<brx+bumpsize)
                {
                    if(chrypos[character]>tly-bumpsize && chrypos[character]<bry+bumpsize)
                    {
                        if(chralive[character] && chrisitem[character]==FALSE)
                        {
                            // Found a live one
                            return character;
                        }
                        else
                        {
                            // Found something else
                            foundother = character;
                        }
                    }
                }
            }
        }
        character++;
    }



    // No characters found
    return foundother;
}

//--------------------------------------------------------------------------------------------
unsigned short who_is_blocking_passage_ID(int passage, unsigned int idsz)
{
    // ZZ> This function returns MAXCHR if there is no character in the passage who
    //     have an item with the given ID.  Otherwise, the index of the first character
    //     found is returned...  Only finds living characters...
    float tlx, tly, brx, bry;
    unsigned short character, sTmp;
    float bumpsize;


    // Passage area
    tlx = (passtlx[passage]<<7)-CLOSETOLERANCE;
    tly = (passtly[passage]<<7)-CLOSETOLERANCE;
    brx = ((passbrx[passage]+1)<<7)+CLOSETOLERANCE;
    bry = ((passbry[passage]+1)<<7)+CLOSETOLERANCE;


    // Look at each character
    character = 0;
    while(character < MAXCHR)
    {
        if(chron[character])
        {
            bumpsize = chrbumpsize[character];
            if((!chrisitem[character]) && bumpsize!=0 && chrinpack[character]==0)
            {
                if(chrxpos[character]>tlx-bumpsize && chrxpos[character]<brx+bumpsize)
                {
                    if(chrypos[character]>tly-bumpsize && chrypos[character]<bry+bumpsize)
                    {
                        if(chralive[character])
                        {
                            // Found a live one...  Does it have a matching item?

                            // Check the pack
                            sTmp = chrnextinpack[character];
                            while(sTmp != MAXCHR)
                            {
                                if(capidsz[chrmodel[sTmp]][IDSZPARENT]==idsz || capidsz[chrmodel[sTmp]][IDSZTYPE]==idsz)
                                {
                                    // It has the item...
                                    return character;
                                }
                                sTmp = chrnextinpack[sTmp];
                            }
                            // Check left hand
                            sTmp = chrholdingwhich[character][0];
                            if(sTmp != MAXCHR)
                            {
                                sTmp = chrmodel[sTmp];
                                if(capidsz[sTmp][IDSZPARENT]==idsz || capidsz[sTmp][IDSZTYPE]==idsz)
                                {
                                    // It has the item...
                                    return character;
                                }
                            }
                            // Check right hand
                            sTmp = chrholdingwhich[character][1];
                            if(sTmp != MAXCHR)
                            {
                                sTmp = chrmodel[sTmp];
                                if(capidsz[sTmp][IDSZPARENT]==idsz || capidsz[sTmp][IDSZTYPE]==idsz)
                                {
                                    // It has the item...
                                    return character;
                                }
                            }
                        }
                    }
                }
            }
        }
        character++;
    }



    // No characters found
    return MAXCHR;
}

//--------------------------------------------------------------------------------------------
int close_passage(int passage)
{
    // ZZ> This function makes a passage impassable, and returns TRUE if it isn't blocked
    int x, y, cnt;
    float tlx, tly, brx, bry;
    unsigned int fan;
    unsigned short character;
    float bumpsize;
    unsigned short numcrushed;
    unsigned short crushedcharacters[MAXCHR];


    if((passmask[passage]&(MESHFXIMPASS|MESHFXWALL)))
    {
        // Make sure it isn't blocked
        tlx = (passtlx[passage]<<7)-CLOSETOLERANCE;
        tly = (passtly[passage]<<7)-CLOSETOLERANCE;
        brx = ((passbrx[passage]+1)<<7)+CLOSETOLERANCE;
        bry = ((passbry[passage]+1)<<7)+CLOSETOLERANCE;
        numcrushed = 0;
        character = 0;
        while(character < MAXCHR)
        {
            bumpsize = chrbumpsize[character];
            if(chron[character]&&(!chrinpack[character])&&chrattachedto[character]==MAXCHR&&chrbumpsize[character]!=0)
            {
                if(chrxpos[character]>tlx-bumpsize && chrxpos[character]<brx+bumpsize)
                {
                    if(chrypos[character]>tly-bumpsize && chrypos[character]<bry+bumpsize)
                    {
                        if(chrcanbecrushed[character]==FALSE)
                        {
                            return FALSE;
                        }
                        else
                        {
                            crushedcharacters[numcrushed] = character;
                            numcrushed++;
                        }
                    }
                }
            }
            character++;
        }


        // Crush any unfortunate characters
        cnt = 0;
        while(cnt < numcrushed)
        {
            character = crushedcharacters[cnt];
            chralert[character] |= ALERTIFCRUSHED;
            cnt++;
        }
    }

    // Close it off
    if(passage < numpassage)
    {
        passopen[passage] = FALSE;
        y = passtly[passage];
        while(y <= passbry[passage])
        {
            x = passtlx[passage];
            while(x <= passbrx[passage])
            {
                fan = meshfanstart[y]+x;
                meshfx[fan] = meshfx[fan]|passmask[passage];
                x++;
            }
            y++;
        }
    }
    return TRUE;
}

//--------------------------------------------------------------------------------------------
void clear_passages()
{
    // ZZ> This function clears the passage list ( for doors )
    numpassage = 0;
    numshoppassage = 0;
}

//--------------------------------------------------------------------------------------------
void add_shop_passage(int owner, int passage)
{
    // ZZ> This function creates a shop passage
    if(passage < numpassage && numshoppassage < MAXPASS)
    {
        // The passage exists...
        shoppassage[numshoppassage] = passage;
        shopowner[numshoppassage] = owner;  // Assume the owner is alive
        numshoppassage++;
    }
}

//--------------------------------------------------------------------------------------------
void add_passage(int tlx, int tly, int brx, int bry, unsigned char open, unsigned char mask)
{
    // ZZ> This function creates a passage area
    if(tlx < 0)  tlx = 0;
    if(tlx > meshsizex-1)  tlx = meshsizex-1;
    if(tly < 0)  tly = 0;
    if(tly > meshsizey-1)  tly = meshsizey-1;
    if(brx < 0)  brx = 0;
    if(brx > meshsizex-1)  brx = meshsizex-1;
    if(bry < 0)  bry = 0;
    if(bry > meshsizey-1)  bry = meshsizey-1;
    if(numpassage < MAXPASS)
    {
        passtlx[numpassage] = tlx;
        passtly[numpassage] = tly;
        passbrx[numpassage] = brx;
        passbry[numpassage] = bry;
        passmask[numpassage] = mask;
        passtracktype[numpassage] = IGNOTRACK;  // Interactive music
        passtrackcount[numpassage] = 1;         // Interactive music
        numpassage++;
        if(open)
            passopen[numpassage-1]=TRUE;
        else
            passopen[numpassage-1]=FALSE;
    }
}

//--------------------------------------------------------------------------------------------
void setup_passage(char *modname)
{
    // ZZ> This function reads the passage file
    char newloadname[256];
    unsigned char cTmp;
    int tlx, tly, brx, bry;
    unsigned char open, mask;
    FILE *fileread;


    // Reset all of the old passages
    clear_passages();


    // Load the file
    make_newloadname(modname, FILENAME("/gamedat/passage.txt"), newloadname);
    fileread = fopen(newloadname, "r");
    if(fileread)
    {
        while(goto_colon_yesno(fileread))
        {
            fscanf(fileread, "%d%d%d%d", &tlx, &tly, &brx, &bry);
            cTmp = get_first_letter(fileread);
                open = FALSE;
                if(cTmp == 'T' || cTmp == 't') open = TRUE;
            cTmp = get_first_letter(fileread);
                mask = MESHFXIMPASS|MESHFXWALL;
                if(cTmp == 'T' || cTmp == 't') mask = MESHFXIMPASS;
            cTmp = get_first_letter(fileread);
                if(cTmp == 'T' || cTmp == 't') mask = MESHFXSLIPPY;
            add_passage(tlx, tly, brx, bry, open, mask);
        }
        fclose(fileread);
    }
}

