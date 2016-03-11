
#include "Vector.h"
#include "Util.h"
#include <iostream>
#include <math.h>

#include "FastConv.h"

CFastConv::CFastConv( void )
{
    reset();
}

CFastConv::~CFastConv( void )
{
    reset();
    deallocate();
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

	delete pCFastConv;

	pCFastConv = 0;

	return kNoError;

}

Error_t CFastConv::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength /*= 8192*/)
{
	m_pfImpulseResponse = pfImpulseResponse;
    
	m_iLengthOfIr = iLengthOfIr;
    if(iBlockLength!=64){
        std::cout<< "MMY PROBLEM IS HERE" << std::endl;}
    m_iBlockLength = iBlockLength;

    allocate();
    
  
    //m_pCFft->init(iBlockLength);
    
    std::cout << "IR len init " << m_iLengthOfIr << std::endl;
    std::cout << "blok len init " << m_iBlockLength << std::endl;

    return kNoError;
}

void CFastConv::allocate() {
    m_pfTailBuffer = new float[m_iLengthOfIr-1];
    CVectorFloat::setZero(m_pfTailBuffer, m_iLengthOfIr-1);
    
    m_pfInputTemp = new float[m_iBlockLength];
    CVectorFloat::setZero(m_pfInputTemp, m_iBlockLength);
    
    m_pfIrTemp = new float[m_iBlockLength];
    CVectorFloat::setZero(m_pfIrTemp, m_iBlockLength);
    
    m_pfOutputTemp = new float[m_iBlockLength];
    CVectorFloat::setZero(m_pfOutputTemp, m_iBlockLength );
    
    CFft::create(m_pCFft);
    
    m_pcInputSpectrum = new CFft::complex_t[m_iBlockLength];
    m_pcIrSpectrum = new CFft::complex_t[m_iBlockLength];
    m_pcResultSpectrum = new CFft::complex_t[m_iBlockLength];
}

void CFastConv::deallocate() {
    
    if(m_pfTailBuffer){
        delete [] m_pfTailBuffer;
        m_pfTailBuffer = 0;
    }
    
    if(m_pfInputTemp){
        delete [] m_pfInputTemp;
        m_pfInputTemp = 0;
    }
    
    if(m_pfIrTemp){
        delete [] m_pfIrTemp;
        m_pfIrTemp = 0;
    }
    if(m_pfOutputTemp){
        delete [] m_pfOutputTemp;
        m_pfOutputTemp=0;
    }
    
    if(m_pcInputSpectrum){
        delete [] m_pcInputSpectrum;
        m_pcInputSpectrum = 0;
    }
    if(m_pcIrSpectrum){
        delete [] m_pcIrSpectrum;
        m_pcIrSpectrum = 0;
    }
    if(m_pcResultSpectrum){
        delete [] m_pcResultSpectrum;
        m_pcResultSpectrum = 0;
    }
    
    if(m_pCFft){
        CFft::destroy(m_pCFft);
    }
}


Error_t CFastConv::reset()
{

     m_iLengthOfIr = 0;
     m_iBlockLength = 0;
     m_iNumInputBlocks = 0;
     m_iNumIrBlocks = 0;
    
     m_iNumInputZeros = 0;
     m_iNumIrZeros = 0;
	 m_pfImpulseResponse = 0;

	 m_iLengthOfIr = 0;

     m_iBlockLength = 0;

    return kNoError;
}

Error_t CFastConv::process (float *pfInputBuffer, float *pfOutputBuffer, int iLengthOfBuffers, bool isFast )
{
	if (!isFast) {
		processTimeDomain(pfInputBuffer, pfOutputBuffer, iLengthOfBuffers);
	}
    else{
        
        m_iNumInputBlocks = ceil((double)iLengthOfBuffers / m_iBlockLength) ;
        m_iNumInputZeros = (m_iBlockLength * m_iNumInputBlocks) - iLengthOfBuffers;
        
        m_iNumIrBlocks = ceil((double)m_iLengthOfIr / m_iBlockLength) ;
        m_iNumIrZeros = (m_iBlockLength * m_iNumIrBlocks) - m_iLengthOfIr;
        
        for (int i = 0; i < m_iLengthOfIr -1; i++){
            pfOutputBuffer[i] += m_pfTailBuffer[i];
            //std::cout << "tail buff "<<m_pfTailBuffer[i]<< std::endl;
            m_pfTailBuffer[i] = 0;
        }
        
        //Input blocking
        for(int i = 0; i  < m_iNumInputBlocks; i++)
        {
            if (i == m_iNumInputBlocks - 1) {
                //m_pfInputTemp = &pfInputBuffer[i * m_iBlockLength]; //OUR ERROR IS HERE
                //CVectorFloat::setZero(&m_pfInputTemp[m_iBlockLength - m_iNumInputZeros], m_iNumInputZeros); //AND HERE
                
                for (int z = 0; z < m_iBlockLength; z++){
                    m_pfInputTemp[z] = pfInputBuffer[(i * m_iBlockLength) + z];
                }
                CVectorFloat::setZero(&m_pfInputTemp[m_iBlockLength - m_iNumInputZeros], m_iNumInputZeros);
                
                
                // std::cout <<  "Block number "<< i << std::endl;
                //  for (int z = 0; z < m_iBlockLength; z++){
                //   std::cout << m_pfInputTemp[z] << std::endl;
                // }
            }
            else {
                for (int z = 0; z < m_iBlockLength; z++){
                    m_pfInputTemp[z] = pfInputBuffer[(i * m_iBlockLength) + z];
                }
            }
            
            //INPUT FFT
            m_pCFft->doFft(m_pcInputSpectrum, m_pfInputTemp);
            
            //IR blocking
            for(int j = 0; j  < m_iNumIrBlocks; j++)
            {
                if (j == m_iNumIrBlocks - 1) {
                    //  m_pfIrTemp = &m_pfImpulseResponse[j * m_iBlockLength];
                    // CVectorFloat::setZero(&m_pfIrTemp[m_iBlockLength - m_iNumIrZeros], m_iNumIrZeros);
                    
                    for (int z = 0; z < m_iBlockLength; z++){
                        m_pfIrTemp[z] = m_pfImpulseResponse[(j * m_iBlockLength) + z];
                    }
                    CVectorFloat::setZero(&m_pfIrTemp[m_iBlockLength - m_iNumIrZeros], m_iNumIrZeros);
                    
                }
                else {
                    for (int z = 0; z < m_iBlockLength; z++){
                        m_pfIrTemp[z] = m_pfImpulseResponse[(j * m_iBlockLength) + z];
                    }
                }
                
                //IR FFT
                m_pCFft->doFft(m_pcIrSpectrum, m_pfIrTemp);
                
                //CONV
                CVectorFloat::copy(m_pcResultSpectrum, m_pcInputSpectrum, m_iBlockLength);
                m_pCFft->mulCompSpectrum(m_pcResultSpectrum, m_pcIrSpectrum);
                
                /*
                if (((m_iBlockLength * j) + (m_iBlockLength * i)) >= iLengthOfBuffers){
                    m_pCFft->doInvFft(m_pfOutputTemp, m_pcResultSpectrum);
                    CVectorFloat::copy(&m_pfTailBuffer[(m_iBlockLength * j) + (m_iBlockLength * i)], m_pfOutputTemp, m_iBlockLength);
                }
                else{
                    m_pCFft->doInvFft(m_pfOutputTemp, m_pcResultSpectrum);
                    CVectorFloat::copy(&pfOutputBuffer[(m_iBlockLength * j) + (m_iBlockLength * i)], m_pfOutputTemp, m_iBlockLength);
                }
                 */
                
                m_pCFft->doInvFft(m_pfOutputTemp, m_pcResultSpectrum);
                
                std::cout <<m_iBlockLength + m_iLengthOfIr-1 << std::endl;
                
                for (int x = 0; x < m_iBlockLength * 2; x++){
                    if ( ((m_iBlockLength * j) + (m_iBlockLength * i) + x) >= iLengthOfBuffers){
                        m_pfTailBuffer[((m_iBlockLength * j) + (m_iBlockLength * i) + x) - iLengthOfBuffers] = m_pfOutputTemp[x];
                    }
                    else{
                        pfOutputBuffer[((m_iBlockLength * j) + (m_iBlockLength * i) + x)] = m_pfOutputTemp[x];
                    }
                }
                
            }
        }
        
    }
	
    
    return kNoError;
}

Error_t CFastConv::processTimeDomain(float *pfInputBuffer, float *pfOutputBuffer, int iLengthOfBuffers)
{

    m_iNumInputBlocks = ceil((double)iLengthOfBuffers / m_iBlockLength) ;
    m_iNumInputZeros = (m_iBlockLength * m_iNumInputBlocks) - iLengthOfBuffers;
    
    if(m_iBlockLength!=64)
        std::cout<< "m_iBlockLength got changed " <<std::endl;
    
    m_iNumIrBlocks = ceil((double)m_iLengthOfIr / m_iBlockLength) ;
    m_iNumIrZeros = (m_iBlockLength * m_iNumIrBlocks) - m_iLengthOfIr;
    
    //std::cout <<  "block len "<< m_iBlockLength << std::endl;
    //std::cout <<  "buff len "<< iLengthOfBuffers << std::endl;

    //std::cout <<  "num Inputblox "<< m_iNumInputBlocks << std::endl;
    //std::cout <<  "num Inputzeros "<< m_iNumInputZeros << std::endl;
    
    //std::cout <<  "num IRblox "<< m_iNumIrBlocks << std::endl;
    //std::cout <<  "num IRzeros "<< m_iNumIrZeros << std::endl;

    
    //std::cout <<  "ir len "<< m_iLengthOfIr -1 << std::endl;
    
    for (int i = 0; i < m_iLengthOfIr -1; i++){
        pfOutputBuffer[i] += m_pfTailBuffer[i];
        //std::cout << "tail buff "<<m_pfTailBuffer[i]<< std::endl;
        m_pfTailBuffer[i] = 0;
    }
    
    //std::cout <<  "in buff  "<< std::endl;
   // for (int z = 0; z < iLengthOfBuffers; z++){
       // std::cout << pfInputBuffer[z] << std::endl;
   // }

    
    //Input blocking
    for(int i = 0; i  < m_iNumInputBlocks; i++)
    {
        if (i == m_iNumInputBlocks - 1) {
            //m_pfInputTemp = &pfInputBuffer[i * m_iBlockLength];
            //CVectorFloat::setZero(&m_pfInputTemp[m_iBlockLength - m_iNumInputZeros], m_iNumInputZeros);
            
            for (int z = 0; z < m_iBlockLength; z++){
                m_pfInputTemp[z] = pfInputBuffer[(i * m_iBlockLength) + z];
            }
            CVectorFloat::setZero(&m_pfInputTemp[m_iBlockLength - m_iNumInputZeros], m_iNumInputZeros);
            
            
           // std::cout <<  "Block number "<< i << std::endl;
          //  for (int z = 0; z < m_iBlockLength; z++){
             //   std::cout << m_pfInputTemp[z] << std::endl;
           // }
        }
        else {
            for (int z = 0; z < m_iBlockLength; z++){
                m_pfInputTemp[z] = pfInputBuffer[(i * m_iBlockLength) + z];
            }
        }
        
  
        //IR blocking
        for(int j = 0; j  < m_iNumIrBlocks; j++)
        {
            if (j == m_iNumIrBlocks - 1) {
              //  m_pfIrTemp = &m_pfImpulseResponse[j * m_iBlockLength];
               // CVectorFloat::setZero(&m_pfIrTemp[m_iBlockLength - m_iNumIrZeros], m_iNumIrZeros);
                
                for (int z = 0; z < m_iBlockLength; z++){
                    m_pfIrTemp[z] = m_pfImpulseResponse[(j * m_iBlockLength) + z];
                }
                CVectorFloat::setZero(&m_pfIrTemp[m_iBlockLength - m_iNumIrZeros], m_iNumIrZeros);
                
            }
            else {
                for (int z = 0; z < m_iBlockLength; z++){
                    m_pfIrTemp[z] = m_pfImpulseResponse[(j * m_iBlockLength) + z];
                }
            }
            
            for (int k = 0; k < 2 * m_iBlockLength; k++){
                for (int l = 0; l < m_iBlockLength; l++){
                    if(k - l < m_iBlockLength && k - l >= 0){
                        if ((k + (m_iBlockLength * j) + (m_iBlockLength * i)) >= iLengthOfBuffers){
                            
                            m_pfTailBuffer[((k + (m_iBlockLength * j) + (m_iBlockLength * i))) - iLengthOfBuffers] = m_pfTailBuffer[((k + (m_iBlockLength * j) + (m_iBlockLength * i))) - iLengthOfBuffers] + (m_pfInputTemp[l] * m_pfIrTemp[k-l]);
                           // std::cout << "tail buff "<<m_pfTailBuffer[((k + (m_iBlockLength * j) + (m_iBlockLength * i)))]<< std::endl;

                        }
                        else{
                           // std::cout << k + (m_iBlockLength * j) + (m_iBlockLength * i)<< "   " << pfOutputBuffer[k + (m_iBlockLength * j)+ (m_iBlockLength * i)]<< std::endl;
                            //std::cout << (m_pfInputTemp[l] * m_pfIrTemp[k-l]) << std::endl;
                            pfOutputBuffer[k + (m_iBlockLength * j) + (m_iBlockLength * i)] = pfOutputBuffer[k + (m_iBlockLength * j) + (m_iBlockLength * i)] + (m_pfInputTemp[l] * m_pfIrTemp[k-l]);
                        }
                    }

                }
                

            }
            
        }
        
        
    }
    
    
    
    return kNoError;
}

Error_t CFastConv::flushBuffer(float *pfReverbTail) {

	for (int i = 0; i < m_iLengthOfIr-1; i++) {
        pfReverbTail[i] = m_pfTailBuffer[i];
       std::cout<< "FlushBuffer "<< pfReverbTail[i]<< std::endl;
        m_pfTailBuffer[i] = 0;
	}
    

	return kNoError;
}

