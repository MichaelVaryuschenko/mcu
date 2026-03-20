from PIL import Image
import serial

image = Image.open('H:/Documents/MIPT/4th_term/MCU/ant-mcu/5 Как абстрагироваться от железа/Задания/pics/get.jpg')
px = image.load()
width, height = image.size
ser = serial.Serial(port='COM33', baudrate=115200, timeout=0.0)
if (ser.is_open):
    print(f"Port {ser.name} opened")
else:
    print(f"Port {ser.name} closed")
for i in range(width):
    for j in range(height):
        payload = "disp_px " + str(i) + " " + str(j) + " " + str(hex((px[i,j][0]<<16) + (px[i,j][1]<<8)+px[i,j][2])) + "\n"
        ser.write(payload.encode('ascii'))       
ser.close()
print("Port closed")
