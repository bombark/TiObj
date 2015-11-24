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


### Load File
```cpp
#include <iostream>
#include <tiobj.hpp>

// g++ load.cpp -o load -ltiobj

using namespace std;

int main(){
	TiObj obj;
	obj.loadFile("teste.ti");
	cout << obj;
	return 0;
}
```


### Save File
```cpp
#include <iostream>
#include <tiobj.hpp>

// g++ load.cpp -o load -ltiobj

using namespace std;

int main(){
	TiObj obj;
	obj.loadFile("teste.ti");
	obj.saveFile("dest.ti");
	return 0;
}
```


### Setting Variables
```cpp
#include <iostream>
#include <tiobj.hpp>

// g++ load.cpp -o load -ltiobj

using namespace std;

int main(){
	TiObj obj;
	obj["name"] = "Name";
	obj["old"] = 24;
	obj["score"] = 12.333;
	obj.setObj("subject", "name='Portugues'");
	return 0;
}
```



## Contributing


