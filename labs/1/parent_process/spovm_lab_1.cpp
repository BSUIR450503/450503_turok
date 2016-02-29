// spovm_lab_1.cpp : Defines the entry point for the console application.
//

#include "proc_object.h"
#ifdef _WIN32
#define FNAME "spovm_lab1_childprocess.exe"
#elif linux
#define FNAME "./spovm_lab1_childprocess"
#endif
#include <fstream>
using namespace std;
int main(int argc, char * argv[])
{
	string filename,param;
	int result;
    filename= FNAME ;
	cout << "X?" <<endl;
	//cin >> param;
	getline(cin,param,'\n');
    myprocess pr(filename,param,1,result);
    cout<<"exit code:"<<result<<endl;
    ifstream file;
    file.open("tmp");
    file >> result;
    cout <<"result from file:"<< result<<endl;
    cout<<"any key";
	cin >> param;
	return 0;
}
