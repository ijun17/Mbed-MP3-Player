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

    void start() { // LCD를 초기화
        lcd.setMode(TextLCD::DispOn);
        lcd.setBacklight(TextLCD::LightOff);
        lcd.setCursor(TextLCD::CurOff_BlkOff);

        selectMusicId = 0;
        mp3_state = menu;
        lcdUpdate("[MP3] menu", "> "+musicList[selectMusicId].getName());

        int longNameMusicI=0;
        Timer t1;
        wait(1);
        while (true) {
            if(button1.read()){
                if(mp3_state==menu)nextMusic();
                else pauseMusic();
                wait(0.5);
            }
            if(button2.read()){
                if(mp3_state==menu)playMusic();
                else stopMusic();
                wait(0.5);
            }
            if(mp3_state==playing){
                if(musicList[selectMusicId].update(buzzer)){
                    if(lcdBuffer[1].length()+musicList[selectMusicId].getLyric().length() <=16)
                        lcdUpdate("[MP3] playing", lcdBuffer[1]+musicList[selectMusicId].getLyric());
                    else lcdUpdate("[MP3] playing", "> "+musicList[selectMusicId].getLyric());
                }
                if(!musicList[selectMusicId].isPlaying())stopMusic();
            }
            if(mp3_state==menu && musicList[selectMusicId].getName().length()>14){

            }
        }
    }

    void addMusic(Music &music) { musicList.push_back(music); }

    void nextMusic() { //다음 노래를 선택
        if (++selectMusicId == musicList.size())selectMusicId = 0;
        lcdUpdate("", "> "+musicList[selectMusicId].getName());
    }

    void pauseMusic() { //노래를 일시정지
        if(mp3_state==playing){
            musicList[selectMusicId].pause(buzzer);
            mp3_state=pause;
            lcdUpdate("[MP3] pause", "");
        }else{
            musicList[selectMusicId].unpause(buzzer);
            mp3_state=playing;
            lcdUpdate("[MP3] playing", "");
        }
    }

    void stopMusic() { //노래를 중지
        musicList[selectMusicId].stop(buzzer);
        buzzer = 0.0;
        mp3_state=menu;
        lcdUpdate("[MP3] menu", "> "+musicList[selectMusicId].getName());
    }

    void playMusic() {
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
