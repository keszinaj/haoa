# Import the library
import argparse
import parseandeval as paev
def print_logo():
    print('''

         _       _    _                   _            _          
        / /\    / /\ / /\                /\ \         / /\        
       / / /   / / // /  \              /  \ \       / /  \       
      / /_/   / / // / /\ \            / /\ \ \     / / /\ \      
     / /\ \__/ / // / /\ \ \          / / /\ \ \   / / /\ \ \     
    / /\ \___\/ // / /  \ \ \        / / /  \ \_\ / / /  \ \ \    
   / / /\/___/ // / /___/ /\ \      / / /   / / // / /___/ /\ \   
  / / /   / / // / /_____/ /\ \    / / /   / / // / /_____/ /\ \  
 / / /   / / // /_________/\ \ \  / / /___/ / // /_________/\ \ \ 
/ / /   / / // / /_       __\ \_\/ / /____\/ // / /_       __\ \_\\
\/_/    \/_/ \_\___\     /____/_/\/_________/ \_\___\     /____/_/
                                                               
'''                                                           
    )

def user_input():
    print('You are in input mode. Enter your command:')
    user_cmd = ""
    while user_cmd != "END":
        user_cmd = input('> ')
        user_cmd = paev.my_parser(user_cmd)
        if(user_cmd.c_correct):
            user_cmd.execute()
        else:
            print("Wrong syntax!")
# Create the parser
parser = argparse.ArgumentParser(
    prog="haoa",
    description="Fake usb communication software",
    epilog="Thanks for using %(prog)s! :)",
)
# add group
group = parser.add_mutually_exclusive_group()
# Add an argument
group.add_argument('-f','--file', type=str, help="execute code from pointed file")
group.add_argument('-i', '--myinput', action='store_true', help="coding mode")
# Parse the argument
args = parser.parse_args()
print(args)
print_logo()
if(args.myinput):
    user_input()
    
else:
    print("wczytywanie pliku")
    



    
    


    

