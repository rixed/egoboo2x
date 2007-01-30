// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"

//--------------------------------------------------------------------------------------------
void render_fan(unsigned int fan)
{
	// ZZ> This function draws a mesh fan
	//D3DLVERTEX v[MAXMESHVERTICES];
	//D3DTLVERTEX vt[MAXMESHVERTICES];
	//D3DTLVERTEX vtlist[MAXMESHCOMMANDSIZE];
	GLVERTEX v[MAXMESHVERTICES];	
	Uint16 commands;
	Uint16 vertices;
	Uint16 basetile;
	Uint16 texture;
	Uint16 cnt, tnc, entry, vertex;
	Uint32 badvertex;
	//	unsigned char red, grn, blu;
	float offu, offv;
	//	float z;
	//DWORD ambi;
	//	DWORD fogspec;


	// vertex is a value from 0-15, for the meshcommandref/u/v variables
	// badvertex is a value that references the actual vertex number

	Uint16 tile = meshtile[fan];               // Tile
	Uint8 fx = meshfx[fan];                   // Fx bits
	Uint16 type = meshtype[fan];               // Command type ( index to points in fan )

	if (tile == FANOFF) {
		return;
	}

	// Animate the tiles
	if (fx & MESHFXANIM)
	{
		if(type >= (MAXMESHTYPE>>1))
		{
			// Big tiles
			basetile = tile & biganimtilebaseand;// Animation set
			tile += animtileframeadd << 1;         // Animated tile
			tile = (tile & biganimtileframeand) + basetile;
		}
		else
		{
			// Small tiles
			basetile = tile & animtilebaseand;// Animation set
			tile += animtileframeadd;         // Animated tile
			tile = (tile & animtileframeand) + basetile;
		}
	}
	offu = meshtileoffu[tile];          // Texture offsets
	offv = meshtileoffv[tile];          //

	texture = (tile>>6)+1;                  // 64 tiles in each 256x256 texture
	vertices = meshcommandnumvertices[type];// Number of vertices
	commands = meshcommands[type];          // Number of commands

	// Original points
	badvertex = meshvrtstart[fan];          // Get big reference value

	for (cnt = 0; cnt < vertices; cnt++)
		{
		v[cnt].x = (float) meshvrtx[badvertex] *RESCALE;
		v[cnt].y = (float) meshvrty[badvertex] *RESCALE;
		v[cnt].z = (float) meshvrtz[badvertex] *RESCALE;
		v[cnt].r = v[cnt].g = v[cnt].b = (float)meshvrtl[badvertex] / 255.0f;
		v[cnt].s = meshcommandu[type][badvertex % 16] + offu;
		v[cnt].t = meshcommandv[type][badvertex % 16] + offv;
		badvertex++;
	}

	// Change texture if need be
	if(meshlasttexture != texture)
	{
		glBindTexture ( GL_TEXTURE_2D, GLTexture_GetTextureID( &txTexture[texture] ));
		meshlasttexture = texture;
	}

	// Render each command
	entry = 0;
	for (cnt = 0; cnt < commands; cnt++)
	{
		glBegin (GL_TRIANGLE_FAN);
		for (tnc = 0; tnc < meshcommandsize[type][cnt]; tnc++)
		{
			vertex = meshcommandvrt[type][entry];
			glTexCoord2f ( meshcommandu[type][vertex]+offu, meshcommandv[type][vertex]+offv);
			glVertex3fv ( &v[vertex].x );
			entry++;
		}
		glEnd();
	}
}

//--------------------------------------------------------------------------------------------
void render_water_fan(unsigned int fan, unsigned char layer, unsigned char mode)
{
	// ZZ> This function draws a water fan
	GLVERTEX v[MAXMESHVERTICES];	
	Uint16 type;
	Uint16 commands;
	Uint16 vertices;
	Uint16 texture, frame;
	Uint16 cnt, tnc, entry, vertex;
	Uint32 badvertex;
	//	unsigned char red, grn, blu;
	float offu, offv;
	//	float z;
	DWORD ambi;
	//	DWORD fogspec;

	// vertex is a value from 0-15, for the meshcommandref/u/v variables
	// badvertex is a value that references the actual vertex number

	// To make life easier
	type = 0;                           // Command type ( index to points in fan )
	offu = waterlayeru[layer];          // Texture offsets
	offv = waterlayerv[layer];          //
	frame = waterlayerframe[layer];     // Frame

	texture = layer+5;                      // Water starts at texture 5
	vertices = meshcommandnumvertices[type];// Number of vertices
	commands = meshcommands[type];          // Number of commands

	// Original points
	badvertex = meshvrtstart[fan];          // Get big reference value
	// Corners
	for (cnt = 0; cnt < vertices; cnt++)
	{
		v[cnt].x = meshvrtx[badvertex] *RESCALE;
		v[cnt].y = meshvrty[badvertex] *RESCALE;
		v[cnt].z = (waterlayerzadd[layer][frame][mode][cnt]+waterlayerz[layer]) *RESCALE;

		ambi = (DWORD) meshvrtl[badvertex]>>1;
		ambi+= waterlayercolor[layer][frame][mode][cnt];
		v[cnt].r = v[cnt].g = v[cnt].b = (float)ambi / 255.0f;
		v[cnt].a = (float)waterlayeralpha[layer] / 255.0f;

		badvertex++;
	}


	// Change texture if need be
	if(meshlasttexture != texture)
	{
		glBindTexture ( GL_TEXTURE_2D, GLTexture_GetTextureID( &txTexture[texture] ));
		meshlasttexture = texture;
	}

	// Render each command
	entry = 0;
	v[0].s = 1+offu;
	v[0].t = 0+offv;
	v[1].s = 1+offu;
	v[1].t = 1+offv;
	v[2].s = 0+offu;
	v[2].t = 1+offv;
	v[3].s = 0+offu;
	v[3].t = 0+offv;
	for (cnt = 0; cnt < commands; cnt++)
	{
		glBegin (GL_TRIANGLE_FAN);
		for (tnc = 0; tnc < meshcommandsize[type][cnt]; tnc++)
		{
			vertex = meshcommandvrt[type][entry];
//			glColor4fv( &v[vertex].r );
			glColor4x( 0, 0, 0, 0x7000 );
			glTexCoord2fv ( &v[vertex].s );
			glVertex3fv ( &v[vertex].x );
			entry++;
		}
		glEnd ();
	}
}
