
#include "Vector.h"
#include "Util.h"

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

	m_pfTailBuffer = new float[iLengthOfIr];

    return kNoError;
}

Error_t CFastConv::reset()
{

	m_pfImpulseResponse = 0;

	m_iLengthOfIr = 0;

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
	//process here

	for (int i = 0; i < iLengthOfBuffers+m_iLengthOfIr; i++)
	{
		pfOutputBuffer[i] = 0;
		m_pfTailBuffer[i] = 0;
		for (int j = 0; j < m_iLengthOfIr; j++){
			if ((i - j) >= 0) {
				pfOutputBuffer[i] = pfOutputBuffer[i] + pfInputBuffer[i - j] * m_pfImpulseResponse[j];
			}
			if (i >= iLengthOfBuffers && (i - j) < iLengthOfBuffers && (i - j) >= 0) {
				m_pfTailBuffer[i-iLengthOfBuffers] = m_pfTailBuffer[i - iLengthOfBuffers] + pfInputBuffer[i - j] * m_pfImpulseResponse[j];
			}
		}
	}
	
	return kNoError;
}

Error_t CFastConv::flushBuffer(float *pfOutputBuffer, int iLengthofBuffer) {

	for (int i = 0; i < iLengthofBuffer; i++) {
		if (i < m_iLengthOfIr) {
			pfOutputBuffer[i] = m_pfTailBuffer[i];
		}
		else {
			pfOutputBuffer[i] = 0;
		}
	}

	return kNoError;
}
