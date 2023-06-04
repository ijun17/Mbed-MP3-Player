#pragma once

#include "mbed.h"
#include <vector>
#include <string>
using namespace std;


class Music {
private:
    string name;      //노래 제목
    string* pitches;  //음계(배열)
    string* lyrics;   //가사(배열)
    int* beats;       //박자(배열)
    int length;       //배열 길이

    int playID;  //재생중인 구간
    Timer timer;

    static const int PITCH[7];//{9,10,0,2,4,5,7}={A,B,C,D,E,F,G}
    //pitchs[i]에서 옥타브 추출
    int getOctave(int i){return pitches[i][pitches[i].length()-1]-'0';} 
    //pitchs[i]에서 음계 추출
    int getPitch(int i){return PITCH[pitches[i][0]-'A'] + (pitches[i][1]=='#');}
    // 옥타브와 음계로 주파수 계산
    int getFrequency(int i) {
        const int baseFreq = 55; //기준 주파수(1옥타브 라)
        return baseFreq * pow(2, getOctave(i)+getPitch(i)/12.0); // 주파수 반환
    }
    float getTime(int i){return beats[i]/4.0;}
public:
    Music(string name, string pitches[], string lyrics[], int beats[], int length)
        :name(name),pitches(pitches),lyrics(lyrics),beats(beats),length(length){}

    bool isPlaying(){return playID<length;}
    string &getName() {return name;}
    string &getLyric() { return lyrics[playID]; }

    void play(){ //재생
        playID=-1; 
        timer.start();
    }
    void stop(PwmOut& buzzer){ //중단
        playID=length;
        buzzer = 0.0;
        timer.reset();
    }
    void pause(PwmOut& buzzer){ //일시중지
        buzzer = 0.0;
        timer.stop();
    }
    void unpause(PwmOut& buzzer){ //일시중지 해제
        timer.start();
        buzzer = 0.5;
        buzzer.period(1.0 / getFrequency(playID));
    }

    bool update(PwmOut& buzzer){
        if(playID==-1 || (isPlaying() && getTime(playID) < timer.read())){
            buzzer = 0.5;
            playID++;
            timer.reset();
            if(isPlaying()){
                buzzer.period(1.0 / getFrequency(playID));
                timer.start();
                return true;
            }else return false;
        }
        return false;
    }
};

const int Music::PITCH[7]={9,10,0,2,4,5,7};//{0,2,3,5,7,9,10};//