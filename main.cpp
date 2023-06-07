#include "mbed.h"

#include "Music.h"
#include "MusicSample.h"
#include "MP3Player.h"

int main() 
{
    Music music1(name1,pitchs1,lyrics1,beats1,length1);
    Music music2(name2,pitchs2,lyrics2,beats2,length2);
    Music musicT(nameT,pitchsT,lyricsT,beatsT,lengthT);

    MP3Player mp3(D5, D14, D15, D3, D4);
    mp3.addMusic(music1);
    mp3.addMusic(music2);
    mp3.addMusic(musicT);
    mp3.start();
}