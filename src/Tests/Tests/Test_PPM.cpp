//
//  Test_PPM.cpp
//  MUSI8903
//  Created by Keshav Venkat on 4/18/16.
//
//

#include "MUSI8903Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>
#include <iostream>

#include "UnitTest++.h"

#include "Vector.h"

#include "PPM.h"

SUITE(PPM)
{
    struct PPMData
    {
        PPMData()
        {
            CPPM::createInstance(m_pPPM);
            
            m_pPPM->initInstance( m_fSampleRate,  m_iNumChannels, 0.01 , 1.5);
            
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
        
        
        ~PPMData()
        {

            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
            }
            delete [] m_ppfInputTmp;
            
            CPPM::destroyInstance(m_pPPM);

            //delete [] m_pfIR;
            //m_pfIR = 0;
            
            
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
                m_pPPM->process(m_ppfInputData, m_ppfOutputData, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }
        }
        
        
        CPPM *m_pPPM;
        
        float  **m_ppfInputData,
        **m_ppfOutputData,
        **m_ppfOutputTmp,
        **m_ppfInputTmp,
        m_pfOutput,
        *m_pfIR;
        int m_iIRLength = 51;
        int     m_iDataLength = 45;
        float   m_fMaxDelayLength;
        int     m_iBlockLength;
        int     m_iNumChannels = 2;
        float   m_fSampleRate = 44100;
        
    };
    
    
   TEST_FIXTURE(PPMData, ZeroInputTest)
    {
       // Send zeros as input into the peak meter, verify if output remains zero.
        
        m_pPPM->initInstance( m_fSampleRate,  m_iNumChannels, .01, 1.5);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            m_ppfInputData[i]   = new float [m_iDataLength];
            CVectorFloat::setZero(m_ppfInputData[i], m_iDataLength);
        }
        
        TestProcess();
        
        
        for (int i = 0; i < m_iDataLength ; i++)
        {
            for (int j= 0; j<m_iNumChannels; j++) {
                CHECK_CLOSE(m_ppfInputData[j][i], m_ppfOutputData[j][i], 1e-3F);
                CHECK_CLOSE(0, m_ppfOutputData[j][i], 1e-3F);

            }
        }
            
    }
    
    
    
    TEST_FIXTURE(m_pPPM, InputBlockLengthTest)
    {
        
        m_iDataLength = 100;
        
        m_iIRLength = 51;
        
        int iDelayInSamples = 5;
        
        for (int z = 0; z < m_iNumBlockSizes; z++){
            
            CVectorFloat::setZero(m_pfOutputData, m_iDataLength+m_iIRLength-1);
            CVectorFloat::setZero(m_pfInputData, m_iPaddedInputLength);
            m_pfInputData[iDelayInSamples-1] = 1;
            
            for (int i = 0; i < m_iIRLength; i++) {
                m_pfIR[i] = ((float)rand() / (float)(RAND_MAX) * 2.0F - 1.F) ;
            }
            
            m_pCFastConv->init(m_pfIR, m_iIRLength, m_iConvBlockLength );
            TestProcess();
            
            
            for (int i = 0; i < m_iDataLength + m_iIRLength -1; i++)
            {
                
                if (i < m_iIRLength + iDelayInSamples-1 && i >= iDelayInSamples-1){
                    CHECK_CLOSE(m_pfIR[i-iDelayInSamples+1], m_pfOutputData[i], 1e-3F);
                }
                else{
                    CHECK_CLOSE(0, m_pfOutputData[i], 1e-3F);
                }
                
            }
            
        }
        
    }
    
   
 
}

#endif //WITH_TESTS
