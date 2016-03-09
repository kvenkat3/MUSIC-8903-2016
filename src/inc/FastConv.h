
#if !defined(__FastConv_HEADER_INCLUDED__)
#define __FastConv_HEADER_INCLUDED__

#pragma once

#include "ErrorDef.h"
#include "Fft.h"

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

	static Error_t create(CFastConv*& pCFastConv);

	static Error_t destroy(CFastConv*& pCFastConv);

    /*! computes cost and path w/o back-tracking
    \param pfInputBuffer (mono)
    \param pfOutputBuffer (mono)
    \param iLengthOfBuffers can be anything from 1 sample to 10000000 samples
    \return Error_t
    */
    Error_t process (float *pfInputBuffer, float *pfOutputBuffer, int iLengthOfBuffers, bool isFast);
 
	Error_t processTimeDomain(float *pfInputBuffer, float *pfOutputBuffer, int iLengthOfBuffers);

	Error_t flushBuffer(float *pfReverbTail);

private:
	int m_iLengthOfIr;
    int m_iBlockLength;
    int m_iNumInputBlocks;
    int m_iNumIrBlocks;
    
    int m_iNumInputZeros;
    int m_iNumIrZeros;
    
	float *m_pfImpulseResponse;
	float *m_pfTailBuffer;
    float *m_pfOutputTemp;
    float *m_pfInputTemp;

    float *m_pfIrTemp;
    
    CFft::complex_t *m_pcInputSpectrum;
    CFft::complex_t *m_pcIrSpectrum;
    CFft::complex_t *m_pcResultSpectrum;
    
    CFft *m_pCFft;
};


#endif
