เอกสารอธิบายการใช้งาน Code บนบอร์ด Arduino mega 2560 และบอร์ด Arduino uno AT328p และเชือมต่อ PCB เข้ากับบอร์ด

มีอยู่สองไฟล์ที่ใช้คือ workon_mega2.ino และ workon_uno2.ino

workon_mega2.ino ใช้กับ Arduino mega 2560

workon_uno2.ino ใช้กับ Arduino uno AT328p

พอร์ตการเชื่อมต่อได้คอมเม้นต์ไว้ใน Source code เรียบร้อยแล้ว(จึงขออธิบายเพิ่มเติมเพื่อความเข้าใจ)
*****************************************************************************

โมดูลวัด CO2 ขา data ต่อเข้าขา A1 บนบอร์ด Arduino mega 2560

โมดูลวัด VOC ขา data ต่อเข้าขา A2 บนบอร์ด Arduino mega 2560

เชื่อมต่อขา 7 ของบอร์ด Arduino mega 2560 เข้ากับขา 7 ของบอร์ด Arduino uno AT328p เพื่อปรับลอจิกทางไฟฟ้า

เชื่อมต่อ GND ของทั้ง 2 บอร์ดเข้าด้วยกันเพื่อปรับลอจิกทางไฟฟ้า

ไฟแสดงสถานะที่ 1(led_status1) ต่อเข้ากับขา 2 บนบอร์ด Arduino mega 2560

ไฟแสดงสถานะที่ 2(led_status2) ต่อเข้ากับขา 3 บนบอร์ด Arduino mega 2560

พอร์ต UART(Serial RX1,TX1) ที่ 1 บนบอร์ด Arduino mega 2560 เชื่อมต่อ ขา RX,TX จากบอร์ด Arduino uno AT328p เพื่อส่งข้อมูลหากัน (https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf)

พอร์ต UART(Serial RX3,TX3) ที่ 3 บนบอร์ด Arduino mega 2560 เชื่อมต่อ ขา RX,TX จากบอร์ด A9G module เพื่อส่งข้อมูลหากัน (https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf)(https://ai-thinker-open.github.io/GPRS_C_SDK_DOC/en/hardware/pudding-dev-board.html)

ส่วน พอร์ต UART(Serial RX2,TX2) ที่ 2 ล่าสุดไม่ใช้แล้วสามารถลบโค้ดได้

โมดูลวัด SEN55-SDN-T ต่อเข้าขา SDA,SCL บนบอร์ด Arduino mega 2560 (https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf)

โมดูลวัด CH4 ขา data ต่อเข้าขา A0 บนบอร์ด Arduino uno AT328p

โมดูลวัด mq135(วัด NH3) ขา data ต่อเข้าขา A1 บนบอร์ด Arduino uno AT328p

โมดูลวัด CO ขา data เป็น I2C ให้ต่อเข้าพอร์ต I2C(SDA,SCL) ของบอร์ด Arduino uno AT328p (https://www.arnabkumardas.com/arduino-tutorial/pin-configuration-and-io-multiplexing/)

ที่ SoftwareSerial mega(2,3); จาก RX,TX จากบอร์ด Arduino mega 2560 ให้ต่อเข้าขา 2 และ ขา 3 บนบอร์ด Arduino uno AT328p ให้ดูควบคู่กับบรรทัดที่ 21 ของเอกสารนี้

ที่ SoftwareSerial ss(4,5); จาก RX,TX จากบอร์ด GPS module ให้ต่อเข้าขา 4 และ ขา 5 บนบอร์ด Arduino uno AT328p (เป็นโมดูลเรียกดูพิกัดละติจูดกับลองจิจูดของอุปกรณ์ ยังไม่ดีเท่าไหร่เพราะรับสัญญาณดาวเทียมไม่ค่อยได้)

ไฟเลี้ยง 5 V ที่จะต่อเข้าโมดูลใช้จากบอร์ดไหนก็ได้เพราะทั้งบอร์ด GND ถึงกันแล้ว

โมดูลวัด SO2 ยังไม่มีการพัฒนาในโค้ด สามารถต่อพอร์ตไหนก็ได้ที่ยังว่างอยู่โดยสามารถหา lib มาใช้ได้ ถ้าแย่จริงๆให้ใช้ Analogread อ่านค่าสดไปใช้และเทียบบัญญัติไตรยางค์กับค่า Calibrate ไปก่อน ถ้ากลับจากกรมจะรีบมาสานต่อครับ

*****************************************************************************

อย่าลืมโหลด Library ที่ใช้ไปลงด้วยนะครับ

ปล.ตอนนี้เรื่องที่จะบอกผมคิดออกแค่นี้ครับ ถ้าอยากได้อะไรเพิ่มเติมแจ้งมาในไลน์ได้เลยครับ

ปล.ไฟล์อื่นๆในโปรเจกต์นี้ที่ไม่ได้พูดถึง ส่วนใหญ่จะเป็นไฟล์ไว้ Calibrate โมดูลครับ



