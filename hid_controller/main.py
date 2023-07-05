from key_mapping import key_mapped
import time

'''
{
    type:

}
Mój syntax:
WRITE ala ma kota
CLICK CTRLALTDEL
END
ENDQ --exit without execution
'''
def hid_write(report):
    with open('/dev/hidg0', 'rb+') as fd:
        fd.write(report.encode())

class Command:
  def __init__(self, t, val):
    self.c_type = t
    self.c_val = val

def my_lexspars(input_cmd):
    n = len(input_cmd)
    last = input_cmd.pop()
    if(last == 'ENDQ'):
        print("Komendy zostaly anulowane")
        return
    elif(last != 'END' ):
       print("Zły syntax")
       return 
    else:
       parsed_cmd = []
       for c in input_cmd:
          inf = my_parser(c)
          if(inf == -1):
             print("Parsing error")
             return -1
          else:
             parsed_cmd.append(inf)
       my_eval(parsed_cmd)

             
            
       
def my_parser(one_cmd):
    try:
        c_type = one_cmd[:one_cmd.index(" ")]
        val = one_cmd[one_cmd.index(" ")+1:]
        return Command(c_type, val)
    except:
       return -1
       

def my_eval(exe_cmd):
   for obj in exe_cmd:
      for l in obj.c_val:
         hid_write(key_mapped[l])
         hid_write(chr(0)*8)
      print(obj.c_val)
      print(obj.c_type)

while 1:
    print("Wybierz co chcesz zrobić\n1. Payload 1\n2.Payload 2\n3.Payload 3\n4. New payload\n")
    x = input()
    if(x == '1'):
       None
    elif(x == '2'):
       None
    elif(x == '3'):
       None
    elif(x == '4'):
       y = []
       inp = ''
       print("Enter code:\n")
       while (inp != 'END' and inp != 'ENDQ'):
          inp = input()
          y.append(inp)
       time.sleep(10)
       my_lexspars(y)
       
    else:
       print("Wrong value")
       time.sleep(4)
    

