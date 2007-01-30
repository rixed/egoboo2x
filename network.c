// network.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"

//--------------------------------------------------------------------------------------------
void close_session()
{
    // ZZ> This function gets the computer out of a network game
/*PORT
    if(networkon)
    {
        lpDirectPlay3A->Close();
    }
*/
}

//--------------------------------------------------------------------------------------------
int add_player(unsigned short character, unsigned short player, unsigned char device)
{
    // ZZ> This function adds a player, returning FALSE if it fails, TRUE otherwise
    int cnt;

    if(plavalid[player] == FALSE)
    {
        chrisplayer[character] = TRUE;
        plaindex[player] = character;
        plavalid[player] = TRUE;
        pladevice[player] = device;
        if(device != INPUTNONE)  nolocalplayers = FALSE;
        plalatchx[player] = 0;
        plalatchy[player] = 0;
        plalatchbutton[player] = 0;
        cnt = 0;
        while(cnt < MAXLAG)
        {
            platimelatchx[player][cnt] = 0;
            platimelatchy[player][cnt] = 0;
            platimelatchbutton[player][cnt] = 0;
            cnt++;
        }
        if(device != INPUTNONE)
        {
            chrislocalplayer[character] = TRUE;
            numlocalpla++;
        }
        numpla++;
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------------------------------------
void clear_messages()
{
    // ZZ> This function empties the message buffer
    int cnt;

    cnt = 0;
    while(cnt < MAXMESSAGE)
    {
        msgtime[cnt] = 0;
        cnt++;
    }
}

//--------------------------------------------------------------------------------------------
void clear_select()
{
    // ZZ> This function clears the RTS select list
    numrtsselect = 0;
}

//--------------------------------------------------------------------------------------------
void add_select(unsigned short character)
{
    // ZZ> This function selects a character
    if(numrtsselect < MAXSELECT)
    {
        rtsselect[numrtsselect] = character;
        numrtsselect++;
    }
}

//--------------------------------------------------------------------------------------------
void check_add(unsigned char key, char bigletter, char littleletter)
{
    // ZZ> This function adds letters to the net message
/*PORT
    if(KEYDOWN(key))
    {
        if(keypress[key]==FALSE)
        {
            keypress[key] = TRUE;
            if(netmessagewrite < MESSAGESIZE-2)
            {
                if(KEYDOWN(DIK_LSHIFT) || KEYDOWN(DIK_RSHIFT))
                {
                    netmessage[netmessagewrite] = bigletter;
                }
                else
                {
                    netmessage[netmessagewrite] = littleletter;
                }
                netmessagewrite++;
                netmessage[netmessagewrite] = '?'; // The flashing input cursor
                netmessage[netmessagewrite+1] = 0;
            }
        }
    }
    else
    {
        keypress[key] = FALSE;
    }
*/
}

//--------------------------------------------------------------------------------------------
void start_building_packet()
{
    // ZZ> This function starts building a network packet
    packethead = 0;
    packetsize = 0;
}

//--------------------------------------------------------------------------------------------
void start_reading_packet()
{
    // ZZ> This function starts reading a network packet
    packethead = 0;
}

//--------------------------------------------------------------------------------------------
void add_packet_uc(unsigned char uc)
{
    // ZZ> This function appends an unsigned char to the packet
    unsigned char* ucp;
    ucp = (unsigned char*) (&packetbuffer[packethead]);
    *ucp = uc;
    packethead+=1;
    packetsize+=1;
}

//--------------------------------------------------------------------------------------------
void add_packet_sc(signed char sc)
{
    // ZZ> This function appends a signed char to the packet
    signed char* scp;
    scp = (signed char*) (&packetbuffer[packethead]);
    *scp = sc;
    packethead+=1;
    packetsize+=1;
}

//--------------------------------------------------------------------------------------------
void add_packet_us(unsigned short us)
{
    // ZZ> This function appends an unsigned short to the packet
    unsigned short* usp;
    usp = (unsigned short*) (&packetbuffer[packethead]);
    *usp = us;
    packethead+=2;
    packetsize+=2;
}

//--------------------------------------------------------------------------------------------
void add_packet_ss(signed short ss)
{
    // ZZ> This function appends a signed short to the packet
    signed short* ssp;
    ssp = (signed short*) (&packetbuffer[packethead]);
    *ssp = ss;
    packethead+=2;
    packetsize+=2;
}

//--------------------------------------------------------------------------------------------
void add_packet_ui(unsigned int ui)
{
    // ZZ> This function appends an unsigned int to the packet
    unsigned int* uip;
    uip = (unsigned int*) (&packetbuffer[packethead]);
    *uip = ui;
    packethead+=4;
    packetsize+=4;
}

//--------------------------------------------------------------------------------------------
void add_packet_si(signed int si)
{
    // ZZ> This function appends a signed int to the packet
    signed int* sip;
    sip = (signed int*) (&packetbuffer[packethead]);
    *sip = si;
    packethead+=4;
    packetsize+=4;
}

//--------------------------------------------------------------------------------------------
void add_packet_sz(char *string)
{
    // ZZ> This function appends a null terminated string to the packet
    char* cp;
    char cTmp;
    int cnt;

    cnt = 0;
    cTmp = 1;
    cp = (char*) (&packetbuffer[packethead]);
    while(cTmp != 0)
    {
        cTmp = string[cnt];
        *cp = cTmp;
        cp+=1;
        packethead+=1;
        packetsize+=1;
        cnt++;
    }
}

//--------------------------------------------------------------------------------------------
void read_packet_sz(char *text)
{
    // ZZ> This function reads a null terminated string from the packet
    unsigned char uc;
    unsigned short outindex;


    outindex = 0;
    uc = packetbuffer[packethead];
    packethead++;
    while(uc != 0 && outindex < 255)
    {
        text[outindex] = uc;
        outindex++;
        uc = packetbuffer[packethead];
        packethead++;
    }
    text[outindex] = 0;
}

//--------------------------------------------------------------------------------------------
unsigned char read_packet_uc()
{
    // ZZ> This function reads an unsigned char from the packet
    unsigned char uc;
    unsigned char* ucp;
    ucp = (unsigned char*) (&packetbuffer[packethead]);
    uc = *ucp;
    packethead+=1;
    return uc;
}

//--------------------------------------------------------------------------------------------
signed char read_packet_sc()
{
    // ZZ> This function reads a signed char from the packet
    signed char sc;
    signed char* scp;
    scp = (signed char*) (&packetbuffer[packethead]);
    sc = *scp;
    packethead+=1;
    return sc;
}

//--------------------------------------------------------------------------------------------
unsigned short read_packet_us()
{
    // ZZ> This function reads an unsigned short from the packet
    unsigned short us;
    unsigned short* usp;
    usp = (unsigned short*) (&packetbuffer[packethead]);
    us = *usp;
    packethead+=2;
    return us;
}

//--------------------------------------------------------------------------------------------
signed short read_packet_ss()
{
    // ZZ> This function reads a signed short from the packet
    signed short ss;
    signed short* ssp;
    ssp = (signed short*) (&packetbuffer[packethead]);
    ss = *ssp;
    packethead+=2;
    return ss;
}

//--------------------------------------------------------------------------------------------
unsigned int read_packet_ui()
{
    // ZZ> This function reads an unsigned int from the packet
    unsigned int ui;
    unsigned int* uip;
    uip = (unsigned int*) (&packetbuffer[packethead]);
    ui = *uip;
    packethead+=4;
    return ui;
}

//--------------------------------------------------------------------------------------------
signed int read_packet_si()
{
    // ZZ> This function reads a signed int from the packet
    signed int si;
    signed int* sip;
    sip = (signed int*) (&packetbuffer[packethead]);
    si = *sip;
    packethead+=4;
    return si;
}

//--------------------------------------------------------------------------------------------
int still_reading_packet()
{
    // ZZ> This function tells if there's still data left in the packet
    return (packethead < packetsize);
}

//--------------------------------------------------------------------------------------------
void send_packet_to_host()
{
/*PORT
    // ZZ> This function sends a packet to the host
    lpDirectPlay3A->Send(selfid, DPID_SERVERPLAYER, 0, packetbuffer, packetsize);
*/
}

//--------------------------------------------------------------------------------------------
void send_packet_to_all_players()
{
    // ZZ> This function sends a packet to all the players
/*PORT
    lpDirectPlay3A->Send(selfid, DPID_ALLPLAYERS, 0, packetbuffer, packetsize);
*/
}

//--------------------------------------------------------------------------------------------
void send_packet_to_host_guaranteed()
{
    // ZZ> This function sends a packet to the host
/*PORT
    lpDirectPlay3A->Send(selfid, DPID_SERVERPLAYER, DPSEND_GUARANTEED, packetbuffer, packetsize);
*/
}

//--------------------------------------------------------------------------------------------
void send_packet_to_all_players_guaranteed()
{
    // ZZ> This function sends a packet to all the players
/*PORT
    lpDirectPlay3A->Send(selfid, DPID_ALLPLAYERS, DPSEND_GUARANTEED, packetbuffer, packetsize);
*/
}

//--------------------------------------------------------------------------------------------
void send_packet_to_one_player_guaranteed(int player)
{
    // ZZ> This function sends a packet to one of the players
/*PORT
    if(player < numplayer)
    {
        lpDirectPlay3A->Send(selfid, netplayerid[player], DPSEND_GUARANTEED, packetbuffer, packetsize);
    }
*/
}

//--------------------------------------------------------------------------------------------
void copy_file_to_all_players(char *source, char *dest)
{
    // ZZ> This function copies a file on the host to every remote computer.
    //     Packets are sent in chunks of COPYSIZE bytes.  The max file size
    //     that can be sent is 2 Megs ( TOTALSIZE ).
/*PORT
    FILE* fileread;
    int packetsize, packetstart;
    int filesize;
    int fileisdir;
    char cTmp;


    if(networkon && hostactive)
    {
        fileisdir = GetFileAttributes(source);
        fileisdir = ((fileisdir&FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY);
        if(fileisdir)
        {
            start_building_packet();
            add_packet_us(TO_REMOTE_DIR);
            add_packet_sz(dest);
            send_packet_to_all_players_guaranteed();
        }
        else
        {
            fileread = fopen(source, "rb");
            if(fileread)
            {
                fseek(fileread, 0, SEEK_END);
                filesize = ftell(fileread);
                fseek(fileread, 0, SEEK_SET);
                if(filesize > 0 && filesize < TOTALSIZE)
                {
                    packetsize = 0;
                    packetstart = 0;
                    start_building_packet();
                    numfilesent++;
                    add_packet_us(TO_REMOTE_FILE);
                    add_packet_sz(dest);
                    add_packet_ui(filesize);
                    add_packet_ui(packetstart);
                    while(packetstart < filesize)
                    {
                        fscanf(fileread, "%c", &cTmp);
                        add_packet_uc(cTmp);
                        packetsize++;
                        packetstart++;
                        if(packetsize >= COPYSIZE)
                        {
                            // Send off the packet
                            send_packet_to_all_players_guaranteed();


                            // Start on the next 4K
                            packetsize = 0;
                            start_building_packet();
                            add_packet_us(TO_REMOTE_FILE);
                            add_packet_sz(dest);
                            add_packet_ui(filesize);
                            add_packet_ui(packetstart);
                        }
                    }
                    // Send off the packet
                    send_packet_to_all_players_guaranteed();
                }
                fclose(fileread);
            }
        }
    }
*/
}

//------------------------------------------------------------------------------
void copy_file_to_host(char *source, char *dest)
{
    // ZZ> This function copies a file on the remote to the host computer.
    //     Packets are sent in chunks of COPYSIZE bytes.  The max file size
    //     that can be sent is 2 Megs ( TOTALSIZE ).
/*PORT
    FILE* fileread;
    int packetsize, packetstart;
    int filesize;
    int fileisdir;
    char cTmp;


    if(hostactive)
    {
        // Simulate a network transfer
        fileisdir = GetFileAttributes(source);
        fileisdir = ((fileisdir&FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY);
        if(fileisdir)
        {
            make_directory(dest);
        }
        else
        {
            copy_file(source, dest);
        }
    }
    else
    {
        fileisdir = GetFileAttributes(source);
        fileisdir = ((fileisdir&FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY);
        if(fileisdir)
        {
            start_building_packet();
            add_packet_us(TO_HOST_DIR);
            add_packet_sz(dest);
            send_packet_to_host_guaranteed();
        }
        else
        {
            fileread = fopen(source, "rb");
            if(fileread)
            {
                fseek(fileread, 0, SEEK_END);
                filesize = ftell(fileread);
                fseek(fileread, 0, SEEK_SET);
                if(filesize > 0 && filesize < TOTALSIZE)
                {
                    numfilesent++;
                    packetsize = 0;
                    packetstart = 0;
                    start_building_packet();
                    add_packet_us(TO_HOST_FILE);
                    add_packet_sz(dest);
                    add_packet_ui(filesize);
                    add_packet_ui(packetstart);
                    while(packetstart < filesize)
                    {
                        fscanf(fileread, "%c", &cTmp);
                        add_packet_uc(cTmp);
                        packetsize++;
                        packetstart++;
                        if(packetsize >= COPYSIZE)
                        {
                            // Send off the packet
                            send_packet_to_host_guaranteed();


                            // Start on the next 4K
                            packetsize = 0;
                            start_building_packet();
                            add_packet_us(TO_HOST_FILE);
                            add_packet_sz(dest);
                            add_packet_ui(filesize);
                            add_packet_ui(packetstart);
                        }
                    }
                    // Send off the packet
                    send_packet_to_host_guaranteed();
                }
                fclose(fileread);
            }
        }
    }
*/
}

//--------------------------------------------------------------------------------------------
void copy_directory_to_host(char *dirname, char *todirname)
{
    // ZZ> This function copies all files in a directory
/*PORT
    char searchname[128];
    char fromname[128];
    char toname[128];
    WIN32_FIND_DATA wfdData;
    HANDLE hFind;
    BOOL keeplooking;


    // Search for all files
    sprintf(searchname, "%s\\*.*", dirname);
    hFind = FindFirstFile(searchname, &wfdData);
    keeplooking = 1;
    if(hFind != INVALID_HANDLE_VALUE)
    {
        // Make the new directory
        copy_file_to_host(dirname, todirname);
        // Copy each file
        while(keeplooking)
        {
            sprintf(fromname, "%s\\%s", dirname, wfdData.cFileName);
            sprintf(toname, "%s\\%s", todirname, wfdData.cFileName);
            copy_file_to_host(fromname, toname);
            keeplooking = FindNextFile(hFind, &wfdData);
        }
    }
*/
}

//--------------------------------------------------------------------------------------------
void copy_directory_to_all_players(char *dirname, char *todirname)
{
    // ZZ> This function copies all files in a directory
/*PORT
    char searchname[128];
    char fromname[128];
    char toname[128];
    WIN32_FIND_DATA wfdData;
    HANDLE hFind;
    BOOL keeplooking;


    // Search for all files
    sprintf(searchname, "%s\\*.*", dirname);
    hFind = FindFirstFile(searchname, &wfdData);
    keeplooking = 1;
    if(hFind != INVALID_HANDLE_VALUE)
    {
        // Make the new directory
        copy_file_to_all_players(dirname, todirname);
        // Copy each file
        while(keeplooking)
        {
            sprintf(fromname, "%s\\%s", dirname, wfdData.cFileName);
            sprintf(toname, "%s\\%s", todirname, wfdData.cFileName);
            copy_file_to_all_players(fromname, toname);
            keeplooking = FindNextFile(hFind, &wfdData);
        }
    }
*/
}

//--------------------------------------------------------------------------------------------
void talk_to_host()
{
    // ZZ> This function sends the latch packets to the host machine
/*PORT    int player;

    // Let the players respawn
    if(KEYDOWN(DIK_SPACE)
       && ( alllocalpladead || respawnanytime )
       && respawnvalid
       && rtscontrol == FALSE
       && netmessagemode == FALSE)
    {
        player = 0;
        while(player < MAXPLAYER)
        {
            if(plavalid[player] && pladevice[player] != INPUTNONE)
            {
                plalatchbutton[player]|=LATCHBUTTONRESPAWN;  // Press the respawn button...
            }
            player++;
        }
    }

  // Start talkin'
    if(networkon && hostactive==FALSE && rtscontrol == FALSE)
    {
        start_building_packet();
        add_packet_us(TO_HOST_LATCH);                           // The message header
        player = 0;
        while(player < MAXPLAYER)
        {
            // Find the local players
            if(plavalid[player] && pladevice[player] != INPUTNONE)
            {
                add_packet_uc(player);                          // The player index
                add_packet_uc(plalatchbutton[player]);          // Player button states
                add_packet_ss(plalatchx[player]*SHORTLATCH);    // Player motion
                add_packet_ss(plalatchy[player]*SHORTLATCH);    // Player motion
            }
            player++;
        }

        // Send it to the host        
        send_packet_to_host();
    }
*/
}


//--------------------------------------------------------------------------------------------
void talk_to_remotes()
{
    // ZZ> This function sends the character data to all the remote machines
    int player, time;
    signed short sTmp;

    if(wldframe > STARTTALK)
    {
        if(hostactive && rtscontrol == FALSE)
        {
            time = wldframe+lag;
            
            time = wldframe+1;

            // Now pretend the host got the packet...
            time = time&LAGAND;
            player = 0;
            while(player < MAXPLAYER)
            {
                if(plavalid[player])
                {
                    platimelatchbutton[player][time] = plalatchbutton[player];
                    sTmp = plalatchx[player]*SHORTLATCH;
                    platimelatchx[player][time] = sTmp/SHORTLATCH;
                    sTmp = plalatchy[player]*SHORTLATCH;
                    platimelatchy[player][time] = sTmp/SHORTLATCH;
                }
                player++;
            }
            numplatimes++;
        }
    }
}

//--------------------------------------------------------------------------------------------
void listen_for_packets()
{
/*PORT
    // ZZ> This function reads any new messages and sets the player latch and matrix needed
    //     lists...
    DWORD readnumber;
    DPID  sender, holder;
    HRESULT hr;
    FILE* filewrite;
    int filesize;
    int newfilesize;
    int fileposition;
    char filename[256];
    unsigned short header, player;
    unsigned char  who;
    unsigned int stamp;
    unsigned short whichorder;
    unsigned int what, when;
    int cnt, time;


    if(networkon)
    {
        // Listen for new messages
        hr = lpDirectPlay3A->GetMessageCount(selfid, &readnumber);
        if(hr == DP_OK)
        {
            while(readnumber > 0)
            {
                packetsize = MAXSENDSIZE;
                hr = lpDirectPlay3A->Receive(&sender, &holder, DPRECEIVE_ALL, packetbuffer, &packetsize);
                if(hr == DP_OK)
                {
                    // Read the message
                    start_reading_packet();
                    header = read_packet_us();
                    if(header == TO_ANY_TEXT)
                    {
                        debug_message((char *) (&packetbuffer[packethead]));
                    }
                    if(header == TO_HOST_MODULEOK && hostactive)
                    {
                        playersready++;
                        if(playersready >= numplayer)
                        {
                            readytostart = TRUE;
                        }
                    }
                    if(header == TO_HOST_LATCH && hostactive)
                    {
                        // Read latches for each player sent
                        while(still_reading_packet())
                        {
                            player = read_packet_uc();
                            plalatchbutton[player] = read_packet_uc();
                            plalatchx[player] = read_packet_ss()/SHORTLATCH;
                            plalatchy[player] = read_packet_ss()/SHORTLATCH;
                        }
                    }
                    if(header == TO_HOST_IM_LOADED && hostactive)
                    {
                        playersloaded++;
                        if(playersloaded >= numplayer)
                        {
                            // Let the games begin...
                            waitingforplayers = FALSE;
                            start_building_packet();
                            add_packet_us(TO_REMOTE_START);
                            send_packet_to_all_players_guaranteed();
                        }
                    }
                    if(header == TO_HOST_RTS && hostactive)
                    {
                        whichorder = get_empty_order();
                        if(whichorder < MAXORDER)
                        {
                            // Add the order on the host machine
                            cnt = 0;
                            while(cnt < MAXSELECT)
                            {
                                who = read_packet_uc();
                                orderwho[whichorder][cnt] = who;
                                cnt++;
                            }
                            what = read_packet_ui();
                            when = wldframe + orderlag;
                            orderwhat[whichorder] = what;
                            orderwhen[whichorder] = when;


                            // Send the order off to everyone else
                            start_building_packet();
                            add_packet_us(TO_REMOTE_RTS);
                            cnt = 0;
                            while(cnt < MAXSELECT)
                            {
                                add_packet_uc(orderwho[whichorder][cnt]);
                                cnt++;
                            }
                            add_packet_ui(what);
                            add_packet_ui(when);
                            send_packet_to_all_players_guaranteed();
                        }
                    }
                    if(header == TO_HOST_FILE)
                    {
                        read_packet_sz(filename);
                        newfilesize = read_packet_ui();


                        // Change the size of the file if need be
                        filewrite = fopen(filename, "rb");
                        if(filewrite)
                        {
                            fseek(filewrite, 0, SEEK_END);
                            filesize = ftell(filewrite);
                            fseek(filewrite, 0, SEEK_SET);
                            fclose(filewrite);
                            if(filesize != newfilesize)
                            {
                                // Destroy the old file
                                numfile++;
                                filewrite = fopen(filename, "wb");
                                if(filewrite)
                                {
                                    filesize = 0;
                                    while(filesize < newfilesize)
                                    {
                                        fprintf(filewrite, "%c", 0);
                                        filesize++;
                                    }
                                    fclose(filewrite);
                                }
                            }
                        }
                        else
                        {
                            // File must be created
                            numfile++;
                            filewrite = fopen(filename, "wb");
                            if(filewrite)
                            {
                                filesize = 0;
                                while(filesize < newfilesize)
                                {
                                    fprintf(filewrite, "%c", 0);
                                    filesize++;
                                }
                                fclose(filewrite);
                            }
                        }




                        // Go to the position in the file and copy data
                        fileposition = read_packet_ui();
                        filewrite = fopen(filename, "r+b");
                        if(filewrite)
                        {
                            if(fseek(filewrite, fileposition, SEEK_SET) == 0)
                            {
                                while(still_reading_packet())
                                {
                                    who = read_packet_uc();
                                    fprintf(filewrite, "%c", who);
                                }
                            }
                            fclose(filewrite);
                        }
                    }
                    if(header == TO_HOST_DIR && hostactive)
                    {
                        read_packet_sz(filename);
                        make_directory(filename);
                    }
                    if(header == TO_HOST_FILESENT && hostactive)
                    {
                        cnt = read_packet_ui();
                        numfileexpected += cnt;
                        numplayerrespond++;
                    }
                    if(header == TO_REMOTE_FILESENT && hostactive==FALSE)
                    {
                        cnt = read_packet_ui();
                        numfileexpected += cnt;
                        numplayerrespond++;
                    }
                    if(header == TO_REMOTE_MODULE && hostactive==FALSE && readytostart==FALSE)
                    {
                        seed = read_packet_ui();
                        rtslocalteam = read_packet_uc();
                        localmachine = rtslocalteam;
                        sprintf(pickedmodule, "%s", (char *) (&packetbuffer[packethead]));


                        // Check to see if the module exists
                        pickedindex = find_module(pickedmodule);
                        if(pickedindex == -1)
                        {
                            // The module doesn't exist locally
                            // !!!BAD!!!  Copy the data from the host
                            pickedindex = 0;
                        }

                        // Make ourselves ready
                        readytostart = TRUE;

                        // Tell the host we're ready
                        start_building_packet();
                        add_packet_us(TO_HOST_MODULEOK);
                        send_packet_to_host_guaranteed();
                    }
                    if(header == TO_REMOTE_START && hostactive==FALSE)
                    {
                        waitingforplayers = FALSE;
                    }
                    if(header == TO_REMOTE_RTS && hostactive==FALSE)
                    {
                        whichorder = get_empty_order();
                        if(whichorder < MAXORDER)
                        {
                            // Add the order on the remote machine
                            cnt = 0;
                            while(cnt < MAXSELECT)
                            {
                                who = read_packet_uc();
                                orderwho[whichorder][cnt] = who;
                                cnt++;
                            }
                            what = read_packet_ui();
                            when = read_packet_ui();
                            orderwhat[whichorder] = what;
                            orderwhen[whichorder] = when;
                        }
                    }
                    if(header == TO_REMOTE_FILE && hostactive==FALSE)
                    {
                        read_packet_sz(filename);
                        newfilesize = read_packet_ui();


                        // Change the size of the file if need be
                        filewrite = fopen(filename, "rb");
                        if(filewrite)
                        {
                            fseek(filewrite, 0, SEEK_END);
                            filesize = ftell(filewrite);
                            fseek(filewrite, 0, SEEK_SET);
                            fclose(filewrite);
                            if(filesize != newfilesize)
                            {
                                // Destroy the old file
                                numfile++;
                                filewrite = fopen(filename, "wb");
                                if(filewrite)
                                {
                                    filesize = 0;
                                    while(filesize < newfilesize)
                                    {
                                        fprintf(filewrite, "%c", 0);
                                        filesize++;
                                    }
                                    fclose(filewrite);
                                }
                            }
                        }
                        else
                        {
                            // File must be created
                            numfile++;
                            filewrite = fopen(filename, "wb");
                            if(filewrite)
                            {
                                filesize = 0;
                                while(filesize < newfilesize)
                                {
                                    fprintf(filewrite, "%c", 0);
                                    filesize++;
                                }
                                fclose(filewrite);
                            }
                        }




                        // Go to the position in the file and copy data
                        fileposition = read_packet_ui();
                        filewrite = fopen(filename, "r+b");
                        if(filewrite)
                        {
                            if(fseek(filewrite, fileposition, SEEK_SET) == 0)
                            {
                                while(still_reading_packet())
                                {
                                    who = read_packet_uc();
                                    fprintf(filewrite, "%c", who);
                                }
                            }
                            fclose(filewrite);
                        }
                    }
                    if(header == TO_REMOTE_DIR && hostactive==FALSE)
                    {
                        read_packet_sz(filename);
                        make_directory(filename);
                    }
                    if(header == TO_REMOTE_LATCH && hostactive==FALSE)
                    {
                        stamp = read_packet_ui();
                        time = stamp&LAGAND;
                        if(nexttimestamp == -1)
                        {
                            nexttimestamp = stamp;
                        }
                        if(stamp < nexttimestamp)
                        {
                            debug_message("OUT OF ORDER PACKET");
                            outofsync = TRUE;
                        }
                        if(stamp <= wldframe)
                        {
                            debug_message("LATE PACKET");
                            outofsync = TRUE;
                        }
                        if(stamp > nexttimestamp)
                        {
                            debug_message("MISSED PACKET");
                            nexttimestamp = stamp;  // Still use it
                            outofsync = TRUE;
                        }
                        if(stamp == nexttimestamp)
                        {
                            // Remember that we got it
                            numplatimes++;


                            // Read latches for each player sent
                            while(still_reading_packet())
                            {
                                player = read_packet_uc();
                                platimelatchbutton[player][time] = read_packet_uc();
                                platimelatchx[player][time] = read_packet_ss()/SHORTLATCH;
                                platimelatchy[player][time] = read_packet_ss()/SHORTLATCH;
                            }
                            nexttimestamp = stamp+1;
                        }
                    }
                }
                readnumber--;
            }
        }
    }
*/
}

//--------------------------------------------------------------------------------------------
void unbuffer_player_latches()
{
    // ZZ> This function sets character latches based on player input to the host
    int cnt, time, character;


    // Copy the latches
    time = wldframe&LAGAND;
    cnt = 0;
    while(cnt < MAXPLAYER)
    {
        if(plavalid[cnt] && !rtscontrol)
        {
            character = plaindex[cnt];

			chrlatchx[character] = platimelatchx[cnt][time];
			chrlatchy[character] = platimelatchy[cnt][time];
            chrlatchbutton[character] = platimelatchbutton[cnt][time];
            
			// Let players respawn
            if((chrlatchbutton[character] & LATCHBUTTONRESPAWN) && respawnvalid)
            {
                if(chralive[character] == FALSE)
                {
                    respawn_character(character);
                    teamleader[chrteam[character]] = character;
                    chralert[character] |= ALERTIFCLEANEDUP;
                    // Cost some experience for doing this...  Never lose a level
                    chrexperience[character] = chrexperience[character] * EXPKEEP;
                }
                chrlatchbutton[character] &= 127;
            }
        }
        cnt++;
    }
    numplatimes--;
}

//--------------------------------------------------------------------------------------------
void chug_orders()
{
    // ZZ> This function takes care of lag in orders, issuing at the proper wldframe
    int cnt, character, tnc;

    cnt = 0;
    while(cnt < MAXORDER)
    {
        if(ordervalid[cnt] && orderwhen[cnt] <= wldframe)
        {
            if(orderwhen[cnt] < wldframe)
            {
                debug_message("MISSED AN ORDER");
            }
            tnc = 0;
            while(tnc < MAXSELECT)
            {
                character = orderwho[cnt][tnc];
                if(character < MAXCHR)
                {
                    chrorder[character] = orderwhat[cnt];
                    chrcounter[character] = tnc;
                    chralert[character]|=ALERTIFORDERED;
                }
                tnc++;
            }
            ordervalid[cnt] = FALSE;
        }
        cnt++;
    }
}

//--------------------------------------------------------------------------------------------
void send_rts_order(int x, int y, unsigned char order, unsigned char target)
{
    // ZZ> This function asks the host to order the selected characters
    unsigned int what, when, whichorder, cnt;

    if(numrtsselect > 0)
    {
        x = (x >> 6) & 1023;
        y = (y >> 6) & 1023;
        what = (target << 24) | (x << 14) | (y << 4) | (order&15); 
        if(hostactive)
        {
            when = wldframe + orderlag;
            whichorder = get_empty_order();
            if(whichorder != MAXORDER)
            {
                // Add a new order on own machine
                orderwhen[whichorder] = when;
                orderwhat[whichorder] = what;
                cnt = 0;
                while(cnt < numrtsselect)
                {
                    orderwho[whichorder][cnt] = rtsselect[cnt];
                    cnt++;
                }
                while(cnt < MAXSELECT)
                {
                    orderwho[whichorder][cnt] = MAXCHR;
                    cnt++;
                }


                // Send the order off to everyone else
                if(networkon)
                {
                    start_building_packet();
                    add_packet_us(TO_REMOTE_RTS);
                    cnt = 0;
                    while(cnt < MAXSELECT)
                    {
                        add_packet_uc(orderwho[whichorder][cnt]);
                        cnt++;
                    }
                    add_packet_ui(what);
                    add_packet_ui(when);
                    send_packet_to_all_players_guaranteed();
                }
            }
        }
        else
        {
            // Send the order off to the host
            start_building_packet();
            add_packet_us(TO_HOST_RTS);
            cnt = 0;
            while(cnt < numrtsselect)
            {
                add_packet_uc(rtsselect[cnt]);
                cnt++;
            }
            while(cnt < MAXSELECT)
            {
                add_packet_uc(MAXCHR);
                cnt++;
            }
            add_packet_ui(what);
            send_packet_to_host_guaranteed();
        }
    }
}

//--------------------------------------------------------------------------------------------
void setup_network()
{
    // ZZ> This starts up the network and logs whatever goes on
/*PORT
    HRESULT             hr;


    serviceon = FALSE;
    numsession = 0;
    numservice = 0;
    if(globalnetworkerr)  fprintf(globalnetworkerr, "This file helps debug networking...\n");
    if(networkon)
    {
        // Create the direct play interface
        hr = CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
                               IID_IDirectPlay3A, (LPVOID*)&lpDirectPlay3A);
        serviceon = TRUE;
        if(hr != DP_OK) { networkon = 0; serviceon = 0; }
        if(globalnetworkerr)
        {
            if(networkon)  fprintf(globalnetworkerr, "  Interface OK\n");
            else           fprintf(globalnetworkerr, "  ERROR:  Interface broken\n");
        }
    }
    else
    {
        // Network wasn't turned on
        if(globalnetworkerr)  fprintf(globalnetworkerr, "  ERROR:  Network not turned on\n");
    }
    if(networkon)
    {
        // Get the different service providers
        if(globalnetworkerr)  fprintf(globalnetworkerr, "  Looking for services...\n");
        lpDirectPlay3A->EnumConnections(NULL, ConnectionsCallback, hGlobalWindow, 0);
        if(numservice == 0 && globalnetworkerr)  fprintf(globalnetworkerr, "    ERROR:  None found...\n");
    }
*/
}

//--------------------------------------------------------------------------------------------
void find_open_sessions()
{
/*PORT
    // ZZ> This function finds some open games to join
    DPSESSIONDESC2      sessionDesc;
    HRESULT             hr;

    if(networkon)
    {
        numsession = 0;
        if(globalnetworkerr)  fprintf(globalnetworkerr, "  Looking for open games...\n");
        ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
        sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
        sessionDesc.guidApplication = NETWORKID;
        hr = lpDirectPlay3A->EnumSessions(&sessionDesc, 0, SessionsCallback, hGlobalWindow, DPENUMSESSIONS_AVAILABLE);
        if(globalnetworkerr)  fprintf(globalnetworkerr, "    %d sessions found\n", numsession);
    }
*/
}

//--------------------------------------------------------------------------------------------
void find_all_players()
{
    // ZZ> This function finds all the players in the game
/*PORT
    HRESULT             hr;

    if(networkon)
    {
        numplayer = 0;
//        if(globalnetworkerr)  fprintf(globalnetworkerr, "  Looking for players...\n");
        hr = lpDirectPlay3A->EnumPlayers(NULL, PlayersCallback, hGlobalWindow, DPENUMPLAYERS_ALL);
//        if(globalnetworkerr)  fprintf(globalnetworkerr, "    %d players found\n", numplayer);
    }
*/
}

//--------------------------------------------------------------------------------------------
int create_player(int host)
{
    // ZZ> This function tries creating a player
/*PORT
    DPNAME              dpName;
    HRESULT             hr;

    if(networkon)
    {
        ZeroMemory(&dpName, sizeof(DPNAME));
        dpName.dwSize = sizeof(DPNAME);
        dpName.lpszShortNameA = nethostname;
        dpName.lpszLongNameA = NULL;
        if(host)
        {
//            hr = lpDirectPlay3A->CreatePlayer(&selfid, &dpName, lpDPInfo->hPlayerEvent, NULL, 0, DPPLAYER_SERVERPLAYER);
            hr = lpDirectPlay3A->CreatePlayer(&selfid, &dpName, NULL, NULL, 0, DPPLAYER_SERVERPLAYER);
        }
        else
        {
//            hr = lpDirectPlay3A->CreatePlayer(&selfid, &dpName, lpDPInfo->hPlayerEvent, NULL, 0, 0);
            hr = lpDirectPlay3A->CreatePlayer(&selfid, &dpName, NULL, NULL, 0, 0);
        }
        if(hr == DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  Player created\n");
        if(hr != DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  ERROR:  Player not created\n");
        if(hr == DP_OK)  return TRUE;
    }
    return FALSE;
*/
return FALSE;
}

//--------------------------------------------------------------------------------------------
int join_session(int session)
{
    // ZZ> This function tries to join one of the sessions we found
/*PORT
    DPSESSIONDESC2      sessionDesc;
    HRESULT             hr;

    if(networkon)
    {
        if(session < numsession)
        {
            // Try joining the game
            ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
            sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
            sessionDesc.guidInstance = *netlpsessionguid[session];

            hr = lpDirectPlay3A->Open(&sessionDesc, DPOPEN_JOIN);
            if(hr == DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  Joined session %d\n", session);
            if(hr != DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  ERROR:  Could not join session %d\n", session);
            if(hr != DP_OK)  return FALSE;


            // Try creating a player
            return create_player(FALSE);
        }
        else
        {
            if(globalnetworkerr)  fprintf(globalnetworkerr, "  ERROR:  Session %d not available\n", session);
        }
    }
    return FALSE;
*/
return FALSE;
}

//--------------------------------------------------------------------------------------------
void stop_players_from_joining()
{
    // ZZ> This function stops players from joining a game
/*PORT
    DPSESSIONDESC2      sessionDesc;
    HRESULT             hr;

    if(hostactive)
    {
        ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
        sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
        sessionDesc.dwFlags = DPSESSION_JOINDISABLED | DPSESSION_NOMESSAGEID | DPSESSION_NODATAMESSAGES | DPSESSION_CLIENTSERVER;
        sessionDesc.guidApplication = NETWORKID;
        sessionDesc.dwMaxPlayers = MAXNETPLAYER;
        sessionDesc.dwCurrentPlayers = numplayer;
        sessionDesc.lpszSessionNameA = nethostname;
        hr = lpDirectPlay3A->SetSessionDesc(&sessionDesc, 0);
        if(hr == DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  Stopped players from joining\n");
        if(hr != DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  ERROR:  Players can still join\n");
    }
*/
}

//--------------------------------------------------------------------------------------------
int host_session()
{
    // ZZ> This function tries to host a new session
/*PORT
    DPSESSIONDESC2      sessionDesc;
    HRESULT             hr;

    if(networkon)
    {
        // Try to create a new session
        ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
        sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
        sessionDesc.dwFlags = DPSESSION_NOMESSAGEID | DPSESSION_NODATAMESSAGES | DPSESSION_CLIENTSERVER;
        sessionDesc.guidApplication = NETWORKID;
        sessionDesc.dwMaxPlayers = MAXNETPLAYER;
        sessionDesc.dwCurrentPlayers = 1;
        sessionDesc.lpszSessionNameA = nethostname;
        hr = lpDirectPlay3A->Open(&sessionDesc, DPOPEN_CREATE);
        if(hr == DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  Hosted a new session\n");
        if(hr != DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  ERROR:  New session not created\n");
        if(hr != DP_OK)  return FALSE;


        // Try to create a host player
        return create_player(TRUE);
    }
    else
    {
        // Run in solo mode
        return TRUE;
    }
*/
return FALSE;
}

//--------------------------------------------------------------------------------------------
void turn_on_service(int service)
{
    // This function turns on a network service ( IPX, TCP, serial, modem )
/*PORT
    HRESULT             hr;
    if(networkon && service < numservice)
    {
        hr = lpDirectPlay3A->InitializeConnection(netlpconnectionbuffer[service], 0);
        if(hr == DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  Service %d ( %s ) OK\n", service, netservicename[service]);
        if(hr != DP_OK && globalnetworkerr)  fprintf(globalnetworkerr, "  ERROR:  Service %d ( %s ) broken\n", service, netservicename[service]);
        if(hr != DP_OK) networkon = FALSE;
    }
    else
    {
        if(service == numservice)
        {
            if(globalnetworkerr)  fprintf(globalnetworkerr, "  Working in No Network mode\n");
        }
        else
        {
            if(globalnetworkerr)  fprintf(globalnetworkerr, "  ERROR:  Service %d not available\n", service);
        }
        networkon = FALSE;
    }
*/
}

