#include "MUSI8903Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"
#include "MyProject.h"

SUITE(Vibrato)
{
    struct VibData
    {
        VibData() :
        m_pMyProject(0),
        m_ppfInputData(0),
        m_ppfOutputData(0),
        m_iDataLength(100),
        m_fMaxDelayLength(441),
        m_iBlockLength(512),
        m_iNumChannels(1),
        m_fSampleRate(8000),
        m_fDepth(.1),
        m_fFreq(50)
        {
            CMyProject::create(m_pMyProject);
            
            m_ppfInputData  = new float*[m_iNumChannels];
            m_ppfOutputData = new float*[m_iNumChannels];
            m_ppfInputTmp   = new float*[m_iNumChannels];
            m_ppfOutputTmp  = new float*[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i]   = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfInputData[i], m_iDataLength);
                m_ppfOutputData[i]  = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfOutputData[i], m_iDataLength);
            }
        }
        
        ~VibData()
        {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
            }
            delete [] m_ppfOutputTmp;
            delete [] m_ppfInputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;
            
            CMyProject::destroy(m_pMyProject);
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
        void TestProcessInplace()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);
                
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pMyProject->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }
        }
        
        
        
        CMyProject  *m_pMyProject;
        float       **m_ppfInputData,
        **m_ppfOutputData,
        **m_ppfInputTmp,
        **m_ppfOutputTmp;
        int     m_iDataLength;
        float   m_fMaxDelayLength;
        int     m_iBlockLength;
        int     m_iNumChannels;
        float   m_fSampleRate;
        float   m_fDelay;
        float   m_fDepth;
        float   m_fFreq;
        
    };
    
    TEST_FIXTURE(VibData, ZeroInput)
    {
        m_pMyProject->init(m_iNumChannels, m_fSampleRate, m_fMaxDelayLength, m_fFreq, m_fDepth);
 
        TestProcess();
        
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);
    }
    
  
}

#endif //WITH_TESTS