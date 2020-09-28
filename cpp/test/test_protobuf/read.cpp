#include "lm.helloworld.pb.h" 
#include <iostream>
#include <fstream>
 
using namespace std;

// g++ -g -Wall -o read read.cpp lm.helloworld.pb.cc  -I/home/lix/lix/test/libprefix/include -L/home/lix/lix/test/libprefix/lib -lprotobuf -pthread 

void ListMsg(const lm::helloworld & msg) { 
  cout << msg.id() << endl; 
  cout << msg.str() << endl; 
 } 
 
int main(int argc, char* argv[]) 
{ 

  lm::helloworld msg1; 
 
  { 
    fstream input("./log", ios::in | ios::binary); 
    if (!msg1.ParseFromIstream(&input)) { 
      cerr << "Failed to parse address book." << endl; 
      return -1; 
    } 
  } 
 
  ListMsg(msg1); 

  return 0; 
 } 
