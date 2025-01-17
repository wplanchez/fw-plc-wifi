#include "system.h"
//#include <drv_rpm.h>

#define AntiRebote1(PIN)    do{if(input(PIN)==1){break;}}while(1)
#define AntiRebote0(PIN)    do{if(input(PIN)==0){break;}}while(1)

int1 Flag_ejecutar,Flag_salir;
int1 Flag_sub_menu1,Flag_Show,Flag_Alarmas, Flag_Arranque,Flag_MostrarADC;

int Status, Status_modo;
int time,segundos,minutos,seg,min;
int Select = 0;
int16 transfer_on = 0;
int16 Vtemperatura,Vnivel,Vch1,Vch2;
int1 Flag_Automatico = 0;
int1 Flag_Manual = 0;
////////////////////////////////////////////////
int1 Completado;     // Cuenta del periodo completada
int1 Flanco;         // Variable para la deteccion del flanco:
                     // Flanco = 0 (Espera Flanco de subida)
                     // Flanco = 1 (Espera Flanco de bajada)
int Numero_Flanco;   // Indica el numero de flanco

int16 T1,T2,T3,F,DUTY_Actual;    // Variables para guardar 
                                       //los valores del Timer
int16 T_High,T_Low,T_Total;            // Variables para guardar:
                              // T_Higth ->  Semiperiodo Alto
                              // T_Low -> Semiperiodo Bajo
                              // T_Total -> Priodo total
Float P,P_ms,Ta,Vout;         // P -> Periodo
                              // F -> Frecuencia

int32 rpm,rpm_Old;
///////////////////////////////////////////////

void drv_gpio_adc_select(){
   
   if (input(BOTON_MOSTRAR)==0){ 
      //delay_ms(250);
      AntiRebote1(BOTON_MOSTRAR);
      Select++;
      if(Select>4){Select=0;Flag_Show = 0;}
   }   
}


void drv_gpio_int_rb(){}

void drv_gpio_clear(){}

void drv_gpio_finish1(){
   Flag_ejecutar = 0;
   Flag_sub_menu1 = 1;
   //Status_nivel = 1;   
}

void drv_gpio_finish0(){
   Flag_ejecutar = 0;
   Flag_sub_menu1 = 0;
   //Status_nivel = 0;   
}

void drv_gpio_off(){
   output_c(0b00000000); 
   output_e(0b00000000);  
   set_pwm1_duty(5);
}

void drv_gpio_control_time(){
   
   seg++;
   if(seg>59){
      min++;
      seg = 0;
   }
   
   lcd_gotoxy(1,2);
   
   if(seg<10){
      printf(lcd_putc,"Tiempo:%2dm:0%ds\n",min,seg);
   }
   else{
      printf(lcd_putc,"Tiempo:%2dm:%2ds\n",min,seg);
   }
   
}

void drv_gpio_seg(int segundos){

int i,j;

   for(i = 1; i <=segundos; i++){
      
      for(j = 0; j <=10; j++){
         delay_ms(100);
   
      }
   }  
      
   i = 0;

}


void drv_gpio_pulsadores(){}

//GPIO ADC

int16 drv_gpio_adc_read(int channel){

   int16 medicion=0;                     //variable entera que contendra la medicion del convertidor AD
   int1 done = 0;                        //Contendra 1 si el convertidor termin� de convertir
   //AN0_AN1_AN2_AN3_AN4_AN5 
   //setup_adc_ports(AN0_AN1_AN2_AN3_AN4_AN5); // CANALES ANALOGICOS A0 A1 A2 A3 A5 E0 
   setup_adc_ports(ALL_ANALOG);
   setup_adc(ADC_CLOCK_INTERNAL); // conversion analogica con el reloj interno
   set_adc_channel (channel);          //Establecemos el canal de lectura analogica
   delay_ms(1);                       //a small delay is required after setting the channel
   medicion=read_adc ();               // Hace conversi�n AD 
   done = adc_done();                  //done = 1 si el convertidor termino de convertir
   while(!done)
   {
      done = adc_done();               //Mientras no acabe de convertir se seguira ciclando
   }
   setup_adc (adc_off);                //detenemos al convertidor
   //medicion = (medicion/1023.0)*5;
   return medicion;   
}

void drv_gpio_adc_lcd(){

   Vtemperatura = drv_gpio_adc_read(0);
   Vnivel = drv_gpio_adc_read(1);
   printf(lcd_putc,"\fTEMP : %2.1fv\n",(Vtemperatura/1023.0)*5);
   printf(lcd_putc,"NIVEL: %2.1fv\n",(Vnivel/1023.0)*5);
   delay_ms(100);

}

void drv_gpio_adc_show(int channel){

   switch(channel){
      case 0:
      Flag_Show = 0;
       drv_rpm_int_ext();
      drv_rpm_capture();
      //delay_ms(100);
      break;
    case 1:
      Flag_Show = 0;
      Vch1 = drv_gpio_adc_read(0);
      printf(lcd_putc,"\fCOMBUST.: %2.1fv\n",(Vch1/1023.0)*5);   
    break;
    
    case 2:
    
      Flag_Show = 0;
      Vch1 = drv_gpio_adc_read(1);
      printf(lcd_putc,"\fALTERNAD.: %2.1fv\n",((Vch1*3.2)/1023.0)*5);
    break;
    
    case 3:
    
      Flag_Show = 0;
      Vch1 = drv_gpio_adc_read(2);
      Vch2 = drv_gpio_adc_read(3);
      delay_ms(1);
      
      printf(lcd_putc,"\fGEN_F1 : %2.1fv\n",((Vch1*37.6)/1023.0)*5);
      printf(lcd_putc,"GEN_F2: %2.1fv\n",((Vch2*37.6)/1023.0)*5);
      //delay_ms(100);
    break;
    
    case 4:
    
      Flag_Show = 0;
      Vch1 = drv_gpio_adc_read(4);
      Vch2 = drv_gpio_adc_read(5);
      delay_ms(1);
      
      printf(lcd_putc,"\fCALL_F1 : %2.1fv\n",((Vch1*37.6)/1023.0)*5);
      printf(lcd_putc,"CALL_F2: %2.1fv\n",((Vch2*37.6)/1023.0)*5);
      //delay_ms(100);
      
       break; 
    
    case 5: 
     
      break;
   }   
}


///////////////////////////////////////////////////////////////////////////////


void drv_rpm_int_rb(void){}

void drv_rpm_int_ext(void){

  ++Numero_Flanco;         // Cuento flanco que nos llegan
  
   if(Flanco==0){           // He recibido Flanco de Subida
      if(Numero_Flanco == 1){// Primer Flanco Alto
         set_timer1(0);       // Reinicio TMR1
         T1=get_timer1();     // Guardo en T1 el valor de TMR1 al primer 
    }                      // Flanco de Subida
   if(Numero_Flanco == 3){// Segundo Flanco de subida
         T3=get_timer1();     // Guardo en T3 el valor de TMR1 al primer Flanco de Subida
         if(Completado == 0){ // Si los datos anteriores han sido procesados ...
         Completado = 1;    // Indico que ya hay nuevos datos de flancos para calcular
      }
    }
    ext_int_edge(0,H_TO_L); // Configuro para capturar siguiente flanco de Bajada
    Flanco=1;               // Indico que el siguiente flanco ser? de Bajada

  } 
  
  else {                    // He recibido Flanco de Bajada
      T2=get_timer1();          // Guardo en T2 el valor de TMR1 al Flanco de Bajada
      ext_int_edge(0,L_TO_H);   // Configuro para capturar siguiente flanco de subida
      Flanco=0;                 // Indico que el siguiente flanco ser? de Subida
  }
  if(Numero_Flanco == 3){     
    Numero_Flanco =0;        
        drv_rpm_show(); 
  }
  
   
  
}

void drv_rpm_capture(void)
{
if ((F==0)||(F==1)){ // si la frecuencia es igial a 0 o 1
         set_pwm1_duty(5); // saca el ciclo util minimo
         F = 0;            // y reinicia todas las variables
         Ta = 0;
         rpm = 0;
         P_ms = 0.00;
      }
      
      if(Completado==1){               // Detecto que ya hay datos de flancos ...
         if((T3>T2)&&(T2>T1)){         // Compruebo que estoy en la misma vuelta de TMR1
            T_High = T2 - T1;          // Calculo en Tick's de TMR1 el tiempo en Alto
            T_Low = T3 - T2;           // Calculo en Tick's de TMR1 el tiempo en Bajo
            T_Total = T_High + T_Low;  // Calculo en Tick's de TMR1 el Periodo del Pulso
            P = 8.0  * T_Total;        // Calculo en uS el tiempo.
            P_ms = P/1000;             // Periodo en milisegundos
            F = 1 / (P / 1000000);     // Calculo la Frecuencia en segundos
            F = F + 1;
            rpm = F*60;
         }  // Fin del if anidado
      Completado=0;           // Indico que ya han sido procesados los datos.
      }                       // Fin del if 
      
      if(rpm_Old!=0 && rpm!=0){rpm =(rpm+rpm_Old)/2;} // Saca el promedio
                                                       // de las rpm
      if(rpm==0){rpm_Old=0;}
         rpm_Old=rpm;
         
      //drv_rpm_show();
}

void drv_rpm_show(void){

   //printf(lcd_putc, "\f%ldrpm",rpm);
   printf(lcd_putc, "\f%ldrpm",F*60);
   printf(lcd_putc, "\n%luhz ->%3.2fms",F,P_ms);
   //printf(lcd_putc, "\n%ldrpm -> %1.2fV",rpm,Vout);
 }

int1 drv_gpio_alarmas(void){

   if (input(TEMP_ALARMA)==0 ){ 
      //AntiRebote1(TEMP_ALARMA);
      delay_ms(200);
      output_c(0b00000000);
      printf(lcd_putc,"\fALARMA TEMP.\n");
      //delay_ms(2000);
      Status_modo = MODO_INICIO;
      Flag_Alarmas = 1;
      Flag_Automatico = 0;
      Flag_Manual = 0;
   }  
      
   if (input(PRESION_ALARMA)==0){ 
      //AntiRebote1(PRESION_ALARMA );
      delay_ms(200);
      output_c(0b00000000);
      printf(lcd_putc,"\fALARMA PRESION\n");
      Flag_Alarmas = 1;
      Status_modo = MODO_INICIO;
      Flag_Automatico = 0;
      Flag_Manual = 0;
      //delay_ms(2000);
   }   
   
   if (input(P_EMERGENCIA)==0){ 
      //AntiRebote1(PRESION_ALARMA );
      
      delay_ms(200);
      output_c(0b00000000);
      printf(lcd_putc,"\f   PARADA  DE\n");
      printf(lcd_putc,"   EMERGENCIA\n");
      Flag_Alarmas = 1;
      Status_modo = MODO_INICIO;
      Flag_Automatico = 0;
      Flag_Manual = 0;
      //delay_ms(500);
   }
  
   if (input(STOP_RESET)==0){ 
      //AntiRebote1(PRESION_ALARMA );
      delay_ms(200);
      output_c(0b00000000);
      printf(lcd_putc,"\f   STOP/FALLA\n");
      Flag_Alarmas = 1;
      Status_modo = SIN_MODO;
      Flag_Automatico = 0;
      Flag_Manual = 0;
      //delay_ms(2000);
   }
   if(input(TEMP_ALARMA)== 1 && input(PRESION_ALARMA)== 1 && input(P_EMERGENCIA)==1 && input(STOP_RESET)==1){
       Flag_Alarmas = 0;
   }

   return Flag_Alarmas;
}

void drv_gpio_modo(void){

   if (input(F_AUTOMATICO)==0){ 
      //AntiRebote1(PRESION_ALARMA );
      delay_ms(200);
      printf(lcd_putc,"\f FUNCIONAMIENTO\n");
      printf(lcd_putc,"   AUTOMATICO\n");
      Status_modo = MODO_INICIO;
      //Flag_Arranque = 1;
   }
   
   if (input(F_AUTOMATICO)==1){ 
      //AntiRebote1(PRESION_ALARMA );
      delay_ms(200);
      printf(lcd_putc,"\f FUNCIONAMIENTO\n");
      printf(lcd_putc,"     MANUAL\n");
      Status_modo = MODO_INICIO;
      //Flag_Arranque = 1;
   }
}


void drv_gpio_arranque(void){

   if (input(A_AUTOMATICO)==0 && input(A_MANUAL)==1){ 
      //AntiRebote1(PRESION_ALARMA );
      delay_ms(200);
      Status_modo = MODO_ARRANQUE;
      Select = 0;
      Flag_Alarmas = 0;
   }
   

}

void drv_gpio_arranque_P_on(void){
   output_bit(ARRANQUE_ENCENDIDO,1);
}

void drv_gpio_arranque_P_off(void){
   output_bit(ARRANQUE_ENCENDIDO,0);
}

void drv_gpio_arranque_A_on(void){
   output_bit(E_ALTERNADOR,1);
}

void drv_gpio_arranque_A_off(void){
   output_bit(E_ALTERNADOR,0);
}

void drv_gpio_solenoide_on(void){
   output_bit(SOLENOIDE,1);
}

void drv_gpio_solenoide_off(void){
   output_bit(SOLENOIDE,0);
}

void drv_gpio_transfer_on(void){
   output_bit(TRANSFER,1);
} 

void drv_gpio_transfer_off(void){
   output_bit(TRANSFER,0);
}

void drv_gpio_generador_activo(void){

   Vch1 = drv_gpio_adc_read(2);
   Vch2 = drv_gpio_adc_read(3);
   drv_gpio_solenoide_on();  // Solenoide On Intento de arranque
   
   
 
   if(Vch1 > 0 || Vch2 > 0){
   
      drv_gpio_arranque_P_off(); // Arranque para encendido off. se apaga cuando 
                                 // se cta generacionde voltaje en el generador
      drv_gpio_arranque_A_on();  // Exitacion alternador ON. se activa cuando la 
                                 // planta electrica ha entrado en funcionamiento
   }
   
   else{
      
      drv_gpio_arranque_P_on(); // Arranque para encendido ON
    
   }
   
}

void drv_activar_transfer(void){

   if(transfer_on > 240){
      drv_gpio_transfer_on();
      transfer_on = 0;
   }
  
   transfer_on++;

}


