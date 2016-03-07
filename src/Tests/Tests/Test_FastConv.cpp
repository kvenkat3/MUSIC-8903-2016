#include "MUSI8903Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

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
            m_pfIR(0),
            m_iIRLength(51),
            m_iDataLength(100),
            m_iBlockLength(128)
			
        {
			CFastConv::create(m_pCFastConv);
            
            m_pfInputData = new float [m_iDataLength];
            CVectorFloat::setZero(m_pfInputData, m_iDataLength);

            m_pfOutputData = new float [m_iDataLength];
            CVectorFloat::setZero(m_pfOutputData, m_iDataLength);

            m_pfIR = new float [m_iIRLength];
            CVectorFloat::setZero( m_pfIR, m_iIRLength);
			
            }

        
        ~FastConvData()
        {

            
			CFastConv::destroy(m_pCFastConv);

			delete[] m_pfInputData;
			delete[] m_pfOutputData;
			delete[] m_pfIR;
			
        }
		
		
        void TestProcess()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);

                m_pCFastConv->process(m_pfInputData, m_pfOutputData, iNumFrames,false);
                
                iNumFramesRemaining -= iNumFrames;
            }


        }
		
        
        CFastConv *m_pCFastConv;

		float *m_pfInputData,
			*m_pfOutputData,
			*m_pfIR;
        int m_iIRLength,
            m_iDataLength,
            m_iBlockLength;
			
    };


	TEST_FIXTURE(FastConvData, IrTest)
	{
		// identity: generate a random IR (length 51 s), set it (init) and check the the output of a delayed (5 samples) impulse is the shifted IR.

		m_iIRLength = 51;

		int iDelayInSamples = 5;

		CVectorFloat::setZero(m_pfInputData, m_iDataLength);
		m_pfInputData[iDelayInSamples - 1] = 1;

		for (int i = 0; i < m_iIRLength; i++) {
			m_pfIR[i] = ((float)rand() / (RAND_MAX)) + 1;
		}

		m_pCFastConv->init(m_pfIR, m_iIRLength, m_iBlockLength);

		TestProcess();

		/*for (int i = 0; i < m_iDataLength - iDelayInSamples; i++)
		{
			CHECK_CLOSE(m_pfInputData[i], m_pfOutputData[i + iDelayInSamples], 1e-3F);

		}*/
    }

    TEST_FIXTURE(FastConvData, InputBlockLengthTest)
    {
    }

}

#endif //WITH_TESTS