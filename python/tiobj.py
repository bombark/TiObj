#import ctypes;
import os;

#tiparser = ctypes.cdll.LoadLibrary("libtiparser.so")
#text = ctypes.string_at(  tiparser.parseFile( ctypes.c_char_p("teste.ti") )  );
#tiparser.parseFile( ctypes.c_char_p("opaaa") );


def tiparseFile(filename):
	proc = os.popen("tiparser < "+filename);
	buffer = proc.read()
	proc.close()

	stack = [];
	base = {}
	base["box"] = []
	obj = base


	cur = 4
	while cur<len(buffer):
		c = buffer[cur+1]
		if ( c == 'a' ):
			size = ord(buffer[cur+2]) + ord(buffer[cur+3])*0x100 + ord(buffer[cur+4])*0x10000 + ord(buffer[cur+5])*0x1000000;
			attr_name = buffer[cur+6:cur+6+size]
			cur = cur + 6 + size
			val  = ord(buffer[cur]) + ord(buffer[cur+1])*0x100 + ord(buffer[cur+2])*0x10000 + ord(buffer[cur+3])*0x1000000;
			obj[ attr_name ] = val
			cur = cur + 4

		elif ( c == 'b' ):
			size = ord(buffer[cur+2]) + ord(buffer[cur+3])*0x100 + ord(buffer[cur+4])*0x10000 + ord(buffer[cur+5])*0x1000000;
			attr_name = buffer[cur+6:cur+6+size]
			cur = cur + 6 + size
			val  = ord(buffer[cur]) + ord(buffer[cur+1])*0x100 + ord(buffer[cur+2])*0x10000 + ord(buffer[cur+3])*0x1000000;
			obj[ attr_name ] = val
			cur = cur + 4

		elif ( c == 'c' or c == 'd' ):
			size = ord(buffer[cur+2]) + ord(buffer[cur+3])*0x100 + ord(buffer[cur+4])*0x10000 + ord(buffer[cur+5])*0x1000000;
			attr_name = buffer[cur+6:cur+6+size]
			cur = cur + 6 + size
			size = ord(buffer[cur]) + ord(buffer[cur+1])*0x100 + ord(buffer[cur+2])*0x10000 + ord(buffer[cur+3])*0x1000000;
			obj[ attr_name ] = buffer[cur+4:cur+4+size]
			cur = cur + 4 + size
		elif ( c == 'e' ):
			size = ord(buffer[cur+2]) + ord(buffer[cur+3])*0x100 + ord(buffer[cur+4])*0x10000 + ord(buffer[cur+5])*0x1000000;
			attr_name = buffer[cur+6:cur+6+size]
			cur = cur + 6 + size
			novo = {}
			novo["box"] = []
			obj[ attr_name ] = novo
			stack.append( obj )
			obj = novo
		elif ( c == 'f' ):
			size = ord(buffer[cur+2]) + ord(buffer[cur+3])*0x100 + ord(buffer[cur+4])*0x10000 + ord(buffer[cur+5])*0x1000000;
			attr_name  = buffer[cur+6:cur+6+size]
			cur = cur + 6 + size
			size = ord(buffer[cur]) + ord(buffer[cur+1])*0x100 + ord(buffer[cur+2])*0x10000 + ord(buffer[cur+3])*0x1000000;
			class_name = buffer[cur+4:cur+4+size]
			cur = cur + 4 + size;
			novo = {}
			novo["box"] = []
			novo["class"] = class_name
			obj[ attr_name ] = novo
			stack.append( obj )
			obj = novo
		elif ( c == 'g' ):
			novo = {}
			novo["box"] = []
			obj["box"].append( novo )
			stack.append( obj )
			obj = novo
			cur = cur + 2
		elif ( c == 'h' ):
			size = ord(buffer[cur+2]) + ord(buffer[cur+3])*0x100 + ord(buffer[cur+4])*0x10000 + ord(buffer[cur+5])*0x1000000;
			attr_name  = buffer[cur+6:cur+6+size]
			cur = cur + 6 + size
			novo = {}
			novo["box"] = []
			novo["class"] = attr_name;
			obj["box"].append( novo )
			stack.append( obj )
			obj = novo

		elif ( c == 'i' ):
			if ( len(stack) > 0 ):
				obj = stack.pop();
			cur = cur + 2;
		else:
			print 'error ',c
			break;


	return obj;


obj = tiparseFile("all.ti")
#print obj




