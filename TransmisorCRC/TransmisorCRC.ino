#include <SoftwareSerial.h>

SoftwareSerial mySerial1(11, 10); // RX, TX 

int SENSOR = 7;
int temperatura = 0;
int pinLM35 = 0;
String temperaturaString;

void setup()
{
  Serial.begin(9600);
  mySerial1.begin(2400); 
}

void loop()
{
  temperatura = analogRead(pinLM35);
  temperatura = ((5.0 * temperatura * 100.0)/1024.0) ;

  if(isnan(temperatura)){
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }

  Serial.print("Temperatura:");
  Serial.println(temperatura);
  
  temperatura = MessageCRC(temperatura, 9);
  temperaturaString = String(temperatura);

  
  for(int n = 0; n<3; n++){
    mySerial1.print("U");
    mySerial1.print(temperaturaString);
  }

  Serial.print("El mensaje transmitido es el siguiente:");
  Serial.println(temperaturaString);
  Serial.println(" ");  
  temperaturaString = "";
  
  delay(100);
  //12 chars... 12x8=96bits... 96bits/2400bps =  40ms
  // 2400bps la velocidad de la com seria del transmisor
}

int MessageCRC(int message, int polynomial){
  //Primer polinomio x^3 + 1
  //Es igual a BIN => 1001 = DEC => 9

  //Ahora sigue la division del polinomio generador (1001) entre la trama, a la 
  //Cual se debe de hacer un corrimiento a la izquierda igual al grado del polinomio en decimal
  //O sea x^3 +1, o si esta en BIN (1001) es igual al numero de bits - 1

  //Quedando así una division, con una trama de datos ejemplo de (101101)
  // 101101000 / 1001 , Se usa la division con la compuerta XOR 

  //Se hará la division hasta haber acabado con todos los bits de la trama, y por ultimo
  //se guardará el residuo de la division, la cantidad de bits del residuo deben ser
  //el mismo numero de bits igual al grado del polinomio

  //Una vez hecho ahora en vez de los ceros colocados a la trama, se sustituira por el residuo obtenido y 
  //Este sera el que sea enviado hacia el receptor

  //Aqui se usar primero el polinomio x^3 + 1 => 1001

  //x^3 + 1 => (1001) __ x^4+x^3+1 => (11001) => 25
  int polynomialDec = polynomial; 
  int grado = minOne(polynomialDec);
  
  //Desplazamiento de ceros igual al grado del polinomio
  int dataWithDespCeros = message << grado; 

  int CRCObtenido;
  int polynomialBit; 
  int messageBit; 
  int messageToTransmit;
  
  Serial.print("Mensaje a Transmitir: ");
  Serial.println(message,BIN);
  
  Serial.print("Mensaje recorrido: ");
  Serial.println(dataWithDespCeros,BIN);
  
  Serial.println("Polinomio en Decimal y Binario");
  Serial.print("Polynomial: ");
  Serial.println(polynomialDec);
  
  Serial.print("Polynomial in BIN: ");
  Serial.println(polynomialDec, BIN);
  
   
  messageBit = minOne(dataWithDespCeros); 
  polynomialBit = minOne(polynomialDec);
  
  //Se hace el corrimiento del polinomio para que este del mismo tamaño que el mensaje
  polynomialDec = polynomialDec << messageBit - polynomialBit;
  Serial.print("Corrimiento de polinomio al tamaño del mensaje: ");
  Serial.println(polynomialDec, BIN);

  //XOR del mensaje y polinomio recorridos
  CRCObtenido = CRC(dataWithDespCeros, polynomialDec, grado);
  Serial.print("CRC Obtenido despues de algoritmo: ");
  Serial.println(CRCObtenido,BIN);
  
  messageToTransmit = dataWithDespCeros ^ CRCObtenido;
  Serial.print("El mensaje a transmitir es: ");
  Serial.println(messageToTransmit,BIN);
  
  return messageToTransmit;
}

int minOne(int message){
  //En esta funcion se requiere obtener el primer indice de un 1, o sea el bit mas significativo
  //Ej, 001101001, Aqui el primer indice que contiene un 1, seria el indice 6, o sea el bit numero 7
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
  //Aqui para obtener el CRC lo que se hace es hacer el XOR del mensajey el polinomio recorridos
  //el resultado de este XOR será el residuo obtenido, por lo que se volvera a mandar en la misma funcion haciendo recursividad
  //Se hara la recursividad hasta el punto donde el numero de bits del residuo sea igual al del grado del polinomo
  //En ese punto se terminaran los recursividades y se obtendra por ultimo el codigo CRC a colocar junto con el mensaje a enviar
  
  int obtenidoCRC = message; 
  int polynomialAux = polynomial;
   
  if(minOne(message) < grado || message == 0){ 
    return obtenidoCRC;
  }else{
    obtenidoCRC = message ^ polynomialAux;
    polynomialAux = polynomial >> minOne(polynomial) - minOne(obtenidoCRC);
    obtenidoCRC = CRC(obtenidoCRC,polynomialAux, grado);
  }
  return obtenidoCRC;
}
