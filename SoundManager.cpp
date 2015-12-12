#include "SoundManager.h"


ALCdevice *device;                                                          //Create an OpenAL Device
ALCcontext *context;
ALfloat ListenerOri[] = { 0.0, 0.0, 0.0,  0.0, 1.0, 0.0 }; //Orientation of the listener
int mainMenuSource, hoverSource, playSource , clickSource, themeSource,
	horrorSource1,horrorSource2,horrorSource3,suspenseSource,
	breathSourceFast,gameOverSource;

using namespace std;


int SoundManager::endWithError(char* msg, int error)
{
	//Display error message in console
	cout << msg << "\n";
	cout << "pause" << endl;
	//  system("PAUSE");
	return error;
}


void SoundManager::playMainMenuMusic() {
	playSound(mainMenuSource,1.0,1.0,AL_TRUE);
}

void SoundManager::playThemeMusic() {
	playSound(themeSource, 1.0, 1.0, AL_TRUE);
}
void SoundManager::playHoverSound() {
	playSound(hoverSource, 1.0, 1.0, AL_FALSE);
}
void SoundManager::playClickSound() {
	playSound(clickSource, 1.0, 1.0, AL_FALSE);
}
void SoundManager::playStartSound() {
	playSound(playSource, 1.0, 1.0, AL_FALSE);
}
void SoundManager::playScaredSound() {
	playSound(suspenseSource, 1.0, 1.0, AL_FALSE);
}
void SoundManager::playBreathFastSound() {
	playSound(breathSourceFast, 1.0, 1.0, AL_TRUE);
}

void SoundManager::stopBreathFastSound() {
	alSourceStop(breathSourceFast);
}

void SoundManager::stopThemeMusic() {
	alSourceStop(mainMenuSource);
}

void SoundManager::stopMainMenuMusic() {
	alSourceStop(mainMenuSource);
}

void SoundManager::loadAllSoundFiles() {
	mainMenuSource = loadWavFile("Media/sounds/mainMenu.wav");
	suspenseSource = loadWavFile("Media/sounds/suspense.wav");
	themeSource = loadWavFile("Media/sounds/theme.wav");
	breathSourceFast = loadWavFile("Media/sounds/breathingFast.wav");
	//clickSource = loadWavFile("Media/sounds/thunder.wav");
	//playSource = loadWavFile("Media/sounds/thunder.wav");
	hoverSource = loadWavFile("Media/sounds/hover.wav");
}

int SoundManager::loadWavFile(char* filePath) {
	FILE *fp = NULL;       //Create FILE pointer for the WAVE file
	fp = fopen(filePath, "rb");     //Open the WAVE file
	if (!fp) return endWithError("Failed to open file");                        //Could not open file

																				//Variables to store info about the WAVE file (all of them is not needed for OpenAL)
	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	DWORD dataSize;

	//Check that the WAVE file is OK
	fread(type, sizeof(char), 4, fp);                                              //Reads the first bytes in the file
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F') {            //Should be "RIFF"
		endWithError("No RIFF");
		return -1;																		//Not RIFF
	}
	fread(&size, sizeof(DWORD), 1, fp);                                           //Continue to read the file
	fread(type, sizeof(char), 4, fp);                                             //Continue to read the file
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E') {          //This part should be "WAVE"
		endWithError("not WAVE");														 //Not WAVE
		return -1;
	}
	fread(type, sizeof(char), 4, fp);                                              //Continue to read the file
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ') {          //This part should be "fmt "
		endWithError("not fmt ");														//Not fmt
		return -1;
	}
																					//Now we know that the file is a acceptable WAVE file
																					//Info about the WAVE data is now read and stored
	fread(&chunkSize, sizeof(DWORD), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a') {          //This part should be "data"
		endWithError("Missing DATA");
		return -1;																		 //not data
	}
	fread(&dataSize, sizeof(DWORD), 1, fp);                                        //The size of the sound data is read

	unsigned char* buf = new unsigned char[dataSize];                            //Allocate memory for the sound data
	cout << fread(buf, sizeof(BYTE), dataSize, fp) << " bytes loaded\n";           //Read the sound data and display the

	ALuint source;   //Is the name of source (where the sound come from)
	ALuint buffer; //Stores the sound data
	ALuint frequency = sampleRate;     //The Sample Rate of the WAVE file
	ALenum format = 0;    //The audio format (bits per sample, number of channels)

	alGenBuffers(1, &buffer);     //Generate one OpenAL Buffer and link to "buffer"
	alGenSources(1, &source); //Generate one OpenAL Source and link to "source"
	if (alGetError() != AL_NO_ERROR) {
		endWithError("Error GenSource");     //Error during buffer/source generation
		return -1;
	}
	//Figure out the format of the WAVE file
	if (bitsPerSample == 8)
	{
		if (channels == 1)
			format = AL_FORMAT_MONO8;
		else if (channels == 2)
			format = AL_FORMAT_STEREO8;
	}
	else if (bitsPerSample == 16)
	{
		if (channels == 1)
			format = AL_FORMAT_MONO16;
		else if (channels == 2)
			format = AL_FORMAT_STEREO16;
	}
	if (!format) {
		endWithError("Wrong BitPerSample");  //Not valid format
		return -1;
	}
	alBufferData(buffer, format, buf, dataSize, frequency);  //Store the sound data in the OpenAL Buffer
	if (alGetError() != AL_NO_ERROR) {
		endWithError("Error loading ALBuffer");  //Error during buffer loading
		return -1;
	}
	fclose(fp); 
	alSourcei(source, AL_BUFFER, buffer); //Link the buffer to the source
	return source;
}

void SoundManager::playSound(int source,float pitch,float gain,ALboolean loop) {
	alSourcef(source, AL_PITCH, pitch);                                 //Set the pitch of the source
	alSourcef(source, AL_GAIN, gain);                                 //Set the gain of the source
	alSourcei(source, AL_LOOPING, loop);																		//PLAY
	alSourcePlay(source);                                                       //Play the sound buffer linked to the source
	if (alGetError() != AL_NO_ERROR) {
		endWithError("Error playing sound"); //Error when playing sound
		return;
	}
}

void SoundManager::setListenerPosition(ALfloat* pos) {
	alListenerfv(AL_POSITION, pos);   //Set position of the listener
}

void SoundManager::setListenerVelocity(ALfloat* velocitiy) {
	alListenerfv(AL_VELOCITY, velocitiy);   //Set velocity of the listener
}

void SoundManager::setSourcePosition(int source,ALfloat* pos) {
	alSourcefv(source, AL_POSITION, pos); //Set the position of the source
}

void SoundManager::setSourceVelocity(int source,ALfloat* velocitiy) {
	alSourcefv(source, AL_VELOCITY, velocitiy);   //Set the velocity of the source
}

void SoundManager::clean() {
   alcMakeContextCurrent(NULL);                                                //Make no context current
   alcDestroyContext(context);                                                 //Destroy the OpenAL Context
   alcCloseDevice(device); 
}

void SoundManager::init() {

	device = alcOpenDevice(NULL);                                               //Open the device
	if (!device) {
		endWithError("no sound device");                         //Error during device oening
		return;
	}
	context = alcCreateContext(device, NULL);                                   //Give the device a context
	alcMakeContextCurrent(context);                                             //Make the context the current
	if (!context) {
		endWithError("no sound context");                       //Error during context handeling
		return;
	}
	alListenerfv(AL_ORIENTATION, ListenerOri);  //Set orientation of the listener
}