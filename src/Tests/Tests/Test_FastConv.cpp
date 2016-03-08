#include "MUSI8903Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>
#include <iostream>

#include "UnitTest++.h"

#include "Vector.h"

#include "FastConv.h"

SUITE(FastConv)
{
    struct FastConvData
    {
        FastConvData() :
            m_pCFastConv(0),
            m_pfInputData(0),
            m_pfOutputData(0),
            m_pfInputDataTemp(0),
            m_pfOutputDataTemp(0),
            m_pfIR(0),
            m_iIRLength(51),
            m_iDataLength(100),
            m_iBlockLength(30)
			
        {
			CFastConv::create(m_pCFastConv);
            
            m_pfInputData = new float [m_iDataLength];
            CVectorFloat::setZero(m_pfInputData, m_iDataLength);

            m_pfOutputData = new float [m_iDataLength+m_iIRLength-1];
            CVectorFloat::setZero(m_pfOutputData, m_iDataLength);

            m_pfIR = new float [m_iIRLength];
            CVectorFloat::setZero( m_pfIR, m_iIRLength);
            
			
            }

        
        ~FastConvData()
        {

			delete[] m_pfInputData;
			delete[] m_pfOutputData;
			delete[] m_pfIR;
            
            CFastConv::destroy(m_pCFastConv);
			
        }
		
		
        void TestProcess()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);
                
                m_pfInputDataTemp = &m_pfInputData[m_iDataLength - iNumFramesRemaining];
                m_pfOutputDataTemp = &m_pfOutputData[m_iDataLength - iNumFramesRemaining];

                m_pCFastConv->process(m_pfInputDataTemp, m_pfOutputDataTemp, iNumFrames,false);
                
                iNumFramesRemaining -= iNumFrames;
            }
            
            m_pCFastConv->flushBuffer(&m_pfOutputData[m_iDataLength]);


        }
		
        
        CFastConv *m_pCFastConv;

		float *m_pfInputData,
			*m_pfOutputData,
            *m_pfInputDataTemp,
            *m_pfOutputDataTemp,
			*m_pfIR;
        int m_iIRLength,
            m_iDataLength,
            m_iBlockLength;
			
    };


	TEST_FIXTURE(FastConvData, IrTest)
	{
		// identity: generate a random IR (length 51 s), set it (init) and check the the output of a delayed (5 samples) impulse is the shifted IR.

        m_pCFastConv->reset();
        
        m_iIRLength = 51;

		int iDelayInSamples = 5;

		CVectorFloat::setZero(m_pfInputData, m_iDataLength);
		m_pfInputData[iDelayInSamples-1] = 1;

		for (int i = 0; i < m_iIRLength; i++) {
			m_pfIR[i] = ((float)rand() / (RAND_MAX)) ;
          //  std::cout << "ir " << m_pfIR[i] << std::endl;
		}

		m_pCFastConv->init(m_pfIR, m_iIRLength, m_iBlockLength);

        for (int i = 0; i < m_iIRLength; i++) {
          //  std::cout << "ir " << m_pfIR[i] << std::endl;
        }
        
        
		TestProcess();

		for (int i = 0; i < m_iDataLength + m_iIRLength -1; i++)
		{
            
            if (i < m_iIRLength + iDelayInSamples && i > iDelayInSamples-1){
                std::cout << i << "   " << m_pfIR[i-iDelayInSamples+1] << "   " << m_pfOutputData[i] << std::endl;
                CHECK_CLOSE(m_pfIR[i-iDelayInSamples+1], m_pfOutputData[i], 1e-3F);
            }
            else{
                std::cout << i << "   " << m_pfOutputData[i] << std::endl;
                CHECK_CLOSE(0, m_pfOutputData[i], 1e-3F);
            }
			
		}
    }

    TEST_FIXTURE(FastConvData, InputBlockLengthTest)
    {
    }

}

#endif //WITH_TESTS