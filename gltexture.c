// GLTexture.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h" // GAC - Needed for Win32 stuff
#include "gltexture.h"


/********************> GLTexture_Load() <*****/
void		GLTexture_Load( GLTexture *texture, const char *filename )
{
	
	SDL_Surface	*tempSurface, *imageSurface;
	
	/* Load the bitmap into an SDL_Surface */
	imageSurface = SDL_LoadBMP( FILENAME(filename) );
	
	/* Make sure a valid SDL_Surface was returned */
	if ( imageSurface != NULL )
	{
		/* Generate an OpenGL texture */
		glGenTextures( 1, &texture->textureID );
		
		/* Set up some parameters for the format of the OpenGL texture */
		glBindTexture( GL_TEXTURE_2D, texture->textureID );					/* Bind Our Texture */
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );	/* Linear Filtered */
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );	/* Linear Filtered */
		
		/* Set the original image's size (incase it's not an exact square of a power of two) */
		texture->imgHeight = imageSurface->h;
		texture->imgWidth = imageSurface->w;
		
		/* Determine the correct power of two greater than or equal to the original image's size */
		texture->txDimensions = 2;
		while ( ( texture->txDimensions < texture->imgHeight ) && ( texture->txDimensions < texture->imgWidth ) )
			texture->txDimensions *= 2;
		
		/* Set the texture's alpha */
		texture->alpha = 1;
		
		/* Create a blank SDL_Surface (of the smallest size to fit the image) & copy imageSurface into it*/
		if(imageSurface->format->Gmask)
		{
			tempSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, texture->txDimensions, texture->txDimensions, 24, imageSurface->format->Rmask, imageSurface->format->Gmask, imageSurface->format->Bmask, imageSurface->format->Amask );
		}
		else
		{
#ifdef _LITTLE_ENDIAN
			tempSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, texture->txDimensions, texture->txDimensions, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000);
#else
			tempSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, texture->txDimensions, texture->txDimensions, 24, 0xFF0000, 0x00FF00, 0x0000FF, 0x000000);
#endif
		}
		SDL_BlitSurface( imageSurface, &imageSurface->clip_rect, tempSurface, &imageSurface->clip_rect );
		
		/* actually create the OpenGL textures */
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tempSurface->w, tempSurface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, tempSurface->pixels );
		
		/* get rid of our SDL_Surfaces now that we're done with them */
		SDL_FreeSurface( tempSurface );
		SDL_FreeSurface( imageSurface );
	}
	
}

/********************> GLTexture_LoadA() <*****/
void		GLTexture_LoadA( GLTexture *texture, const char *filename, Uint32 key )
{
	
	/* The key param indicates which color to set alpha to 0.  All other values are 0xFF. */
	SDL_Surface	*tempSurface, *imageSurface;
	Sint16  x,y;
	Uint32  *p;

	/* Load the bitmap into an SDL_Surface */
	imageSurface = SDL_LoadBMP( FILENAME(filename) );
	
	/* Make sure a valid SDL_Surface was returned */
	if ( imageSurface != NULL )
	{
		/* Generate an OpenGL texture */
		glGenTextures( 1, &texture->textureID );
		
		/* Set up some parameters for the format of the OpenGL texture */
		glBindTexture( GL_TEXTURE_2D, texture->textureID );					/* Bind Our Texture */
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );	/* Linear Filtered */
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );	/* Linear Filtered */
		
		/* Set the original image's size (incase it's not an exact square of a power of two) */
		texture->imgHeight = imageSurface->h;
		texture->imgWidth = imageSurface->w;
		
		/* Determine the correct power of two greater than or equal to the original image's size */
		texture->txDimensions = 2;
		while ( ( texture->txDimensions < texture->imgHeight ) && ( texture->txDimensions < texture->imgWidth ) )
			texture->txDimensions *= 2;
		
		/* Set the texture's alpha */
		texture->alpha = 1;
		
		/* Create a blank SDL_Surface (of the smallest size to fit the image) & copy imageSurface into it*/
              //SDL_SetColorKey(imageSurface, SDL_SRCCOLORKEY,0);
              //cvtSurface = SDL_ConvertSurface(imageSurface, &fmt, SDL_SWSURFACE);
		
		if(imageSurface->format->Gmask)
		{
			tempSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, texture->txDimensions, texture->txDimensions, 32, imageSurface->format->Rmask, imageSurface->format->Gmask, imageSurface->format->Bmask, imageSurface->format->Amask );
		}
		else
		{
#ifdef _LITTLE_ENDIAN
			tempSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, texture->txDimensions, texture->txDimensions, 32, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000);
#else
			tempSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, texture->txDimensions, texture->txDimensions, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000);
#endif
		}
		//SDL_BlitSurface( cvtSurface, &cvtSurface->clip_rect, tempSurface, &cvtSurface->clip_rect );
		SDL_BlitSurface( imageSurface, &imageSurface->clip_rect, tempSurface, &imageSurface->clip_rect );
              
		/* Fix the alpha values */
		SDL_LockSurface(tempSurface);
		p = tempSurface->pixels;
		for( y = (texture->txDimensions - 1) ;y >= 0; y-- )
		{
			for( x = (texture->txDimensions - 1); x >= 0; x-- )
			{
				if( p[x+y*texture->txDimensions] != key )
				{
#ifdef _LITTLE_ENDIAN
					p[x+y*texture->txDimensions] = p[x+y*texture->txDimensions] | 0xFF000000;
#else
					p[x+y*texture->txDimensions] = p[x+y*texture->txDimensions] | 0x000000FF;
#endif
				}
			}
		}
		SDL_UnlockSurface(tempSurface);
		
		/* actually create the OpenGL textures */
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tempSurface->w, tempSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tempSurface->pixels );
		
		/* get rid of our SDL_Surfaces now that we're done with them */
		SDL_FreeSurface( tempSurface );
		SDL_FreeSurface( imageSurface );
	}

}

/********************> GLTexture_GetTextureID() <*****/
GLuint		GLTexture_GetTextureID( GLTexture *texture )
{
	
	return texture->textureID;
	
}

/********************> GLTexture_GetImageHeight() <*****/
GLsizei		GLTexture_GetImageHeight( GLTexture *texture )
{
	
	return texture->imgHeight;
	
}

/********************> GLTexture_GetImageWidth() <*****/
GLsizei		GLTexture_GetImageWidth( GLTexture *texture )
{
	
	return texture->imgWidth;
	
}

/********************> GLTexture_GetDimensions() <*****/
GLsizei		GLTexture_GetDimensions( GLTexture *texture )
{
	
	return texture->txDimensions;
	
}

/********************> GLTexture_SetAlpha() <*****/
void		GLTexture_SetAlpha( GLTexture *texture, GLfloat alpha )
{
	
	texture->alpha = alpha;
	
}

/********************> GLTexture_GetAlpha() <*****/
GLfloat		GLTexture_GetAlpha( GLTexture *texture )
{
	
	return texture->alpha;
	
}

/********************> GLTexture_Release() <*****/
void		GLTexture_Release( GLTexture *texture )
{
	/* Delete the OpenGL texture */
	if (glIsTexture(texture->textureID)) {
		glDeleteTextures( 1, &texture->textureID );
	}
	
	/* Reset the other data */
	texture->imgHeight = texture->imgWidth = texture->txDimensions = 0;
	
}
