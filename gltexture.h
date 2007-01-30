#ifndef	_GLTEXTURE_H_
#define	_GLTEXTURE_H_


/**> HEADER FILES <**/
#include <GL/gl.h>


/**> DATA STRUCTURE: GLTexture <**/
typedef struct GLTexture
{
	GLuint	textureID;				/* The OpenGL texture number */
	GLint	internalFormat;			/* GL_RGB or GL_RGBA */
	GLsizei	imgHeight, imgWidth;	/* the height & width of the original image */
	GLsizei	txDimensions;			/* the height/width of the the OpenGL texture (must be a power of two) */
	GLfloat	alpha;					/* the alpha for the texture */
}	GLTexture;


/**> FUNCTION PROTOTYPES: GLTexture <**/
void		GLTexture_Load( GLTexture *texture, const char *filename );
void		GLTexture_LoadA( GLTexture *texture, const char *filename, Uint32 key );
GLuint		GLTexture_GetTextureID( GLTexture *texture );
GLsizei		GLTexture_GetImageHeight( GLTexture *texture );
GLsizei		GLTexture_GetImageWidth( GLTexture *texture );
GLsizei		GLTexture_GetDimensions( GLTexture *texture );
void		GLTexture_SetAlpha( GLTexture *texture, GLfloat alpha );
GLfloat		GLTexture_GetAlpha( GLTexture *texture );
void		GLTexture_Release( GLTexture *texture );


#endif

