from keyMapping import key_mapped
import time

class Command():
  def __init__(self, t, val):
    self.c_type = t
    self.c_val = val
    self.c_correct = True

'''
   Object represent WAIT command
'''
class Wait(Command):
   def __init__(self, t, val):
        super().__init__(t, val)
        self.c_correct = val.isdigit()
   def execute(self):
       time.sleep(int(self.c_val))
       print("SLEEP completed")

'''
   Object represent WRITE command
'''
class Write(Command):
   def __init__(self, t, val):
        super().__init__(t, val)
   def execute(self):
       with open('/dev/hidg0', 'rb+') as fd:
        for l in self.c_val:
          try:
              fd.write(key_mapped[l].encode())
              uc = chr(0) * 8
              fd.write(uc.encode())
          except: 
              print(l + "sign is not supported")
       print("WRITE completed")

'''
   Object represent CLICK command
'''
class Click(Command):
   def __init__(self, t, val):
        super().__init__(t, val)  
   def execute(self):
       with open('/dev/hidg0', 'rb+') as fd:
        for l in self.c_val:
          try:
              fd.write(key_mapped[l].encode())
              uc = chr(0) * 8
              fd.write(uc.encode())
          except: 
              print(l + "click is not supported")
       print("click completed")  

'''
   Function parse user input
'''
def my_parser(one_cmd):
    if one_cmd == "END":
           print("Ending...")
           exit()
    try:
        c_type = one_cmd[:one_cmd.index(" ")] # take type
        val = one_cmd[one_cmd.index(" ")+1:] # take value
        if(c_type == "WRITE"):
            return Write(c_type, val)
        elif(c_type == "CLICK"):
            return Click(c_type, val)
        elif(c_type == "WAIT"):
            return Wait(c_type, val.strip())
        else:
            tmp = Command(c_type, val)
            tmp.c_correct = False
            return tmp     
    except:
        tmp = Command(" ", " ")
        tmp.c_correct = False
        return tmp
