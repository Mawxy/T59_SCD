#define threeWay_column1      2 
#define threeWay_inlet        3 
#define threeWay_column2      4 
#define twoWay_column1        5 
#define twoWay_column2        6 

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
    //Set relay pins all to oupout
  pinMode(threeWay_column1, OUTPUT);
  pinMode(threeWay_inlet,   OUTPUT);
  pinMode(threeWay_column2, OUTPUT);
  pinMode(twoWay_column1,   OUTPUT);
  pinMode(twoWay_column2,   OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

    digitalWrite(threeWay_inlet,   HIGH); 

  delay(5000);
  
  digitalWrite(threeWay_column1,  LOW); 
  digitalWrite(threeWay_column2,  LOW);
  digitalWrite(twoWay_column1,   LOW); 
  digitalWrite(twoWay_column2,    HIGH);

    digitalWrite(threeWay_inlet,    LOW);

  delay(5000);
    
  digitalWrite(threeWay_column1,  HIGH);
  digitalWrite(threeWay_column2, HIGH);
  digitalWrite(twoWay_column1,    HIGH);
  digitalWrite(twoWay_column2,   LOW);

}
