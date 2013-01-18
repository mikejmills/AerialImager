
 
#include "Object.h"
#include <math.h>

/*-------------------------------------------------------------------*/
object::object()
{
    //Setup UP Vec should have accessor
    up.Set(0, 1, 0);
}

/*-------------------------------------------------------------------*/

void object::set_pos(Vec *p)
{
    pos.Set(p);
}

void object::set_pos(double x, double y, double z)
{
    pos.Set(x, y, z);
}

Vec *object::get_pos()
{
    return &pos;
}

void object::set_dir(Vec *d)
{
    dir.Set(d);
}

void object::set_dir(double x, double y, double z)
{
    dir.Set(x, y, z);
}


Vec *object::get_dir()
{
    return &(dir);
}


/*--------------------------------------------------------------------*/

void object::move(float amount)
{
    Vec scaledir;
    
    scaledir.Set(&dir);
    scaledir.Scl_p(amount);
    pos.Add(&pos, &scaledir);
}



#ifdef STANDALONE
#include <stdio.h>
int main()
{
    return 0;
}    
#endif