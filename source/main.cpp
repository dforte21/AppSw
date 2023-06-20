#include "appsw.hpp"

#define	PATH "/home/swembedded/Desktop/AppSw"

int main(){
	try{
		appSw boot(PATH);
	}
	catch (exception &e){
		cout << e.what();
	}
}