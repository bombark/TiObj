<?php
	$raw = "";
	exec("tiparser all.ti", $raw);
	$data = implode('\n',$raw);

	$stack = array();
	$obj = new StdClass;
	//$obj->box = array();

	$i = 4;
	$final=strlen($data);
	while ( $i<$final ){
		$cmd = $data[$i+1];

		if ( $cmd == 'a' ){
			$size = ord($data[$i+2]) + ord($data[$i+3])*0x100 + ord($data[$i+4])*0x10000 + ord($data[$i+5])*0x1000000;
			$i += 6;
			$name = substr($data,$i,$size);
			$i += $size;
			$obj->$name = (int) ord($data[$i]) + ord($data[$i+1])*0x100 + ord($data[$i+2])*0x10000 + ord($data[$i+3])*0x1000000;
			$i += 4;

		} else if ( $cmd == 'b' ){
			$size = ord($data[$i+2]) + ord($data[$i+3])*0x100 + ord($data[$i+4])*0x10000 + ord($data[$i+5])*0x1000000;
			$i += 6;
			$name = substr($data,$i,$size);
			$i += $size;
			$val = (ord($data[$i]) + ord($data[$i+1])*0x100 + ord($data[$i+2])*0x10000 + ord($data[$i+3])*0x1000000);
			settype ($val,"float");
			echo $val;
			$i += 4;

		} else if ( $cmd == 'c' || $cmd == 'd' ){
			$size = ord($data[$i+2]) + ord($data[$i+3])*0x100 + ord($data[$i+4])*0x10000 + ord($data[$i+5])*0x1000000;
			$i += 6;
			$name = substr($data,$i,$size);
			$i += $size;

			$size  = ord($data[$i]) + ord($data[$i+1]) + ord($data[$i+2]) + ord($data[$i+3]);
			$i += 4;
			$obj->$name = substr($data,$i,$size);
			$i += $size;

		} else if ( $cmd == 'e' ){
			$size = ord($data[$i+2]) + ord($data[$i+3])*0x100 + ord($data[$i+4])*0x10000 + ord($data[$i+5])*0x1000000;
			$i += 6;
			$name = substr($data,$i,$size);
			$i += $size;

			array_push($stack, $obj);
			$novo = new StdClass();
			//$novo->box = array();
			$obj->$name = $novo;
			$obj = $novo;

		} else if ( $cmd == 'f' ){
			$size = ord($data[$i+2]) + ord($data[$i+3])*0x100 + ord($data[$i+4])*0x10000 + ord($data[$i+5])*0x1000000;
			$i += 6;
			$name = substr($data,$i,$size);
			$i += $size;

			$novo = new StdClass();
			//$novo->box = array();
			$size  = ord($data[$i]) + ord($data[$i+1]) + ord($data[$i+2]) + ord($data[$i+3]);
			$i += 4;
			$novo->class = substr($data,$i,$size);
			$i += $size;

			array_push($stack, $obj);
			$obj->$name = $novo;
			$obj = $novo;

		} else if ( $cmd == 'g' ){
			array_push($stack, $obj);
			$novo = new StdClass();
			//$novo->box = array();
			$obj->box[] = $novo;
			$obj = $novo;
			$i += 2;

		} else if ( $cmd == 'h' ){
			array_push($stack, $obj);
			$novo = new StdClass();
			//$novo->box = array();
			$size = ord($data[$i+2]) + ord($data[$i+3])*0x100 + ord($data[$i+4])*0x10000 + ord($data[$i+5])*0x1000000;
			$i += 6;
			$novo->class = substr($data,$i,$size);
			$i += $size;

			$obj->box[] = $novo;
			$obj = $novo;

		} else if ( $cmd == 'i' ){
			if ( count($stack) > 0 )
				$obj = array_pop($stack);
			$i += 2;
		} else {
			echo "Error";
			break;
		}
	}
	print_r($obj);
?>
