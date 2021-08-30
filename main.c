/*
* microprcess_project.c
*
* Created: 2021-05-12 오전 8:51:42
* Author : user
*/
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

void COMMAND(unsigned char byte);
void DATA(unsigned char byte);
void LCD_INIT(void);
void STRING(unsigned char font[], unsigned char n);
void MOVE(int y, int x);
void WIN();
void GAMEOVER();
void GAMESTART();
void baseball();
void FNDset();

unsigned char buff = 0x00;
unsigned char string[] = "";
int game_st = 0;
int namber = 0;
unsigned int ran[] = {0,0,0,0};
unsigned int num[] = {0,0,0,0};
unsigned char fnd[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
int strike = 0 , ball =0, out = 0;


int main(void)
{
	DDRA = 0x00;
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;
	DDRE = 0x0F;
	PORTA = 0xFF;
	PORTC = 0x00;
	PORTD = 0x00;
	
	LCD_INIT();


	while(1){
		if((PINA&0x01) == 0x00){
			GAMESTART();
		}
		if(buff==0xFF && game_st == 1){
			GAMEOVER();
			game_st = 0;
		}
		
			FNDset();
		
		if((PINA&0x04) == 0x00 && game_st == 1){
			if(num[0] == 9) num[0] = 0;
			else num[0]++;
			while((PINA&0x04) == 0x00){
				FNDset();
			}
			
		}
		if((PINA&0x08) == 0x00 && game_st == 1){
			if(num[1] == 9) num[1] = 0;
			else num[1]++;
			while((PINA&0x08) == 0x00){
				FNDset();
			}
			
		}
		if((PINA&0x10) == 0x00 && game_st == 1){
			if(num[2] == 9) num[2] = 0;
			else num[2]++;
			while((PINA&0x10) == 0x00){
				FNDset();
			}
			
		}
		if((PINA&0x20) == 0x00 && game_st == 1){
			if(num[3] == 9) num[3] = 0;
			else num[3]++;
			while((PINA&0x20) == 0x00){
				FNDset();
			}
			
		}
		
		if((PINA&0x02) == 0x00 && game_st == 1) {
			baseball();
			if(strike == 4){
				WIN();
			}
			strike = 0;
			ball = 0;
			out = 0;
			
				buff = buff << 1;
				buff++;
		}
		PORTD = buff;
		
		
	}
}

void LCD_INIT(void)
{
	_delay_ms(30);	// 전원 투입 후 30ms 이상 지연
	
	//Function set
	COMMAND(0b00101000);
	// 인터페이스(DL)=0(4bit), 라인(N)=1(2라인), 폰트(F)=0(5*8 dot)
	_delay_us(39);	// 39us 이상 지연

	//Display ON/OFF Control
	COMMAND(0b00001100);
	// 화면 표시(D)=1(on), 커서(C)=0(off), 블링크(B)=0(off)
	_delay_us(39);	// 39us 이상 지연

	//Clear Display
	COMMAND(0b00000001);
	// 화면을 클리어하고 , 커서가 홈위치인 0번지로 돌아감.
	_delay_ms(1.53);	// 1.53ms 이상 지연

	//Entry Mode Set
	COMMAND(0b00000110);
	// 커서방향(I/D)=1(address증가), 표시이동(S)=0(이동하지 않음)
}

void COMMAND(unsigned char byte)
{
	_delay_ms(2);

	PORTC=byte&0xf0;// 상위4비트 명령어 쓰기
	PORTC&=0b11111100;  // RS = 0, RW = 0, 명령어를 쓰도록 설정
	_delay_us(1);
	PORTC|=0b00000100; // E = 1, lcd 동작
	_delay_us(1);
	PORTC&=0b11111011;// E = 0

	PORTC=(byte<<4)&0xf0;// 하위4비트 명령어 쓰기
	PORTC&=0b11111100;// RS = 0, RW = 0, 명령어를 쓰도록 설정
	_delay_us(1);
	PORTC|=0b00000100; // E = 1, lcd 동작
	_delay_us(1);
	PORTC&=0b11111011;// E = 0
}


void DATA(unsigned char byte)
{
	_delay_ms(2);

	PORTC=byte&0xf0;// 상위4비트 명령어 쓰기
	PORTC |= 0b00000001;// RS = 1, 데이터 설정
	PORTC &= 0b11111101;// RW = 0, 쓰기 설정
	_delay_us(1);
	PORTC|=0b00000100; // E = 1, lcd 동작
	_delay_us(1);
	PORTC&=0b11111011;// E = 0

	PORTC=(byte<<4)&0xf0;// 하위4비트 데이터 쓰기
	PORTC |= 0b00000001;// RS = 1, 데이터 설정
	PORTC &= 0b11111101;// RW = 0, 쓰기 설정
	_delay_us(1);
	PORTC|=0b00000100; // E = 1, lcd 동작
	_delay_us(1);
	PORTC&=0b11111011;// E = 0
}

void STRING(unsigned char font[], unsigned char n)
{
	unsigned char i;
	for(i=0;i<n;i++)
	{
		DATA(font[i]);
	}
}

void MOVE(int y, int x)     //커서 이동
{
	unsigned char data;
	if(y==1) 	data=0x80+x-1;           //1행
	else     	data=0xc0+x-1;            //2행
	COMMAND(data);
}



void baseball(){
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if((ran[i] == num[j]) && i == j){
				strike++;
			}
			if((ran[i] == num[j]) && i != j){
				ball++;
			}
			if(ran[i] != num[j]){
				out++;
			}
		}
		
	}
	LCD_INIT();
	unsigned char string[] = "S:0 B:0";
	STRING(string, sizeof(string)-1);
	MOVE(1,3);
	DATA(strike+0x30);
	MOVE(1,7);
	DATA(ball+0x30);
	
	while((PINA&0x02) == 0x00){}
	_delay_ms(500);
	
}

void WIN(){
	LCD_INIT();
	unsigned char string[] = "YOU WIN";
	STRING(string, sizeof(string)-1);
	game_st = 0;
}

void GAMEOVER(){
	LCD_INIT();
	unsigned char string[] = "GAME OVER";
	STRING(string, sizeof(string)-1);
	game_st = 0;
	MOVE(2,1);
	DATA(ran[0]+0x30);
	MOVE(2,2);
	DATA(ran[1]+0x30);
	MOVE(2,3);
	DATA(ran[2]+0x30);
	MOVE(2,4);
	DATA(ran[3]+0x30);
}
void GAMESTART(){
	LCD_INIT();
	num[0] = 0;
	num[1] = 0;
	num[2] = 0;
	num[3] = 0;
	FNDset();
	unsigned char string[] = "GAME START";
	STRING(string, sizeof(string)-1);
	for(int i = 0; i < 4; i++){
		ran[i] = rand()%9+1;
		for(int j = 0; j < i; j++){
			if(ran[i] == ran[j]){
				i--;
				break;
			}
		}
		_delay_ms(1000);
		LCD_INIT();
		
		
		game_st = 1;
		buff = 0X00;
		
		
	}
}

void FNDset(){
	PORTE = 0x08;
	PORTB=fnd[num[0]];
	_delay_ms(1);
	PORTE = 0x04;
	PORTB=fnd[num[1]];
	_delay_ms(1);
	PORTE = 0x02;
	PORTB=fnd[num[2]];
	_delay_ms(1);
	PORTE = 0x01;
	PORTB=fnd[num[3]];
	_delay_ms(1);
}


