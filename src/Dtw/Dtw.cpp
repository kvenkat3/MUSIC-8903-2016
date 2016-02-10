
#include "Dtw.h"

CDtw::CDtw( void ) :
    m_bIsInitialized(false)
{
    reset();
}

CDtw::~CDtw( void )
{
    reset();
}

Error_t CDtw::init( float **ppfDistanceMatrix, int iNumRows, int iNumCols )
{
    reset();


    m_bIsInitialized    = true;
    return kNoError;
}

Error_t CDtw::reset()
{
    m_bIsInitialized    = false;

    return kNoError;
}

Error_t CDtw::process()
{
    if (!m_bIsInitialized)
        return kNotInitializedError;

    return kNoError;
}

int CDtw::getPathLength() const
{
    return 0;
}

float CDtw::getPathCost() const
{
    return 0.0F;
}

Error_t CDtw::getPath( int *piPathResult ) const
{
    if (!piPathResult)
        return kFunctionInvalidArgsError;

    return kNoError;
}
