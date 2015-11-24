# TiObj
	TiObj is a lightweight data-interchange format based in the format JSON and QML.

	- TiObj has a especific field for the class of the object and it enables subclasses.
	- TiObj has a box of objects for easily way of search based in SQL.
	- TiObj has a simple sintax.



	Figura mostrado o objeto(atributos, caixa);

	The Box enables easily way of search. For example
	tiobj.select("Page");
	Here the select will return all objects of the class Page.

	tiobj.select("Page","name=page1");
	Here the select will return all objects of the class Page and that has the name equal to "page1".


![Shimmer](https://github.com/facebook/Shimmer/blob/master/shimmer.gif?raw=true)

## Usage


## Compiling


## Syntax


## Examples


## Load File
```cpp
#include <iostream>
#include <string.h>
#include <tiobj.hpp>

using namespace std;

int main(){
	TiObj obj;
	obj.loadFile("teste.ti");
	cout << obj;
	return 0;
}
```

## Contributing
See the CONTRIBUTING file for how to help out.

