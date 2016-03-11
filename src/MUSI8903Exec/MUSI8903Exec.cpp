
#include <iostream>
#include <ctime>

#include "MUSI8903Config.h"

#include "AudioFileIf.h"

#include "FastConv.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath,
                            sIrFilePath;

    static const int        kBlockSize          = 1024;

    clock_t                 time                = 0;

    float                   **ppfAudioData      = 0;
    
    float                   **ppfAudioDataOut   = 0;
    
    float                   **ppfIrData             = 0;

    CAudioFileIf            *phAudioFile        = 0;
    
    CAudioFileIf            *phIrFile           = 0;
    
    int                     iConvBlockLength    = 64;
    
    long long                     iIrLength           = 0;
    
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;
    
    CAudioFileIf::FileSpec_t stIrFileSpec;
    
    CFastConv               *pCFastConv;
    

    showClInfo ();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 3)
    {
        return -1;
    }
    else
    {
        sInputFilePath  = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
        sIrFilePath = argv[2];
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);
    
    CAudioFileIf::create(phIrFile);
    phIrFile->openFile(sIrFilePath, CAudioFileIf::kFileRead);
    if (!phIrFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phIrFile->getFileSpec(stIrFileSpec);


    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open (sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData            = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float [kBlockSize];
    
    ppfAudioDataOut         = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOut[i] = new float [kBlockSize];
    
    CFastConv::create(pCFastConv);

    time                    = clock();
    //////////////////////////////////////////////////////////////////////////////
    
    phIrFile->getLength(iIrLength);
    
    phIrFile->readData(ppfIrData, iIrLength);
    
    ppfIrData               = new float* [stIrFileSpec.iNumChannels];
    for (int i = 0; i < stIrFileSpec.iNumChannels; i++)
        ppfIrData[i] = new float [iIrLength];
    
    pCFastConv->init(ppfIrData[0], iIrLength, iConvBlockLength);
    
///////////////////////////////////
    
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);

        for (int i = 0; i < iNumFrames; i++)
        {
            pCFastConv->process(ppfAudioData[0], ppfAudioDataOut[0], iNumFrames,false);

            hOutputFile << ppfAudioData[0][i] << "\t";
            
            hOutputFile << endl;
   
        }
    }

    cout << "reading/writing done in: \t"    << (clock()-time)*1.F/CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    
    CFastConv::destroy(pCFastConv);
    
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfAudioData[i];
    delete [] ppfAudioData;
    ppfAudioData = 0;
    
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfAudioDataOut[i];
    delete [] ppfAudioDataOut;
    ppfAudioDataOut = 0;

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfIrData[i];
    delete [] ppfIrData;
    ppfIrData = 0;

    return 0;
    
}


void     showClInfo()
{
    cout << "GTCMT MUSI8903" << endl;
    cout << "(c) 2016 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

