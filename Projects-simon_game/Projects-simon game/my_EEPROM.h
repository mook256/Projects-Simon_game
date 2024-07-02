void EEPROM_Erase_and_Write1B(uint16_t addr, uint8_t data)
{
  while(EECR & (1<<EEPE)) ; //วนซ้ำจนกว่าบิต EEPE จะเปลี่ยนเป็นตรรกะต่ำ
  EECR = 0x00;      //mode erase and write
  EEAR = addr;      //คัดลอกพารามิเตอร์รับเข้า addr สู่เรจิสเตอร์ EEAR
  EEDR = data;      //คัดลอกพารามิเตอร์รับเข้า data สู่เรจิสเตอร์ EEDR
  char backupSREG;    //ตัวแปรสำหรับเก็บค่าสถานะของเรจิสเตอร์ตัวบ่งชี้
  backupSREG = SREG;    //คัดลอกค่าในเรจิสเตอร์ตัวบ่งชี้ใส่ในตัวแปรที่ตั้งไว้
  cli();        //ปิดทางการขัดจังหวะส่วนกลางของตัวประมวลผล
  EECR |= (1<<EEMPE);   //สั่งให้บิต EEMPE ใน EECR เป็นตรรกะสูง
  EECR |= (1<<EEPE);    //สั่งให้บิต EEPE ใน EECR เป็นตรรกะสูง
  SREG = backupSREG;    //คืนค่าเรจิสเตอร์ตัวบ่งชี้กลับสู่สถานะเดิม
}



void EEPROM_Erase_only(uint16_t addr)
{  
  while(EECR & (1<<EEPE)) ; //วนซ้ำจนกว่าบิต EEPE จะเปลี่ยนเป็นตรรกะต่ำ
  EECR = 0b01 << EEPM0;   //ลบอย่างเดียว 
  EEAR = addr;      //คัดลอกพารามิเตอร์รับเข้า addr สู่เรจิสเตอร์ EEAR
  EEDR = 0xFF;      //คัดลอกพารามิเตอร์รับเข้า data สู่เรจิสเตอร์ EEDR
  char backupSREG;    //ตัวแปรสำหรับเก็บค่าสถานะของเรจิสเตอร์ตัวบ่งชี้
  backupSREG = SREG;    //คัดลอกค่าในเรจิสเตอร์ตัวบ่งชี้ใส่ในตัวแปรที่ตั้งไว้
  cli();        //ปิดทางการขัดจังหวะส่วนกลางของตัวประมวลผล
  EECR |= (1<<EEMPE);   //สั่งให้บิต EEMPE ใน EECR เป็นตรรกะสูง
  EECR |= (1<<EEPE);    //สั่งให้บิต EEPE ใน EECR เป็นตรรกะสูง
  SREG = backupSREG;    //คืนค่าเรจิสเตอร์ตัวบ่งชี้กลับสู่สถานะเดิม
}


void EEPROM_Write_to_Empty_location(uint16_t addr, uint8_t data)
{
  while(EECR & (1<<EEPE)) ; //วนซ้ำจนกว่าบิต EEPE จะเปลี่ยนเป็นตรรกะต่ำ
  EECR = 0b10 << EEPM0;    //เขียนบนตำแหน่งที่ลบเสร็จแล้วก่อนหน้านี้
  EEAR = addr;      //คัดลอกพารามิเตอร์รับเข้า addr สู่เรจิสเตอร์ EEAR
  EEDR = data;      //คัดลอกพารามิเตอร์รับเข้า data สู่เรจิสเตอร์ EEDR
  char backupSREG;    //ตัวแปรสำหรับเก็บค่าสถานะของเรจิสเตอร์ตัวบ่งชี้
  backupSREG = SREG;    //คัดลอกค่าในเรจิสเตอร์ตัวบ่งชี้ใส่ในตัวแปรที่ตั้งไว้
  cli();        //ปิดทางการขัดจังหวะส่วนกลางของตัวประมวลผล
  EECR |= (1<<EEMPE);   //สั่งให้บิต EEMPE ใน EECR เป็นตรรกะสูง
  EECR |= (1<<EEPE);    //สั่งให้บิต EEPE ใน EECR เป็นตรรกะสูง
  SREG = backupSREG;    //คืนค่าเรจิสเตอร์ตัวบ่งชี้กลับสู่สถานะเดิม
}




unsigned char EEPROM_read1byte(uint16_t addr)  //ชื่อฟังก์ชันและพารามิเตอร์รับเข้า
{       
  while (EECR & (1<<EEPE)) ;    //วนซ้ำจนกว่าบิต EEPE จะเปลี่ยนเป็นตรรกะต่ำ
  EEAR = addr;        //คัดลอกพารามิเตอร์รับเข้าสู่เรจิสเตอร์ EEAR
  EECR |= (1<<EERE);      //สั่งให้บิต EERE เป็นตรรกะสูงเพื่อเริ่มการอ่าน
  return EEDR;        //นำค่าที่ปรากฏใน EEDR ไปใช้งาน
}


void display_all_data_in_EEPROM()
{
  Serial.println("Data in the EEPROM");
  int i;
  uint8_t c;
  for (i=0;i<1024;i++)
  {
    c = EEPROM_read1byte(i);
    if( (i>0) && (i%16==0) )
        Serial.println("");
    Serial.print(c);
    Serial.print(" ");
  }
  Serial.println("\n\r--------------------------");  
}


void update_if_data_changed(uint16_t addr, uint8_t data)
{
  uint8_t x;
  x = EEPROM_read1byte(addr);
  if (x!=data)
    EEPROM_Erase_and_Write1B(addr, data);
}
