#pragma once

#include "mbed.h"
#include "TextLCD.h"
#include "Music.h"
#include <string>
#include <vector>
using namespace std;


class MP3Player {
private:
    vector<Music> musicList;            //저장된 노래 배열
    int selectMusicId;                  //현재 선택한 노래 인덱스
    enum State { menu, playing, pause }; // mp3 상태
    State mp3_state;                    // mp3 상태

    //장치
    PwmOut buzzer;                      //노래를 출력할 부저
    I2C i2c_lcd;                        //I2c
    TextLCD_I2C lcd;                    //LCD 장치
    DigitalIn button1;                  //버튼1
    DigitalIn button2;                  //버튼2

    string lcdBuffer[2];
    bool lcdUpdated;
    string selectedMusicName;
    Timer nameTimer;

public:
    MP3Player(PinName buzzorPin, PinName SDA, PinName SCL, PinName btn1, PinName btn2)
      : buzzer(buzzorPin), 
      i2c_lcd(SDA, SCL),lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2, TextLCD::HD44780),
      button1(btn1, PullDown),button2(btn2, PullDown) 
    {
        lcd.setMode(TextLCD::DispOn);
        lcd.setBacklight(TextLCD::LightOff);
        lcd.setCursor(TextLCD::CurOff_BlkOff);
    }

    void start() {
        mp3_state = menu;
        selectMusicId = 0;
        selectedMusicName = musicList[selectMusicId].getName();
        lcdUpdate("[MP3] menu", "> "+selectedMusicName);

        wait(1);
        nameTimer.start();
        while (true) {
            //버튼 1 : 다음 노래, 일시 중지
            if(button1.read()){
                if(mp3_state==menu)nextMusic();
                else pauseMusic();
                restartNameTimer();
            }
            //버튼 2 : 재생, 중지
            if(button2.read()){
                if(mp3_state==menu)playMusic();
                else stopMusic();
                restartNameTimer();
            }
            //가사 출력
            if(mp3_state==playing){ 
                if(musicList[selectMusicId].update(buzzer)){
                    if(lcdBuffer[1].length()+musicList[selectMusicId].getLyric().length() <=16)
                        lcdUpdate("[MP3] playing", lcdBuffer[1]+musicList[selectMusicId].getLyric());
                    else lcdUpdate("[MP3] playing", "> "+musicList[selectMusicId].getLyric());
                }
                if(!musicList[selectMusicId].isPlaying()){
                    stopMusic();
                    restartNameTimer();
                }
            }
            //긴 제목 오른쪽 시프트
            if(mp3_state==menu && selectedMusicName.length()>14){
                if(int(nameTimer) >= selectedMusicName.length())restartNameTimer();
                lcdUpdate("[MP3] menu", "> "+selectedMusicName.substr(int(nameTimer),14));
            }
        }
    }

    void restartNameTimer(){
        nameTimer.reset();
        nameTimer.start();
    }

    void addMusic(Music &music) { musicList.push_back(music); }

    void nextMusic() { //다음 노래를 선택
        if (++selectMusicId == musicList.size())selectMusicId = 0;
        lcdUpdate("", "> "+musicList[selectMusicId].getName());
        selectedMusicName = musicList[selectMusicId].getName();
        wait(0.5); //위치 중요
    }

    void pauseMusic() { //노래를 일시정지
        if(mp3_state==playing){
            lcdUpdate("[MP3] pause", "");
            musicList[selectMusicId].pause(buzzer);
            mp3_state=pause;
            wait(0.5); //위치 중요
        }else{
            lcdUpdate("[MP3] playing", "");
            wait(0.5); //위치 중요
            musicList[selectMusicId].unpause(buzzer);
            mp3_state=playing;
            
        }
    }

    void stopMusic() { //노래를 중지
        lcdUpdate("[MP3] menu", "> "+musicList[selectMusicId].getName());
        musicList[selectMusicId].stop(buzzer);
        mp3_state=menu;
        wait(0.5); //위치 중요
    }

    void playMusic() {
        lcdUpdate("[MP3] playing", "> ");
        wait(0.5); //위치 중요
        musicList[selectMusicId].play();
        mp3_state=playing;
    }

    void lcdUpdate(string s1,string s2){
        const int MAX_LENGTH=16;
        if(s1.length()==0)s1=lcdBuffer[0];
        if(s2.length()==0)s2=lcdBuffer[1];
        lcdBuffer[0]=s1;
        lcdBuffer[1]=s2;
        s1=(s1+string(' ',16)).substr(0,MAX_LENGTH);
        s2=(s2+string(' ',16)).substr(0,MAX_LENGTH);
        lcd.printf((s1+s2).data());
    }
};
