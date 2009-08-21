#include "Building.h"
#include <TextureManager.h>

Building::Building(int x1, int y1, int x2, int y2, int z, int mat_type, int mat_idx, int type)
: x1( x1 ),y1( y1 ),x2( x2 ),y2( y2 ),z( z ),mat_type( mat_type ),mat_idx( mat_idx ),type( type ) { }

Building::~Building()
{
    // NOP
}

bool Building::Draw(CameraOrientation Orientation)
{
    float xa,xb,ya,yb, r, g, b;
    xa = x1 %16 - 0.5;
    xb = x2 - x1 + 1;
    ya = y1 %16 - 0.5;
    yb = y2 - y1 + 1;

    glNormal3f(0.0,0.0,1.0);
    TEXTURE->BindTexturePoint(0, 0,0);
    glVertex3f(xa     ,ya     ,-0.4);
    TEXTURE->BindTexturePoint(0, 0,1);
    glVertex3f(xa     ,ya + yb,-0.4);
    TEXTURE->BindTexturePoint(0, 1,1);
    glVertex3f(xa + xb,ya + yb,-0.4);

    TEXTURE->BindTexturePoint(0, 1,1);
    glVertex3f(xa + xb,ya + yb,-0.4);
    TEXTURE->BindTexturePoint(0, 1,0);
    glVertex3f(xa + xb,ya     ,-0.4);
    TEXTURE->BindTexturePoint(0, 0,0);
    glVertex3f(xa     ,ya     ,-0.4);
    return true;
}
