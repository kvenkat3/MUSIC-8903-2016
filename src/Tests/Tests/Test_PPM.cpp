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
#include "AudioFileIf.h"

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
            m_ppfMatlabData = new float*[m_iNumChannels];
            m_ppfOutputData = new float*[m_iNumChannels];
            m_ppfInputTmp   = new float*[m_iNumChannels];
            m_ppfOutputTmp  = new float*[m_iNumChannels];
            
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i]   = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfInputData[i], m_iDataLength);
                m_ppfOutputData[i]  = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfOutputData[i], m_iDataLength);
                m_ppfMatlabData[i]   = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfMatlabData[i], m_iDataLength);
                
            }
            
            phOnesAudioFile        = 0;
            phOnesPPMAudioFile       = 0;
            
            
            // Read Audio Files
            sOnesFilePath = "Ones.wav" ;
            CAudioFileIf::create(phOnesAudioFile);
            phOnesAudioFile->openFile(sOnesFilePath, CAudioFileIf::kFileRead);
            if (!phOnesAudioFile->isOpen())
            {
                std::cout << "Wave file open error!";
            }
            phOnesAudioFile->getFileSpec(sOnesAudioFileSpec);
            
            sOnesPPMFilePath = "Ones_PPM.wav" ;
            CAudioFileIf::create(phOnesPPMAudioFile);
            phOnesPPMAudioFile->openFile(sOnesPPMFilePath, CAudioFileIf::kFileRead);
            if (!phOnesPPMAudioFile->isOpen())
            {
                std::cout << "Wave file open error!";
            }
            phOnesPPMAudioFile->getFileSpec(stOnesPPMAudioFileSpec);
            
            
            sPulseAudioFilePath = "Pulse.wav" ;
            CAudioFileIf::create(phPulseAudioFile);
            phPulseAudioFile->openFile(sPulseAudioFilePath, CAudioFileIf::kFileRead);
            if (!phPulseAudioFile->isOpen())
            {
                std::cout << "Wave file open error!";
            }
            phPulseAudioFile->getFileSpec(sPulseAudioFileSpec);
            
            sPulsePPMAudioFilePath = "Pulse_PPM.wav" ;
            CAudioFileIf::create(phPulsePPMAudioFile);
            phPulsePPMAudioFile->openFile(sPulsePPMAudioFilePath, CAudioFileIf::kFileRead);
            if (!phPulsePPMAudioFile->isOpen())
            {
                std::cout << "Wave file open error!";
            }
            phPulsePPMAudioFile->getFileSpec(sPulsePPMAudioFileSpec);
            
            sSinAudioFilePath = "Sin.wav" ;
            CAudioFileIf::create(phSinAudioFile);
            phSinAudioFile->openFile(sSinAudioFilePath, CAudioFileIf::kFileRead);
            if (!phSinAudioFile->isOpen())
            {
                std::cout << "Wave file open error!";
            }
            phSinAudioFile->getFileSpec(sSinAudioFileSpec);
            
            sSinPPMAudioFilePath= "Sin_PPM.wav" ;
            CAudioFileIf::create(phSinPPMAudioFile);
            phSinPPMAudioFile->openFile(sSinPPMAudioFilePath, CAudioFileIf::kFileRead);
            if (!phSinPPMAudioFile->isOpen())
            {
                std::cout << "Wave file open error!";
            }
            phSinPPMAudioFile->getFileSpec(sSinPPMAudioFileSpec);
            
            
            
            
        }
        
        
        ~PPMData()
        {
            
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
                delete [] m_ppfMatlabData[i];
            }
            delete [] m_ppfInputTmp;
            delete [] m_ppfMatlabData;
            delete [] m_ppfOutputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;
            
            
            CPPM::destroyInstance(m_pPPM);
            
            CAudioFileIf::destroy(phSinPPMAudioFile);
            CAudioFileIf::destroy(phSinAudioFile);
            CAudioFileIf::destroy(phPulsePPMAudioFile);
            CAudioFileIf::destroy(phPulseAudioFile);
            CAudioFileIf::destroy(phOnesPPMAudioFile);
            CAudioFileIf::destroy(phOnesAudioFile);
            
            
            
            
        }
        
        
        void TestProcess()
        {
            
            
            //std::cout << "BLock length " << m_iBlockLength << "\n" << std::endl;
            
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);
                
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c]   = &m_ppfOutputData[c][m_iDataLength - iNumFramesRemaining];
                    
                }
                m_pPPM->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }
        }
        
        
        CPPM *m_pPPM;
        
        float  **m_ppfInputData,
        **m_ppfOutputData,
        **m_ppfMatlabData,
        **m_ppfOutputTmp,
        **m_ppfInputTmp,
        m_pfOutput,
        *m_pfIR;
        std::string  sOnesFilePath,sOnesPPMFilePath, sPulseAudioFilePath, sPulsePPMAudioFilePath, sSinAudioFilePath, sSinPPMAudioFilePath ;
        CAudioFileIf            *phOnesAudioFile,*phOnesPPMAudioFile,
        *phPulseAudioFile, *phPulsePPMAudioFile, *phSinAudioFile, *phSinPPMAudioFile;
        CAudioFileIf::FileSpec_t sOnesAudioFileSpec,stOnesPPMAudioFileSpec,
        sPulseAudioFileSpec, sPulsePPMAudioFileSpec, sSinAudioFileSpec, sSinPPMAudioFileSpec;
        
        int     m_iDataLength = 100;
        int     m_iBlockLength=10;
        int     m_iNumChannels = 1;
        float   m_fSampleRate = 44100;
        
    };
    
    
    TEST_FIXTURE(PPMData, ZeroInputTest)
    {
        // Send zeros as input into the peak meter, verify if output remains zero.
        
        m_pPPM->initInstance( m_fSampleRate,  m_iNumChannels, .01, 1.5);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CVectorFloat::setZero(m_ppfInputData[i], m_iDataLength);
            
        }
        
        TestProcess();
        
        for (int i = 0; i < m_iDataLength ; i++)
        {
            for (int c= 0; c<m_iNumChannels; c++)
            {
                CHECK_CLOSE(m_ppfInputData[c][i], m_ppfOutputData[c][i], 1e-3F);
                CHECK_CLOSE(0, m_ppfOutputData[c][i], 1e-3F);
            }
        }
        
    }
    
    
    
    
    TEST_FIXTURE(PPMData, AttackTimeTest)
    {
        //Send in test signal of one's to determine attack time calculations veracity.
        m_iNumChannels =1;
        
        m_fSampleRate = 2000;
        m_pPPM->initInstance( m_fSampleRate,  m_iNumChannels, .01, .01);
        m_iDataLength = 100;
        
        for (int c = 0; c < m_iNumChannels; c++)
        {
            CVectorFloat::setZero(m_ppfInputData[c], m_iDataLength);
            CVectorFloat::setZero(m_ppfOutputData[c], m_iDataLength);
        }
        
        long long iFrames = m_iDataLength;
        phOnesAudioFile->readData(m_ppfInputData, iFrames);
        phOnesPPMAudioFile->readData(m_ppfMatlabData, iFrames);
        
        TestProcess();
        
        for (int i = 0; i < m_iDataLength ; i++)
        {
            
            for (int c= 0; c<m_iNumChannels; c++)
            {
                
                CHECK_CLOSE(m_ppfMatlabData[c][i], m_ppfOutputData[c][i], 1e-3F);
                
            }
        }
        
    }
    
    TEST_FIXTURE(PPMData, ReleaseTimeTest)
    {
        //Send in test signal of a pulse to test attack time calculations veracity.
        m_iNumChannels =1;
        
        m_fSampleRate = 2000;
        m_pPPM->initInstance( m_fSampleRate,  m_iNumChannels, .01, .01);
        m_iDataLength = 100;
        
        for (int c = 0; c < m_iNumChannels; c++)
        {
            CVectorFloat::setZero(m_ppfInputData[c], m_iDataLength);
            CVectorFloat::setZero(m_ppfOutputData[c], m_iDataLength);
        }
        
        long long iFrames = m_iDataLength;
        phPulseAudioFile->readData(m_ppfInputData, iFrames);
        phPulsePPMAudioFile->readData(m_ppfMatlabData, iFrames);
        
        TestProcess();
        
        for (int i = 0; i < m_iDataLength ; i++)
        {
            
            for (int c= 0; c<m_iNumChannels; c++)
            {
                
                CHECK_CLOSE(m_ppfMatlabData[c][i], m_ppfOutputData[c][i], 1e-3F);
                //std::cout<< "Matlab "<< m_ppfMatlabData[0][i] << " " << "C++ : " << m_ppfOutputData[0][i] << "\n" <<std::endl;
                
            }
        }
        
    }
    TEST_FIXTURE(PPMData, BlockSizeTest)
    {
        //Run three different test cases to test robustness to block length changes
        int *blockSizes;
        blockSizes = new int[3];
        blockSizes[0] = 30;
        blockSizes[1] = 10;
        blockSizes[2] = 60;
        m_fSampleRate = 2000;
        m_iNumChannels =1;
        m_iDataLength = 100;
        long long iFrames = m_iDataLength;
        phSinAudioFile->readData(m_ppfInputData, iFrames);
        phSinPPMAudioFile->readData(m_ppfMatlabData, iFrames);
        
        for(int j = 0; j < 3; j++){
            m_iBlockLength = blockSizes[j];
            if (m_iBlockLength>m_iDataLength)
                std::cout<< "Block length greater than Data Length \n" << std::endl;
            
            
            m_pPPM->initInstance( m_fSampleRate,  m_iNumChannels, .01, .01);
            
            for (int c = 0; c < m_iNumChannels; c++)
            {
                CVectorFloat::setZero(m_ppfOutputData[c], m_iDataLength);
            }
            
            TestProcess();
            
            for (int i = 0; i < m_iDataLength ; i++)
            {
                
                for (int c= 0; c<m_iNumChannels; c++)
                {
                    
                    CHECK_CLOSE(m_ppfMatlabData[c][i], m_ppfOutputData[c][i], 1e-3F);
                    
                }
            }
        }
        
        delete [] blockSizes;
        
    }
    TEST_FIXTURE(PPMData, SinWaveTest)
    {
        //Run sin wave to determine if algo can evaluate recurring peaks and troughs
        
        m_fSampleRate = 2000;
        m_iNumChannels =1;
        m_iDataLength = 100;
        long long iFrames = m_iDataLength;
        phSinAudioFile->readData(m_ppfInputData, iFrames);
        phSinPPMAudioFile->readData(m_ppfMatlabData, iFrames);
        
            m_iBlockLength = 10;
            if (m_iBlockLength>m_iDataLength)
                std::cout<< "Block length greater than Data Length \n" << std::endl;
            
        
            m_pPPM->initInstance( m_fSampleRate,  m_iNumChannels, .01, .01);
            
            for (int c = 0; c < m_iNumChannels; c++)
            {
                CVectorFloat::setZero(m_ppfOutputData[c], m_iDataLength);
            }
            
            TestProcess();
            
            for (int i = 0; i < m_iDataLength ; i++)
            {
                
                for (int c= 0; c<m_iNumChannels; c++)
                {
                    
                    CHECK_CLOSE(m_ppfMatlabData[c][i], m_ppfOutputData[c][i], 1e-3F);
                    
                }
            }
        }
    
    TEST_FIXTURE(PPMData, MaxPPMTest)
    {
        //Find max PPM value in the entire channel
        
        m_fSampleRate = 2000;
        m_iNumChannels =1;
        m_iDataLength = 100;
        long long iFrames = m_iDataLength;
        phSinAudioFile->readData(m_ppfInputData, iFrames);
        phSinPPMAudioFile->readData(m_ppfMatlabData, iFrames);
        
        m_iBlockLength = 10;
        if (m_iBlockLength>m_iDataLength)
            std::cout<< "Block length greater than Data Length \n" << std::endl;
        m_pPPM->initInstance( m_fSampleRate,  m_iNumChannels, .01, .01);
        
        for (int c = 0; c < m_iNumChannels; c++)
        {
            CVectorFloat::setZero(m_ppfOutputData[c], m_iDataLength);
        }
        
        TestProcess();
        
        float max=m_pPPM->getPPMChannelValue(0);
      
                
                CHECK_CLOSE(0.8415, max, 1e-3F);
                //max ppm value compared with value calculated in MATLAB
                
        
        
    }
    
    
    
    
    
    
    
    
    
    
}

#endif //WITH_TESTS
