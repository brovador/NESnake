#!/usr/bin/python
#encoding: utf-8

'''
Convert .nam files into .h headers
with the contents of the bytes

Usage:
python nam-to-c <filename.nam> <outfile.h>

Output:
<filename>.h with the contents of the generated file
'''
import os
import sys
import ntpath
from subprocess import call

script_path = os.path.dirname(os.path.realpath(__file__))

def main(file, file_out):
    filename = ntpath.basename(file).replace(' ', '_').replace('-', '_').replace('.nam', '')

    file = os.path.abspath(file)
    file_out = os.path.abspath(file_out)

    out = '''
const unsigned char {0}_nam[{1}] = {{
    {2}
}};
    '''

    #file_tmp = file + '.tmp'
    #command = '{0}/rle "{1}" "{2}"'.format(script_path, file, file_tmp)
    #call(command, shell = True)
    
    bytes = []
    size = 0
    with open(file, 'rb') as f:
        contents = f.read()
        size = len(contents)
        bytes = ['0x{:02x}'.format(ord(c)) for c in contents]

    bytes = [', '.join(bytes[x:x+8]) for x in xrange(0, len(bytes), 8)]
    out = out.format(filename, size, ',\n\t'.join(bytes))
    
    with open(file_out, 'w') as f:
        f.write(out)

    #os.remove(file_tmp)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print 'Usage: nam-to-c <.nam file> <.h out file>'
    else:
        script, file_in, file_out = sys.argv
        main(file_in, file_out)

