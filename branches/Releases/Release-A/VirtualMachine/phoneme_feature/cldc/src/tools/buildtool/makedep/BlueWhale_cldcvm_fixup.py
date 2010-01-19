import sys
import re

print("cldcvm_fixup");

matches = [
    [r'const int _rom_check_JavaFrame__stack_bottom_pointer_offset = -20;',"const int _rom_check_JavaFrame__stack_bottom_pointer_offset = 4;\n"],
    [r'const int _rom_check_JavaFrame__return_address_offset = 4;',"const int _rom_check_JavaFrame__return_address_offset = -20;\n"],
    [r'const int _rom_check_EntryFrame__stored_obj_value_offset = -20;',"const int _rom_check_EntryFrame__stored_obj_value_offset = 4;\n"],
    [r'const int _rom_check_EntryFrame__fake_return_address_offset = 4;',"const int _rom_check_EntryFrame__fake_return_address_offset = -20;\n"],
    [r'const int _rom_check_StackValue__stack_tag_offset = -4;',"const int _rom_check_StackValue__stack_tag_offset = 4;\n"]
    ]

if len(sys.argv) < 2:
    print "!Error, no file specified"
    sys.exit(-1)

fileToModify = sys.argv[1]
file = open(fileToModify,'r')
lines = file.readlines() # read the whole file into memory
file.close()

outdata = []

for fileLine in lines:
    found = 0
    for srchString,repString in matches:
        if re.search(srchString,fileLine):
            outdata.append(repString)
            found = 1
    if found == 0:
        outdata.append(fileLine)

out = open(fileToModify,'w')
out.writelines(outdata)
out.close()
