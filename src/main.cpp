#include <M5Stack.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define LGFX_AUTODETECT         
#define LGFX_USE_V1            
#include <LovyanGFX.hpp>       
#include <LGFX_AUTODETECT.hpp>  

static LGFX lcd;                  
static LGFX_Sprite sprite(&lcd);

#define JST (3600L * 9)

JsonDocument jsonData;
 
 //SDカードからWiFi接続に関するファイルを読み込む
bool LoadWifiFile()
{
        //SDカードが使えなかったら
        if (!SD.begin())
        {
                sprite.println("SD card does not exist or is malfunctioning");
                sprite.pushSprite(0, 0);
                return false;
        }

        //SDカードにファイルがあったら
        if (SD.exists("/WIFI.txt"))
        {
                delay(500);
                //ファイルを読み取り専用で開く
                File myFile = SD.open("/WIFI.txt", FILE_READ); 

                if (myFile)
                {
                        while (myFile.available())
                        {
                                //Json形式に変換
                                deserializeJson(jsonData, myFile);
                        }
                        myFile.close(); 
                        return true;
                }
                else
                {
                        sprite.println("WIFI.txt could not be read!");
                        sprite.pushSprite(0, 0);
                        return false;
                }
        }
        else
        {
                sprite.println("WIFI.txt doesn't exit.");
                sprite.pushSprite(0, 0);
                return false;
        }
}

//WiFi接続をする
void ConnectToWiFi(void)
{
        //ファイルを読み込めたら
        if (LoadWifiFile())
        {
                //ファイルからSSIDとパスワードを読み込み
                String i_ssid = jsonData["ssid"].as<String>();
                String i_pass = jsonData["pass"].as<String>();

                char buf_ssid[33], buf_pass[65]; 
                i_ssid.toCharArray(buf_ssid, 33);
                i_pass.toCharArray(buf_pass, 65);

                //WiFi接続
                WiFi.begin(buf_ssid, buf_pass); 

                sprite.println("Wi-Fi Conecting..."); 
                sprite.pushSprite(0, 0);

                while (WiFi.status() != WL_CONNECTED)
                {
                        delay(500);
                }

                sprite.println("WiFi Connected");
                sprite.pushSprite(0, 0);
        }
}

//時刻を表示する
void ShowLocalTime(void)
{
        struct tm tm;
        
        //現在時間を取得出来たら
        if (getLocalTime(&tm))
        {
                //画面に時刻を表示する
                sprite.fillScreen(BLACK);
                sprite.setCursor(60, 80);
                sprite.printf("%d/%2d/%2d",
                              tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
                sprite.setCursor(80, 140);
                sprite.printf("%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
                sprite.pushSprite(0, 0);
        }
        else
        {
                sprite.println("Failed to obtain time");
                sprite.pushSprite(0, 0);
        }
}

void setup()
{
        M5.begin();

        lcd.init(); 
        sprite.setColorDepth(8);
        sprite.createSprite(lcd.width(), lcd.height()); 
        sprite.setTextSize(3);

        //WiFi接続
        ConnectToWiFi();

        //NTPの設定
        configTime(JST, 0, "ntp.nict.jp", "time.google.com",
                   "ntp.jst.mfeed.ad.jp");

        //時刻を表示する
        ShowLocalTime();

        //WiFi切断
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
}

void loop()
{
        M5.update();

        //時刻を表示する
        ShowLocalTime();
        
        delay(1000);
}