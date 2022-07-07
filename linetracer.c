#include <avr/io.h>
#include <avr/interrupt.h>

int MODE=0,adc,allblack,semiallblack,rturn,lturn,black,cnt,time1;
double r=700,l=700,speed;
int high[8], low[8];
int fline[8],line[8];
double turn[8]={-23,-8,-3,-1,1,3,8,233};
volatile double err,preerr;

void TIMER_Init(void);
void MOTER_Init(void);
void MOTER_GO(void);
void ADC_Init(void);
void ADC_Start(int pin);
void linetrace(int line[8]);
void INT_Init();
void LED(int line[8]);
ISR(TIMER0_OVF_vect)
{

   TCNT0=100;               //주기 10ms

   if(MODE==1)
   {
      time1++;
      black=0;
      for(int i=0;i<8;i++)
      {
         fline[i]=line[i];
         ADC_Start(i);
         float adcdata=((float)adc-(float)low[i])*100/((float)high[i]-(float)low[i]);   //정규화
         if (adcdata<50)
         {
            
            line[i]=1;
            black++;
         }
         else
         {
            line[i]=0;
         }
      }
      if(allblack==1)
      {
         if(semiallblack==1)
         {
            if(black<4)
            {
               allblack++;
               semiallblack=0;
               linetrace(line);
               l+=50;
               r+=50;
            }
         }
         else if(black>=8)   //전체다 블랙이면 직진
         {
            speed=700;
            semiallblack=1;
            l=0;
            r=0;
         }
         else if(black==0)
         {
            if(fline[7]==1)
            {
               speed=0;
               PORTE=0b00000101;
               l=500;
               r=600;
               line[7]=1;
            }
            else if (fline[0]==1)
            {
               speed=0;
               PORTE=0b00001010;
               l=600;
               r=500;
               line[0]=1;
            }
         }
         else
         {
            linetrace(line);
            r+=50;
            l+=50;
         }
         
      }
      else if(allblack==3&&lturn==1)
      {
         cnt++;
         if(line[3]==1&&line[0]==0&&line[1]==0&&line[2]==0&&cnt>15)
         {
            linetrace(line);
            lturn=0;
            cnt=0;
            allblack++;
         }
      }
      else if (allblack==3)
      {
         lturn=1;
         speed=0;
         PORTE=0b00001010;
         l=600;
         r=600;
      }
      else if (allblack==6)
      {
         PORTE=0b00000000;
         speed=0;
         r=0;
         l=0;
         cnt++;
         if(cnt==300)
         {
            PORTE=0b00000110;
            allblack++;
            cnt=0;
            speed=600;
            l=50;
            r=0;
         }
      }
      else if(allblack==8)
      {
         PORTE=0b00000101;
         speed=600;
         l=0;
         r=0;
         if(line[4]==0&&cnt==1)
         {
            cnt++;
         }
         else if(line[4]==1&&cnt==2)
         {
            linetrace(line);
            allblack++;
            cnt=0;
         }
         else if(line[4]==1&&cnt==0)
         {
            
            cnt++;
         }
      }
      else if(allblack==11)
      {
         speed=0;
         r=0;
         l=0;
         PORTE=0x00;
      }
      else
      {
         if(semiallblack==1)
         {
            if(black<5)
            {
               if(time1>5200)
               {
                  allblack++;
               }
               semiallblack=0;
               linetrace(line);
            }
         }
         else if(black==8)   //전체다 블랙이면 직진
         {
            semiallblack=1;
            speed=750;
            l=0;
            r=0;
            lturn=0;
            rturn=0;
            cnt=0;
         }
         else if(lturn==1)
         {
            speed=0;
            PORTE=0b00001010;
            l=600;
            r=600;
            if(line[7]==1)
            {
               PORTE=0b00000110;
               semiallblack=1;
               speed=750;
               l=0;
               r=0;
               lturn=0;
               rturn=0;
               cnt=0;
            }
            cnt++;
            if(line[3]==1&&line[0]==0&&line[1]==0&&line[2]==0&&cnt>15)
            {
               linetrace(line);
               lturn=0;
               cnt=0;
               speed-=100;
            }
         }
         else if (line[0]==1&&line[1]==1&&line[2]==1&&line[7]==0&&line[6]==0)
         {
            lturn=1;
            speed=0;
            PORTE=0b00001010;
            l=600;
            r=600;
         }
         else if(rturn==1)
         {
            rturn=1;
            speed=0;
            PORTE=0b00000101;
            l=600;
            r=600;
            if(line[0]==1)
            {
               PORTE=0b00000110;
               semiallblack=1;
               speed=750;
               l=0;
               r=0;
               lturn=0;
               rturn=0;
               cnt=0;
            }
            cnt++;
            if(line[4]==1&&line[7]==0&&line[6]==0&&line[5]==0&&cnt>15)
            {
               linetrace(line);
               rturn=0;
               cnt=0;
               speed-=100;
            }
         }
         else if (line[7]==1&&line[6]==1&&line[5]==1&&line[0]==0&&line[1]==0)
         {
            rturn=1;
            speed=0;
            PORTE=0b00000101;
            l=600;
            r=600;
            
         }
         else if(black==0)
         {
            if(fline[7]==1)
            {
               speed=0;
               PORTE=0b00000101;
               l=500;
               r=600;
               line[7]=1;
            }
            else if (fline[0]==1)
            {
               speed=0;
               PORTE=0b00001010;
               l=600;
               r=500;
               line[0]=1;
            }
         }
         else
         {
            linetrace(line);
         }
      }
      if(time1<2800||allblack==4||allblack==9||allblack==8)
      {
         l-=50;
         r-=50;
      }
      else if(time1>4500&&time1<4700)
      {
         l-=150;
         r-=150;
      }
      else if(time1>5300&&allblack==0)
      {
         l-=50;
         r-=50;
      }
      else if(allblack==7||allblack==2)
      {
         l-=100;
         r-=100;
      }
      PORTA=~allblack;
      //LED(line);
      MOTER_GO();
      if(time1<2700||allblack==4|allblack==9||allblack==8)
      {
         l+=50;
         r+=50;
      }
      else if(time1>4000&&time1<4500)
      {
         l+=50;
         r+=50;
      }
      else if(time1>4900&&allblack==0)
      {
         l+=150;
         r+=150;
      }
      else if(allblack==7||allblack==2)
      {
         l+=100;
         r+=100;
      }
      preerr=err;
      
   }
   else if (MODE==0)
   {
      for(int i=0;i<8;i++)
      {
         ADC_Start(i);
         if(adc>high[i])
         {
            high[i]=adc;
         }
         if(adc<low[i])
         {
            low[i]=adc;
         }
      }
   }

}
ISR(INT0_vect)
{
   MODE=1;
   allblack=0;
   PORTE=0b00000110;
}
ISR(INT1_vect)
{
   PORTE=0b00000000;
   PORTA=0x00;
   MODE=0;
   for(int i=0;i<8;i++)
   {
      high[i]=0;
      low[i]=1023;
   }
}

int main(void)
{
   DDRA=0xFF;
   DDRB=0b01100000;   //pwm출력
   DDRD=0b00000000;   //switch입력
   DDRF=0b00000000;   //adc입력
   DDRE=0b00001111;   //모터 방향 출력
   PORTA=0xFF;
   TIMER_Init();
   MOTER_Init();
   ADC_Init();
   INT_Init();
   sei();
    while(1)
    {
        
    }
}

void ADC_Init()
{
   ADMUX=0b01000000;   //기준전압을 avcc
   ADCSRA=0b10000111;   //adc 활성화 샘플링 128
}
void ADC_Start(int pin)
{
   ADMUX=0b01000000|pin;   //data 을 adc변환핀설정 기준 전압을 avcc
   ADCSRA|=0b01000000;   //변환 시작
   while(!(ADCSRA&(1<<ADIF)));
   adc=ADC;
}
void TIMER_Init()
{
   TCCR0=0b00000111;   //nomal모드 분주비 256
   TIMSK=0b00000001;   //interrupt 활정화
   TCNT0=100;         //주기마다 10ms
}
void MOTER_Init()
{
   TCCR1A=0b10100010;   //oc1a compare match clear, bottom set
   TCCR1B=0b00011001;   //fast pwm mode prescaler 1
   ICR1=799;         //20000hz
}
void MOTER_GO()
{
   int ra=(int)(speed+r);
   int lb=(int)(speed+l);
   if(ra<0&&lb<0)
   {
      PORTE=0b00001001;
      ra=-ra;
      lb=-lb;
   }
   else if(ra<0)
   {
      PORTE=0b00000101;
      ra=-ra;
   }
   else if(lb<0)
   {
      PORTE=0b00001010;
      lb=-lb;
   }
   OCR1A=ra;
   OCR1B=lb;
}
void linetrace(int line[8])
{
   err=0;
   PORTE=0b00000110;
   r=0;
   l=0;
   for(int i=0;i<8;i++)
   {
      if(line[i]==1)         //검은색
      {
         err=turn[i];      
      }
   }
   if(black==2)
   {
      err/=2;
   }
   l=err*12+(err-preerr)*30;
   r=-err*12-(err-preerr)*30;
   if(r>=l)
   {
      speed=700-r;
   }
   else
   {
      speed=700-l;
   }
   
}
void INT_Init()
{
   EIMSK=0b00000011;   //0,1핀 인터럽트 활성화
   EICRA=0b00001111;   //0,1핀 rising edge 설정
}
void LED(int line[8])
{
   int led=0;
   for(int i=0;i<8;i++)
   {
      led|=(line[i]<<i);
   }
   PORTA=~led;
}