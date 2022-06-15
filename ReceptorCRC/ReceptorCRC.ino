#include <SoftwareSerial.h>

SoftwareSerial mySerial1(11, 10); // RX, TX

String mensajeString;
String mensajeStringOk;
char caracter;
int sumError = 0;

void setup() {
  Serial.begin(9600);
  mySerial1.begin(2400);
}

void loop() {
  //Serial.println("");
  //Serial.println("----Entra----");
  int mensajeInt;
  int resultadoCRC;
  
  while(mySerial1.available()) {  
    if (mySerial1.available() > 0){    
        caracter = mySerial1.read();         
        mensajeString += caracter;
    }   
  }

  //mensajeString = "101U101110011U011101";
  //Serial.println("Trama recibida: ");
  //Serial.println(mensajeString);
  
    
  int indexOfU = mensajeString.indexOf('U');
 
  //Serial.print("indice obtenido de U: ");
  //Serial.println(indexOfU);
  
  if(indexOfU > 0){
    for(int i = indexOfU + 1; i < indexOfU + 4 ; i++){
      mensajeStringOk += mensajeString[i];
    } 
  }

  //Serial.print("El mensaje recibido en la trama es: ");
  //Serial.println(mensajeStringOk);

  if(mensajeStringOk != ""){
    mensajeInt = mensajeStringOk.toInt();
    resultadoCRC = ProcedimientoCRC(mensajeInt,9);
    
    //Serial.print("El Resultado de la comprobacion CRC es: ");
    //Serial.println(resultadoCRC, BIN);

    if(resultadoCRC == 0 ){
      //Serial.println("El mensaje recibido fue correcto");
    }else{
      sumError++;
      //Serial.println("El mensaje recibido fue erroneo, se suma uno al contador");
    }

    int temperaturaRecibida = mensajeInt >> 3;
  
    //Serial.print("El mensaje recibido es: ");
    //Serial.println(mensajeInt,BIN); 
    //Serial.print("Temperatura recibida: ");
    Serial.println(temperaturaRecibida);
    //Serial.print("El numero de errores es: ");
    //Serial.println(sumError); 
  }else {
    sumError++;
//    Serial.println("El mensaje recibido fue erroneo, se suma uno al contador");
//    Serial.print("El numero de errores es: ");
//    Serial.println(sumError);
  }

  delay(100); //12 chars... 12x8=96bits... 96bits/2400bps =  40ms
  
  mensajeString = "";
  mensajeStringOk = "";
}

int ProcedimientoCRC(int message, int polynomial){
  int messageRecibido = message;
  
  int polynomialDec = polynomial;
  int grado = minOne(polynomialDec);
  
  int polynomialBit; 
  int messageBit; 
  
  int CRCObtenido;
  
  messageBit = minOne(messageRecibido);
  polynomialBit = minOne(polynomialDec);
  
  polynomialDec = polynomialDec << messageBit - polynomialBit;
  
  CRCObtenido = CRC(messageRecibido, polynomialDec, 25);
 
  return CRCObtenido;
}

int minOne(int message){
  int i = 0;
  int firstIndex = 0;
  
  if(message == 0){
    return 0;
  }else{
    for( i=16; i>0; i--){ //int tiene 16 bits maximo a buscar
      if(bitRead(message,i-1) == 1){
        firstIndex = i-1;
        break;
      }
    }
    return firstIndex;
  }
}

int CRC(int message, int polynomial, int grado){
  int obtenidoCRC = message; 

  int polynomialAux = polynomial;
  
  if(minOne(message) == 1 || message == 0){ 
    return obtenidoCRC;
  }else{
    obtenidoCRC = message ^ polynomialAux;
    polynomialAux = polynomial >> minOne(polynomial) - minOne(obtenidoCRC);
    obtenidoCRC = CRC(obtenidoCRC,polynomialAux, grado);
  }
  return obtenidoCRC;
}
