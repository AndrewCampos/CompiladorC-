RED =    '\033[31m'
GREEN =  '\033[32m'
YELLOW = '\033[33m'
BLUE =    '\033[34m'
MAGENTA =    '\033[35m'
CYAN =    '\033[36m'
RESET =  '\033[0;0m'

# Tokenize string from the delimiter
def tokenize(str,delim):
    token = ""
    tokens = list()
    for i in str:
        if(i == delim or i == ';'): 
            tokens.append(token)
            token = ""
            if(i == ';'): return tokens
        else: token += i
    tokens.append(token)
