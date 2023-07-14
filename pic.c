// --- Ligações entre PIC e LCD ---
sbit LCD_RS at RE2_bit;   // PINO 2 DO PORTD INTERLIGADO AO RS DO DISPLAY
sbit LCD_EN at RE1_bit;   // PINO 3 DO PORTD INTERLIGADO AO EN DO DISPLAY
sbit LCD_D7 at RD7_bit;  // PINO 7 DO PORTD INTERLIGADO AO D7 DO DISPLAY
sbit LCD_D6 at RD6_bit;  // PINO 6 DO PORTD INTERLIGADO AO D6 DO DISPLAY
sbit LCD_D5 at RD5_bit;  // PINO 5 DO PORTD INTERLIGADO AO D5 DO DISPLAY
sbit LCD_D4 at RD4_bit;  // PINO 4 DO PORTD INTERLIGADO AO D4 DO DISPLAY

// Selecionando direção de fluxo de dados dos pinos utilizados para a comunicação com display LCD
sbit LCD_RS_Direction at TRISE2_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 2 DO PORTD
sbit LCD_EN_Direction at TRISE1_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 3 DO PORTD
sbit LCD_D7_Direction at TRISD7_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 7 DO PORTD
sbit LCD_D6_Direction at TRISD6_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 6 DO PORTD
sbit LCD_D5_Direction at TRISD5_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 5 DO PORTD
sbit LCD_D4_Direction at TRISD4_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 4 DO PORTD

unsigned int  uiValorAD;     // Variavel que armazena o valor do A/D.
char ucRead;        // Variavel para armazenar o dado lido.

int inteiro;

                      //  "0"  "1"  "2"  "3"  "4"  "5"  "6"  "7"  "8"  "9"
unsigned char ucMask[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
/* Variavel do tipo vetor que armazena os valores correspondentes ao numero
para ser mostrado no display de 7 segmentos */

void display(inteiro){
      PORTD = ucMask[inteiro];
      PORTA.RA5 = 1;
      Delay_ms(2);
      PORTA.RA5 = 0;
      
      PORTD = ucMask[inteiro];
      PORTA.RA4 = 1;
      Delay_ms(2);
      PORTA.RA5 = 0;
      
      PORTD = ucMask[inteiro];
      PORTA.RA3 = 1;
      Delay_ms(2);
      PORTA.RA5 = 0;
      
      PORTD = ucMask[inteiro];
      PORTA.RA2 = 1;
      Delay_ms(2);
      PORTA.RA5 = 0;
}

void main(){
   TRISA.RA2=0;         // Define o pino RA2 do PORTA como saida(Sele??o Display 1).
   TRISA.RA3=0;         // Define o pino RA3 do PORTA como saida(Sele??o Display 2).
   TRISA.RA4=0;         // Define o pino RA4 do PORTA como saida(Sele??o Display 3).
   TRISA.RA5=0;         // Define o pino RA5 do PORTA como saida(Sele??o Display 4).

   TRISD = 0;
   
   TRISC.RC1 = 1;     // PORT C configurado como saída

   PORTA = 255;
   TRISA = 255;                 // Configura PORTA pins como entrada

   ADCON0 = 0b00000001;  // Configura conversor A/D Canal 0, conversão desligada, A/D ligado.
   ADCON1 = 0b11001110;  // Configura todos canais como Digital menos AN0 e REF Interna.
   ADCON2 = 0b10111110;  // Configura conversor A/D para resultado justificado a direita, clock de 20 TAD, clock de Fosc/64.

   PWM1_Init(5000);      // Inicializa módulo PWM com 5Khz
   PWM1_Start();         // Start PWM

   //ADCON1  = 0x0E;                           //Configura os pinos do PORTB como digitais, e RA0 (PORTA) como analógico

   Lcd_Init();                               //Inicializa módulo LCD
   Lcd_Cmd(_LCD_CURSOR_OFF);                 //Apaga cursor
   Lcd_Cmd(_LCD_CLEAR);                      //Limpa display

   UART1_Init(9600);  // Utiliza bibliotecas do compilador para configuração o Baud rate.

   while(1){
      display();
      uiValorAD= ADC_Read(0);     // Lê Canal AD 0
      uiValorAD*=0.24;            // Converte valor para o duty cycle
       if(UART1_Data_Ready()){  // Verifica se um dado foi recebido no buffer
         ucRead = UART1_Read(); // Lê o dado recebido do buffer.
         Delay_ms(50);
         if (ucRead == '0'){
           Lcd_Cmd(_LCD_CLEAR);
           lcd_out(1,1, "Tag lida.");
           PWM1_Set_Duty(0);
           inteiro = 0;
           display();
           Delay_ms(1000);
         }
         else if(ucRead == '1'){
           Lcd_Cmd(_LCD_CLEAR);
           lcd_out(1,1, "Tag cadastrada");
           lcd_out(2,1, "com sucesso!");
           PWM1_Set_Duty(0);
           inteiro = 1;
           display();
           Delay_ms(1000);
         }
         else if(ucRead == '2'){
           Lcd_Cmd(_LCD_CLEAR);
           lcd_out(1,1, "Tag nao");
           lcd_out(2,1, "cadastrada!");
           PWM1_Set_Duty(0);
           inteiro = 2;
           display();
           TRISC.RC1 = 0;
           //PORTC.RC1 = ~PORTC.RC1;  //inversão de estado
           Delay_ms(1000);
           //PORTC.RC1 = ~PORTC.RC1;
           TRISC.RC1 = 1;
         }
         else if(ucRead == '3'){
           Lcd_Cmd(_LCD_CLEAR);
           lcd_out(1,1, "Tag cadastrada.");
           PWM1_Set_Duty(uiValorAD);   // Envia o valor lido de "uiValorAD" para o módulo CCP1 pwm
           Delay_10us;
           inteiro = 3;
           display();
           Delay_ms(1000);
         }
       }
     }
}
