/***********************************************************
*                                        GLE32 Extrusion Library                            *
*                           Copyright(c) 2000 - 2017 by Dave Richards               *
*                                         All Rights Reserved.                          *
*                                               Ver 6.0                                 *
*                                                                                           *
*                                           HISTORY:                                        *
*                                   Linas Vepstas 1990 - 1997                           *
*                                   Dave Richards  2000 - 2017                      *
*                                                                                           *
************************************************************/


#include "stdafx.h"
#include "ExtrusionLib.h"
#include "ExtrusionInternals.h"


//----------------------------------------------------------------------------
void InitializeExtrusionLib()
{
    CreateGC();
}
//----------------------------------------------------------------------------
void ReleaseExtrusionLib()
{
    DestroyGC();
}
//----------------------------------------------------------------------------
