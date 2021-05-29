import serial
import time

#运行前修改SERIAL_PORT为对应的串口号
SERIAL_PORT = "COM3"

str = time.strftime("ST %Y %m %d %H %M %S\r", time.localtime())
print("Send Command:\n"+ str + "\n")
try:
    ser=serial.Serial(SERIAL_PORT,9600,timeout=1)
    ser.write(str.encode("GBK"))
    ser.close();
    
except Exception as e:
    print("---异常---: ", e)

