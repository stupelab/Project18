#include "AudioManager.h"


AudioManager::AudioManager(){}
AudioManager::~AudioManager(){}

void AudioManager::setup(){
    
    //Set buffer size and fill it by zeros
    buffer.resize( N, 0.0 );
    
    //Start the sound output in stereo (2 channels)
    //and sound input in mono (1 channel)
    
    left = new float[N];
    right = new float[N];
    
    left2 = new float[N];
    right2 = new float[N];
    
    
    c1.printDeviceList();
    c1.setDeviceID(2); // in
    c1.setup(0, 2, sampleRate, N, 4);
    c1.setInput(this);
    
    c2.printDeviceList();
    c2.setDeviceID(1); // out
    c2.setup(2, 0, sampleRate, N, 4);
    c2.setOutput(this);
  
    
    soundBuffer.allocate(N, 2);
    audioAnalyzer.setup(sampleRate, N,  2);
    
    
    bool isAnalyzing = true; //Audio Analyzer true by default
}

//--------------------------------------------------------------
void AudioManager::audioIn( float *input, int bufferSize, int nChannels ){
    
    // samples are "interleaved"
    for (int i = 0; i < 256; i++){
        
        left[i] = input[i*2];
        right[i] = input[i*2+1];
    }
    
    ofSoundBuffer buf;
    buf.copyFrom(input, 256, 2, 44100);
    audioAnalyzer.analyze(buf);
    
    
}

//Audio output
void AudioManager::audioOut(float *output, int bufferSize, int nChannels)
{
    
    for (int i=0; i<bufferSize; i++) {
        //Push current audio sample value from buffer
        //into both channels of output.
        //Also global volume value is used
        output[ 2*i ] = left[i] * 0.5;
        output[ 2*i + 1 ] = right[i] * 0.5;
        //Shift to the next audio sample
        //playPos++;
        //When the end of buffer is reached, playPos sets to 0
        //So we hear looped sound
        //playPos %= N;
    }
    
}

void AudioManager::updateDescriptors(){

    //-:get Values:
    descriptors.rms     = audioAnalyzer.getValue(RMS, 0, smoothing);
   /* descriptors.power   = audioAnalyzer.getValue(POWER, 0, smoothing);
    descriptors.pitchFreq = audioAnalyzer.getValue(PITCH_FREQ, 0, smoothing);
    descriptors.pitchConf = audioAnalyzer.getValue(PITCH_CONFIDENCE, 0, smoothing);
    descriptors.pitchSalience  = audioAnalyzer.getValue(PITCH_SALIENCE, 0, smoothing);
    descriptors.inharmonicity   = audioAnalyzer.getValue(INHARMONICITY, 0, smoothing);
    descriptors.hfc = audioAnalyzer.getValue(HFC, 0, smoothing);
    descriptors.specComp = audioAnalyzer.getValue(SPECTRAL_COMPLEXITY, 0, smoothing);
    descriptors.centroid = audioAnalyzer.getValue(CENTROID, 0, smoothing);
    descriptors.rollOff = audioAnalyzer.getValue(ROLL_OFF, 0, smoothing);
    descriptors.oddToEven = audioAnalyzer.getValue(ODD_TO_EVEN, 0, smoothing);
    descriptors.strongPeak = audioAnalyzer.getValue(STRONG_PEAK, 0, smoothing);
    descriptors.strongDecay = audioAnalyzer.getValue(STRONG_DECAY, 0, smoothing);
    //Normalized values for graphic meters:
    descriptors.pitchFreqNorm   = audioAnalyzer.getValue(PITCH_FREQ, 0, smoothing, TRUE);
    descriptors.hfcNorm     = audioAnalyzer.getValue(HFC, 0, smoothing, TRUE);
    descriptors.specCompNorm = audioAnalyzer.getValue(SPECTRAL_COMPLEXITY, 0, smoothing, TRUE);
    descriptors.centroidNorm = audioAnalyzer.getValue(CENTROID, 0, smoothing, TRUE);
    descriptors.rollOffNorm  = audioAnalyzer.getValue(ROLL_OFF, 0, smoothing, TRUE);
    descriptors.oddToEvenNorm   = audioAnalyzer.getValue(ODD_TO_EVEN, 0, smoothing, TRUE);
    descriptors.strongPeakNorm  = audioAnalyzer.getValue(STRONG_PEAK, 0, smoothing, TRUE);
    descriptors.strongDecayNorm = audioAnalyzer.getValue(STRONG_DECAY, 0, smoothing, TRUE);
    
    descriptors.dissonance = audioAnalyzer.getValue(DISSONANCE, 0, smoothing);
    
    descriptors.spectrum = audioAnalyzer.getValues(SPECTRUM, 0, smoothing);
    descriptors.melBands = audioAnalyzer.getValues(MEL_BANDS, 0, smoothing);
    descriptors.mfcc = audioAnalyzer.getValues(MFCC, 0, smoothing);
    descriptors.hpcp = audioAnalyzer.getValues(HPCP, 0, smoothing);
    
    descriptors.tristimulus = audioAnalyzer.getValues(TRISTIMULUS, 0, smoothing);
    
    descriptors.isOnset = audioAnalyzer.getOnsetValue(0);*/

}

