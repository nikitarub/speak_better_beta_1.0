//
//  main.cpp
//  speak_better_beta(1.0)
//
//  Created by Nikita on 28.07.16.
//  Copyright (c) 2016 RIT. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <queue>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;

// Структура, описывающая заголовок WAV файла.
struct WAVHEADER
{
    // WAV-формат начинается с RIFF-заголовка:
    
    // Содержит символы "RIFF" в ASCII кодировке
    // (0x52494646 в big-endian представлении)
    char chunkId[4];
    
    // 36 + subchunk2Size, или более точно:
    // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
    // Это оставшийся размер цепочки, начиная с этой позиции.
    // Иначе говоря, это размер файла - 8, то есть,
    // исключены поля chunkId и chunkSize.
    unsigned int chunkSize;
    
    // Содержит символы "WAVE"
    // (0x57415645 в big-endian представлении)
    char format[4];
    
    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:
    
    // Содержит символы "fmt "
    // (0x666d7420 в big-endian представлении)
    char subchunk1Id[4];
    
    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    unsigned int subchunk1Size;
    
    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    unsigned short audioFormat;
    
    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    unsigned short numChannels;
    
    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    unsigned int sampleRate;
    
    // sampleRate * numChannels * bitsPerSample/8
    unsigned int byteRate;
    
    // numChannels * bitsPerSample/8
    // Количество байт для одного сэмпла, включая все каналы.
    unsigned short blockAlign;
    
    // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
    unsigned short bitsPerSample;
    
    // Подцепочка "data" содержит аудио-данные и их размер.
    
    // Содержит символы "data"
    // (0x64617461 в big-endian представлении)
    char subchunk2Id[4];
    
    // numSamples * numChannels * bitsPerSample/8
    // Количество байт в области данных.
    unsigned int subchunk2Size;
    
    // Далее следуют непосредственно Wav данные.
};

struct SAMPLE2
{
    unsigned short ch1;
 
    //unsigned short ch2; //если используется двух канальная запись | if there're 2 channels
};




// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------


int main() {
    
    
     // -------------------------------------------------------------
    // Открывем нужные нам файлы дла записи и обработки | open needed files
     // -------------------------------------------------------------
    
    ofstream Y_vvod;
    ifstream Y_read;
    ofstream X_vvod;
    ifstream X_read;
    
    Y_vvod.open("/Users/nikita/Documents/speak_better/filies/Y_obr1.csv");
    Y_read.open("/Users/nikita/Documents/speak_better/filies/Y_obr1.csv");
    X_vvod.open("/Users/nikita/Documents/speak_better/filies/X_obr1.csv");
    X_read.open("/Users/nikita/Documents/speak_better/filies/X_obr1.csv");

    ifstream urav_file;
    
    urav_file.open("/Users/nikita/Desktop/test2.txt");

    // файл входных звуковых данных формата .wav для обработки | .wav data to be processed
    
    FILE *file;
    file = fopen("/Users/nikita/Desktop/misc/Eee_text5_1.wav", "rb");
  
    if (!file)
    {
        printf("Failed open file, error %d", 0);
        return 0;
    }
    
    
    // -------------------------------------------------------------
  // данные и их типы | data and it's type
    // -------------------------------------------------------------
    
    float ur_result = 0; // значение вашего результата в процентах | your result in %
    
    float x_time = 0; // отсчет количество данных | amount of data to be processed
    
    vector<int> g; // хранение данных эталонного файла | storage of sample data
    
    char input [256]; // временных массив | temporary array
    
    int value = 0; // временной значение | temporary value
    
    
    // заполнение данных эталонного файла | sample data storage input
    while (!urav_file.eof()) {
        urav_file >> input;
        value = atoi(input);
        g.push_back(value);
        //cout <<  value << endl;
    }

    
    int proisv_g = 0; // значение условной производной | relative derivative
    
    
    long double S_g = 0; // Площадь под графиком эталона | Area under sample graph
    
    // нахождение площади | area result
    for (int e = 0; e < g.size(); e++) {
        S_g = S_g + g[e];
    }
    
    long double S_f = 0;  // Площадь под графиком обрабатываемого файла | Area under graph of file to be processed
    
    int proisv_f = 0;
    
    int vrem = 0; // временные значения входного файла | temporary value of input file
    
    vector<int> f; //  значения входного файла | value of input file
    
    int sovp_count = 0; // количество временных совпадений | amount of temporary coincidence
    
    int kol_sovp = 0; // количество обобщенных совпадений | amount of summarize coincidence
  

    // вставь сюда ссылку
    
    WAVHEADER header;
    
    unsigned long res = fread(&header, sizeof(WAVHEADER), 1, file);
    
    
    // Выводим полученные данные | data output
    printf("Размер входного файла: %u Б\n", (header.chunkSize + 8)); // + 8 байт данных на id сам chunkSize
    printf("Частота дискретизации: %u Гц\n", header.sampleRate);
    printf("Количество каналов: %d\n", header.numChannels);
    //printf("Bits per sample: %d\n", header.bitsPerSample);
    
    //printf("blockAlign: %d\n", header.blockAlign);
    
    
    int iDurationMinutes = 0; // длительность в минутах | duration in minutes
    float fDurationSeconds = 0; // длительность в секундах с минутами | duration in seconds with minutes
    float sec_dur = 0; // длительность в секундах | duration in seconds
    
    
    // Посчитаем длительность воспроизведения в секундах | duration in seconds
    fDurationSeconds = (1.0 * header.chunkSize) / (header.bitsPerSample / 8.0) / header.numChannels / header.sampleRate;
    
    sec_dur = (1.0 * header.chunkSize) / (header.bitsPerSample / 8.0) / header.numChannels / header.sampleRate;
    
    
    iDurationMinutes = (int)floor(fDurationSeconds) / 60;
    fDurationSeconds = fDurationSeconds - (iDurationMinutes * 60);
    printf("Длительность: %d:%f\n", iDurationMinutes, fDurationSeconds);
    
    
    SAMPLE2 data;
    
    data.ch1 = 0;
 
    while (fread(&data, 1, 1, file) ){
       
        // ввод данных в файл Y_obr1.csv для наглядности | data input into Y_obr1.csv just let it be there
        //Y_vvod << data.ch1 << endl;
        
        vrem = data.ch1;
        f.push_back(vrem);
        
    }
    
    Y_vvod.close();
    
    
    
    // Основной алгоритм | Main algorithm
    
    int c = 0;
    for ( c = 0; c < f.size() - g.size(); c = c + 10) { // измени шаг для увеличения/уменьшения точности | change step to increase or decrease exactness
        
        for (int t = 0; t<g.size(); t++) {
            S_f = S_f + f[t+c]; // Площадь окна | window area
        }
        
        
        if ((0.7 <= S_f/S_g)&&(S_f/S_g <= 1.3)) {
            
            for (int r = 0; r < g.size() - 1; r++) {

                proisv_g = -(g[r]-g[r+1]); // Нахождение условной прозводной g | relative derivative g
                
                proisv_f = -(f[r+c]-f[r+1+c]); // Нахождение условной прозводной f | relative derivative f

                //                proisv_g = -(abs((g[r]+g[r+1])/2)-abs((g[r+2]+g[r+3])/2));
                //
                //                proisv_f = -( abs((f[r+c]+f[r+c+1])/2) - abs((f[r+c]+f[r+c+1])/2));
                
                //cout << c + 1 << " : " << proisv_g << " | " << proisv_f;
                
                if ((proisv_g - 30 <= proisv_f)&&(proisv_f <= proisv_g+30)) { // погрешность расхождений амплетуд
                    //                    cout << c + 1 << " : " << proisv_g << " | " << proisv_f;
                    //
                    //                    cout << " sovp"  << endl;
                    sovp_count++;
                }else {//cout  << endl;
                }
            }
            
            if ((sovp_count >= g.size()*0.73)) {
                kol_sovp++;
                c = c + (header.sampleRate / sec_dur); // пропускной шаг | next step forward
                //cout << c << endl;
            }
            sovp_count = 0;
        }
        //cout << c + 1 << " | " << S_f << endl;
        S_f = 0;
    }
    
    //cout << endl <<  c << endl ;
    
    //cout << urav << endl;
    
    ur_result = kol_sovp / sec_dur;
    
   // cout << endl << S_g << endl << S_f << endl << S_f/S_g << endl << endl << kol_sovp << endl <<  sec_dur << endl << endl << ur_result;
    
    
    cout << endl << "Количество совпадений: "<< kol_sovp;
    
    cout << endl << "Время записи в секундах: " <<  sec_dur << endl;

    cout  << endl << "Ваш результат: " << ur_result << endl;
    
    if (ur_result <= 0.15) {
        cout << endl << "Молодец" << endl;
    }
    
    if ((ur_result >= 0.15)&&(ur_result <= 0.3)) {
        cout << endl << "Хорошо, но можно лучше" << endl;
    }
    
    if ((ur_result >= 0.3)&&(ur_result <= 0.5)) {
        cout << endl << "Ёще чуть-чуть и будет хорошо" << endl;
    }
    
    if ((ur_result >= 0.5)&&(ur_result <= 0.7)) {
        cout << endl << "Сойдет, попробуй ещё" << endl;
    }
    
    if ((ur_result >= 1)&&(ur_result <= 2)) {
        cout << endl << "Плохо, нужна тренировка" << endl;
    }
    
    if ((ur_result >= 2)) {
        cout << endl << "Учиться, учиться и ещё раз учиться!" << endl << "Ну или проблема с качеством звука, перезапиши ещё раз" << endl;
    }
    
    return 0;
}