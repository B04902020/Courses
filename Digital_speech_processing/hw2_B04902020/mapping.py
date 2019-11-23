# coding=utf8
import sys
in_file = sys.argv[1]
out_file = sys.argv[2]

table = {}
fp = open(in_file, 'rb')
for line in fp:
    data = line.decode('big5-hkscs').strip().split(' ')
    chi_word = data[0]                   
    zy_list = data[1].split('/')    
    
    for each_zy in zy_list:
        if each_zy[0] in table:
            table[each_zy[0]].append(chi_word)
        else:
            table[each_zy[0]] = [chi_word]
fp.close()

for key in table:
    table[key] = list(set(table[key]))              #去除重複中文字

check_chi = []

fp = open(out_file, 'wb')
for key in table:
    fp.write((key+" ").encode('big5-hkscs'))
    for value in table[key]:
        fp.write((value+" ").encode('big5-hkscs'))
    fp.write(bytes("\n", 'big5-hkscs'))

    for value in table[key]:
        if value not in check_chi: 
            check_chi.append(value)
            fp.write((value+" ").encode('big5-hkscs'))
            fp.write((value+"\n").encode('big5-hkscs'))
