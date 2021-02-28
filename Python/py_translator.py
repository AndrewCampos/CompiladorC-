import util

binary = list()
error = False
opcode = [['add','000000','R'],['sub','000000','R'],['mult','000000','R'],['div','000000','R'],['and','000000','R'],
         ['or','000000','R'],['nand','000000','R'],['nor','000000','R'],['sle','000000','R'],['slt','000000','R'],
         ['sge','000000','R'],['addi','000001','I'],['subi','000010','I'],['divi','000011','I'],['multi','000100','I'],
         ['andi','000101','I'],['ori','000110','I'],['nori','000111','I'],['slei','001000','I'],['slti','001001','I'],
         ['beq','001010','I'],['bne','001011','I'],['blt','001100','I'],['bgt','001101','I'],['sti','001110','I'],
         ['ldi','001111','I'],['str','010000','I'],['ldr','010001','I'],['hlt','010010','SYS'],['in','010011','SYS'],
         ['out','010100','SYS'],['jmp','010101','J'],['jal','010110','J'],['jst','010111','J'],['sdisk','011000','OS'],
         ['ldisk','011001','OS'],['sleep','011010','OS'],['wake','011011','OS'],['lstk','011100','OS'],['sstk','011101','OS']]

funct = [['add','000000'],['sub','000001'],['mult','000010'],['div','000011'],['and','000100'],['or','000101'],
         ['nand','000110'],['nor','000111'],['sle','001000'],['slt','001001'],['sge','001010']]

def getReg(reg):
    if reg == '$zero': return '0'
    elif reg == '$ret': return '30'
    elif reg ==  '$lp': return '31'
    if reg[1] == 'r' : return reg[2:]
    elif reg[1] == 'p':
        temp = int(reg[2:])
        return str(temp+19)
    else:
        print('Unexpected reg "'+ reg +'"')
        error = True

def getOpcode(op):
    for i in opcode:
        if i[0] == op: 
            return i
    return ['x','x','PSEUDO']

def getFunct(op):
    for i in funct:
        if i[0] == op: 
            return i[1]
        
count = -1
file_name = "/home/andrew/Documentos/Unifesp/CompiladorC-/gerados/"
file_name += input("File name: ")
for c in file_name:
    count+=1
    if c == '.':
        break

if count == len(file_name)-1:
    file_name += ".asb"
elif file_name[count:] != '.asb':
    print(util.RED + 'Wrong file tipe "' + file_name[count:] + '"' + util.RESET)
    exit(-1)

print("Translating assembly code from source: " + util.CYAN + file_name[53:] + util.RESET)

try:
    source = open(file_name,'r')
except:
    print(util.RED + "Can't open the file" + util.RESET)
    exit(-1)

count = 0
for i in source:
    instruction = "ram[" + str(count) + "] = {6'b"
    count += 1
    if i[0] == '/':
        instruction = i[:-1]
        binary.append(instruction)
        continue
    else:
        line = util.tokenize(i," ")
    idx = 0
    op = getOpcode(line[0])
    if op[2] == 'R':
        instruction += op[1] + ",5'd" + getReg(line[1]) + ",5'd" + getReg(line[2]) + ",5'd" + getReg(line[3]) + ",5'b00000,6'b" + getFunct(line[0]) + '}; // ' + op[0]
    elif op[2] == 'I':
        if op[0] == 'sti' or op[0] == 'ldi':
            instruction += op[1] + ",5'd0,5'd" + getReg(line[1]) + ",16'd" + line[2] + '}; // ' + op[0]
        else:
            instruction += op[1] + ",5'd" + getReg(line[1]) + ",5'd" + getReg(line[2]) + ",16'd" + line[3] + '}; // ' + op[0]
    elif op[2] == 'J':
        if op[0] == 'jst':
            instruction += op[1] + ",26'd0}; // " + op[0]
        else:
            instruction += op[1] + ",26'd" + line[1] + '}; // ' + op[0]
    elif op[2] == 'SYS':
        if op[0] == 'hlt':
            instruction += op[1] + ",26'd0}; // " + op[0]
        else:
            instruction += op[1] + ",5'd0,5'd" + getReg(line[1]) + ",16'd0}; // " + op[0]
    elif op[2] == 'OS':
        if op[0] == 'lstk' or op[0] == 'sstk':
            instruction += op[1] + ",5'd0,5'd" + getReg(line[1]) + ",16'd0}; // " + op[0]
        else:
            instruction += op[1] + ",26'd0}; // " + op[0]
    else:
        if line[0] == 'mov':
            instruction += "000001,5'd" + getReg(line[1]) + ",5'd" + getReg(line[2]) + ",16'd0}; // " + line[0]
        elif line[0] == 'put':
            instruction += "000001,5'd00000,5'd" + getReg(line[1]) + ",16'd" + line[2] + "}; // " + line[0]
        elif line[0] == 'ctso':
            instruction += "010110,26'd0}; // " + op[0]
        else:
            print(util.RED +'Unexpected instruction "' + line[0] + '"' + util.RESET)
            exit(-1)
    binary.append(instruction)

source.close()
print(util.GREEN + "Assembly code successfully translated!" + util.RESET)
code = open(file_name[:-4] + '.bin','w')
for instr in binary:
    code.write(instr+'\n')