
#include <iostream>
#include <ctime>

#include "MUSI8903Config.h"

#include "AudioFileIf.h"

#include "MyProject.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();


//CHANGE depth to seconds. MAYBE (from 0-1)

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize          = 1024;

    clock_t                 time                = 0;

    float                   **ppfAudioData      = 0;

    CAudioFileIf            *phAudioFile        = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    CMyProject              *pMyProject         = 0;
    
    float                   **ppfAudioDataOut   = 0;
    
    Error_t                 error;
    
    float                   LFOFreq             = 0;
    
    float                   LFODepth            = 0;

    showClInfo ();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 4)
    {
        return -1;
    }
    else
    {
        sInputFilePath  = argv[1];
        LFOFreq         = std::stof(argv[2]);
        LFODepth        = std::stof(argv[3]);
        sOutputFilePath = sInputFilePath + ".txt";
        
    }

    if(LFOFreq<0 || LFOFreq>10000)
    {
        std::cout << "Frequency out of range 0-10000 Hz" << endl;
        return -1;

    }
    
    if(LFODepth>1 || LFODepth<0)
    {
        std::cout << "Depth out of range of 0-1" << endl;

        return -1;
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
    
    ppfAudioDataOut            = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOut[i] = new float [kBlockSize];

    time                    = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    CMyProject::create(pMyProject);
    pMyProject->init(stFileSpec.iNumChannels, stFileSpec.fSampleRateInHz,stFileSpec.fSampleRateInHz/50, LFOFreq, LFODepth);
    
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        
        error = pMyProject->process(ppfAudioData, ppfAudioDataOut, iNumFrames);

        for (int i = 0; i < iNumFrames; i++)
        {
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
            {
                hOutputFile << ppfAudioDataOut[c][i] << "\t";
            }
            hOutputFile << endl;
        }
    }

    cout << "reading/writing done in: \t"    << (clock()-time)*1.F/CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    CMyProject::destroy(pMyProject);
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfAudioData[i];
    delete [] ppfAudioData;
    
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfAudioDataOut[i];
    delete [] ppfAudioDataOut;
    ppfAudioDataOut = 0;

    return 0;
    
}


void     showClInfo()
{
    cout << "GTCMT MUSI8903" << endl;
    cout << "(c) 2016 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

