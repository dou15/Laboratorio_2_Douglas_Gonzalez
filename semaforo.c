#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define INTERRUPT_BOTTON1 PD2 //INT0, PIN PD2 
#define INTERRUPT_BOTTON2 PD3 //INT1, PIN PD3

//LEDs semaforos
#define LDPV  PB1 //paso vehiculos
#define LDVD  PB0 //vehiculos detenidos
#define LDPP1 PB3 //paso peatones 1
#define LDPD1 PB2 //peatones detenidos 1
#define LDPP2 PB5 //paso peatones 2
#define LDPD2 PB4 //peatones detenidos 2

unsigned char count;
unsigned char count2;
unsigned char toggle;
unsigned char buttonPush;
unsigned char TRUE;
unsigned char FALSE;


/******************** Estados posibles ********************/
unsigned char estado                         ; 
unsigned char nextEstado                     ;
unsigned char PasoVehiculos               = 0; // Estado por defecto
unsigned char TransicionVehiculosPeatones = 1;
unsigned char PasoPeatones                = 2;
unsigned char TransicionPeatonesVehiculos = 3;

void estados(){
    if(estado==PasoVehiculos){
        PORTB |=  (1 << LDPV);
        PORTB &= ~(1 << LDVD);
        PORTB &= ~(1 << LDPP1);
        PORTB |=  (1 << LDPD1);
        PORTB &= ~(1 << LDPP2);
        PORTB |=  (1 << LDPD2);
    }
    if(estado==TransicionVehiculosPeatones){        
        if(toggle==0){
        PORTB |=  (1 << LDPV);
        PORTB &= ~(1 << LDVD);
        PORTB &= ~(1 << LDPP1);
        PORTB |=  (1 << LDPD1);
        PORTB &= ~(1 << LDPP2);
        PORTB |=  (1 << LDPD2);
        toggle =   1;
        }
        else{
        PORTB &= ~(1 << LDPV);
        PORTB &= ~(1 << LDVD);
        PORTB &= ~(1 << LDPP1);
        PORTB |=  (1 << LDPD1);
        PORTB &= ~(1 << LDPP2);
        PORTB |=  (1 << LDPD2);
        toggle =   0;
        }
    }
    if(estado==PasoPeatones){
        PORTB &= ~(1 << LDPV);
        PORTB |=  (1 << LDVD);
        PORTB |=  (1 << LDPP1);
        PORTB &= ~(1 << LDPD1);
        PORTB |=  (1 << LDPP2);
        PORTB &= ~(1 << LDPD2);
    }
    if(estado==TransicionPeatonesVehiculos){
        if(toggle==0){
        PORTB &= ~(1 << LDPV);
        PORTB |=  (1 << LDVD);
        PORTB |=  (1 << LDPP1);
        PORTB &= ~(1 << LDPD1);
        PORTB |=  (1 << LDPP2);
        PORTB &= ~(1 << LDPD2);
        toggle =   1;
        }
        else{
        PORTB &= ~(1 << LDPV);
        PORTB |=  (1 << LDVD);
        PORTB &= ~(1 << LDPP1);
        PORTB &= ~(1 << LDPD1);
        PORTB &= ~(1 << LDPP2);
        PORTB |=  (1 << LDPD2);
        toggle =   0;
        }
    }
}

/******************** Interrupciones ********************/
ISR(INT0_vect)         
{
    buttonPush = TRUE;
}

ISR(INT1_vect)        
{
    buttonPush = TRUE;
}

ISR(TIMER0_OVF_vect){
   //Pasan vehiculos. 
   // count = 62 equivale a 1 segundo. Al completar count2 = 10 deben transcurrir 10 segundos aproximadamente 
   if(count == 62 && count2 < 10 && estado == PasoVehiculos && nextEstado == PasoVehiculos){        
     if(buttonPush == TRUE){
         estado     = PasoVehiculos;
         estados();
         estado     = PasoVehiculos;
         nextEstado = TransicionVehiculosPeatones;
         count = 0;
         count2++;
     }
     else{
         estado     = PasoVehiculos;
         nextEstado = PasoVehiculos;
         estados();
         count  = 0;
         count2 = 0;
     }
   }
   
   //Transición paso vehiculos a peatones
   if(count == 62 && count2 < 13 && estado == PasoVehiculos && nextEstado == TransicionVehiculosPeatones){
     if(count2 < 13){
         buttonPush = FALSE;
         estado     = TransicionVehiculosPeatones;
         estados();
         estado     = PasoVehiculos;
         nextEstado = TransicionVehiculosPeatones;
         count = 0;
         count2++;
     }
     else{
         estado     = TransicionVehiculosPeatones;
         nextEstado = PasoPeatones;
         estados();
         count  = 0;
         count2 = 0;
    }
   }
   
   //Pasan peatones
   if(count == 62 && count2 < 10 && estado == TransicionVehiculosPeatones && nextEstado == PasoPeatones){
     if(count2 < 9){  
         estado     = PasoPeatones;
         estados();
         count = 0;
         count2++;
         estado     = TransicionVehiculosPeatones;
         nextEstado = PasoPeatones;
     }
     else{
         estado     = PasoPeatones;
         nextEstado = TransicionPeatonesVehiculos;
         count2++;
    }
   }
   
   //Transición paso peatones a vehiculos
   if(count == 62 && count2 < 13 && estado == PasoPeatones && nextEstado == TransicionPeatonesVehiculos){
     if(count2 < 13){    
         estado     = TransicionPeatonesVehiculos;
         estados();
         count = 0;
         count2++;
         estado     = PasoPeatones;
         nextEstado = TransicionPeatonesVehiculos;
     }
     else{
         estado     = PasoVehiculos;
         nextEstado = PasoVehiculos;
         estados();
         count  = 0;
         count2 = 0;
    }
   }
   
   else{
     count++;    
   }
} 


/******************** Función Main ********************/
int main(void)
{
    //inicializando variables
    TRUE       = 1;
    FALSE      = 0;
    estado     = PasoVehiculos;
    nextEstado = PasoVehiculos;
    count      = 0;
    toggle     = 0;
    buttonPush = FALSE;
    count2     = 0;
   
     DDRB   =  0xFF;                     //configure PORTB as output	
     PORTD |= (1 << PORTD2);             // activa the Pull-up D2
     PORTD |= (1 << PORTD3);             // activa the Pull-up D3 
     TCNT0  =  0x00;                     //TCNT0 – Timer/Counter Register Writing to the TCNT0 Register blocks (removes) the Compare Match on the following timer clock
     TCCR0B = (1<<CS00)   | (1<<CS02);   //Timer/Counter Control Register B (CS02=1,CS01=0,CS00=1 -> 1024 (From prescaler)) 
     TIMSK  = (1<<TOIE0);                //Timer/Counter Interrupt Mask Register Bit 1 – TOIE0: Timer/Counter0 Overflow Interrupt Enable
     GIMSK |= ((1<<INT0)  | (1<<INT1));  // enabling the INT0 and INT1 (external interrupt) 
     MCUCR |= ((1<<ISC11) | (1<<ISC01)); // Configuring as falling edge 
     sei();                              //Enable the Global interrupt  
     
    while(1)
    {   
    } 
   
}
