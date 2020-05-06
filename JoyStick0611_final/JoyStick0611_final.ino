/* 2018 09 25 MRT Line Maker M의 코드를 추가합니다. 
 * 2019 01 24  3/17날 울산 격투대회에 맞추어 모든 모드 삽입완료
 * 2019 04 12  shift & Control Key 수정했습니다. 수정내용은 핀번호가 6 5 번이 맞습니다. 
                                                         PULLDOWN방식으로 되어 있어서 LOW, HIGH가 반대여야 맞습니다. 
                                                         로보라이프와 엠알티에서는 컨트롤키와 쉬프트키를 사용합니다. 
                                                         OLED에 표시 되도록 했습니다. 
 * 2019 05 07 MRT모드에서 15D 모드로 전환 할 수 있게 했습니다.
 * 2019 06 11 OLED를 정의 할수 있도록 했습니다.  첫번째 라인에서 주석을 제거 하면 OLED를 사용하게 됩니다. 
*/
//define OLED_USED 1

#include <LobotServoController.h> //LX15C용 라이브러리 사용하셔야 합니다. 
#include <PS2X_lib.h> //for v1.6

#define ROBOTIS 0     //모드 설정
#define ROBOLIFE 1
#define RCCAR 2
#define MRT 3
#define LX15C 4

#define PS2_DAT        8    // 
#define PS2_CMD        9
#define PS2_SEL        11
#define PS2_CLK        10

#define pressures   false  // ?
#define rumble      false

#define shiftButton 5    //
#define ctrlButton  6
#define modeSwitch1 2
#define modeSwitch2 3

#define joy_down 10     //
#define joy_up 235

PS2X ps2x; // create PS2 Controller Class

byte vibrate = 0;         //
char controlFlag = 0;
int modeRcCar = 0;
int modeCnt = 0;
int start_botton = 0;
int select_botton = 0;
int temp_select = 1;
int mode = 0;
int Flag = 0;
int Select_flag = 0;
int Select_off = 0;
int mission2=0;
int mission2flag=0;
int walkflag1=0;
int walkflag2=0;
int walkflag3=0;


/* For OLED */
#ifdef OLED_USED
  #include <TimerOne.h>
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #define OLED_RESET 4
  Adafruit_SSD1306 display(OLED_RESET);
  char *maker[]={"ROBOTIS","ROBOLIFE","MINI","MRT","LX15C"};
  char *MRTMode[] = {"Basic working","Front Fight","Side Fight","Soccer play","Mission complete","Hockey play","LX15C"};
#endif

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 


static unsigned long tm= 0 ;
static unsigned long pTm=-1,dspTm=-1;
static int shift=HIGH;
static int ctrl=HIGH;
static int pMaker = -1;
#define HIGH 0
#define LOW 1

LobotServoController myse(Serial);

void doSomething()
{
  tm++;  
  if (tm >  50000) tm = 0;   
}
#ifdef OLED_USED
void dispOLEDText(int n, unsigned char c, unsigned char c1)
{
  
  char tmp[128];   
  pMaker = n;    
  if (dspTm == tm) return;
  dspTm = tm;
  
  display.clearDisplay();
  display.setCursor(0,0);  
  
  if (n != 4)  // ROBOTIS모드가 아닐때는 쉬프트와 컨트롤을 표시 한다. 
  {
    if      (shift==HIGH) c1 = 's';
    else if (ctrl ==HIGH) c1 = 'c';
    else c1 = ' ';
  }
  
  switch (n)
  {
    case 0 : 
      display.setTextSize(1);
      display.println("V 190412_0");
      delay(500);
      display.print("M ");display.println(maker[controlFlag]);
      delay(500);
      display.println("Designed by 3DSS ");
      delay(500);
      display.println("Programmed by MRT International");
      delay(500);
      display.setTextSize(2);
      break;
    case 1 :  
      display.setTextSize(1);
      sprintf(tmp,"MRT exe Mode");
      display.println(tmp);
      sprintf(tmp,"/%s",MRTMode[mode]);
      display.println(tmp);
            
      display.setTextSize(2);      
      sprintf(tmp,"C= %02d / %c ",c,c1);
      display.println(tmp);
      break;    
    case 2 :  
      sprintf(tmp,"MRT remocon Mode");
      display.println(tmp);
      sprintf(tmp,"/%s",MRTMode[mode]);
      display.println(tmp);
      
      display.setTextSize(2);
      sprintf(tmp,"C= %02d / %c ",c,c1);
      display.println(tmp);
    case 3 : // MINI RCCAR
      display.print("M=");display.println(maker[controlFlag]);  // Only Lobotis Mode
      sprintf(tmp,"C= %02d / %02d ",c,c1);
      display.println(tmp);
      break;
    case 4 : // LOBOTIS
      display.print("M=");display.println(maker[controlFlag]);  // Only Lobotis Mode
      sprintf(tmp,"C= %02d / %02d ",c,c1);
      display.println(tmp);
      break;
    case 5 : // LOBOLIFE
      display.print("M=");display.println(maker[controlFlag]);      
      sprintf(tmp,"C= %c / %c ",c,c1);
      display.println(tmp);
      break; 
    case 6 : // LX15C
      display.setTextSize(2);
      display.setCursor(0,0);
      display.println("15CMode");
      sprintf(tmp,"Cmd= %03d",(int) c);
      display.println(tmp);
      break;
  }
  display.display();
}
void initOled()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)  
  
  Timer1.initialize(1000); // 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt(doSomething);    
  
  display.display();
  delay(100);
  display.clearDisplay();  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  dispOLEDText(0, 'a','a');
}

#else
void dispOLEDText(int n, unsigned char c, unsigned char c1){}
void initOled(){}  
#endif
/* For OLED */

void sendPacketROBOTIS(int data)
{
    unsigned char data_L = data & 0xff;
    unsigned char data_H = (data & 0xff00) >> 8;
    Serial.print(0xFF,HEX); 
    Serial.print(0x55,HEX); 
    Serial.print(data_L,HEX); 
    Serial.print(~data_L & 0xff,HEX); 
    Serial.print(data_H,HEX); 
    Serial.println(~data_H & 0xff,HEX); 
    dispOLEDText(4,data_L, data_H);
}

void sendPacketROBOLIFE(char data)
{
  Serial.print(data);
  dispOLEDText(5, data,data);
  
}

void sendPacketRCCAR(byte data)
{
  Serial.write(data);
  dispOLEDText(3, data,data);
}

void setup(){
  
  pinMode(modeSwitch1, INPUT);
  pinMode(modeSwitch2, INPUT);
  pinMode(shiftButton, INPUT);
  pinMode(ctrlButton , INPUT);  

  if((digitalRead(modeSwitch1) == 1) && (digitalRead(modeSwitch2) == 1))
  {
    controlFlag = MRT;
    Serial.begin(115200);    
    delay(1000);
  }
  else if((digitalRead(modeSwitch1) == 1) && (digitalRead(modeSwitch2) == 0))
  {
    controlFlag = ROBOTIS;
    Serial.write("ROBOTIS");
    Serial.begin(57600);
    delay(1000);
  }
  else if((digitalRead(modeSwitch1) == 0) && (digitalRead(modeSwitch2) == 1))
  {
    controlFlag = ROBOLIFE;
    Serial.write("ROBOLIFE");
    Serial.begin(38400); 
    delay(1000);
  }
  else if((digitalRead(modeSwitch1) == 0) && (digitalRead(modeSwitch2) == 0))
  {
    controlFlag = RCCAR;
    Serial.write("RCCAR");
    Serial.begin(115200); 
    delay(1000);
  }
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  initOled();
  delay(100);  
}

void sendPacketMRTEXE(unsigned char exeIndex)
{                           // 0     1      2     3     4     5     6     7  
  unsigned char exeCmd[15] = {0xff, 0xff, 0x4c, 0x53, 0x00, 0x00, 0x00, 0x00, 
                              0x30, 0x0c, 0x03,  92,  0x00, 0x14, 0x54}; // Index 01 명령어 샘플
                            //  8     9    10    11    12    13    14 
                                                 //'Index
                                                       //Delay mSec 
                                                             //Speed
                                                                   // CheckSum
  exeCmd[11] = exeIndex;
  exeCmd[14] = 0x00;    // checksum clear
    
  for (int i=6;i<14;i++){
    exeCmd[14]+=exeCmd[i];    
  }
  Serial.write(exeCmd,15);   
  dispOLEDText(1,exeCmd[11],exeCmd[11]);
}

void sendPacketMRTLED(unsigned char mNo,unsigned char r,unsigned char g,unsigned char b)
{
  unsigned char ledCmd[16]={0xff,0xff,0x4c,0x53,
                            0x00,0x00,0x00,0x00,
                            0x30,0x05,0x04,
                            0xff,0x00,0x00,0x00,0x00};
                            //    r    g    b    checksum

  ledCmd[11] = mNo;
  ledCmd[12] = r;
  ledCmd[13] = g;
  ledCmd[14] = b;
  ledCmd[15] = 0x00;    // checksum
    
  for (int i=6;i<15;i++)  {
    ledCmd[15]+=ledCmd[i];    
  }
  Serial.write(ledCmd,16);
  dispOLEDText(2,ledCmd[11],ledCmd[11]);
  
}
void sendPacket15DEXE(uint8_t numOfAction, uint16_t Times)
{
  char tmp[128];   
  myse.runActionGroup(numOfAction,Times);  
  delay(20);
#ifdef OLED_USED
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  sprintf(tmp,"15C Mode");  
  display.println(tmp);  
  sprintf(tmp,"Motion=%03d",numOfAction);    
  display.println(tmp);
  display.display();  
  myse.waitForStopping(2000);  
#endif
}
void processMRTV1()
{  
  int shift = digitalRead(shiftButton); // 사용하지 않음 
  int ctrl = digitalRead(ctrlButton);
  int basicMotion[7]={1,22,39,55,75,102,0};  
  ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
  //죠이스틱패들을 두개 동시에 위나 아래로 햇을때 /////////////////////////////////
  if(ps2x.Analog(PSS_LY) > joy_up && ps2x.Analog(PSS_RY) > joy_up) mode= 6;  
  if(ps2x.Analog(PSS_LY) < joy_down && ps2x.Analog(PSS_RY) < joy_down) mode= 6;
  //////////////////////////////////////////////////////////   
  if(ps2x.Button(PSB_SELECT))     // 모드 설정 변경 1~6
  {
    if(Select_off==0) 
    {
      Select_flag+=1;
      Select_off=1;
      if(Select_flag > 6) Select_flag=0;
      mode=Select_flag;
      delay(10);
      if     (mode!=6) sendPacketMRTEXE(basicMotion[mode]);  //각모드 기본동작
      else if(mode==6) sendPacket15DEXE(0,1); // 15C 컨트롤러의 기본동작 을 보냄 
      delay(200);
    }
  }
  else if(ps2x.Button(PSB_START) && mode!=6 )  sendPacketMRTEXE(basicMotion[mode]);  //각 모드 기본 동작      
  else if(Select_off==1)    Select_off=0; // 디바운스 방지 
    
  if (mode == 6) // 15C 컨트롤러모드임 
  {
    uint8_t exeMotionNo = 0;
    uint8_t shiftFlag = 0;   
    
    if(ctrl  == HIGH) shiftFlag = 2; // 얘는 아두이노에서 받아 오는 신호임        
        
    // ps2x.read_gamepad(false, vibrate); 
    if     (ps2x.Button(PSB_PAD_UP))      exeMotionNo = 1 + (shiftFlag * 10);
    else if(ps2x.Button(PSB_PAD_DOWN))    exeMotionNo = 2 + (shiftFlag * 10);
    else if(ps2x.Button(PSB_PAD_LEFT))    exeMotionNo = 3 + (shiftFlag * 10);
    else if(ps2x.Button(PSB_PAD_RIGHT))   exeMotionNo = 4 + (shiftFlag * 10);
    
    else if(ps2x.Button(PSB_TRIANGLE))    exeMotionNo = 5 + (shiftFlag * 10);
    else if(ps2x.Button(PSB_CROSS))       exeMotionNo = 6 + (shiftFlag * 10);
    else if(ps2x.Button(PSB_SQUARE))      exeMotionNo = 7 + (shiftFlag * 10);
    else if(ps2x.Button(PSB_CIRCLE))      exeMotionNo = 8 + (shiftFlag * 10);

    else if(ps2x.Button(PSB_L1))          exeMotionNo = 9 + (shiftFlag * 10);
    else if(ps2x.Button(PSB_R1))          exeMotionNo = 10 + (shiftFlag * 10);
    else if(ps2x.Button(PSB_L2))          exeMotionNo = 11 + (shiftFlag * 10);    
    else if(ps2x.Button(PSB_R2))          exeMotionNo = 12 + (shiftFlag * 10);

          // 기존 
          /*     
          else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_TRIANGLE))    exeMotionNo = 13;
          else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_CROSS))       exeMotionNo = 14;
          else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_SQUARE))      exeMotionNo = 15;
          else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_CIRCLE))      exeMotionNo = 16;
          
          else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_L1))          exeMotionNo = 17;
          else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_R1))          exeMotionNo = 18;
          else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_L2))          exeMotionNo = 19;    
          else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_R2))          exeMotionNo = 20;
          */
    
    /* 신규 */
    //LEFT STICK
    else if(ps2x.Analog(PSS_LY) < joy_down)  exeMotionNo = 13+ (shiftFlag * 10);// 위로 올림    
    else if(ps2x.Analog(PSS_LY) > joy_up)    exeMotionNo = 14+ (shiftFlag * 10);// 아래로 내림
        
    else if(ps2x.Analog(PSS_LX) < joy_down)  exeMotionNo = 15+ (shiftFlag * 10);// 좌로    
    else if(ps2x.Analog(PSS_LX) > joy_up)    exeMotionNo = 16+ (shiftFlag * 10);// 우로 

    //RIGHT STICK
    else if(ps2x.Analog(PSS_RY) < joy_down)  exeMotionNo = 17+ (shiftFlag * 10);    
    else if(ps2x.Analog(PSS_RY) > joy_up)    exeMotionNo = 18+ (shiftFlag * 10);
        
    else if(ps2x.Analog(PSS_RX) < joy_down)  exeMotionNo = 19+ (shiftFlag * 10);    
    else if(ps2x.Analog(PSS_RX) > joy_up)    exeMotionNo = 20+ (shiftFlag * 10);    
        
    else if(ps2x.Button(PSB_START))       myse.stopActionGroup();
    else                                  exeMotionNo = 0;
    //if (exeMotionNo > -1) 
    {
      sendPacket15DEXE(exeMotionNo,1);      
    }
  }
  else if(mode==0)   //기본모드
  {
    if(ps2x.Button(PSB_SELECT))             sendPacketMRTEXE(1);//기본자세
    else if(ps2x.Button(PSB_PAD_UP))
    {   
      if(shift == HIGH)     // 빠르게 걷기 mode 5
      {
        sendPacketMRTEXE(103); //걷기
        sendPacketMRTEXE(104);
      }
      else if(ctrl == HIGH) // 살살 걷기 mode 4   
      {
        sendPacketMRTEXE(76); //걷기
        sendPacketMRTEXE(77);
      }
      else
      {
        sendPacketMRTEXE(2); //걷기
        sendPacketMRTEXE(3);
        sendPacketMRTEXE(4); 
      }
      walkflag1=1;
    }           
    else if(ps2x.Button(PSB_PAD_RIGHT))
    {      
      if(shift == HIGH) sendPacketMRTEXE(8);// 우회전
      else  sendPacketMRTEXE(6); //오른쪽이동
    }
    else if(ps2x.Button(PSB_PAD_LEFT))
    {
      if(shift == HIGH) sendPacketMRTEXE(7);// 좌회전
      else sendPacketMRTEXE(5); //좌로이동   
    }
    else if(ps2x.Button(PSB_PAD_DOWN))
    {
      if(shift == HIGH)     // 빠르게 걷기 mode 5
      {
        sendPacketMRTEXE(105); //걷기
        sendPacketMRTEXE(106);
      }
      else if(ctrl == HIGH) // 살살 걷기 mode 4   
      {
        sendPacketMRTEXE(78); //걷기
        sendPacketMRTEXE(79);
      }
      else
      {
        sendPacketMRTEXE(9); //뒤로이동   
        sendPacketMRTEXE(11); //뒤로이동 
        sendPacketMRTEXE(10); //뒤로이동 
        //sendPacketMRTEXE(9); //뒤로이동
      }
      walkflag2=1;  
    }
    else if(ps2x.Button(PSB_L2))
    {
      if(ps2x.Button(PSB_R2))  sendPacketMRTEXE(1);   //정지   
    }
    else if(ps2x.Button(PSB_R2))    
    {
      if(ps2x.Button(PSB_L2))  sendPacketMRTEXE(1);   //정지
    }
    else if(ps2x.Button(PSB_L1))
    {
      if(ps2x.Button(PSB_TRIANGLE)) sendPacketMRTEXE(14);   //앞으로 일어나기
    }
    else if(ps2x.Button(PSB_R1))
    {
      if(ps2x.Button(PSB_PAD_UP)) sendPacketMRTEXE(15);  //뒤로 일어나기
    }
    else if(ps2x.Button(PSB_TRIANGLE))      sendPacketMRTEXE(18);  //인사
    else if(ps2x.Button(PSB_CIRCLE))        sendPacketMRTEXE(17);  //승리포즈
    else if(ps2x.Button(PSB_CROSS))         sendPacketMRTEXE(16);  //패배포즈 
    else if(ps2x.Button(PSB_SQUARE))        sendPacketMRTEXE(19);  //허리인사
    else if(walkflag1==1)
    {
      delay(260);
      sendPacketMRTEXE(4);
      walkflag1=0;
    }
    else if(walkflag2==1)
    {
      delay(260);
      sendPacketMRTEXE(10);
      walkflag2=0;
    }
  }

  
  else if(mode==1)  //정면격투
  {
    if(ps2x.Button(PSB_SELECT))             sendPacketMRTEXE(22); //기본자세
    else if(ps2x.Button(PSB_PAD_UP))        sendPacketMRTEXE(24); //걷기   
    else if(ps2x.Button(PSB_PAD_RIGHT))     
    {
      if(shift == HIGH) sendPacketMRTEXE(29); //우회전
      else sendPacketMRTEXE(27); //오른쪽이동
    }
    else if(ps2x.Button(PSB_PAD_LEFT))
    {
      if(shift == HIGH) sendPacketMRTEXE(28); //좌회전
      else sendPacketMRTEXE(26); //좌로이동   
    }
    else if(ps2x.Button(PSB_PAD_DOWN))      sendPacketMRTEXE(25); //뒤로이동   
    else if(ps2x.Button(PSB_L2))
    {
      if(ps2x.Button(PSB_R2))  sendPacketMRTEXE(22);     //정지   
    }
    else if(ps2x.Button(PSB_R2))    
    {
      if(ps2x.Button(PSB_L2))  sendPacketMRTEXE(22);   //정지
    }
    else if(ps2x.Button(PSB_L1))
    {
      if(ps2x.Button(PSB_TRIANGLE)) sendPacketMRTEXE(37);   //앞으로 일어나기
    }
    else if(ps2x.Button(PSB_R1))
    {
      if(ps2x.Button(PSB_PAD_UP)) sendPacketMRTEXE(38);  //뒤로 일어나기
    }
    else if(ps2x.Button(PSB_TRIANGLE))
    {
      if(shift == HIGH) sendPacketMRTEXE(33);//라이트 스트레이트
      else sendPacketMRTEXE(30);  //잽
    }
    else if(ps2x.Button(PSB_CIRCLE))
    {
      if(shift == HIGH) sendPacketMRTEXE(34); //라이트훅
      else sendPacketMRTEXE(31);  //레프트 훅
    }
    else if(ps2x.Button(PSB_CROSS))
    {
      if(shift == HIGH) sendPacketMRTEXE(35); //라이트 어퍼컷
      else sendPacketMRTEXE(32);  //어퍼컷
    } 
    else if(ps2x.Button(PSB_SQUARE))
    {
     if(shift == HIGH) sendPacketMRTEXE(36); //원투
      else sendPacketMRTEXE(36);  //원투
    }
  }
  
  
  else if(mode==2)  //측면격투
  {
    if(ps2x.Button(PSB_SELECT))             sendPacketMRTEXE(39); //기본자세
    else if(ps2x.Button(PSB_PAD_UP))        sendPacketMRTEXE(41); //걷기   
    else if(ps2x.Button(PSB_PAD_RIGHT))     
    {
      if(shift == HIGH) sendPacketMRTEXE(46); //우회전
      else sendPacketMRTEXE(44); //오른쪽이동
    }
    else if(ps2x.Button(PSB_PAD_LEFT))
    {
      if(shift == HIGH) sendPacketMRTEXE(43); //좌회전
      else sendPacketMRTEXE(45); //좌로이동   
    }
    else if(ps2x.Button(PSB_PAD_DOWN))      sendPacketMRTEXE(42); //뒤로이동   
    else if(ps2x.Button(PSB_L2))
    {
      if(ps2x.Button(PSB_R2))  sendPacketMRTEXE(39);     //정지   
    }
    else if(ps2x.Button(PSB_R2))    
    {
      if(ps2x.Button(PSB_L2))  sendPacketMRTEXE(39);   //정지
    }
    else if(ps2x.Button(PSB_L1))
    {
      if(ps2x.Button(PSB_TRIANGLE)) sendPacketMRTEXE(53);   //앞으로 일어나기
    }
    else if(ps2x.Button(PSB_R1))
    {
      if(ps2x.Button(PSB_PAD_UP))  sendPacketMRTEXE(54);  //뒤로 일어나기
    }
    else if(ps2x.Button(PSB_TRIANGLE))
    {
      if(shift == HIGH) sendPacketMRTEXE(47);//숄더
      else sendPacketMRTEXE(51);  //잽
    }
    else if(ps2x.Button(PSB_CIRCLE))
    {
      if(shift == HIGH) sendPacketMRTEXE(48); //팔꿈치
      else sendPacketMRTEXE(52);  //원투
    }
    else if(ps2x.Button(PSB_CROSS))
    {
      if(shift == HIGH) sendPacketMRTEXE(49); //펀치
      else sendPacketMRTEXE(51);  //잽
    } 
    else if(ps2x.Button(PSB_SQUARE))
    {
     if(shift == HIGH) sendPacketMRTEXE(50); //스핀블로우
      else sendPacketMRTEXE(52);  //원투
    }
  }
  
  else if(mode==3)  //축구모드
  {
    if(ps2x.Button(PSB_SELECT))             sendPacketMRTEXE(55); //기본자세
    else if(ps2x.Button(PSB_PAD_UP))        
    {
      sendPacketMRTEXE(56); //뛰기 
      //sendPacketMRTEXE(57);  
      walkflag1=1;
    }
    else if(ps2x.Button(PSB_PAD_RIGHT))     
    {
      if(shift == HIGH) sendPacketMRTEXE(63); //우회전
      else sendPacketMRTEXE(61); //오른쪽이동
    }
    else if(ps2x.Button(PSB_PAD_LEFT))
    {
      if(shift == HIGH) sendPacketMRTEXE(62); //좌회전
      else sendPacketMRTEXE(60); //좌로이동   
    }
    else if(ps2x.Button(PSB_PAD_DOWN))
    {
      sendPacketMRTEXE(58); //뒤로뛰기  
      //sendPacketMRTEXE(59); 
      walkflag2=1;
    }
    else if(ps2x.Button(PSB_L2))
    {
      if(ps2x.Button(PSB_R2))  sendPacketMRTEXE(55);     //정지   
    }
    else if(ps2x.Button(PSB_R2))    
    {
      if(ps2x.Button(PSB_L2))  sendPacketMRTEXE(55);   //정지
    }
    else if(ps2x.Button(PSB_L1))
    {
      if(ps2x.Button(PSB_TRIANGLE)) sendPacketMRTEXE(73);   //앞으로 일어나기
    }
    else if(ps2x.Button(PSB_R1))
    {
      if(ps2x.Button(PSB_PAD_UP))  sendPacketMRTEXE(74);  //뒤로 일어나기
    }
    else if(ps2x.Button(PSB_TRIANGLE))
    {
      if(shift == HIGH) sendPacketMRTEXE(68);//왼쪽 강한슛
      else sendPacketMRTEXE(66);  // 왼쪽 슛
    }
    else if(ps2x.Button(PSB_CIRCLE))
    {
      if(shift == HIGH) sendPacketMRTEXE(72); //가운데 막기
      else sendPacketMRTEXE(71);  // 오른쪽 막기
    }
    else if(ps2x.Button(PSB_CROSS))
    {
      if(shift == HIGH) sendPacketMRTEXE(69); //오른쪽 강한슛
      else sendPacketMRTEXE(67);  //오른쪽 슛
    } 
    else if(ps2x.Button(PSB_SQUARE))
    {
     if(shift == HIGH) sendPacketMRTEXE(72); //가운데 막기
      else sendPacketMRTEXE(70);  //왼쪽막기
    }
    else if(walkflag1==1)   //앞으로가다 발 제자리
    {
      delay(260);
      sendPacketMRTEXE(57);
      walkflag1=0;
    }
    else if(walkflag2==1)   //뒤로가다 발 제자리
    {
      delay(260);
      sendPacketMRTEXE(59);
      walkflag2=0;
    }
    //else if(ps2x.Button(PSB_R1))      //r1 test
    //{
    //  sendPacketMRTEXE(54);
    //}
  }


  else if(mode==4) //미션1모드
  {
    if(ps2x.Button(PSB_CROSS))
    {
      if(mission2flag==0)  //미션2 모드로 변경
      {
        mission2++;
        mission2flag=1;
        if(mission2 > 1) mission2=0;
      }
      delay(250);
      if(mission2==0) sendPacketMRTEXE(101);
      else if(mission2==1) sendPacketMRTEXE(97);
    }
    else if(mission2flag==1) mission2flag=0;
    
    if(ps2x.Button(PSB_SELECT))             
    {
      if(mission2==1) sendPacketMRTEXE(97); //엎드리기
      else sendPacketMRTEXE(75); //기본자세
    }
    else if(ps2x.Button(PSB_PAD_UP))        
    {
      if(mission2==1)
      {
        sendPacketMRTEXE(98); //엎드려 앞으로가기
      }
      else
      {
        if(ps2x.Button(PSB_CIRCLE)) sendPacketMRTEXE(85); //주운 상태에서 앞으로
        else sendPacketMRTEXE(76); //앞으로 
        //sendPacketMRTEXE(77);  
        walkflag1==1;
      }     
    }
    else if(ps2x.Button(PSB_PAD_RIGHT))     
    {
      if(mission2==1)
      {
        sendPacketMRTEXE(100); //엎드려 우로
      }
      else if(ps2x.Button(PSB_CIRCLE)) 
      {
        if(shift == HIGH) sendPacketMRTEXE(92); //주운 상태로 우회전
        else sendPacketMRTEXE(90);  //주운 상태에서 우로
      }
      else if(shift == HIGH) sendPacketMRTEXE(83);  //우회전
      else sendPacketMRTEXE(81); //오른쪽이동
    }
    else if(ps2x.Button(PSB_PAD_LEFT))
    {
      if(mission2==1)
      {
        sendPacketMRTEXE(99);   //엎드려 좌로
      }
      else if(ps2x.Button(PSB_CIRCLE))
      { 
        if(shift == HIGH) sendPacketMRTEXE(91); //주운상태로 좌회전  
        else sendPacketMRTEXE(89); //주운 상태에서 좌로
      }
      else if(shift == HIGH) sendPacketMRTEXE(82);  //좌회전
      else sendPacketMRTEXE(80);  //좌로이동   
    }
    else if(ps2x.Button(PSB_PAD_DOWN))
    {
      if(mission2==1)sendPacketMRTEXE(98); //엎드려 앞으로
      else
      {
        if(shift == HIGH) sendPacketMRTEXE(87);   //주은 상태로 뒤로
        else sendPacketMRTEXE(78); //뒤로      
        //sendPacketMRTEXE(79); 
        walkflag2==1;
      }
    }
    else if(ps2x.Button(PSB_L2))
    {
      if(mission2==1)sendPacketMRTEXE(97);     //엎드려 정지
      else if(ps2x.Button(PSB_R2))  sendPacketMRTEXE(75);     //정지   
    }
    else if(ps2x.Button(PSB_R2))    
    {
      if(mission2==1)sendPacketMRTEXE(98);    //엎드려 정지
      else if(ps2x.Button(PSB_L2))  sendPacketMRTEXE(75);   //정지
    }
    else if(ps2x.Button(PSB_L1))
    {
      
      if(ps2x.Button(PSB_TRIANGLE))
      {
        if(mission2==1) delay(100);
        else sendPacketMRTEXE(95);    //앞으로 일어나기
      }
    }
    else if(ps2x.Button(PSB_R1))
    {
      if(ps2x.Button(PSB_PAD_UP))
      {
        if(mission2==1) delay(100);
        else sendPacketMRTEXE(96);    //뒤로 일어나기
      }
    }
    else if(ps2x.Button(PSB_TRIANGLE))
    {
      if(mission2==1) delay(100);
      else if(shift == HIGH) sendPacketMRTEXE(84);    //줍기
      else    sendPacketMRTEXE(84);   //줍기
    }
/*    else if(ps2x.Button(PSB_CIRCLE))
    {
      if(ps2x.Button(PSB_PAD_UP)) sendPacketMRTEXE(85); //주운 상태에서 앞으로
      if(ps2x.Button(PSB_PAD_RIGHT)) sendPacketMRTEXE(90); // 주운상태에서 오른쪽
      if(ps2x.Button(PSB_PAD_LEFT)) sendPacketMRTEXE(89); //주운상태에서 왼쪽
      if(ps2x.Button(PSB_PAD_DOWN)) sendPacketMRTEXE(87); //주운상태에서 뒤로
    }
*/
    else if(ps2x.Button(PSB_SQUARE))
    {
      if(mission2==1) delay(100);
      else if(shift == HIGH) sendPacketMRTEXE(93);    //놓기
      else    sendPacketMRTEXE(94);   //놓기
    }
    else if(walkflag1==1)
    {
      delay(300);
      sendPacketMRTEXE(77);   //앞으로가다 제자리
      walkflag1=0;
    }
    else if(walkflag2==1)
    {
      delay(300);
      sendPacketMRTEXE(79);   //뒤로가다 제자리
      walkflag2=0;
    }
  }

  
  else if(mode==5)  //하키 모드
  {
    if(ps2x.Button(PSB_SELECT))             sendPacketMRTEXE(102); //기본자세
    else if(ps2x.Button(PSB_PAD_UP))        
    {
      sendPacketMRTEXE(103); //앞으로 
      //sendPacketMRTEXE(104);  
      walkflag1=1;
    }
    else if(ps2x.Button(PSB_PAD_RIGHT))     
    {
      if(shift == HIGH) sendPacketMRTEXE(110); //우회전
      else sendPacketMRTEXE(108); //오른쪽이동
      walkflag2=1;
    }
    else if(ps2x.Button(PSB_PAD_LEFT))
    {
      if(shift == HIGH) sendPacketMRTEXE(109); //좌회전
      else sendPacketMRTEXE(107); //좌로이동   
    }
    else if(ps2x.Button(PSB_PAD_DOWN))
    {
      sendPacketMRTEXE(105); //뒤로
      //sendPacketMRTEXE(106); 
      walkflag2=1;
    }
    else if(ps2x.Button(PSB_L2))
    {
      if(ps2x.Button(PSB_R2))  sendPacketMRTEXE(102);     //정지   
    }
    else if(ps2x.Button(PSB_R2))    
    {
      if(ps2x.Button(PSB_L2))  sendPacketMRTEXE(102);   //정지
    }
    else if(ps2x.Button(PSB_L1))
    {
      if(ps2x.Button(PSB_TRIANGLE)) sendPacketMRTEXE(113);   //앞으로 일어나기
    }
    else if(ps2x.Button(PSB_R1))
    {
      if(ps2x.Button(PSB_PAD_UP))  sendPacketMRTEXE(114);  //뒤로 일어나기
    }
    else if(ps2x.Button(PSB_TRIANGLE))
    {
      if(shift == HIGH) sendPacketMRTEXE(111);//왼쪽 슛
      else sendPacketMRTEXE(111);  // 왼쪽 슛
    }
    else if(ps2x.Button(PSB_CIRCLE))
    {
      if(shift == HIGH) sendPacketMRTEXE(112); //오른쪽 슛
      else sendPacketMRTEXE(112);  // 오른쪽 슛
    }
    else if(ps2x.Button(PSB_CROSS))
    {
      if(shift == HIGH) sendPacketMRTEXE(112); //오른쪽 슛
      else sendPacketMRTEXE(112);  //오른쪽 슛
    } 
    else if(ps2x.Button(PSB_SQUARE))
    {
     if(shift == HIGH) sendPacketMRTEXE(111); // 왼쪽 슛
      else sendPacketMRTEXE(111);  // 왼쪽 슛
    }
    else if(walkflag1==1)   //앞으로가다 발 제자리
    {
      delay(260);
      sendPacketMRTEXE(104);
      walkflag1=0;
    }
    else if(walkflag2==1)   //뒤로가다 발 제자리
    {
      delay(260);
      sendPacketMRTEXE(106);
      walkflag2=0;
    }
  }
}    
/////////////////////////////////////////////////////////////////////////////////////////// MRT_END
  
void processROBOLIFE()
{
    shift = digitalRead(shiftButton);
    ctrl = digitalRead(ctrlButton);
    
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))   
    {
      if(shift == HIGH) sendPacketROBOLIFE('Y');
      else              sendPacketROBOLIFE('y');  
    }
    else if(ps2x.Button(PSB_SELECT))
    {
      if(shift == HIGH) sendPacketROBOLIFE('Z');
      else              sendPacketROBOLIFE('z');    
    }

    else if(ps2x.Button(PSB_PAD_UP)) {  
      if(shift == HIGH)     sendPacketROBOLIFE('K');
      else if(ctrl == HIGH) sendPacketROBOLIFE('0'); // zero
      else                  sendPacketROBOLIFE('k');
    }
    else if(ps2x.Button(PSB_PAD_RIGHT)){
      if(shift == HIGH)     sendPacketROBOLIFE('L');
      else if(ctrl == HIGH) sendPacketROBOLIFE('1'); // 1
      else                  sendPacketROBOLIFE('l');   
    }
    else if(ps2x.Button(PSB_PAD_LEFT)){
      if(shift == HIGH)     sendPacketROBOLIFE('N');
      else if(ctrl == HIGH) sendPacketROBOLIFE('3');
      else                  sendPacketROBOLIFE('n');    
    }
    else if(ps2x.Button(PSB_PAD_DOWN)){
      if(shift == HIGH)     sendPacketROBOLIFE('M');
      else if(ctrl == HIGH) sendPacketROBOLIFE('2');
      else                  sendPacketROBOLIFE('m');    
    }   

    else if(ps2x.Button(PSB_L3))
    {
      if(shift == HIGH) sendPacketROBOLIFE('W');
      else              sendPacketROBOLIFE('w');  
    }
    else if(ps2x.Button(PSB_R3))
    {
      if(shift == HIGH) sendPacketROBOLIFE('I');
      else              sendPacketROBOLIFE('i');  
    }
    else if(ps2x.Button(PSB_L2))         
    {
      if(shift == HIGH)     sendPacketROBOLIFE('O');
      else if(ctrl == HIGH) sendPacketROBOLIFE('9');
      else                  sendPacketROBOLIFE('o');  
    }
    else if(ps2x.Button(PSB_R2))      
    {
      if(shift == HIGH)     sendPacketROBOLIFE('P');
      else if(ctrl == HIGH) sendPacketROBOLIFE('b');
      else                  sendPacketROBOLIFE('p');
    }
    else if(ps2x.Button(PSB_L1))         
    {
      if(shift == HIGH)     sendPacketROBOLIFE('Q');
      else if(ctrl == HIGH) sendPacketROBOLIFE('8');
      else                  sendPacketROBOLIFE('q');  
    }
    else if(ps2x.Button(PSB_R1))      
    {
      if(shift == HIGH)     sendPacketROBOLIFE('R');
      else if(ctrl == HIGH) sendPacketROBOLIFE('a');
      else                  sendPacketROBOLIFE('r');
    }
    else if(ps2x.Button(PSB_TRIANGLE))      
    {
      if(shift == HIGH)     sendPacketROBOLIFE('S');
      else if(ctrl == HIGH) sendPacketROBOLIFE('4');
      else                  sendPacketROBOLIFE('s');
    }

    else if(ps2x.Button(PSB_CIRCLE))         
    {
      if(shift == HIGH)     sendPacketROBOLIFE('T');
      else if(ctrl == HIGH) sendPacketROBOLIFE('5');
      else                  sendPacketROBOLIFE('t');    
    }
    else if(ps2x.Button(PSB_CROSS)) 
    {
      if(shift == HIGH)     sendPacketROBOLIFE('U');
      else if(ctrl == HIGH) sendPacketROBOLIFE('6');
      else                  sendPacketROBOLIFE('u');    
    }
    else if(ps2x.Button(PSB_SQUARE))
    {
      if(shift == HIGH)     sendPacketROBOLIFE('V');
      else if(ctrl == HIGH) sendPacketROBOLIFE('7');
      else                  sendPacketROBOLIFE('v');    
    }
    ////////////////////////////////////////
    else if(ps2x.Analog(PSS_LY) < joy_down)
    {
      if(shift == HIGH) sendPacketROBOLIFE('D');
      else              sendPacketROBOLIFE('d');  
    }
    else if(ps2x.Analog(PSS_LY) > joy_up)
    {
      if(shift == HIGH) sendPacketROBOLIFE('C');
      else              sendPacketROBOLIFE('c');  
    }
    
    else if(ps2x.Analog(PSS_LX) < joy_down)
    {
      if(shift == HIGH) sendPacketROBOLIFE('H');
      else              sendPacketROBOLIFE('h');  
    }
    else if(ps2x.Analog(PSS_LX) > joy_up)
    {
      if(shift == HIGH) sendPacketROBOLIFE('G');
      else              sendPacketROBOLIFE('g');  
    }
    //////////////////////////////////////////////////////////
    else if(ps2x.Analog(PSS_RY) < joy_down)
    {
      if(shift == HIGH) sendPacketROBOLIFE('B');
      else              sendPacketROBOLIFE('b'); 
    }
    else if(ps2x.Analog(PSS_RY) > joy_up)
    { 
      if(shift == HIGH) sendPacketROBOLIFE('A');
      else              sendPacketROBOLIFE('a');  
    }
    
    else if(ps2x.Analog(PSS_RX) < joy_down)
    {
      if(shift == HIGH) sendPacketROBOLIFE('F');
      else              sendPacketROBOLIFE('f');  
    }
    else if(ps2x.Analog(PSS_RX) > joy_up)
    {
      if(shift == HIGH) sendPacketROBOLIFE('E');
      else              sendPacketROBOLIFE('e');  
    }
}
void processROBOTIS()
{
  int txPacket = 0x00;
    ps2x.read_gamepad(false, vibrate); 
    
    if(ps2x.Button(PSB_START))   
    {}
    if(ps2x.Button(PSB_SELECT))
    {}

    if(ps2x.Button(PSB_PAD_UP)) 
      txPacket |= 0x01;
    if(ps2x.Button(PSB_PAD_RIGHT))
      txPacket |= 0x08;
    if(ps2x.Button(PSB_PAD_LEFT))      
      txPacket |= 0x04;
    if(ps2x.Button(PSB_PAD_DOWN))
      txPacket |= 0x02;

    if(ps2x.Button(PSB_L3))
    {}
    if(ps2x.Button(PSB_R3))
    {}
    if(ps2x.Button(PSB_L2))           
      txPacket |= 0x200;
    if(ps2x.Button(PSB_R2))  
      txPacket |= 0x100;
    if(ps2x.Button(PSB_L1))        
      txPacket |= 0x200;
    if(ps2x.Button(PSB_R1))   
      txPacket |= 0x100;
      
    if(ps2x.Button(PSB_TRIANGLE))  
      txPacket |= 0x10;
    if(ps2x.Button(PSB_CIRCLE))  
      txPacket |= 0x80;
    if(ps2x.Button(PSB_CROSS)) 
      txPacket |= 0x40;
    if(ps2x.Button(PSB_SQUARE))
      txPacket |= 0x20;
    ////////////////////////////////////////
    if(ps2x.Analog(PSS_LY) < joy_down)
    {}
    if(ps2x.Analog(PSS_LY) > joy_up)
    {}
    
    if(ps2x.Analog(PSS_LX) < joy_down)
    {}
    if(ps2x.Analog(PSS_LX) > joy_up)
    {}
    //////////////////////////////////////////////////////////
    if(ps2x.Analog(PSS_RY) < joy_down)
    {}
    if(ps2x.Analog(PSS_RY) > joy_up)
    {}
    
    if(ps2x.Analog(PSS_RX) < joy_down)
    {}
    if(ps2x.Analog(PSS_RX) > joy_up)
    {}
    
    if(txPacket != 0x00) 
      sendPacketROBOTIS(txPacket);
}
void processRCCAR()
{
    byte txPacket = 0x00;
    ps2x.read_gamepad(false, vibrate); 
    if(modeRcCar == 0)
    {
      if(ps2x.Button(PSB_START))   
        txPacket = 0x10;
      else if(ps2x.Button(PSB_SELECT))
        txPacket = 0x0D;
  
      else if(ps2x.Button(PSB_PAD_UP)) 
        txPacket = 0x01;
      else if(ps2x.Button(PSB_PAD_RIGHT))
        txPacket = 0x02;
      else if(ps2x.Button(PSB_PAD_LEFT))      
        txPacket = 0x04;
      else if(ps2x.Button(PSB_PAD_DOWN))
        txPacket = 0x03;
        
      else if(ps2x.Button(PSB_L2))           
        txPacket = 0x09;
      else if(ps2x.Button(PSB_R2))  
        txPacket = 0x0A;
      else if(ps2x.Button(PSB_L1))        
        txPacket = 0x0B;
      else if(ps2x.Button(PSB_R1))   
        txPacket = 0x0C;
        
      else if(ps2x.Button(PSB_TRIANGLE))  
        txPacket = 0x05;
      else if(ps2x.Button(PSB_CIRCLE))  
        txPacket = 0x06;
      else if(ps2x.Button(PSB_CROSS)) 
        txPacket = 0x07;
      else if(ps2x.Button(PSB_SQUARE))
        txPacket = 0x08;
  
      sendPacketRCCAR(txPacket);
      
      if(txPacket == 0x0D)
        modeCnt ++;
       
        
      if(modeCnt > 15)
      {
        modeRcCar = 1;
         modeCnt = 0;
      }

      
    }
    else if(modeRcCar == 1)
    {
      
      if(ps2x.ButtonPressed(PSB_START))   
        txPacket = 0x10;
      else if(ps2x.ButtonPressed(PSB_SELECT))
        txPacket = 0x0D;
        
  
      else if(ps2x.ButtonPressed(PSB_PAD_UP)) 
        txPacket = 0x01;
      else if(ps2x.ButtonPressed(PSB_PAD_RIGHT))
        txPacket = 0x02;
      else if(ps2x.ButtonPressed(PSB_PAD_LEFT))      
        txPacket = 0x04;
      else if(ps2x.ButtonPressed(PSB_PAD_DOWN))
        txPacket = 0x03;
        
      else if(ps2x.ButtonPressed(PSB_L2))           
        txPacket = 0x09;
      else if(ps2x.ButtonPressed(PSB_R2))  
        txPacket = 0x0A;
      else if(ps2x.ButtonPressed(PSB_L1))        
        txPacket = 0x0B;
      else if(ps2x.ButtonPressed(PSB_R1))   
        txPacket = 0x0C;
        
      else if(ps2x.ButtonPressed(PSB_TRIANGLE))  
        txPacket = 0x05;
      else if(ps2x.ButtonPressed(PSB_CIRCLE))  
        txPacket = 0x06;
      else if(ps2x.ButtonPressed(PSB_CROSS)) 
        txPacket = 0x07;
      else if(ps2x.ButtonPressed(PSB_SQUARE))
        txPacket = 0x08;
      if(txPacket != 0x00) // descrete
      {
        sendPacketRCCAR(txPacket);
        sendPacketRCCAR(0x00);
      }  
      
      if(ps2x.Button(PSB_SELECT))
        txPacket = 0xFF;
      
      if(txPacket == 0xFF)
        modeCnt ++;
       
        
      if(modeCnt > 15)
      {
         modeCnt = 0;
         modeRcCar = 0;
      }
    }
  
}

void loop() {
  if     (controlFlag == MRT)       processMRTV1();    
  else if(controlFlag == ROBOLIFE)  processROBOLIFE();  
  else if(controlFlag == ROBOTIS)   processROBOTIS();    
  else if(controlFlag == RCCAR)     processRCCAR();      
  dispOLEDText(pMaker,0x00,0x00);
}
