
#include "Vector.h"
#include "Util.h"
#include <iostream>

#include "FastConv.h"

CFastConv::CFastConv( void )
{

    reset();
}

CFastConv::~CFastConv( void )
{
    reset();
}

Error_t CFastConv::create(CFastConv*& pCFastConv)
{
	pCFastConv = new CFastConv();

	if (!pCFastConv)
		return kUnknownError;

	return kNoError;
    
    
}

Error_t CFastConv::destroy(CFastConv*& pCFastConv)
{
	if (!pCFastConv)
		return kUnknownError;

	pCFastConv->reset();

	delete pCFastConv;

	pCFastConv = 0;

	return kNoError;

}

Error_t CFastConv::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength /*= 8192*/)
{
	m_pfImpulseResponse = pfImpulseResponse;

	m_iLengthOfIr = iLengthOfIr;

	m_pfTailBuffer = new float[iLengthOfIr-1];
    CVectorFloat::setZero(m_pfTailBuffer, iLengthOfIr-1);

    return kNoError;
}

Error_t CFastConv::reset()
{

	m_pfImpulseResponse = 0;

	m_iLengthOfIr = 0;

    if(m_pfTailBuffer){
      //  delete [] m_pfTailBuffer;
    }
    m_pfTailBuffer = 0;

    return kNoError;
}

Error_t CFastConv::process (float *pfInputBuffer, float *pfOutputBuffer, int iLengthOfBuffers, bool isFast )
{
	if (!isFast) {
		processTimeDomain(pfInputBuffer, pfOutputBuffer, iLengthOfBuffers);
	}
	//process here
    return kNoError;
}

Error_t CFastConv::processTimeDomain(float *pfInputBuffer, float *pfOutputBuffer, int iLengthOfBuffers)
{
   /* if(m_iLengthOfIr>iLengthOfBuffers){
        
        for (int i = 0; i < iLengthOfBuffers+m_iLengthOfIr-1; i++)
        {
            pfOutputBuffer[i] = 0;
            for (int j = 0; j <iLengthOfBuffers ; j++){
                if (i-j >= 0 && i-j < m_iLengthOfIr)
                {
                    //    std::cout << "in buff " <<pfInputBuffer[i-j] << std::endl;
                    if (i >= m_iLengthOfIr){
                        m_pfTailBuffer[i-m_iLengthOfIr] = m_pfTailBuffer[i - m_iLengthOfIr] + m_pfImpulseResponse[i - j] * pfInputBuffer[j];
                        //std::cout << "in Tailbuffer " <<m_pfTailBuffer[i-iLengthOfBuffers] << std::endl;
                        
                    }
                    else{
                        pfOutputBuffer[i] = pfOutputBuffer[i] +  m_pfImpulseResponse[i - j] * pfInputBuffer[j];
                    }
                }
            }
            if (i < iLengthOfBuffers-1){
                pfOutputBuffer[i] += m_pfTailBuffer[i];
                //  std::cout << "in Tailbuffer " <<m_pfTailBuffer[i] << std::endl;
                
            }
            // std::cout << "out buff " << pfOutputBuffer[i] << std::endl;
        }
        
        
        
        return kNoError;
        
        
    }
    else{*/
        //Code you had starts here
        //process here
        for (int i = 0; i < iLengthOfBuffers+m_iLengthOfIr-1; i++)
        {
            pfOutputBuffer[i] = 0;
            for (int j = 0; j < m_iLengthOfIr; j++){
                if (i-j >= 0 && i-j < iLengthOfBuffers)
                {
                //    std::cout << "in buff " <<pfInputBuffer[i-j] << std::endl;
                    if (i >= iLengthOfBuffers){
                        m_pfTailBuffer[i-iLengthOfBuffers] = m_pfTailBuffer[i - iLengthOfBuffers] + pfInputBuffer[i - j] * m_pfImpulseResponse[j];
                        //std::cout << "in Tailbuffer " <<m_pfTailBuffer[i-iLengthOfBuffers] << std::endl;

                    }
                    else{
                        pfOutputBuffer[i] = pfOutputBuffer[i] + pfInputBuffer[i - j] * m_pfImpulseResponse[j];
                    }
                }
            }
            if (i < m_iLengthOfIr-1){
                pfOutputBuffer[i] += m_pfTailBuffer[i];
              //  std::cout << "in Tailbuffer " <<m_pfTailBuffer[i] << std::endl;

            }
           // std::cout << "out buff " << pfOutputBuffer[i] << std::endl;
        }
        
        //Code you had ends here
        
        
        return kNoError;
    //}
}

Error_t CFastConv::flushBuffer(float *pfReverbTail) {

	for (int i = 0; i < m_iLengthOfIr; i++) {
			pfReverbTail[i] = m_pfTailBuffer[i];
	}

	return kNoError;
}
