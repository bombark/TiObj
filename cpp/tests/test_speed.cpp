#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../include/tiobj.hpp"

#include <string.h>
#include <jansson.h>
#include <stdlib.h>

#include <opencv2/core/core.hpp>

#include <yaml-cpp/yaml.h>


using namespace std;



volatile size_t G_i = 0;
volatile bool   G_continue;


void* terminator(void* ){
	unsigned total=0;
	unsigned secs = 5;
	for (unsigned i=0; i<secs; i++){
		G_i = 0;
		sleep(1);
		size_t cont = G_i;
		fprintf(stderr, "opaaaa %ld\n", cont);
		total += cont;
	}
	G_continue = false;
	fprintf(stderr, "media: %f\n", total/(double)secs);
	return NULL;
}




int main(int argc, char** argv){
	bool exec = false;
	if ( argc < 2 ){
		cerr << "./test_speed name\n";
		return 1;
	} else if ( argc == 3 ) {
		exec = true;
	}

	pthread_t id;
	string name, url, base_url;
	ofstream file;
	mkdir("tmp",0755);
	name = argv[1];
	base_url = "../tests/" + name + ".ti";

	TiObj base( false, base_url );


	cout << "Testing loading TiOn speed " << base_url << endl;
	G_continue = true;
	pthread_create(&id, NULL, &terminator, NULL);
	while ( G_continue ){
		TiObj b(true, base_url);
		G_i += 1;
	}


	if ( exec ){
		url  = "tmp/"+name+".ti";
		cout << "Testing loading TiOs speed " << url << endl;
		base->save(url, "binary");

		G_continue = true;
		pthread_create(&id, NULL, &terminator, NULL);
		while ( G_continue ){
			TiObj b(true, url);
			G_i += 1;
		}
	}


	url  = "tmp/"+name+".json";
	cout << "Testing loading Json speed " << url << endl;
	base->save(url, "json");
	G_continue = true;
	pthread_create(&id, NULL, &terminator, NULL);
	while ( G_continue ){
		json_t *root;
		json_error_t error;
		std::ifstream json_file( url );
		json_file.seekg(0, std::ios::end);
		size_t size = json_file.tellg();
		std::string buffer(size, ' ');
		json_file.seekg(0);
		json_file.read(&buffer[0], size);
		json_file.close();
		root = json_loads(buffer.c_str(), 0, &error);
		json_decref(root);
		//free(text);
		G_i += 1;
	}


	url  = "tmp/"+name+".yaml";
	cout << "Testing loading Yaml::OpenCv speed " << url << endl;
	base->save(url, "yaml");
	G_continue = true;
	pthread_create(&id, NULL, &terminator, NULL);
	while ( G_continue ){
		cv::FileStorage fs(url, cv::FileStorage::READ);
		G_i += 1;
	}


	url  = "tmp/"+name+".yaml";
	cout << "Testing loading Yaml::Yaml speed " << url << endl;
	base->save(url, "yaml");
	G_continue = true;
	pthread_create(&id, NULL, &terminator, NULL);
	while ( G_continue ){
		YAML::Node config = YAML::LoadFile( url );
		G_i += 1;
	}

	return 0;
}
