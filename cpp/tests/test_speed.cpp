#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "../include/tiobj.hpp"

using namespace std;



volatile size_t G_i = 0;


void* terminator(void* ){
	while(1){
		sleep(1);
		fprintf(stderr, "opaaaa %ld\n", G_i);
		G_i = 0;
	}
}




int main(int argc, char** argv){
	pthread_t id;
	pthread_create(&id, NULL, &terminator, NULL);


	while (1){
		TiObj b(false, "../tests/var_rand_100000.ti");
		/*TiObj b(false, "var_100000.ts");
		string var = b.atStr("teste2");
		*/
		//string var = b.atStr("teste2");
		G_i += 1;
	}

	/*static char buffer[1024*1024];
	while (1){
		//TiObj b(false, "../tests/big.ti");
		//TiObj b(false, "all.ts");
		FILE* fd = fopen("../tests/big.ti","r");
		fread(buffer, 1, 21221, fd);
		fclose(fd);
		G_i += 1;
	}*/

	/*TiObj b;
	for (int i=0; i<100000; i++){
		b.set( Join("teste%s").at(i).ok, i );
	}
	cout << b;*/


	return 0;
}
