<?php

require("tiparser.php");


class TiObj {
	public $class = "";
	public $box   = array();

	function __construct($obj_str=""){
		if ( $obj_str != "" )
			$this->loadText($obj_str);
	}

	function loadText($obj_str){
		$parser = new TiParser();
		$parser->loadText($obj_str);
		$parser->parse($this, 0);
	}

	function loadFile($filename){
		$parser = new TiParser();
		$parser->loadFile($filename);
		$parser->parse($this, 0);
	}

	function toString(){
		$res = "";
		$class_vars = get_object_vars($this);
		$res .= $this->class."{";
		foreach ($class_vars as $name => $value) {
			if ( $name == "class" || $name == "box" )
				continue;
			$res .= "$name=";
			if ( is_string($this->$name) ){
				$res .= "'$value';";
			} else if ( is_int($this->$name) ){
				$res .= "$value;";
			} else if ( is_float($this->$name) ){
				$res .= "$value;";
			} else if ( is_array($this->$name) ){
				$res .= "[";
				foreach ($this->$name as $item){
					if ( is_string($item) ){
						$res .= "'$item';";
					} else if ( is_int($item) ){
						$res .= "$item;";
					}
				}
				$res .= "]";
			} else if ( is_object($this->$name) ){
				$res .= $this->$name->toString();
			}
		}
		foreach ($this->box as $item){
			$res .= $item->toString();
		}
		$res .= "}";
		return $res;
	}

	function write(){
	}

	function isA($classname){
		$vetclass = explode(":",$this->class);
		foreach ($vetclass as $class){
			if ( $class == $classname )
				return true;
		}
		return false;
	}

	function has($field){
		return isset($this->$field);
	}

	function addObject($obj){
		$this->box[] = $obj; 
	}

	function select($classes, $where=""){
		$out = new TiObj();
		$vetclass = explode(",",$classes);

		foreach ( $this->box as $item ){
			foreach ( $vetclass as $class ){
				if ( $item->isA($class) ){
					$out->box[] = $item;
				}
			}
		}

		return $out;
	}
}



?>
