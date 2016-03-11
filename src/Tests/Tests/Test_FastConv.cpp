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
        FastConvData()
        {
            
            m_iPaddedInputLength = int(ceil((double)m_iDataLength/m_iInputBufferBlockLength) * m_iInputBufferBlockLength);
            
            m_pfInputData = new float [m_iPaddedInputLength];
            CVectorFloat::setZero(m_pfInputData, m_iPaddedInputLength);

            m_pfOutputData = new float [m_iDataLength+m_iIRLength-1];
            CVectorFloat::setZero(m_pfOutputData, m_iDataLength+m_iIRLength-1);

            m_pfIR = new float [m_iIRLength];
            CVectorFloat::setZero( m_pfIR, m_iIRLength);
            
            CFastConv::create(m_pCFastConv);
            
            m_piBlockSizes = new int [m_iNumBlockSizes];
            m_piBlockSizes[0] = 1;
            m_piBlockSizes[1] = 13;
            m_piBlockSizes[2] = 17;
            m_piBlockSizes[3] = 1023;
            m_piBlockSizes[4] = 1897;
            m_piBlockSizes[5] = 2048;
            m_piBlockSizes[6] = 5000;
			
        }

        
        ~FastConvData()
        {
            CFastConv::destroy(m_pCFastConv);
            
			delete [] m_pfInputData;
            m_pfInputData = 0;
			delete [] m_pfOutputData;
            m_pfOutputData = 0;
            delete [] m_pfIR;
            m_pfIR = 0;
        
            m_iPaddedInputLength =0;
          
     
			
        }
		
		
        void TestProcess()
        {

            
            int iNumFramesRemaining = m_iDataLength;
            //Zero pad Input Buffer if data length is smaller than buffer block length
          /* if (m_iDataLength < m_iInputBufferBlockLength){
                m_iNumBufferZeros = m_iInputBufferBlockLength - m_iDataLength;
                CVectorFloat::setZero(&m_pfInputData[m_iInputBufferBlockLength - m_iNumBufferZeros], m_iNumBufferZeros);
            }
           */
            
           // CVectorFloat::setZero(m_pfOutputData, m_iDataLength);

            
            
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iInputBufferBlockLength);
                
                //int iNumFrames = m_iInputBufferBlockLength;
                
                
                m_pfInputDataTemp = &m_pfInputData[m_iDataLength - iNumFramesRemaining];
                m_pfOutputDataTemp = &m_pfOutputData[m_iDataLength - iNumFramesRemaining];
                
                //std::cout << "offset" << m_iDataLength - iNumFramesRemaining << std::endl;
               // std::cout << "offset" << m_pfInputData[m_iDataLength - iNumFramesRemaining] << std::endl;

               // std::cout << "Input Data" << std::endl;
                //for (int z = 0; z < m_iDataLength; z++){
                  //  std::cout << m_pfInputData[z] << std::endl;
                //}
                
               // std::cout << "iNumFramesRemaining " << iNumFramesRemaining << std::endl;
               // for (int z = 0; z < iNumFrames; z++){
                  //  std::cout << m_pfInputDataTemp[z] << std::endl;
               // }


                m_pCFastConv->process(m_pfInputDataTemp, m_pfOutputDataTemp, iNumFrames,false);
                
               // std::cout << "out temp " << iNumFramesRemaining << std::endl;
              //  for (int z = 0; z < iNumFrames; z++){
                 //   std::cout << m_pfOutputDataTemp[z] << std::endl;
               // }

                
                iNumFramesRemaining -= iNumFrames;
                
            }
            /*
            std::cout << " Output Data before Flushbuff " << std::endl;

            for (int z = 0; z < m_iDataLength+m_iIRLength-1; z++){
              //  std::cout << m_pfOutputData[z] << std::endl;
            }
            std::cout << " End Output Data before Flushbuff " << std::endl;
             */
            
            m_pCFastConv->flushBuffer(&m_pfOutputData[m_iDataLength]);


        }
		
        
        CFastConv *m_pCFastConv;

		float *m_pfInputData,
			*m_pfOutputData,
            *m_pfInputDataTemp,
            *m_pfOutputDataTemp,
            *m_pfTailBuffer,
			*m_pfIR;
        int m_iIRLength = 51,
            m_iDataLength = 1000,
            m_iInputBufferBlockLength = 40,
            m_iConvBlockLength = 64,
            *m_piBlockSizes,
            m_iNumBlockSizes = 7,
            m_iNumBufferZeros = 0,
            m_iPaddedInputLength = 0;
			
    };


	TEST_FIXTURE(FastConvData, IrTest)
	{
		// identity: generate a random IR (length 51 s), set it (init) and check the the output of a delayed (5 samples) impulse is the shifted IR.
        
        m_iIRLength = 51;
        
        m_iDataLength = 45;

		int iDelayInSamples = 5;
        CVectorFloat::setZero(m_pfOutputData, m_iDataLength+m_iIRLength-1);
		CVectorFloat::setZero(m_pfInputData, m_iPaddedInputLength);
		m_pfInputData[iDelayInSamples-1] = 1;

		for (int i = 0; i < m_iIRLength; i++) {
			m_pfIR[i] = ((float)rand() / (float)(RAND_MAX) * 2.0F - 1.F) ;
          // std::cout << "ir " << m_pfIR[i] << std::endl;
		}
        /*
        for (int i = 0; i < m_iDataLength; i++) {
            std::cout << "InputData " << m_pfInputData[i] << std::endl;
        }
        
        std::cout << "IR len " << m_iIRLength << std::endl;
        std::cout << "conv blox lin " << m_iConvBlockLength << std::endl;
        

         */

		m_pCFastConv->init(m_pfIR, m_iIRLength, m_iConvBlockLength );
       // std::cout<<m_pCFastConv->m_iBlockLength<<std::endl;
		TestProcess();
        
    
		for (int i = 0; i < m_iDataLength + m_iIRLength -1; i++)
		{
            
            if (i < m_iIRLength + iDelayInSamples-1 && i >= iDelayInSamples-1){
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
        
        m_iDataLength = 100;
    
        m_iIRLength = 51;
        
        int iDelayInSamples = 5;
        
        for (int z = 0; z < m_iNumBlockSizes; z++){
            
            CVectorFloat::setZero(m_pfOutputData, m_iDataLength+m_iIRLength-1);
            CVectorFloat::setZero(m_pfInputData, m_iPaddedInputLength);
            m_pfInputData[iDelayInSamples-1] = 1;
            
            for (int i = 0; i < m_iIRLength; i++) {
                m_pfIR[i] = ((float)rand() / (float)(RAND_MAX) * 2.0F - 1.F) ;
              //  std::cout << "ir " << m_pfIR[i] << std::endl;
            }
            
     
            //for (int i = 0; i < m_iDataLength; i++) {
              //  std::cout << "InputData " << m_pfInputData[i] << std::endl;
            //}
     
            
           // std::cout << "IR len " << m_iIRLength << std::endl;
           // std::cout << "conv blox lin " << m_iConvBlockLength << std::endl;
            
            
            
            m_pCFastConv->init(m_pfIR, m_iIRLength, m_iConvBlockLength );
            // std::cout<<m_pCFastConv->m_iBlockLength<<std::endl;
            TestProcess();
            
            
            for (int i = 0; i < m_iDataLength + m_iIRLength -1; i++)
            {
                
                if (i < m_iIRLength + iDelayInSamples-1 && i >= iDelayInSamples-1){
                  //  std::cout << i << "   " << m_pfIR[i-iDelayInSamples+1] << "   " << m_pfOutputData[i] << std::endl;
                    CHECK_CLOSE(m_pfIR[i-iDelayInSamples+1], m_pfOutputData[i], 1e-3F);
                }
                else{
                  //  std::cout << i << "   " << m_pfOutputData[i] << std::endl;
                    CHECK_CLOSE(0, m_pfOutputData[i], 1e-3F);
                }
                
            }
            
        }

    }

    

}

#endif //WITH_TESTS