#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "TivaES.h"


void ConfiguraPinPWM1A(void){
    unsigned long temp;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
    temp = SYSCTL_RCGC2_R;
    // Pin PB4
    GPIO_PORTB_DIR_R |= 0x10;
    GPIO_PORTB_AMSEL_R &= ~(0x10);
    // Función alterna habilitada
    GPIO_PORTB_AFSEL_R |= 0x10;
    // PB4 conectado a T1CCP0
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) | 0x70000;
    GPIO_PORTB_DEN_R |= 0x10;
}

void ConfiguraPinPWM1B(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1));
    // Pin  PB5 como salida
    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_AMSEL_R &= ~(0x20);
    // Función alterna habilitada
    GPIO_PORTB_AFSEL_R |= 0x20;
    // PB5 conectado a T1CCP0
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFF0FFFFF) | 0x700000;
    GPIO_PORTB_DEN_R |= 0x20;
}


void ConfiguraPWM1A(void){
    // Activamos señal de reloj para TIMER1A
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
    while(!(SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R1));
    // TIMER1: Modo cuenta descendente, inhabilitado
    TIMER1_CTL_R &= ~(0x0101);
    TIMER1_CFG_R = 0x00000004; //16bits
    //TnAMS bit a 1, TnCMR bit a 0, y TnMR a 2
    TIMER1_TAMR_R = 0x0000000A; //
    //frecuencia base = 100Hz
        TIMER1_TAPR_R = 0x2; //8 bits más significativos    TIMER1_TBPR_R = 0x2; // 8 bits mas significativos
        TIMER1_TAILR_R = 0x70FF; // 16 bits menos significativos
        //Ciclo de trabajo = 100%
        TIMER1_TBPMR_R = 0x0; //8 bits más significativos
        TIMER1_TBMATCHR_R = 0x0001; // 16 bits menos significativos
        // Habilitamos el timer y comenzamos a generar el PWM
        TIMER1_CTL_R |= 0x0101;
}

void ConfiguraPWM1B(void){
    // Activamos señal de reloj para TIMER1A
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
    while(!(SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R1));
    // TIMER1: Modo cuenta descendente, inhabilitado
    TIMER1_CTL_R &= ~(0x0101);
    TIMER1_CFG_R = 0x00000004; //16bits
    //TnAMS bit a 1, TnCMR bit a 0, y TnMR a 2
    TIMER1_TBMR_R = 0x0000000A; //
    //frecuencia base = 100Hz
        TIMER1_TBPR_R = 0x2; //8 bits más significativos    TIMER1_TBPR_R = 0x2; // 8 bits mas significativos
        TIMER1_TBILR_R = 0x70FF; // 16 bits menos significativos
        //Ciclo de trabajo = 100%
        TIMER1_TBPMR_R = 0x0;
         TIMER1_TBMATCHR_R = 0x0001;
        // Habilitamos el timer y comenzamos a generar el PWM
        TIMER1_CTL_R |= 0x0101;
}

void Actualiza_PWMA1( uint32_t F_PWM, uint32_t DC)
{
    uint32_t ValorTope;
    uint32_t ValorDC;

    ValorTope = (16000000/F_PWM)-1; // Determinando el nuevo valor tope
   // ValorDC = (((100 - DC)*ValorTope)/100); // Determinando el nuevo valor comparador DC
   ValorDC = DC;
    // Frecuencia
    TIMER1_TAPR_R = (ValorTope & 0xFF0000)>>16; // 8 Bits más significativos
    TIMER1_TAILR_R = (ValorTope & 0x00FFFF); // 16 Bits menos significativos
    // Ciclo de trabajo inicial
    TIMER1_TAPMR_R = (ValorDC & 0xFF0000)>>16; // 8 Bits más significativos
    TIMER1_TAMATCHR_R = (ValorDC & 0x00FFFF); // 16 Bits menos significativos
}

void Actualiza_PWMB( uint32_t F_PWM, uint32_t DC)


{
    uint32_t ValorTope;
    uint32_t ValorDC;

    ValorTope = (16000000/F_PWM)-1; // Determinando el nuevo valor tope
// ValorDC = (((100- DC)*ValorTope)/100); // Determinando el nuevo valor comparador DC
 ValorDC = DC;
    // Frecuencia
    TIMER1_TBPR_R = (ValorTope & 0xFF0000)>>16; // 8 Bits más significativos
    TIMER1_TBILR_R = (ValorTope & 0x00FFFF); // 16 Bits menos significativos
    // Ciclo de trabajo inicial
    TIMER1_TBPMR_R = (ValorDC & 0xFF0000)>>16; // 8 Bits más significativos
    TIMER1_TBMATCHR_R = (ValorDC & 0x00FFFF); // 16 Bits menos significativos
}



void ConfigPin1 (void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD;
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R3));
    // pd0 , pd1 , pd2 , pd3 salida
    GPIO_PORTD_DIR_R |= 0x0F;//
    GPIO_PORTD_AMSEL_R &= ~(0x0F);//Desactivamos funcion analogica
    GPIO_PORTD_AFSEL_R &= ~(0x0F);//Desactivamos funcion alterna
    GPIO_PORTD_DEN_R |= 0x0F;
}


void Avanza(void)
{

    GPIO_PORTD_DATA_R |= 0x09;
    GPIO_PORTC_DATA_R |= (1<<4);


}

void Retrocede(void)
{
    GPIO_PORTD_DATA_R |= 0x06;
    GPIO_PORTC_DATA_R |= (1<<5);
}


void Derecha(void)
{
    GPIO_PORTD_DATA_R |= 0x05;
    GPIO_PORTC_DATA_R |= (1<<6);
}

void Izquierda(void)
{
    GPIO_PORTD_DATA_R |= 0x0A;
    GPIO_PORTC_DATA_R |= (1<<7);
}


void apagado(void)
{
    GPIO_PORTD_DATA_R &= ~(0x0F);
    GPIO_PORTC_DATA_R &= ~(0xF0);

}

uint32_t solicitanumero(void){
    uint8_t errorFlag;
    uint32_t numero;
    do{
        errorFlag = FALSE;
        // Solicitamos y leemos un ángulo
        UART_TransmiteCadena("solicita numero: ");
        numero = UART_EsperaNumeroB10();
        // Si ángulo mayor de 90 grados hay error
        if(numero>1000000){
            UART_TransmiteCadena("ta mal causa");
            UART_SiguienteLinea();
            errorFlag = TRUE;
        }
    }while(errorFlag);
    return numero;
}





uint8_t EsperaDato(void){
    uint8_t datoRx2;

    while(UART5_FR_R & UART_FR_RXFE);
    datoRx2 = (uint8_t)UART5_DR_R;
    return datoRx2;
}


void ConfiguraUART_HC05(void){ // Configuramos puerto UART5

    unsigned long temp;

    SYSCTL_RCGCUART_R |= (1<<5);// Habilitamos UART5
    temp = SYSCTL_RCGC1_R;
    SYSCTL_RCGC2_R |= ((1<<4)|(1<<5));  // Habilitamos GPIOE
    temp = SYSCTL_RCGC2_R;
    UART5_CTL_R &= ~(1<<0);// Inhabilitamos el UART5
    //Configuramos a 9600 8 N 1 la velocidad de comunicación
    UART5_IBRD_R = (UART5_IBRD_R & 0xFFFF0000) | 104;
    UART5_FBRD_R = (UART5_FBRD_R & 0xFFFFFFC0) | 11;

    //UART5_LCRH_R = (UART5_LCRH_R & 0xFFFFFF00) | 0x70;// 8, N, 1, FIFOs habilitados
    UART5_LCRH_R = (UART5_LCRH_R & 0xFFFFFF00) | 0x70;
    UART5_CTL_R |= (1<<0);  // Habilitamos el UART1
    GPIO_PORTE_AMSEL_R &= ~(0x30);// Desactivamos modo analógico en PE4 y PE5
    GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFF00FFFF)|0x00110000; // Conectamos UART5 a PE4 y PE5
    GPIO_PORTE_AFSEL_R |= 0x30;// Activamos funciones alternas en PE4 y PE5
    GPIO_PORTE_DEN_R |= 0x30;// Activamos funciones digitales en PE4 y PE5
}




void InicializaPORTC(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGCGPIO_R2;
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R2));
    GPIO_PORTC_DIR_R |= 0xF0;
    GPIO_PORTC_AFSEL_R &= ~(0xF0);
    GPIO_PORTC_AMSEL_R &= ~(0xF0);
    GPIO_PORTC_DEN_R |= 0xF0;
}


void ConfigSysTickCarreraLibre(void){
NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
NVIC_ST_RELOAD_R = (NVIC_ST_RELOAD_R&0xFF000000)|0x00FFFFFF;
NVIC_ST_CURRENT_R &= ~(0x00FFFFFF);
NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}




void retardo_ms(uint32_t cantidad_ms){
uint32_t inicioContador, actualContador, diferenciaContador, i;
// Bucle que se repite cantidad_ms veces
for(i=0; i<cantidad_ms; i++){
inicioContador = NVIC_ST_CURRENT_R & 0x00FFFFFF;
// retardo de 1ms
do{
actualContador = NVIC_ST_CURRENT_R & 0x00FFFFFF;
diferenciaContador = (inicioContador-actualContador)&0x00FFFFFF;
}while(diferenciaContador<16000);
}
}



void Config_puertoB (void){
    //Se configura para el pin trig para el ultrasonido
    long temp;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // (0x02) reloj activo para el puerto B
    temp= SYSCTL_RCGC2_R; // esperar algunos ciclos de reloj
    GPIO_PORTB_DIR_R |= 0x04; // Configura PB2 como pin de salida
    GPIO_PORTB_AMSEL_R &= ~0x04; // Se desactivan funciones analógicas de pin PB2 y se habilitan funciones digitales
    GPIO_PORTB_AFSEL_R &= ~0x04; // configura pin como GPIO
    GPIO_PORTB_DEN_R |=0x04; // activar PB2 pin para acceso digital
}

void conf_timercap_fsub(void){
    SYSCTL_RCGC1_R|= (1<<19);   // SE ACTIVA EL TIMER
    SYSCTL_RCGC2_R|=(1<<1);
    while ( (SYSCTL_PRTIMER_R &(1<<3)) == 0);
    TIMER3_CTL_R &= (~TIMER_CTL_TBEN); // Se deshabilita para configurar
    TIMER3_CFG_R = (TIMER3_CFG_R & (~0x7))+4;
    TIMER3_TBMR_R = (TIMER3_TBMR_R & 0xFFF)+ 0x07;
    TIMER3_TBILR_R |= 0xFFFF;
    TIMER3_TBPR_R |= 0XFF;
    // Se configura el pin para el echo
    GPIO_PORTB_DIR_R &= ~(1<<3); // se utiliza pin PB3 como entrada
    GPIO_PORTB_AMSEL_R &= ~(1<<3);
    GPIO_PORTB_PUR_R &= ~(1<<3);
    GPIO_PORTB_PDR_R &= ~(1<<3);
    GPIO_PORTB_AFSEL_R |= (1<<3);
    GPIO_PORTB_PCTL_R |= (GPIO_PORTB_PCTL_R & 0XFFFF0FFF)+0X00007000;
    GPIO_PORTB_DEN_R |= (1<<3);
    TIMER3_CTL_R |= TIMER_CTL_TBEN; // Se habilita
}




uint32_t ultrasonido(void){
    //Función para calcular la distancia
    uint32_t val_ini=0 ,val_fin=0,tiempo, distancia,i;
    //flanco subida
    GPIO_PORTB_DATA_R |= 0x04; //prende el trig
    for(i=0;i< 0x9F;i++);
    GPIO_PORTB_DATA_R &= ~0x04; //apaga trig
    while ((TIMER3_RIS_R & (1<<10))==0); // Se espera a la captura del valor
    val_ini = TIMER3_TBR_R;
    TIMER3_ICR_R = (1<<10); // Se borra el valor capturado
    TIMER3_CTL_R |=(1<<10);
    //flanco bajada
    while ((TIMER3_RIS_R & (1<<10))==0);
    val_fin = TIMER3_TBR_R;
    TIMER3_ICR_R = (1<<10); // Se borra el valor capturado
    TIMER3_CTL_R = (TIMER3_CTL_R & ~((1<<10)|(1<<11)));
    tiempo = (val_ini - val_fin)/16; // Se calcula el tiempo mediante la diferencia de los valores capturados
    distancia = (tiempo * 0.01715); // Se calcula la distancia
    return distancia; // Retorna el valor de la distancia
}


//MODO_MANUAL
void Modo_Manual(){
    uint8_t  led_manual;
    uint8_t flag_princ = 0;
    while (flag_princ!=1) {
        led_manual = EsperaDato();
        flag=led_manual;

        switch(flag){
        case 'W':  Avanza();
        break;
        case 'S':  Retrocede();
        break;
        case 'D':  Derecha();
        break;
        case 'A':  Izquierda();
        break;
        case 'C':  flag_princ = 1;
        }
    }
}


// MODO_AUTOMÁTICO
void Modo_Automatico(){
    uint8_t i, dato, obstaculo;
    uint8_t flag_princ = 0;
    while(flag_princ!=2){
        i = 0;
        if((UART5_FR_R & UART_FR_RXFE)==0);{
            dato = (uint8_t)UART5_DR_R;
            if (dato == 'B')
                flag_princ=2;
        }
        Avanza();
        // FUNCIÓN ULTRASONIDO QUE DEVUELVE UN VALOR (obstaculo)
        if (obstaculo==1){
            while (i<5){
                Derecha();
                i++;
            }
        }
    }
}



void main (void){
    uint32_t num1 = 0;
    uint32_t num2 = 0;
    uint32_t ValFr = 100; // Valor inicial de 100Hz
    ConfiguraUART_HC05();
    ConfigSysTickCarreraLibre();
    ConfiguraPinPWM1A();
    ConfiguraPWM1A();
    ConfiguraPinPWM1B();
    ConfiguraPWM1B();
    ConfigPin1();
    UART_Inicializa();
    Actualiza_PWMA1(ValFr, num1);
    Actualiza_PWMB(ValFr,num2);
    InicializaPORTC();
    Config_puertoB();
    conf_timercap_fsub();
    uint8_t c;
    uint32_t dist;
    uint8_t  flag_princ= 0;

    while(1){
       c= EsperaDato();
        switch(c){
        case 'w':
         adelante();
         break;
        case 'a':
          izquierda();
          break;
        case 's':
          atras();
          break;
        case 'd':
          derecha();
          break;
        case '2':
            adelante();
            dist = ultrasonido();
          if (dist < 15)
          {
           GPIO_PORTC_DATA_R |= (1<<7);
           derecha();
           retardo_ms(3000);
              break;
             }

        default:

           apagado();
        }


      }
}











