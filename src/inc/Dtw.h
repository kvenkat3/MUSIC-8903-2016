
#if !defined(__Dtw_HEADER_INCLUDED__)
#define __Dtw_HEADER_INCLUDED__

#pragma once

#include "ErrorDef.h"


class CDtw
{
public:

    CDtw(void);
    virtual ~CDtw(void);

    Error_t init (float **ppfDistanceMatrix, int iNumRows, int iNumCols);
    Error_t reset ();

    Error_t process ();
 
    int getPathLength () const;
    float getPathCost () const;
    Error_t getPath (int *piPathResult) const;

private:
    bool m_bIsInitialized;
};


#endif
