<?php


class TiBuffer {
	private $text = "";	
	private $cursor;
	private $size;

	function loadText($text){
		$this->cursor = 0;
		$this->text = $text;
		$this->size = strlen($text);
	}

	function loadFile($filename){
		$this->cursor = 0;
		$this->text = file_get_contents($filename);
		$this->size = strlen($text);
	}

	function next(){
		if ( $this->cursor >= $this->size ){
			return '';
		}
		return $this->text[$this->cursor++];
	}

	function back(){
		if ( $this->cursor > 0 ){
			$this->cursor--;
		}
	}

	function isContinue(){
		if ( $this->cursor >= $this->size )
			return false;
		return true;
	}
}

class TiLex {
	private $lastsymbol;
	private $symbols = array();
	private $buffer;

	const  L_NONE = 0;
	const  L_CHAR = 1;
	const   L_INT = 2;
	const L_FLOAT = 3;
	const  L_ASPA = 4;
	const  L_SYMB = 5;


	function __construct (){
		$this->buffer = new TiBuffer();
		for ($i=0; $i<256; $i++){
			$this->symbols[$i] = self::L_NONE;
		}
		for ($i='a'; $i<='z'; $i++){
			$this->symbols[$i] = self::L_CHAR;
		}
		for ($i='A'; $i<='Z'; $i++){
			$this->symbols[$i] = self::L_CHAR;
		}
		for ($i='0'; $i<='9'; $i++){
			$this->symbols[$i] = self::L_INT;
		}
		$this->symbols[' ']  = self::L_NONE;
		$this->symbols['=']  = self::L_SYMB;
		$this->symbols[':']  = self::L_CHAR;
		$this->symbols[';']  = self::L_SYMB;
		$this->symbols['.']  = self::L_SYMB;
		$this->symbols[',']  = self::L_SYMB;
		$this->symbols['(']  = self::L_SYMB;
		$this->symbols[')']  = self::L_SYMB;
		$this->symbols['{']  = self::L_SYMB;
		$this->symbols['}']  = self::L_SYMB;
		$this->symbols['[']  = self::L_SYMB;
		$this->symbols[']']  = self::L_SYMB;
		$this->symbols['\''] = self::L_ASPA;
		$this->symbols['\"'] = self::L_ASPA;
		$this->symbols['-']  = self::L_INT;
		$this->symbols['_']  = self::L_CHAR;
		$this->symbols['@']  = self::L_CHAR;
		$this->symbols['$']  = self::L_CHAR;
		$this->symbols['&']  = self::L_CHAR;
		$this->symbols['#']  = self::L_CHAR;
		$this->lastsymbol = 0;
	}


	function loadFile($filename){
		$this->buffer->loadFile($filename);
	}

	function loadText($text){
		$this->buffer->loadText($text);
	}

	function next(){
		$out["token"] = "";
		$out["type"] = self::L_NONE;
		$c = '';
		$aspa=0;
		$type = $this->symbols[$this->lastsymbol];
		if ( $type == self::L_SYMB ){
			$this->buffer->back();
		} else if ( $type == self::L_CHAR ){
			$this->buffer->back();
		}
		$this->lastsymbol = 0;

		while ( $this->buffer->isContinue() ){
			$c = $this->buffer->next();
			$type = $this->symbols[$c];
			if ( $type != self::L_NONE ){
				$aspa = $c;
				break;
			}
		}
		if ( $type == self::L_SYMB ){
			$out['token'] = $c;
			$out['type']  = self::L_SYMB;
			return $out;
		} else if ( $type == self::L_ASPA ){
			$special = false;
			while ( $this->buffer->isContinue() ){
				$c = $this->buffer->next();
				if ( $special ){
					if ( $c == 'n' )
						$out['token'] .= '\n';
					else if ( $c == 't' )
						$out['token'] .= '\t';
					else if ( $c == '\'' )
						$out['token'] .= '\'';
					else if ( $c == '\"' )
						$out['token'] .= '\"';
					else if ( $c == '\\' )
						$out['token'] .= '\\';
					$special = false;
				} else if ( $c == '\\' ){
					$special = true;
				} else if ( $c == $aspa ){
					break;
				} else
					$out['token'] .= $c;
			}
			$out['type'] = self::L_CHAR;
		} else if ( $type == self::L_CHAR ){
			$out['token'] = $c;
			while ( $this->buffer->isContinue() ){
				$c = $this->buffer->next();
				$type = $this->symbols[$c];
				if ( $type == self::L_CHAR || $type == self::L_INT ){
					$out['token'] .= $c;
				} else {
					$this->lastsymbol = $c;
					break;
				}
			}
			$out['type'] = self::L_CHAR;
		} else if ( $type == self::L_INT ){
			$out['token'] = $c;
			$out['type'] = self::L_INT;
			while ( $this->buffer->isContinue() ){
				$c = $this->buffer->next();
				$type = $this->symbols[$c];
				if ( $c == '.' || $c == 'e' ){
					$out['type'] = self::L_FLOAT;
				} else if ( $type != self::L_INT ){
					$this->lastsymbol = $c;
					break;
				}
				$out['token'] .= $c;
			}
		}
		return $out;
	}

	function isContinue(){
		return $this->buffer->isContinue();
	}
}







class TiParser {
	private $mem_i=0;
	private $lex;
	private $translate = array();
	private $memory = array();
	private $PARSER_ACTION;
	private $PARSER_NEXT;
	private $PARSER_REDUCE;

	function __construct(){
		$this->lex = new TiLex();
		for ($i=0; $i<256; $i++){
			$this->translate[$i] = 0;
		}
		$this->translate['{'] = 4;
		$this->translate['}'] = 5;
		$this->translate['['] = 6;
		$this->translate[']'] = 7;
		$this->translate['='] = 8;
		$this->translate[';'] = 9;

		$this->PARSER_ACTION = array (
				array(0, 0,0,0, 0,0, 0,0, 0,0),
				array(0, 1,0,0, 3,3, 0,0, 0,0),
				array(0, 0,0,0, 3,0, 0,0, 2,0),
				array(0, 1,1,1, 3,0, 3,0, 0,0),
				array(0, 0,0,0, 3,0, 0,0, 0,3),
				array(0, 0,0,0, 0,0, 0,0, 0,3),
				array(0, 0,0,0, 0,0, 0,0, 0,3)
		);

		$this->PARSER_NEXT = array (
				array(0, 0,0,0, 0,0, 0,0, 0,0),
				array(0, 2,0,0, 1,0, 0,0, 0,0),
				array(0, 0,0,0, 1,0, 0,0, 3,0),		
				array(0, 4,5,6, 1,0, 1,0, 0,0),
				array(0, 0,0,0, 1,0, 0,0, 0,1),
				array(0, 0,0,0, 0,0, 0,0, 0,1),
				array(0, 0,0,0, 0,0, 0,0, 0,1)
		);

		$this->PARSER_REDUCE = array (
				array(0, 0,0,0, 0,0, 0,0, 0,0),
				array(0, 0,0,0, 5,4, 0,0, 0,0),
				array(0, 0,0,0, 6,0, 0,0, 0,0),	
				array(0, 0,0,0, 7,0, 9,0, 0,0),	 
				array(0, 0,0,0, 8,0, 0,0, 0,1),
				array(0, 0,0,0, 0,0, 0,0, 0,2),
				array(0, 0,0,0, 0,0, 0,0, 0,3)
		);

	}

	function loadFile($filename){
		$this->lex->loadFile($filename);
	}

	function loadText($text){
		$this->lex->loadText($text);		
	}

	function parse($obj, $level=0){
		$ivalue = 0;
		$fvalue = 0.0;
	
		do {

			$reduce = $this->step();
			if ( $reduce == 0 ){
				return false;
			} else if ( $reduce == 1 ){
				$keyname = $this->memory[$this->mem_i-2];
				$obj->$keyname = $this->memory[$this->mem_i-1];
				$this->memPop(2);
			} else if ( $reduce == 2 ){
				$ivalue = (int)($this->memory[$this->mem_i-1]);
				$keyname = $this->memory[$this->mem_i-2]; 
				$obj->$keyname = $ivalue;
				$this->memPop(2);
			} else if ( $reduce == 3 ){
				$fvalue = (float)($this->memory[$this->mem_i-1]);
				$keyname = $this->memory[$this->mem_i-2];
				$obj->$keyname = $fvalue;
				$this->memPop(2);
			} else if ( $reduce == 4 ){
				break;
			} else if ( $reduce == 5 ){
				if ( $level > 0 ){
					$novo = new TiObj();
					if ( $this->parse($novo, $level+1) == false ){
						return false;
					}
					$obj->addObject($novo);
				} else {
					$level = 1;
				}
			} else if ( $reduce == 6 ){
				if ( $level > 0 ){
					$novo = new TiObj();
					if ( $this->parse($novo, $level+1) == false ){
						return false;
					}
					$novo->class = $this->memory[$this->mem_i-1];
					$obj->addObject($novo);
					$this->memPop(1);
				} else {
					$level = 1;
					$obj->class = $this->memory[$this->mem_i-1];
					$this->memPop(1);
				}
			} else if ( $reduce == 7 ){
				$novo = new TiObj();
				if ( $this->parse($novo, $level+1) == false ){
					return false;
				}
				$keyname = $this->memory[$this->mem_i-1]; 
				$obj->$keyname = $novo;
				$this->memPop(1);
			} else if ( $reduce == 8 ){
				$novo = new TiObj();
				if ( $this->parse($novo, $level+1) == false ){
					return false;
				}
				$novo->class = $this->memory[$this->mem_i-1];
				$keyname = $this->memory[$this->mem_i-2];  
				$obj->$keyname = $novo;
				$this->memPop(2);
			} else if ( $reduce == 9 ){
				$vetor = $this->parseVector();
				if ( $vetor == NULL ){
					return false;
				}
				$keyname = $this->memory[$this->mem_i-1]; 
				$obj->$keyname = $vetor;
				$this->memPop(1);
			}
		} while (true);
		return true;
	}


	function step(){
		$state=1;
		if ( $this->lex->isContinue() == false )
			return 0;
		
		do {
			$fout  = $this->lex->next();
			$token = $fout["token"];
			$type  = $fout["type"];

			// Next state
			$csy = $type;
			if ( $csy == TiLex::L_SYMB ){
				$csy = $this->translate[$token[0]];
			}
			$act = $this->PARSER_ACTION[$state][$csy];
			if ( $act == 0 ){
				echo "ERROR!\n";
				return false;
			} else if ( $act == 1 ){
				$this->memPush($token);
			} else if ( $act == 2 ){
				// nao faz nada;
			} else if ( $act == 3 ){
				$reduce = $this->PARSER_REDUCE[$state][$csy];
				return $reduce;
			}
			$state = $this->PARSER_NEXT[$state][$csy];
		} while ( $this->lex->isContinue() == true );
		if ( $state != 1 ){
			echo "ERROR!\n";
			return 0;
		}

	}
 


	function memPush($str){
		$this->memory[$this->mem_i++] = $str;
	}

	function memPop($size){
		$this->mem_i -= $size;
	}

	function memWrite(){
		/*cout << mem_i << endl;
		for (int i=0; i<mem_i; i++){
			cout << memory[i] << endl;
		}*/
	}

	function parseVector(){
		$out = array();
		while ( $this->lex->isContinue() == true ){
			$fout = $this->lex->next();
			$token = $fout["token"];
			$type = $fout["type"];

			if ( $token == "]" ){
				return $out;
			} else if ($type == TiLex::L_CHAR ){
				$out[] = $token;
			} else if ($type == TiLex::L_INT ){
				$out[] = (int)$token;
			} else if ($type == TiLex::L_FLOAT ){
				$out[] = (float)$token;
			} else if ($type == TiLex::L_SYMB ){
				continue;
			} else {
				// Mensagem de Erro
			}	
		}
		return NULL;	// Colocar aviso de Erro(A pessoa esqueceu de colocar um ']')*/
	}
}

?>
