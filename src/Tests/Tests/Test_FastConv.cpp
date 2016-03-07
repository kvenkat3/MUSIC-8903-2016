#include "MUSI8903Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Vector.h"

#include "FastConv.h"

define RAND_MAX = 1;

SUITE(FastConv)
{
    struct FastConvData
    {
        FastConvData() :
            m_pCFastConv(0),
            m_pfInputData(0),
            m_pfOutputData(0),
            m_pfIR(0),
            m_iIRLength(35),
            m_iDataLength(51),
            m_iBlockLength(171)
        {
            m_pCFastConv new CFastConv(m_pCFastConv);
            
            m_pfInputData   = new float [m_iDataLength];
            CVectorFloat::setZero(m_pfInputData , m_iDataLength);
            m_pfOutputData  = new float [m_iDataLength];
            CVectorFloat::setZero( m_pfOutputData, m_iDataLength);
            m_pfIR = new float [m_iIRLenght];
            CVectorFloat::setZero( m_pfIR, m_iDataLength);

            
            }

        
        ~FastConvData()
        {
            delete [] m_pfInputData;
            delete [] m_pfOutputData;
            delete [] m_pfIR;
            
            delete    m_pCFastConv;
            
        }

        void TestProcess()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);
                
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c]   = &m_ppfOutputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pMyProject->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }
        }
        
        
        CFastConv *m_pCFastConv;

        float m_pfInputData,
            m_pfOutputData,
            m_pfIR;
        int m_iIRLength,
            m_iDataLength,
            m_iBlockLength;

        
        
        
       


    };


    TEST_FIXTURE(FastConvData, IrTest)
    {
   // identity: generate a random IR (length 51 s), set it (init) and check the the output of a delayed (5 samples) impulse is the shifted IR.

       m_pCFastConv->init(iResponse, 51, int iBlockLength /*= 8192*/)
       m_pCFastConv->process(
                             //for(int x = 0; x < 50; ++x)
                             //m_pfIR[x] = (float) rand() / (RAND_MAX))
                             
                             
    }

    TEST_FIXTURE(FastConvData, InputBlockLengthTest)
    {
    }

}

#endif //WITH_TESTS