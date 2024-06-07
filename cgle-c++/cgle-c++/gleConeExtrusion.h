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

#pragma once


//----------------------------------------------------------------------------
class CgleConeExtrusion : public CgleBaseExtrusion
{

public:

    CgleConeExtrusion(int Points, int Facets);
    virtual ~CgleConeExtrusion();
    
    
    void Draw(double Point_Array[][3], float Color_Array[][3], double Radius_Array[], bool bTextured = false);
    
    void WireDraw(double Point_Array[][3], float Color_Array[][3], double Radius_Array[], bool bTextured = false);
    
    
    
protected:

/// @cond
    void  Gen_PolyCone(double Point_Array[][3], float Color_Array[][3], double Radius, bool bTextured);
/// @endcond


private:

    int FFacets;
    
};
//----------------------------------------------------------------------------
//
