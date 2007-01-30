#ifndef _MATHSTUFF_H_
#define _MATHSTUFF_H_


/**> HEADER FILES <**/
//#include "egoboo.h"
#include <math.h>
#include "egobootypedef.h"

/**> MACROS <**/
#define _CNV(i,j) .v[4*i+j]
#define CopyMatrix( pMatrixSource, pMatrixDest ) memcpy( (pMatrixDest), (pMatrixSource), sizeof( GLMATRIX ) )


/**> DATA STRUCTURES <**/
typedef struct glmatrix { float v[16]; } GLMATRIX;
typedef struct glvector { float x,y,z,w; } GLVECTOR;


/**> GLOBAL VARIABLES <**/
extern float                   turntosin[16384];           // Convert chrturn>>2...  to sine


/**> FUNCTION PROTOTYPES <**/
GLVECTOR vsub(GLVECTOR A, GLVECTOR B);
GLVECTOR Normalize(GLVECTOR vec);
GLVECTOR CrossProduct(GLVECTOR A, GLVECTOR B);
float DotProduct(GLVECTOR A, GLVECTOR B);
GLMATRIX IdentityMatrix(void);
GLMATRIX ZeroMatrix(void);
GLMATRIX MatrixMult(const GLMATRIX a, const GLMATRIX b);
GLMATRIX Translate(const float dx, const float dy, const float dz);
GLMATRIX RotateX(const float rads);
GLMATRIX RotateY(const float rads);
GLMATRIX RotateZ(const float rads);
GLMATRIX ScaleXYZ(const float sizex, const float sizey, const float sizez);
GLMATRIX ScaleXYZRotateXYZTranslate(const float sizex, const float sizey, const float sizez, unsigned short turnz, unsigned short turnx, unsigned short turny, float tx, float ty, float tz);
GLMATRIX FourPoints(float orix, float oriy, float oriz, float widx, float widy, float widz, float forx, float fory, float forz, float upx,  float upy,  float upz, float scale);
GLMATRIX ViewMatrix(const GLVECTOR from, const GLVECTOR at, const GLVECTOR world_up, const float roll);
GLMATRIX ProjectionMatrix(const float near_plane, const float far_plane, const float fov);
void	TransformVertices( GLMATRIX *pMatrix, GLVECTOR *pSourceV, GLVECTOR *pDestV, Uint32 pNumVertor );

#endif

