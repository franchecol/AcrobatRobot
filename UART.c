/****************************************************************************************/
/****************************************************************************************/
/***** PONTIFICIA UNIVERSIDAD CAT�LICA DEL PER� *****/
/***** FACULTAD DE CIENCIAS E INGENIER�A *****/
/***** SISTEMAS DIGITALES *****/
/****************************************************************************************/
/***** Archivo: UART.c *****/
/***** Microcontrolador: TM4C123GH6PM *****/
/***** M�dulo(s) uC: UART0 *****/
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/
/***** Autor: Rolando S�nchez Ponce *****/
/***** Fecha: Octubre 2017 *****/
/***** �ltima revisi�n: Mayo 2019 *****/
/****************************************************************************************/
/***** Funciones para comunicaci�n serial as�ncrona mediante el UART0. *****/
/****************************************************************************************/
/***** Conexiones: *****/
/***** U0Rx - PA0 *****/
/***** U0Tx - PA1 *****/
/****************************************************************************************/
/****************************************************************************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "UART.h"
/****************************************************************************************/
/***** F U N C I O N E S P � B L I C A S *****/
/****************************************************************************************/
/****************************************************************************************/
/***** UART_Inicializa() *****/
/****************************************************************************************/
/***** Configura el UART0 con los siguientes par�metros de comunicaci�n serial: *****/
/***** 9600bps, 8 bits de datos, sin paridad, 1 bit de parada. *****/
/***** Para una frecuencia de reloj del sistema de 16MHz. *****/
/****************************************************************************************/
/***** ENTRADAS: Ninguna *****/
/***** SALIDA: Ninguna *****/
/****************************************************************************************/
void UART_Inicializa(void){
    // Activamos el reloj para el UART0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    while(!(SYSCTL_PRUART_R & SYSCTL_PRUART_R0));
    // Inhabilitamos el UART0
    UART0_CTL_R &= ~UART_CTL_UARTEN;
    // Velocidad 9600bps (Fsysclk = 16MHz)
    UART0_IBRD_R = (UART0_IBRD_R & 0xFFFF0000) | 104;
    UART0_FBRD_R = (UART0_FBRD_R & 0xFFFFFFC0) | 11;
    // 8, N, 1, FIFOs habilitados
    UART0_LCRH_R = (UART0_LCRH_R & 0xFFFFFF00) | 0x70;
    // Habilitamos el UART0, recepci�n y transmisi�n
    UART0_CTL_R |= (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
    // Activamos la se�al de reloj para GPIOA
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0));
    // Activamos funciones alternas para PA0 y PA1
    GPIO_PORTA_AFSEL_R |= 0x03;
    // Conectamos UART0 a PA0 y PA1
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)|0x00000011;
    // Desactivamos funciones anal�gicas en PA0 y PA1
    GPIO_PORTA_AMSEL_R &= ~(0x03);
    // Activamos funciones digitales en PA0 y PA1
    GPIO_PORTA_DEN_R |= 0x03;
}
/****************************************************************************************/
/***** UART_EsperaDato() *****/
/****************************************************************************************/
/***** Lee un dato recibido por el puerto serial, para esto espera que haya un dato *****/
/***** en el buffer de recepci�n del UART0 para leerlo. *****/
/****************************************************************************************/
/***** ENTRADAS: Ninguna *****/
/***** SALIDA: Dato recibido por el UART0 *****/
/****************************************************************************************/
uint8_t UART_EsperaDato(void){
    uint8_t datoRx;

    while(UART0_FR_R & UART_FR_RXFE);
    datoRx = (uint8_t)UART0_DR_R;
    return datoRx;
}
/****************************************************************************************/
/***** UART_SondeaRx() *****/
/****************************************************************************************/
/***** Revisa si hay alg�n dato recibido en el buffer de recepci�n del UART0. *****/
/***** Devuelve TRUE si hay dato y FALSE si no hay dato recibido, adem�s si hubiera *****/
/***** lo lee. No hay lazo de espera. *****/
/****************************************************************************************/
/***** ENTRADAS: *****/
/***** *datoRxPtr: Direcci�n de memoria donde se almacenar� el dato *****/
/***** recibido si lo hubiera. *****/
/***** SALIDA: TRUE: Hay dato recibido y leido. *****/
/***** FALSE: No hay dato recibido. *****/
/****************************************************************************************/
uint8_t UART_SondeaRx(uint8_t *datoRxPtr){
    uint8_t hayDatoRxFlag;
    hayDatoRxFlag = FALSE;
    if(!(UART0_FR_R & UART_FR_RXFE)){
        *datoRxPtr = (uint8_t)UART0_DR_R;
        hayDatoRxFlag = TRUE;
    }
    return hayDatoRxFlag;
}
/****************************************************************************************/
/***** UART_TransmiteDato() *****/
/****************************************************************************************/
/***** Escribe un byte en el buffer de transmisi�n del UART0 para ser transmitido *****/
/***** por el puerto serial. Si el buffer estuviera lleno espera hasta que se pueda *****/
/***** escribir. *****/
/****************************************************************************************/
/***** ENTRADAS: *****/
/***** datoTx: Dato a transmitir por el puerto serial *****/
/***** SALIDA: Ninguna *****/
/****************************************************************************************/
void UART_TransmiteDato(uint8_t datoTx){
    while(UART0_FR_R & UART_FR_TXFF);
    UART0_DR_R = (UART0_DR_R & 0xFFFFFF00) | datoTx;
}
/****************************************************************************************/
/***** UART_SiguienteLinea() *****/
/****************************************************************************************/
/***** Transmite los caracteres necesarios para posicionar el cursor de la pantalla *****/
/***** del terminal al inicio de la siguiente l�nea. *****/
/****************************************************************************************/
/***** ENTRADAS: Ninguna *****/
/***** SALIDA: Ninguna *****/
/****************************************************************************************/
void UART_SiguienteLinea(void){
    // Transmitimos nueva l�nea (LF)
    UART_TransmiteDato(LF);
    // Transmitimos retorno de carro (CR)
    UART_TransmiteDato(CR);
}
/****************************************************************************************/
/***** UART_EsperaCadena() *****/
/****************************************************************************************/
/***** Recibe por el puerto serial una cadena de caracteres ASCII y la almacena en *****/
/***** un arreglo. El usuario ingresa ENTER para finalizar y debe ingresar por lo *****/
/***** menos un caracter antes del ENTER. La funci�n inserta 0 al final de la cade- *****/
/***** na para que sea ASCIIZ. La funci�n hace un eco por cada caracter recibido y *****/
/***** posiciona el cursor al inicio de la siguiente l�nea al salir. *****/
/****************************************************************************************/
/***** ENTRADAS: *****/
/***** cadenaASCIIZ[]: Direcci�n de inicio de la cadena ASCIIZ a recibir. *****/
/***** SALIDA: Ninguna *****/
/****************************************************************************************/
void UART_EsperaCadena(uint8_t cadenaASCIIZ[]){
    uint8_t datoRx;
    uint8_t hay1CaracterFlag = FALSE;
    uint8_t finFlag = FALSE;
    uint8_t i = 0;
    do{
        // Esperamos recibir un dato por el puerto serial.
        datoRx = UART_EsperaDato();
        // Si dato recibido es diferente de ENTER.
        if(datoRx != CR){
            if(!hay1CaracterFlag) hay1CaracterFlag = TRUE;
            UART_TransmiteDato(datoRx);
            cadenaASCIIZ[i++] = datoRx;
        }
        // Si dato recibido es ENTER.
        else if(hay1CaracterFlag){
            cadenaASCIIZ[i] = 0;
            UART_SiguienteLinea();
            finFlag = TRUE;
        }
    }while(!finFlag);
}
/****************************************************************************************/
/***** UART_TransmiteCadena() *****/
/****************************************************************************************/
/***** Transmite por el puerto serial una cadena ASCIIZ. *****/
/****************************************************************************************/
/***** ENTRADAS: *****/
/***** cadenaASCIIZ[]: Direcci�n de inicio de la cadena ASCIIZ a transmitir. *****/
/***** SALIDA: Ninguna *****/
/****************************************************************************************/
void UART_TransmiteCadena(const uint8_t cadenaASCIIZ[]){
    uint8_t i;
    for(i=0; cadenaASCIIZ[i]; i++){
        UART_TransmiteDato(cadenaASCIIZ[i]);
    }
}
/****************************************************************************************/
/***** UART_EsperaNumeroB10() *****/
/****************************************************************************************/
/***** Recibe por el puerto serial una cadena de caracteres ASCII que representa *****/
/***** un n�mero en base 10. El usuario ingresa ENTER para finalizar y debe ingre- *****/
/***** sar por lo menos un digito antes del ENTER. *****/
/***** La funci�n hace un eco por cada caracter recibido y posiciona el cursor al *****/
/***** inicio de la siguiente l�nea al salir. *****/
/****************************************************************************************/
/***** ENTRADAS: Ninguna. *****/
/***** SALIDA: N�mero ingresado por el usuario. *****/
/***** Rango [0, 4294967295] *****/
/****************************************************************************************/
uint32_t UART_EsperaNumeroB10(void){
    uint8_t datoRx;
    uint32_t numero = 0;
    uint8_t hay1DigitoFlag = FALSE;
    uint8_t finFlag = FALSE;
    do{
        // Esperamos recibir un dato por el puerto serial.
        datoRx = UART_EsperaDato();
        // Si dato recibido es diferente de ENTER.
        if(datoRx != CR){
            // Si dato recibido es un caracter num�rico
            if(('0'<=datoRx)&&(datoRx<='9')){
                if(!hay1DigitoFlag) hay1DigitoFlag = TRUE;
                UART_TransmiteDato(datoRx);
                numero = 10*numero + datoRx-'0';
            }
        }
        // Si dato recibido es ENTER.
        else if(hay1DigitoFlag){
            UART_SiguienteLinea();
            finFlag = TRUE;
        }
    }while(!finFlag);
    return numero;
}
/****************************************************************************************/
/***** UART_TransmiteNumeroB10() *****/
/****************************************************************************************/
/***** Transmite por el puerto serial los caracteres que representan un n�mero en *****/
/***** base 10. *****/
/****************************************************************************************/
/***** ENTRADAS: *****/
/***** numero: N�mero a transmitirse en caracteres ASCII. *****/
/***** Rango [0, 4294967295] *****/
/***** SALIDA: Ninguna *****/
/****************************************************************************************/
void UART_TransmiteNumeroB10(uint32_t numero){
    uint32_t divisor = 1000000000;
    uint8_t hayDigSigFlag = FALSE;
    uint8_t i, digito;
    // Repetimos 10 veces: m�xima cantidad de d�gitos que puede tener n�mero.
    for(i=0; i<10; i++){
        // Hallamos digito y actualizamos numero y divisor.
        digito = numero / divisor;
        numero = numero % divisor;
        divisor = divisor / 10;
        // Si a�n no llegamos al d�gito m�s significativo de numero.
        if(!hayDigSigFlag){
            // Si digito es diferente de 0
            if(digito != 0){
                hayDigSigFlag = TRUE;
                UART_TransmiteDato(digito + '0');
            }
            // Si digito es 0 y es la �ltima iteraci�n
            else if(i==9){
                UART_TransmiteDato(digito + '0');
            }
        }
        // Si ya hemos llegado al d�gito m�s significativo.
        else{
            UART_TransmiteDato(digito + '0');
        }
    }
}
/****************************************************************************************/
/***** F I N D E L A R C H I V O *****/
/****************************************************************************************/
