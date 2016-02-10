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
    struct VibratoData
    {
        VibratoData() :
        m_pMyProject(0),
        m_ppfInputData(0),
        m_ppfOutputData(0),
        m_iDataLength(35),
        m_fMaxDelayLength(3.F),
        m_iBlockLength(171),
        m_iNumChannels(3),
        m_fSampleRate(20000),
        m_fFreq(50),
        m_fDepth(.0001F)
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
                CVectorFloat::setZero(m_ppfOutputData[i], m_iDataLength);            }
        }
        
        ~VibratoData()
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
        float   m_fFreq;
        float   m_fDepth;
        
    };
    
    TEST_FIXTURE(VibratoData, ZeroInput)
    {
        m_pMyProject->init(m_iNumChannels, m_fSampleRate, m_fMaxDelayLength, m_fFreq, m_fDepth);
        
        TestProcess();
        
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);
    }
    
    TEST_FIXTURE(VibratoData, ZeroModDepth)
    {
        m_pMyProject->init(m_iNumChannels, m_fSampleRate, m_fMaxDelayLength, m_fFreq, 0);
        
        for (int c = 0; c < m_iNumChannels; c++)
            CSynthesis::generateSine (m_ppfInputData[c], 387.F, m_fSampleRate, m_iDataLength, .8F, static_cast<float>(c*M_PI_2));
        
        TestProcess();
        
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);
    }
    
    TEST_FIXTURE(VibratoData, ZeroFreq)
    {
        m_pMyProject->init(m_iNumChannels, m_fSampleRate, m_fMaxDelayLength, 0, m_fDepth);
        
        int iDelayInSamples = round(m_fDepth*m_fSampleRate);
        
        for (int c = 0; c < m_iNumChannels; c++)
            CSynthesis::generateSine (m_ppfInputData[c], 387.F, m_fSampleRate, m_iDataLength, .8F, static_cast<float>(c*M_PI_2));
        
        TestProcess();
        
        for (int c = 0; c < m_iNumChannels; c++)
        {
            for (int i= 0; i < m_iDataLength-iDelayInSamples; i++)
            {
                CHECK_CLOSE(m_ppfInputData[c][i], m_ppfOutputData[c][i+iDelayInSamples], 1e-3F);
                
            }
        }
    }
    
    TEST_FIXTURE(VibratoData, DCInput)
    {
        m_pMyProject->init(m_iNumChannels, m_fSampleRate, m_fMaxDelayLength, m_fFreq, m_fDepth);
        
        int iDelayInSamples = round(m_fDepth*m_fSampleRate);
        
        for (int c = 0; c < m_iNumChannels; c++)
            for (int i= 0; i < m_iDataLength; i++)
            {
                m_ppfInputData[c][i] = 1;
                
            }
        
        TestProcess();
        
        for (int c = 0; c < m_iNumChannels; c++)
        {
            for (int i= 0; i < m_iDataLength-iDelayInSamples; i++)
            {
                CHECK_CLOSE(m_ppfInputData[c][i], m_ppfOutputData[c][i+iDelayInSamples], 1e-3F);
                
            }
        }
    }

    
    TEST_FIXTURE(VibratoData, VaryingBlocksize)
    {
        m_pMyProject->init(m_iNumChannels, m_fSampleRate, m_fMaxDelayLength, m_fFreq, m_fDepth);
        
        for (int c = 0; c < m_iNumChannels; c++)
            CSynthesis::generateSine (m_ppfInputData[c], 387.F, m_fSampleRate, m_iDataLength, .8F, static_cast<float>(c*M_PI_2));
        
        TestProcess();
        
        m_pMyProject->reset();
        m_pMyProject->init(m_iNumChannels, m_fSampleRate, m_fMaxDelayLength, m_fFreq, m_fDepth);
        
 
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                
                int iNumFrames = std::min(static_cast<float>(iNumFramesRemaining), static_cast<float>(rand())/RAND_MAX*17000);
                
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pMyProject->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }
        }
        
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);

    }
    
  
}

#endif //WITH_TESTS