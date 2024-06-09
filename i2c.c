#include <pic.h>
#define _XTAL_FREQ 20000000

#define LCD PORTD
#define RS RB0
#define RW RB1
#define EN RB2

//RTC MACROS FOR DS1307
#define DS1307_address 0XD0

#define second 0x00
#define minute 0x01
#define hour 0x02
#define day 0x03
#define date 0x04
#define month 0x05

#define year 0x06
#define control_reg 0x07

void DS1307_write(char,char ,char ,char,char ,char,char);
void DS1307_read(char slave_address,char register_address);

char decimal_to_bcd(unsigned char value);
void bcd_to_ascii(unsigned char value);
void lcd_cmd(unsigned char cmd);
void lcd_init();
void lcd_data(unsigned char data);
void lcd_string(const unsigned char *words,int length);

unsigned char _sec,_min,_hr,_day,_date,_month,_year,_con;


void lcd_init()
{
lcd_cmd(0x38); //16*2
lcd_cmd(0x06); //increment 1 position
lcd_cmd(0x0C); //LCD ON CURSOR OFF
lcd_cmd(0x01);
 
}
void lcd_data(unsigned char data) //e
{
PORTD   =data;
RS=1;  //DATA=1;
RW=0;  //WRITE
EN=1;
__delay_ms(5); //delay(1000);
EN=0;
}

void lcd_cmd(unsigned char command) //0x38
{
PORTD=command; //0x38
RS=0;  //DATA=1; command=0;
RW=0;  //WRITE
EN=1;
__delay_ms(5);
EN=0;
}



void lcd_word(const char *string,int length)  //100 
{
unsigned char i;
for(i=0;i<length;i++)
{
lcd_data(string[i]); //
}
}

void main(void)
{
TRISC=0XFF; //INPUT OPEN DRAIN SO INPUT
SSPADD=49; //1000 = I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))  //10KHZ
SSPCON=0X28; //SSPENSYN SERIAL PORT PIN FOR I2C0010 1000(I2C MASTER
TRISD=0X00;
PORTD=0X00;
TRISB=0X00;
PORTB=0X00;
lcd_init();
lcd_cmd(0x80);
lcd_word("CLOCk",5);
lcd_cmd(0xC0);
lcd_word("DATE",4);
DS1307_write(3,2,1,1,5,4,20);
__delay_ms(300);

while(1)
{
__delay_ms(20);
DS1307_read(DS1307_address,0);
}
return;

}

char decimal_to_bcd(unsigned char value) //before passing
{
unsigned char msb,lsb,hex;
msb=value/10;
lsb=value%10;
hex=((msb<<4)+lsb);
return hex;  //bcd 
}

void bcd_to_ascii(unsigned char value)
{
unsigned char bcd;
bcd=value;
bcd=bcd&0xf0;
bcd=bcd>>4;
bcd=bcd|0x30;
lcd_data(bcd);
bcd=value;
bcd=bcd&0x0f;
bcd=bcd|0x30;
lcd_data(bcd);  //bcd to ascii
}


void DS1307_write(char _second,char _minute,char _hour,char _day,char _date,char _month,char _year) //DS1307_write(3,2,1,1,5,4,20);
{
//START BIT
SEN=1;
while(SEN);
SSPIF=0;
//SLAVE ADDRESS BITS
SSPBUF=DS1307_address; //send the slave address high and r/w=0 for write
while(!SSPIF); //wait for ack SSPIF IS SET for every 9th clock cycle
SSPIF=0; //clear SSPIF
if(ACKSTAT)
{
PEN=1; //STOP
while(PEN); //WAIT FOR STOP CONDITIN TO COMPLETE
return; //NO ACK EXIT WRITE
}
//starting adddress 0x00
SSPBUF=second; //SEND THE SLAVE ADDEESS HIGH AND WRITE =0
while(!SSPIF);
SSPIF=0;
if(ACKSTAT)
{
PEN=1;
while(PEN);
return;
}

SSPBUF=decimal_to_bcd(_second);
while(!SSPIF);
SSPIF=0;
SSPBUF=decimal_to_bcd(_minute);
while(!SSPIF);
SSPIF=0;

SSPBUF=decimal_to_bcd(_hour);
while(!SSPIF);
SSPIF=0;

SSPBUF=decimal_to_bcd(_day);
while(!SSPIF);
SSPIF=0;

SSPBUF=decimal_to_bcd(_date);
while(!SSPIF);
SSPIF=0;

SSPBUF=decimal_to_bcd(_month);
while(!SSPIF);
SSPIF=0;

SSPBUF=decimal_to_bcd(_year);
while(!SSPIF);
SSPIF=0;

SSPBUF=0x00;
while(!SSPIF);
SSPIF=0;


//STOP 
PEN=1;
while(PEN);
}

void DS1307_read(char slave_address,char register_address)  //DS1307_address,0
{

//start bit
SEN=1;
while(SEN);
SSPIF=0;

//SLAVE ADDRESS BITS 
SSPBUF= slave_address;
while(!SSPIF);
SSPIF=0;

if(ACKSTAT)
{
PEN=1;
while(PEN);
return;
}

SSPBUF=register_address; //0
while(!SSPIF);
SSPIF=0;
if(ACKSTAT)
{
PEN=1;
while(PEN);
return;
}
//repeatd start bit

RSEN=1;
while(RSEN);
SSPIF=0;

//SLAVE ADDDRESS BITS

SSPBUF=(slave_address +1);
while(!SSPIF);
SSPIF=0;
if(ACKSTAT)
{
PEN=1;
while(PEN);
return;
}
//receive enable bit
RCEN=1;
while(!BF);
_sec=SSPBUF; //


ACKDT=0; //PREPARE TO SEND NACK
ACKEN=1; //INITIATE TO SEND NACK
while(ACKEN);


//
RCEN=1;
while(!BF);
_hr=SSPBUF; //


ACKDT=0; //PREPARE TO SEND NACK
ACKEN=1; //INITIATE TO SEND NACK
while(ACKEN);

//RECEIVE ENABLE BIT
RCEN=1;
while(!BF);
_min=SSPBUF; //


ACKDT=0; //PREPARE TO SEND NACK
ACKEN=1; //INITIATE TO SEND NACK
while(ACKEN);


//date
RCEN=1;
while(!BF);
_day=SSPBUF; //


ACKDT=0; //PREPARE TO SEND NACK
ACKEN=1; //INITIATE TO SEND NACK
while(ACKEN);


RCEN=1;
while(!BF);
_date=SSPBUF; //


ACKDT=0; //PREPARE TO SEND NACK
ACKEN=1; //INITIATE TO SEND NACK
while(ACKEN);

RCEN=1;
while(!BF);
_month=SSPBUF; //


ACKDT=0; //PREPARE TO SEND NACK
ACKEN=1; //INITIATE TO SEND NACK
while(ACKEN);

RCEN=1;
while(!BF);
_year=SSPBUF; 


ACKDT=0; //PREPARE TO SEND NACK
ACKEN=1; //INITIATE TO SEND NACK
while(ACKEN);

RCEN=1;
while(!BF);
_con=SSPBUF; //


ACKDT=0; //PREPARE TO SEND NACK
ACKEN=1; //INITIATE TO SEND NACK
while(ACKEN);

PEN=1;
while(PEN);

lcd_cmd(0x87);
bcd_to_ascii(_hr);
lcd_data(':');
bcd_to_ascii(_min);
lcd_data(':');
bcd_to_ascii(_sec);
lcd_data(':');
lcd_cmd(0xc7);
bcd_to_ascii(_date);
lcd_data('/');
bcd_to_ascii(_month);
lcd_data('/');
bcd_to_ascii(_year);
lcd_data('/');

}

void delay(unsigned int delay)
{
while(delay--);
}























