#define IOC_flag   INTCON.IOCIF  
#define seg_flag   IOCCF.f5  

unsigned char segundo, minuto, hora, dias_semana, dia, mes, ano, dois_pontos, seg_dez, min_dez, hora_dez, dia_dez, mes_dez, ano_dez;

void Inicia_relogio(){
   TRISC.f5 = 1;          // Define RC5 como entrada
   IOCCP.f5 = 1;          // Ativa interrup��o na subida em RC5
   IOCCN.f5 = 1;          // Ativa interrup��o na descida em RC5
   INTCON.IOCIE = 1;      // Ativa chave geral IOC (Interrupt-on-change)
   dois_pontos = 1;       // Estado inicial aceso, na piscada dos dois pontos
   I2C1_Init(100000);     // iniciliza I2C com frequencia de 100KHz
}

void leitura_rtc() {
   INTCON.GIE = 0;        //desliga chave geral das interrup��es
   I2C1_Start();          //inicializa comunica��o i2c
   I2C1_Wr(0xD0);         //endere�o fixo para a categoria do DS1307: 1101000X, onde x = 0 para grava��o, e X = 1 para leitura.
   I2C1_Wr(0);            //endere�o onde iremos come�ar a programa��o do rel�gio, neste caso � o endere�o dos segundos.
   I2C1_Repeated_Start(); // Come�o repetido sinal da edi��o I2C
   I2C1_Wr(0xD1);         //endere�o fixo para a categoria do DS1307: 1101000X com o ultimo bit acionado (1) para leitura
   segundo = I2C1_Rd(1);  //l� primeiro byte segundos e informa que ainda leremos mais dados atrav�s do sinalizador (1)
   minuto = I2C1_Rd(1);   //l� segundo byte minutos e informa que ainda leremos mais dados atrav�s do sinalizador (1)
   hora = I2C1_Rd(1);     //l� terceiro byte hora e encerra as leituras de dados, gra�as ao sinalizador (0)
   dias_semana = I2C1_Rd(1);
   dia = I2C1_Rd(1);
   mes = I2C1_Rd(1);
   ano = I2C1_Rd(0);
   I2C1_Stop();           //finaliza comunica��o I2C
   seg_dez = segundo/16;
   min_dez = minuto/16;
   hora_dez = hora/16;
   dia_dez = dia/16;
   mes_dez = mes/16;
   ano_dez = ano/16;
   segundo = segundo-seg_dez*16;
   minuto = minuto-min_dez*16;
   hora = hora-hora_dez*16;
   dia = dia-dia_dez*16;
   if(dia_dez==0) dia_dez=100;
   mes = mes-mes_dez*16;
   if(mes_dez==0) mes_dez=100;
   ano = ano-ano_dez*16;
   INTCON.GIE = 1;        //religa chave geral das interrup��es
}

void acerta_relogio(unsigned char p1,unsigned char p2,unsigned char p3,unsigned char p4,unsigned char p5,unsigned char p6,unsigned char p7){
   INTCON.GIE = 0;        //desliga chave geral das interrup��es
   segundo = p1;
   minuto = p2;
   hora = p3;
   dia = p4;
   mes = p5;
   ano = p6;
   seg_dez = segundo/10;
   min_dez = minuto/10;
   hora_dez = hora/10;
   dia_dez = dia/10;
   mes_dez = mes/10;
   ano_dez = ano/10;
   segundo = segundo-seg_dez*10;
   minuto = minuto-min_dez*10;
   hora = hora-hora_dez*10;
   dia = dia-dia_dez*10;
   mes = mes-mes_dez*10;
   ano = ano-ano_dez*10;   
   I2C1_Start();                //inicializa a comunica��o I2c
   I2C1_Wr(0xD0);               //endere�o fixo para a categoria do DS1307: 1101000X, onde x = 0 � para grava��o, e X = 1 para leitura.
   I2C1_Wr(0);                  //endere�o onde iremos come�o a programa��o do rel�gio, neste caso � endere�o dos segundos.
   I2C1_Wr(seg_dez*16+segundo); //inicializa a contagem dos segundos apartir de 04.
   I2C1_Wr(min_dez*16+minuto);  //inicializa a contagem apartir de 12 minutos.
   I2C1_Wr(hora_dez*16+hora);   //inicializa a contagem dos hora apartir das 09:00hs (formato 24 hora).
   I2C1_Wr(0x01);               //inicializa a contagem da ter�a-feira da semana
   I2C1_Wr(dia_dez*16+dia);     //inicializa a contagem do dia 16 do m�s.
   I2C1_Wr(mes_dez*16+mes);     //inicializa a contagem do m�s de maio.
   I2C1_Wr(ano_dez*16+ano);     //inicializa a contagem do ano 15.
   I2C1_Wr(p7);                 //envia byte de controle definindo o comportamento da onda quadrada 
   I2C1_Stop();                 //condi��o de finaliza��o da comunica��o I2C
   INTCON.GIE = 1;              //religa chave geral das interrup��es
}

void desliga_1Hz(){
   INTCON.GIE = 0;              //desliga chave geral das interrup��es
   I2C1_Start();                //inicializa a comunica��o I2c
   I2C1_Wr(0xD0);               //endere�o fixo para a categoria do DS1307: 1101000X, onde x = 0 � para grava��o, e X = 1 para leitura.
   I2C1_Wr(7);                  //endere�o onde iremos come�ar a programa��o do rel�gio, neste caso � o endere�o do controlador do "Square Wave".
   I2C1_Wr(0x80);               //desativa clock de 1Hz e coloca pino em 1 (5V) para reduzir consumo do resistor de pullup
   I2C1_Stop();                 //condi��o de finaliza��o da comunica��o I2C
   INTCON.GIE = 1;              //religa chave geral das interrup��es
}

void liga_1Hz(){
   INTCON.GIE = 0;              //desliga chave geral das interrup��es
   I2C1_Start();                //inicializa a comunica��o I2c
   I2C1_Wr(0xD0);               //endere�o fixo para a categoria do DS1307: 1101000X, onde x = 0 � para grava��o, e X = 1 para leitura.
   I2C1_Wr(7);                  //endere�o onde iremos come�ar a programa��o do rel�gio, neste caso � o endere�o do controlador do "Square Wave".
   I2C1_Wr(0x10);               //ativa clock de 1Hz no pino OUT
   I2C1_Stop();                 //condi��o de finaliza��o da comunica��o I2C
   INTCON.GIE = 1;              //religa chave geral das interrup��es
}