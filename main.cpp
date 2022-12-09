#include <iostream>
#include "cmath"
#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

struct page{
	int valid;
	int perm;
	int frame;
	string bframe;
	int LRU;
};

struct pageTable{
	int n;
	int m;
	int size;
	int vec_size;
	int pvec_szie;
	vector<page> table;
	void init_sizes(){
		vec_size = exp2(n - log2(size));
		pvec_szie = exp2(m - log2(size));
		table.resize(vec_size);
	}
};

struct clockS{
	page* currentPage;
	int position;
};

pageTable create_table(char* filename){
	pageTable vtable;
	ifstream inputFile;
	inputFile.open (filename);
	if (inputFile.fail())
	{
		cout << "File " << filename << " not found\n";
		exit (2);
	}
	inputFile >> vtable.n >> vtable.m >> vtable.size;
	vtable.init_sizes();
	for(int i =0; i < vtable.vec_size; i++){
		inputFile >> vtable.table.at(i).valid >> vtable.table.at(i).perm >>
			vtable.table.at(i).frame >> vtable.table.at(i).LRU;
		vtable.table.at(i).bframe = bitset<16>(vtable.table.at(i).frame).to_string();
	}

	return vtable;
}

void translate(string& tempinput, int& off, int& temp1, string& offset){
	int temp_int;
	string binary;
	string pnumber;

	temp_int = stoi(tempinput, nullptr, 0);
	binary = bitset<16>(temp_int).to_string();
	offset = binary.substr(off);
	pnumber = binary.substr(0,off);
	temp1 = stoi(pnumber, nullptr,2);
}

void translate_address(pageTable tablein){
	clockS clock;
	clock.currentPage = &tablein.table[0];
	clock.position = 0;
	string tempinput;
	string offset;
	int output;
	int temp1;
	int off = 16 - log2(tablein.size);
	while(getline(cin,tempinput)){
		translate(tempinput, off, temp1, offset);
		if(tablein.table.at(temp1).valid){
			tablein.table.at(temp1).LRU = 1;
			output = stoi(tablein.table.at(temp1).bframe + offset, nullptr,2);
			cout << output <<endl;
		}
		else if(tablein.table.at(temp1).perm){
			cout << "Page Fault ";
			while(tablein.table.at(temp1).valid == 0){
				if(clock.currentPage->valid == 0){
					clock.position++;
					if(clock.position == tablein.table.size())
						clock.position = 0;
					clock.currentPage = &tablein.table[clock.position];
				}
				else if(clock.currentPage->LRU == 1){
					clock.currentPage->LRU = 0;
					clock.position++;
					if(clock.position == tablein.table.size())
						clock.position = 0;
					clock.currentPage = &tablein.table[clock.position];
				}
				else{
					clock.currentPage->valid = 0;
					tablein.table.at(temp1).valid = 1;
					tablein.table.at(temp1).frame = clock.currentPage->frame;
					tablein.table.at(temp1).bframe = clock.currentPage->bframe;
					tablein.table.at(temp1).LRU = 1;
					clock.position++;
					if(clock.position == tablein.table.size())
						clock.position = 0;
					clock.currentPage = &tablein.table[clock.position];
				}
			}
			translate(tempinput, off, temp1, offset);
			output = stoi(tablein.table.at(temp1).bframe + offset, nullptr,2);
			cout << output <<endl;

			//cout << "Disk" <<endl;
		}
		else{
			cout << "SEGFAULT" <<endl;
		}
	}
}

int main(int argc, char * argv[]) {
	if (argc < 2)
	{
		printf ("not enough arguments");
		exit (1);
	}
	pageTable vtable = create_table(argv[1]);
	translate_address(vtable);


	return 0;
}
