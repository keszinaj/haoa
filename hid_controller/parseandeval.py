from keyMapping import key_mapped
import time

'''
{
    type:

}
Mój syntax:
WRITE ala ma kota
CLICK CTRLALTDEL
WAIT 10
END
ENDQ --exit without execution
'''
def hid_write(report):
    with open('/dev/hidg0', 'rb+') as fd:
        fd.write(report.encode())

class Command():
  def __init__(self, t, val):
    self.c_type = t
    self.c_val = val
    self.c_correct = True

class Wait(Command):
   def __init__(self, t, val):
        super().__init__(t, val)
        self.c_correct = val.isdigit()
   def execute(self):
       time.sleep(int(self.c_val))
       print("SLEEP completed")

class Write(Command):
   def __init__(self, t, val):
        super().__init__(t, val)
   def execute(self):
       for l in self.c_val:
         print(key_mapped[l].encode())
         print(chr(0)*8)
       print("WRITE completed")

class Click(Command):
   def __init__(self, t, val):
        super().__init__(t, val)  
   def execute(self):
       print(self.c_val)
       print(self.c_type)
       print(chr(0)*8)
       print("click completed")  

'''WRITE ala ma kota
CLICK CTRLALTDEL
WAIT'''

def my_parser(one_cmd):
    if one_cmd == "END":
           print("Ending...")
           exit()
    try:
        c_type = one_cmd[:one_cmd.index(" ")]
        val = one_cmd[one_cmd.index(" ")+1:]
        if(c_type == "WRITE"):
            return Write(c_type, val)
        elif(c_type == "CLICK"):
            return Click(c_type, val)
        elif(c_type == "WAIT"):
            return Wait(c_type, val)
        else:
            tmp = Command(c_type, val)
            tmp.c_correct = False
            return tmp     
    except:
        tmp = Command(" ", " ")
        tmp.c_correct = False
        return tmp
       

   
