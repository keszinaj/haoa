import argparse
import logo
import parseandeval as paev

'''
   Handle user command input
'''
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

'''
   Handle user script input
'''
def file_input(file):
    try:
        with open(file, 'r') as f:
           for user_cmd in f:
               print(user_cmd[:-1])
               user_cmd = paev.my_parser(user_cmd.rstrip())# with deleted \n           
               if(user_cmd.c_correct):
                  user_cmd.execute()
               else:
                 print("Wrong syntax!")
    except IOError as e:
        print("I/O error({0}): {1}".format(e.errno, e.strerror))

'''
   Handle parsing input
'''
def parse_argument():
    parser = argparse.ArgumentParser(
        prog="haoa",
        description="Fake usb communication software",
        epilog="Thanks for using %(prog)s! :)",
    )
    group = parser.add_mutually_exclusive_group()
    group.add_argument('-f','--file', type=str, help="execute code from pointed file")
    group.add_argument('-i', '--myinput', action='store_true', help="coding mode")
    return parser.parse_args()

'''
   Main function
'''
def main():
    args = parse_argument()
    logo.print_logo()
    if(args.myinput):
        user_input()   
    elif(args.file != None):
        file_input(args.file)
    else:
        user_input()   
   
if __name__ == '__main__': main()

    



    
    


    

