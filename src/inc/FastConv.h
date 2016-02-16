
#if !defined(__FastConv_HEADER_INCLUDED__)
#define __FastConv_HEADER_INCLUDED__

#pragma once

#include "ErrorDef.h"

/*! \brief interface for fast convolution
*/
class CFastConv
{
public:

    CFastConv(void);
    virtual ~CFastConv(void);

    /*! initializes the class with the impulse response and the block length
    \param pfImpulseResponse impulse response samples (mono only)
    \param iLengthOfIr length of impulse response
    \param iBlockLength processing block size
    \return Error_t
    */
    Error_t init (float *pfImpulseResponse, int iLengthOfIr, int iBlockLength = 8192);
    
    /*! resets all internal class members
    \return Error_t
    */
    Error_t reset ();

    /*! computes cost and path w/o back-tracking
    \param pfInputBuffer (mono)
    \param pfOutputBuffer (mono)
    \param iLengthOfBuffers can be anything from 1 sample to 10000000 samples
    \return Error_t
    */
    Error_t process (float *pfInputBuffer, float *pfOutputBuffer, int iLengthOfBuffers );
 
private:
};


#endif