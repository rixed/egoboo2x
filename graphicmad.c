// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"

void render_texmad(unsigned short character, unsigned char trans)
{
	// FIXME: takes 22% of CPU, the 2/3 being spent in glVertex3fv+glTexCoord2f+glColor4fv functions,
	//        more than 50% of which for converting to fixed point.
	//        So using fixedpoint only, here, would lead to between 5 and 10% of speed improvement. +1FPS!
	//        The same for render_fan.
	// ZZ> This function draws a model
	//    D3DLVERTEX v[MAXVERTICES];
	//    D3DTLVERTEX vt[MAXVERTICES];
	//    D3DTLVERTEX vtlist[MAXCOMMANDSIZE];
	GLVERTEX v[MAXVERTICES];	
	unsigned short cnt, tnc, entry;
	unsigned short vertex;
	signed int temp;
	unsigned char ambi;

	// To make life easier
	unsigned short model = chrmodel[character];
	unsigned short texture = chrtexture[character];
	unsigned short frame = chrframe[character];
	unsigned short lastframe = chrlastframe[character];
	unsigned char lip = chrlip[character]>>6;
	unsigned char lightrotation = 
		(chrturnleftright[character]+chrlightturnleftright[character])>>8;
	unsigned char lightlevel = chrlightlevel[character]>>4;
	DWORD alpha = 90;//120;//trans<<24;
	unsigned char spek = chrsheen[character];

	float uoffset = textureoffset[chruoffset[character]>>8];
	float voffset = textureoffset[chrvoffset[character]>>8];
	unsigned char rs = chrredshift[character];
	unsigned char gs = chrgrnshift[character];
	unsigned char bs = chrblushift[character];

	if(phongon && trans == 255)
		spek = 0;

	// Original points with linear interpolation ( lip )
	switch(lip)
	{
		case 0:  // 25% this frame
			for (cnt = 0; cnt < madtransvertices[model]; cnt++)
			{
				temp = madvrtx[lastframe][cnt];
				temp = temp+temp+temp;
				v[cnt].x = (float) ((madvrtx[frame][cnt] + temp)>>2) *RESCALE;
				temp = madvrty[lastframe][cnt];
				temp = temp+temp+temp;
				v[cnt].y = (float) ((madvrty[frame][cnt] + temp)>>2) *RESCALE;
				temp = madvrtz[lastframe][cnt];
				temp = temp+temp+temp;
				v[cnt].z = (float) ((madvrtz[frame][cnt] + temp)>>2) *RESCALE;

				ambi = chrvrta[character][cnt];
				ambi = (((ambi+ambi+ambi)<<1)+ambi+lighttable[lightlevel][lightrotation][madvrta[frame][cnt]])>>3;
				chrvrta[character][cnt] = ambi;
				v[cnt].r = (float) (ambi>>rs) / 255.0f;
				v[cnt].g = (float) (ambi>>gs) / 255.0f;
				v[cnt].b = (float) (ambi>>bs) / 255.0f;
				v[cnt].a = (float) alpha / 255.0f;
			}
			break;
		case 1:  // 50% this frame
			for (cnt = 0; cnt < madtransvertices[model]; cnt++)
			{
				v[cnt].x = (float) ((madvrtx[frame][cnt] +
							madvrtx[lastframe][cnt])>>1) *RESCALE;
				v[cnt].y = (float) ((madvrty[frame][cnt] +
							madvrty[lastframe][cnt])>>1) *RESCALE;
				v[cnt].z = (float) ((madvrtz[frame][cnt] +
							madvrtz[lastframe][cnt])>>1) *RESCALE;

				ambi = chrvrta[character][cnt];
				ambi = (((ambi+ambi+ambi)<<1)+ambi+lighttable[lightlevel][lightrotation][madvrta[frame][cnt]])>>3;
				chrvrta[character][cnt] = ambi;
				v[cnt].r = (float) (ambi>>rs) / 255.0f;
				v[cnt].g = (float) (ambi>>gs) / 255.0f;
				v[cnt].b = (float) (ambi>>bs) / 255.0f;
				v[cnt].a = (float) alpha  / 255.0f;
			}
			break;
		case 2:  // 75% this frame
			for (cnt = 0; cnt < madtransvertices[model]; cnt++)
			{
				temp = madvrtx[frame][cnt];
				temp = temp+temp+temp;
				v[cnt].x = (float) ((madvrtx[lastframe][cnt] + temp)>>2) *RESCALE;
				temp = madvrty[frame][cnt];
				temp = temp+temp+temp;
				v[cnt].y = (float) ((madvrty[lastframe][cnt] + temp)>>2) *RESCALE;
				temp = madvrtz[frame][cnt];
				temp = temp+temp+temp;
				v[cnt].z = (float) ((madvrtz[lastframe][cnt] + temp)>>2) *RESCALE;

				ambi = chrvrta[character][cnt];
				ambi = (((ambi+ambi+ambi)<<1)+ambi+lighttable[lightlevel][lightrotation][madvrta[frame][cnt]])>>3;
				chrvrta[character][cnt] = ambi;
				v[cnt].r = (float) (ambi>>rs) / 255.0f;
				v[cnt].g = (float) (ambi>>gs) / 255.0f;
				v[cnt].b = (float) (ambi>>bs) / 255.0f;
				v[cnt].a = (float) alpha / 255.0f;
			}
			break;
		case 3:  // 100% this frame...  This is the legible one
			for (cnt = 0; cnt < madtransvertices[model]; cnt++)
			{
				v[cnt].x = (float) madvrtx[frame][cnt] *RESCALE;
				v[cnt].y = (float) madvrty[frame][cnt] *RESCALE;
				v[cnt].z = (float) madvrtz[frame][cnt] *RESCALE;

				ambi = chrvrta[character][cnt];
				ambi = (((ambi+ambi+ambi)<<1)+ambi+lighttable[lightlevel][lightrotation][madvrta[frame][cnt]])>>3;
				chrvrta[character][cnt] = ambi;
				v[cnt].r = (float) (ambi>>rs) / 255.0f;
				v[cnt].g = (float) (ambi>>gs) / 255.0f;
				v[cnt].b = (float) (ambi>>bs) / 255.0f;
				v[cnt].a = (float) alpha / 255.0f;
			}
			break;
	}

	glBindTexture( GL_TEXTURE_2D, GLTexture_GetTextureID( &txTexture[texture] ) );    

	glPushMatrix();
	
	glLoadMatrixf(mView.v);
	glMultMatrixf(chrmatrix[character].v);
	glMultMatrixf(mScale);

	// Render each command
	entry = 0;
	for (cnt = 0; cnt < madcommands[model]; cnt++)
	{
		glBegin (madcommandtype[model][cnt]);
		for (tnc = 0; tnc < madcommandsize[model][cnt]; tnc++)
		{
			vertex = madcommandvrt[model][entry];
			glColor4fv( &v[vertex].r );
			float uc = madcommandu[model][entry]+uoffset;
			float vc = madcommandv[model][entry]+voffset;
			glTexCoord2f (uc, vc);
			glVertex3fv (&v[vertex].x);
			entry++;
		}
		glEnd ();
	}

	glPopMatrix();
}

//--------------------------------------------------------------------------------------------
void render_mad(unsigned short character, unsigned char trans)
{
	// ZZ> This function picks the actual function to use
	signed char hide = caphidestate[chrmodel[character]];

	if(hide == NOHIDE || hide != chraistate[character])
	{
/*RXD		if(0 && chrenviro[character])	//RXD
			render_enviromad(character, trans);
		else*/
			render_texmad(character, trans);
	}
}

