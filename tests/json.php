<?php
	$text = file_get_contents("teste.json");
	$obj = json_decode ( $text );
	print_r($obj);
?>
