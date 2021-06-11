#define IOC_flag   INTCON.IOCIF  
#define seg_flag   IOCCF.f5  

unsigned char segundo, minuto, hora, dias_semana, dia, mes, ano, dois_pontos, seg_dez, min_dez, hora_dez, dia_dez, mes_dez, ano_dez;

void Inicia_relogio(){
   TRISC.f5 = 1;          // Define RC5 como entrada
   IOCCP.f5 = 1;          // Ativa interrupção na subida em RC5
   IOCCN.f5 = 1;          // Ativa interrupção na descida em RC5
   INTCON.IOCIE = 1;      // Ativa chave geral IOC (Interrupt-on-change)
   dois_pontos = 1;       // Estado inicial aceso, na piscada dos dois pontos
   I2C1_Init(100000);     // iniciliza I2C com frequencia de 100KHz
}

void leitura_rtc() {
   INTCON.GIE = 0;        //desliga chave geral das interrupções
   I2C1_Start();          //inicializa comunicação i2c
   I2C1_Wr(0xD0);         //endereço fixo para a categoria do DS1307: 1101000X, onde x = 0 para gravação, e X = 1 para leitura.
   I2C1_Wr(0);            //endereço onde iremos começar a programação do relógio, neste caso é o endereço dos segundos.
   I2C1_Repeated_Start(); // Começo repetido sinal da edição I2C
   I2C1_Wr(0xD1);         //endereço fixo para a categoria do DS1307: 1101000X com o ultimo bit acionado (1) para leitura
   segundo = I2C1_Rd(1);  //lê primeiro byte segundos e informa que ainda leremos mais dados através do sinalizador (1)
   minuto = I2C1_Rd(1);   //lê segundo byte minutos e informa que ainda leremos mais dados através do sinalizador (1)
   hora = I2C1_Rd(1);     //lê terceiro byte hora e encerra as leituras de dados, graças ao sinalizador (0)
   dias_semana = I2C1_Rd(1);
   dia = I2C1_Rd(1);
   mes = I2C1_Rd(1);
   ano = I2C1_Rd(0);
   I2C1_Stop();           //finaliza comunicação I2C
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
   INTCON.GIE = 1;        //religa chave geral das interrupções
}

void acerta_relogio(unsigned char p1,unsigned char p2,unsigned char p3,unsigned char p4,unsigned char p5,unsigned char p6,unsigned char p7){
   INTCON.GIE = 0;        //desliga chave geral das interrupções
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
   I2C1_Start();                //inicializa a comunicação I2c
   I2C1_Wr(0xD0);               //endereço fixo para a categoria do DS1307: 1101000X, onde x = 0 é para gravação, e X = 1 para leitura.
   I2C1_Wr(0);                  //endereço onde iremos começo a programação do relógio, neste caso é endereço dos segundos.
   I2C1_Wr(seg_dez*16+segundo); //inicializa a contagem dos segundos apartir de 04.
   I2C1_Wr(min_dez*16+minuto);  //inicializa a contagem apartir de 12 minutos.
   I2C1_Wr(hora_dez*16+hora);   //inicializa a contagem dos hora apartir das 09:00hs (formato 24 hora).
   I2C1_Wr(0x01);               //inicializa a contagem da terça-feira da semana
   I2C1_Wr(dia_dez*16+dia);     //inicializa a contagem do dia 16 do mês.
   I2C1_Wr(mes_dez*16+mes);     //inicializa a contagem do mês de maio.
   I2C1_Wr(ano_dez*16+ano);     //inicializa a contagem do ano 15.
   I2C1_Wr(p7);                 //envia byte de controle definindo o comportamento da onda quadrada 
   I2C1_Stop();                 //condição de finalização da comunicação I2C
   INTCON.GIE = 1;              //religa chave geral das interrupções
}

void desliga_1Hz(){
   INTCON.GIE = 0;              //desliga chave geral das interrupções
   I2C1_Start();                //inicializa a comunicação I2c
   I2C1_Wr(0xD0);               //endereço fixo para a categoria do DS1307: 1101000X, onde x = 0 é para gravação, e X = 1 para leitura.
   I2C1_Wr(7);                  //endereço onde iremos começar a programação do relógio, neste caso é o endereço do controlador do "Square Wave".
   I2C1_Wr(0x80);               //desativa clock de 1Hz e coloca pino em 1 (5V) para reduzir consumo do resistor de pullup
   I2C1_Stop();                 //condição de finalização da comunicação I2C
   INTCON.GIE = 1;              //religa chave geral das interrupções
}

void liga_1Hz(){
   INTCON.GIE = 0;              //desliga chave geral das interrupções
   I2C1_Start();                //inicializa a comunicação I2c
   I2C1_Wr(0xD0);               //endereço fixo para a categoria do DS1307: 1101000X, onde x = 0 é para gravação, e X = 1 para leitura.
   I2C1_Wr(7);                  //endereço onde iremos começar a programação do relógio, neste caso é o endereço do controlador do "Square Wave".
   I2C1_Wr(0x10);               //ativa clock de 1Hz no pino OUT
   I2C1_Stop();                 //condição de finalização da comunicação I2C
   INTCON.GIE = 1;              //religa chave geral das interrupções
}