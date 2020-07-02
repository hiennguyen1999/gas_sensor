// LUU Y: CAM BIEN KHI GAS: KHI CO GAS => TRA VE GIA TRI LOW
//                          KHI KHONG CO GAS TRA VE GIA TRI HIGH
// BAO DONG DUOC NOI VOI CHAN D4 :  D4=LOW TAT BAO DONG
//                                  D4=HIGH BAT BAO DONG
// D3 NHAN TIN HIEU CAM BIEN KHI GAS


String RxBuff = "";                                    // Khai bao bo dem nhan du lieu
int Index_Rxdata = -1;                                 // vi tri cua chuoi nhan duoc
const String myphone = "+84375032037";                  // SDT NGUOI NHAN
const int status_led = 13;
void Gsm_Init();                                       // Cau hinh Module Sim800C
void Gsm_MakeCall(String phone);                       // Ham goi dien
void Gsm_MakeSMS(String phone,String content);          // Ham nhan tin
int trang_thai=1;                                       // THONG BAO TRANG THAI MODULE
                                                        // trang_thai=1 => module dang hoat dong
                                                        // trang_thai=0 => module off
//---- CHUONG TRINH KHOI TAO  ------------
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  pinMode(status_led, OUTPUT);  //Khai bao ngo ra chan dieu khien relay
  digitalWrite(status_led, LOW);   //Khoi tao trang thai ngo ra ban dau
  pinMode(4, OUTPUT);              // D4 DUOC NOI VOI BUZZER
  pinMode(3,INPUT);                 // D3 DUNG DE NHAN TIN HIEU TU CẢM BIEN
  digitalWrite(4, LOW);
  GSM_Init();
  GSM_MakeCall(myphone);          // GOI DIEN BAO CAM BIEN DA SAN SANG HOAT DONG
 // trang_thai=1;
GSM_MakeSMS(myphone,"he thong san sang");
}

void loop() { 
  delay(1000);
  if(Serial.available()){            //Khi co tin nhan gui den module sim thi tien hanh doc va dieu khien
    serialEvent_GSM();               // CHUONG TRINH BAO CO TROM
  }
  if(trang_thai==1){
   if(digitalRead(3)== LOW){       // NEU CO KHI GAS
       digitalWrite(4, HIGH);      // BAT BAO DONG
       GSM_MakeCall(myphone);      // GOI DIEN THOAI
       // delay(1000);
        //serialEvent_GSM(); 
        //digitalRead(3)== LOW;
  }
  
}
}

//----Ham phuc vu ngat khi nhan tin nhan SMS ve SIM800L------

void serialEvent_GSM(){
  while(Serial.available()){                          //Doi den khi co du lieu nhan ve
    char inChar =(char)Serial.read();                  //Doc 1 byte du lieu vua nhan
    RxBuff += inChar;                                   //Ghi byte do vao bo dem RxBuff
    if(RxBuff.length()>=128){
      RxBuff="";
    }
  }
  int i;
// LUC NHAN DUOC TIN NHAN BAT DEN
Index_Rxdata = RxBuff.indexOf("BATBD");              // Tim vi tri cua chuoi "sw1on" trong bo dem nhan RxBuff
  if(Index_Rxdata >= 0)                                                   // Neu tim thay "LAMP_ON" trong RxBuff
  {
    Index_Rxdata = -1;                                                     // 
    //RxBuff = "";                                                                   // Xoa bo dem
    //digitalWrite(4, HIGH);                                             // Dong Relay de bat den // Bat bong den
    trang_thai=1;
    if(trang_thai==1){
        String content_sms;
        content_sms = "he thong duoc BAT";
        GSM_MakeSMS(myphone, content_sms);
        //trang_thai=1;
    }
   
    RxBuff = "";
  }
 
// LUC NHAN DUOC TIN NHAN TAT DEN 
 Index_Rxdata = RxBuff.indexOf("TATBD");              // Tim vi tri cua chuoi "sw1on" trong bo dem nhan RxBuff
  if(Index_Rxdata >= 0)                                                   // Neu tim thay "LAMP_ON" trong RxBuff
  {
    Index_Rxdata = -1;                                                     // 
    //RxBuff = "";                                                                   // Xoa bo dem
    digitalWrite(4, LOW);                                             // Dong Relay de bat den // Bat bong den
    trang_thai=0;
    if(trang_thai==0){
        String content_sms;
        content_sms = "he thong duoc TAT";
        GSM_MakeSMS(myphone, content_sms);
        
    }
    RxBuff = "";
  }
 
// LUC NHAN DUOC TIN NHAN KIEM TRA TRANG THAI 
 Index_Rxdata = RxBuff.indexOf("KIEMTRA");      // Gui tin nhan bao trang thai cua cac relay dieu khien
   if(Index_Rxdata >=0){
      String content_sms;
      if(trang_thai){
        content_sms = "he thong dang BAT";
        GSM_MakeSMS(myphone, content_sms);
      }else{
        content_sms = "he thong dang TAT";
        GSM_MakeSMS(myphone, content_sms);
      }
  RxBuff = "";
   }
   Index_Rxdata = RxBuff.indexOf("LOA");      // Gui tin nhan bao trang thai cua cac relay dieu khien
   if(Index_Rxdata >=0){
      String content_sms;
      if(digitalRead(4)){
        content_sms = "loa dang BAT";
        GSM_MakeSMS(myphone, content_sms);
      }else{
        content_sms = "loa dang TAT";
        GSM_MakeSMS(myphone, content_sms);
      }
  RxBuff = "";
   }
  // RxBuff = "";
}

//-----Cau hinh module GSM------------
void GSM_Power_On(){
  digitalWrite(4, HIGH);                // Du chan PWR_KEY len cao it nhat 1s 
  delay(1500);                                            // o day ta de 1,5s
  digitalWrite(4, LOW);                  // Du chan PWR_KEY xuong thap
  delay(100);                                              // cac ban xem trong Hardware designed sim800C de hieu ro hon
}
void GSM_Init(){
  Serial.println("ATE0");                            // Tat che do phan hoi (Echo mode)
  delay(2000);
  Serial.println("AT+IPR=9600");              // Dat toc do truyen nhan du lieu 9600 bps
  delay(2000);
  Serial.println("AT+CMGF=1");                // Chon che do TEXT Mode
  delay(2000);
  Serial.println("AT+CLIP=1");                  // Hien thi thong tin nguoi goi den
  delay(2000);
  Serial.println("AT+CNMI=2,2");              // Hien thi truc tiep noi dung tin nhan
  delay(2000);  
}
void GSM_MakeCall(String phone){
  Serial.println("ATD" + phone + ";");         // Goi dien 
  delay(20000);                                        // Sau 10s
  Serial.println("ATH");                              // Ngat cuoc goi
  delay(2000);
}
void GSM_MakeSMS(String phone, String content){
  Serial.println("AT+CMGS=\"" + phone + "\"");     // Lenh gui tin nhan
  delay(3000);                                                     // Cho ky tu '>' phan hoi ve 
  Serial.print(content);                                          // Gui noi dung
  Serial.print((char)26);                                         // Gui Ctrl+Z hay 26 de ket thuc noi dung tin nhan va gui tin di
  delay(5000);                                                      // delay 5s
}
