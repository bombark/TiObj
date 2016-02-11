function tiasm_parser(data){
	var size, attr, val, classe;
	magic = data[0] + data[1] + data[2] + data[3];
	if ( magic != "TiV1" )
		console.log("ERROR");

	stack = [];
	base = {}
	obj = base;
	obj["box"] = []
	for (var i=4; i<data.length; ) {
		cmd = data[i]
		i += 2;
		//console.log(cmd)
		if ( cmd == 'a' ){
			size = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;
			attr = data.substr(i,size-1)
			i += (size+1)&0xFFFFFFFE;

			val = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;


			obj[attr] = val;
			//console.log(attr+"="+val)
		} else if ( cmd == 'b' ){
			size = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;
			attr = data.substr(i,size-1)
			i += (size+1)&0xFFFFFFFE;

			val = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;

			obj[attr] = val;
			//console.log(attr+"="+val)

		} else if ( cmd == 'c' || cmd == 'd' ){
			size = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;
			attr = data.substr(i,size-1)
			i += (size+1)&0xFFFFFFFE;

			size = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;
			val  = data.substr(i,size-1)
			i += (size+1)&0xFFFFFFFE;

			obj[attr] = val;
			//console.log(attr+"="+val)
		} else if ( cmd == 'e' ){
			size = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;
			attr = data.substr(i,size-1)
			i += (size+1)&0xFFFFFFFE;

			novo = {"box":[]}
			stack.push( obj )
			obj[attr] = novo;
			obj = novo
		} else if ( cmd == 'f' ){
			size = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;
			attr = data.substr(i,size-1)
			i += (size+1)&0xFFFFFFFE;

			size   = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;
			classe = data.substr(i,size-1)
			i += (size+1)&0xFFFFFFFE;

			novo = {"class":classe,"box":[]}
			obj[attr] = novo;
			stack.push( obj )
			obj = novo

		} else if ( cmd == 'g' ){
			novo = {"box":[]}

			obj["box"].push(novo);
			stack.push( obj )
			obj = novo

		} else if ( cmd == 'h' ){
			size   = data.charCodeAt(i++) + data.charCodeAt(i++)*0x10 +  data.charCodeAt(i++)*0x100 + data.charCodeAt(i++)*0x1000;
			classe = data.substr(i,size-1)
			i += (size+1)&0xFFFFFFFE;

			novo = {"class":classe,"box":[]}
			obj["box"].push(novo);
			stack.push( obj )
			obj = novo

		} else if ( cmd == 'i' ){
			//console.log(stack.length)
			if ( stack.length > 0 )
				obj = stack.pop();
			else
				obj = base
		} else {
			return {"class":"Error"};
		}

		//console.log(i)
	}
	return base;
}
