#define _CRT_SECURE_NO_WARNINGS
#include<bits/stdc++.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <fcntl.h>
#include <errno.h>
#include "dirent.h";
#include <fstream>
#include <cstdlib>
#include <filesystem>
//#include <experimental/filesystem>
//namespace fs = boost::filesystem ;
using namespace std;
using namespace std::tr2::sys;
//namespace filesystem = std::filesystem::xxx;
int BUFFER_SIZE = 1000 * 1024 * 1024;
struct Data
{
	char *data;
	string name;
	unsigned long Size;
};
struct Folder
{
	string FName;
	Folder *Folders[500];
	Folder *prv;
	int n_offolders;
	int n_ofdata;
	Data *data[500];
	map<string, bool>exist;
	map<string, bool>exist_data;
	int FSize;
};
Folder *t, disk;
int size_data;
bool done_read = false;

void read(char *name, char *path){
	FILE *source;
	source = fopen(path, "rb");
	if (source == NULL){
		cout << "Can't Open File.\n";
		return;
	}
	fseek(source, 0, SEEK_END);
	size_data = ftell(source);
	rewind(source);
	if (size_data > BUFFER_SIZE){
		cout << "There isn't Exist Empty Space.\n";
		return;
	}
	if (!size_data)size_data = 1;
	char *buffer = new char[size_data];
	int res = fread(buffer, 1, size_data, source);
	if (res != size_data && !(size_data == 1 || res != 0)){
		cout << "Reading Error.\n";
		return;
	}
	done_read = true;
	BUFFER_SIZE -= size_data;
	t->data[t->n_ofdata] = new Data;
	t->data[t->n_ofdata]->data = buffer;
	t->data[t->n_ofdata]->name = name;
	t->data[t->n_ofdata]->Size = size_data;
	t->FSize += size_data;
	fclose(source);
	return;
}
void write(char *buffer, int n, char *path){
	FILE *destination = fopen(path, "wb");
	fwrite(buffer, 1, n, destination);
	fclose(destination);
}
void copy2disk(char *cur_path, Folder *cur){
	if (cur == NULL)return;
	string path = "mkdir ";
	path += cur_path;
	path += "\\";
	path += cur->FName;
	//للفولدر
	char *P = new char[int(path.size())];
	strcpy(P, path.c_str());
	// create a folder to the path
	system(P);

	//fs::create_directories(P);
	path = path.substr(6, path.size() - 6);
	//cout<<path<<endl;
	P = new char[int(path.size())];
	strcpy(P, path.c_str());
	// inside each folder and create it
	for (int i = 0; i < cur->n_offolders; i++){
		copy2disk(P, cur->Folders[i]);
	}
	// create the data inside each folder
	for (int i = 0; i < cur->n_ofdata; i++){
		string x = path;
		x += '\\';
		x += cur->data[i]->name;
		char *tem = new char[int(x.size())];
		strcpy(tem, x.c_str());
		// store the data to hardisk
		write(cur->data[i]->data, cur->data[i]->Size, tem);
	}
}
// to load data from hard
void load_disk(string cur_path, Folder *cur){
	struct dirent *de;
	char *P = new char[int(cur_path.size())];
	strcpy(P, cur_path.c_str());
	DIR *dr = opendir(P);
	if (dr == NULL)return;
	// Open Everything inside the current folder.
	while ((de = readdir(dr)) != NULL){
		string name = de->d_name;
		//dir first thing show . or .. then show foldrs and files
		if (name == "." || name == "..")continue;
		// if it's a folder
		struct stat s;
		string cur_path2 = cur_path;
		cur_path2 += "\\";
		cur_path2 += de->d_name;
		bool IS = false;
		/*for(int i=0;i<t->n_offolders;i++)
		{*/
		if (t->exist[de->d_name])
		{
			cout << "Sorry this Folder Found with the same name can't be created\n";
			IS = true;
		}
		if (t->exist_data[de->d_name])
		{
			cout << "Sorry this Data Found with the same name can't be created\n";
			IS = true;
		}
		//}
		if (IS)
			continue;
		// path the data
		char *x = new char[int(cur_path2.size())];
		strcpy(x, cur_path2.c_str());
		stat(x, &s);
		if (s.st_mode & S_IFDIR){
			if (cur->n_offolders < 0)cur->n_offolders = 0;
			// creat a folder to the RAM
			cur->Folders[cur->n_offolders] = new Folder;
			cur->Folders[cur->n_offolders]->FName = name;
			cur->exist[name] = 1;
			// Inside this folder
			load_disk(cur_path + "\\" + name, cur->Folders[cur->n_offolders++]);
		}
		// if it's a data
		else {
			/*char *P=new char[int(name.size())];
			strcpy(P,name.c_str());
			read(P,x);*/
			/*if (cur->n_ofdata < 0)cur->n_ofdata = 0;*/
			FILE *source;
			source = fopen(x, "rb");
			// can't open
			if (source == NULL) continue;
			// size data
			fseek(source, 0, SEEK_END);
			size_data = ftell(source);
			rewind(source);
			// stor to buffer
			char *buffer = new char[size_data];
			int res = fread(buffer, 1, size_data, source);
			BUFFER_SIZE -= size_data;
			if (cur->n_ofdata < 0)cur->n_ofdata = 0;
			// store the data
			cur->data[cur->n_ofdata] = new Data;
			cur->data[cur->n_ofdata]->data = buffer;
			cur->data[cur->n_ofdata]->name = name;
			cur->exist_data[name] = 1;
			cur->data[cur->n_ofdata]->Size = size_data;
			fclose(source);
			cur->n_ofdata++;
		}
	}
	closedir(dr);
}
// return size all data inside the folder
int get_sz(Folder *a){
	if (a == NULL)return 0;
	// Size of data inside this folder
	int ans = a->FSize;
	// this Size of data inside folder of the current folder.
	for (int i = 0; i < a->n_offolders; i++){
		ans += get_sz(a->Folders[i]);
	}
	return ans;
}
int main() {
	cout << "By Moamen  CS.\nYou can enter help to know the commands list.\n";

	// from wait a few seconds
	int x = 1e9;
	cout << "Wait for 5 seconds: ";
	while (x--){
		if ((x) % ((int)1e9 / 5) == 0){
			cout << "*";
		}
	}
	system("CLS");
	string Cur_Location = "RamDisk:";
	disk.FName = "RamDisk";
	disk.FSize = 0;
	Folder d;
	d = disk;
	t = &disk;
	int lastB = BUFFER_SIZE;
	t->prv = NULL;
	Folder copy;
	Data copy2;
	vector<string>NameLocations;
	while (true){

		cout << Cur_Location;
		for (int i = 0; i < NameLocations.size(); i++){
			cout << "\\" << NameLocations[i];
		}
		cout << ">";
		string s;
		cin >> s;
		cin.ignore();
		if (s == "help")
		{
			///////
			cout << "CLR    :Clear The Screen.\n";
			cout << "MD     :Creat a Folder.\n";
			cout << "GBack  :Move To Previous location.\n";
			cout << "OPENF  :Open Folder In The Current Location.\n";

			///////
			cout << "D_READ :Read and Store Data In RAM Disk.\n";
			cout << "D_WRITE:Write Data On Hard Disk.\n";
			cout << "DIR    :Show All Data and All Folders.\n";

			///////
			cout << "FCOPY  :Copy Folder.\n";
			cout << "FPASTE :Paste Folder To The Current Location.\n";

			///////
			cout << "RMDIR  :Delete Folder.\n";
			cout << "RMDATA :Delet Data From Current Location.\n";

			///////
			cout << "DCOPY  :Copy Data .\n";
			cout << "DPASTE :Paste Data in The Current Loction.\n";

			///////
			cout << "D_RENAME :Rename Data .\n";
			cout << "F_RENAME :Rename Folder .\n";

			///////
			cout << "R2DISK :Load All Ram on Hard Disk.\n";
			cout << "D2RAM  :Load Folder on Ram. \n";

			///////
			cout << "VOL    :Show The Size Of RamDisk.\n";

			///////
			cout << "EXIT   :To close the program.\n";

		}
		//////////////////////////////////////////////Show Memory Capacity
		else if (s == "VOL"){
			cout << BUFFER_SIZE / pow(1024, 2) << " M\n";
		}
		//////////////////////////////////////////////Data Copy and Paste
		else if (s == "DPASTE"){
			bool ok = 0;
			if (copy2.Size <= 0)continue;
			for (int i = 0; i < t->n_ofdata; i++){
				if (t->data[i] != NULL)
				if (t->data[i]->name == copy2.name)ok = 1;
			}
			if (ok)cout << "There exist File with the same name\n";
			else {
				if (BUFFER_SIZE >= copy2.Size)BUFFER_SIZE -= copy2.Size;
				else {
					cout << "NO Empty Space Found.\n";
					continue;
				}
				if (t->n_ofdata < 0)t->n_ofdata = 0;
				t->data[t->n_ofdata] = &copy2;
				t->n_ofdata++;
				t->exist_data[copy2.name] = 1;
				t->FSize += copy2.Size;
			}
		}
		else if (s == "DCOPY"){
			string name;
			cin >> name;
			int tem = -1;
			for (int i = 0; i < t->n_ofdata; i++){
				if (t->data[i] != NULL)
				if (t->data[i]->name == name){
					tem = i;
					break;
				}
			}
			if (tem == -1)cout << "Not Found\n";
			else {
				copy2 = *t->data[tem];
			}
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Clear Screen
		else if (s == "CLR"){
			system("CLS");
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Create Folder
		else if (s == "MD"){
			string FN;
			cin >> FN;
			if (t->exist[FN]){
				cout << "This Folder name exist.\n";
				continue;
			}
			if (t->n_offolders <= 0){
				t->n_offolders = 0;
			}
			Folder *newfolder = new Folder;
			newfolder->FName = FN;
			t->exist[FN] = 1;
			t->Folders[t->n_offolders] = newfolder;
			t->Folders[t->n_offolders]->FSize = 0;
			t->n_offolders++;

		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Show All Contents of Data and Folder Found
		else if (s == "DIR"){
			for (int i = 0; i < 40; i++)cout << "-";
			cout << "\nName                          " << "Size          " << "Extintion\n";

			for (int i = 0; i < t->n_offolders; i++){
				if (t->Folders[i] != NULL)
					cout << t->Folders[i]->FName;
				for (int j = 0; j < 30 - t->Folders[i]->FName.size(); j++)cout << " ";
				cout << get_sz(t->Folders[i]) / (1024.0 * 1024) << " M            Folder" << endl;
			}
			for (int i = 0; i < t->n_ofdata; i++){
				if (t->data[i] != NULL)
					cout << t->data[i]->name;
				for (int j = 0; j < 30 - t->data[i]->name.size(); j++)cout << " ";
				cout << fixed << setprecision(3) << t->data[i]->Size / pow(1024, 2) << " M" << "            " << t->data[i]->name.substr(t->data[i]->name.rfind('.') + 1) << endl;
			}
			for (int i = 0; i < 40; i++)cout << "-";
			cout << endl;
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Open Folder
		else if (s == "OPENF"){
			string name;
			cin >> name;
			int tem = -1;
			for (int i = 0; i < t->n_offolders; i++){
				if (t->Folders[i] != NULL)
				if (t->Folders[i]->FName == name){
					tem = i;
					break;
				}
			}
			if (tem == -1)cout << "This Folder not found.\n";
			else {
				NameLocations.push_back(name);
				t->Folders[tem]->prv = t;
				t = t->Folders[tem];
			}
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Go to Prevoius Location
		else if (s == "GBack"){
			if (t->FName == "RamDisk")continue;
			t = t->prv;
			NameLocations.pop_back();
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Folder Copy and Paste
		else if (s == "FCOPY"){
			string name;
			cin >> name;
			int tem = -1;
			for (int i = 0; i < t->n_offolders; i++){
				if (t->Folders[i] != NULL)
				if (t->Folders[i]->FName == name){
					tem = i;
					break;
				}
			}
			if (tem == -1)cout << "This	Folder not found.\n";
			else {
				copy = *t->Folders[tem];
			}

		}
		else if (s == "FPASTE"){
			bool ok = 0;
			if (copy.FName == "")continue;
			for (int i = 0; i < t->n_offolders; i++){
				if (t->Folders[i] != NULL)
				if (t->Folders[i]->FName == copy.FName)ok = 1;
			}
			if (ok)cout << "There exist File with the same name.\n";
			else {
				int SZ = 0;
				SZ = get_sz(&copy);
				if (SZ>BUFFER_SIZE){
					cout << "There not exist empty space\n";
					continue;
				}
				if (t->n_offolders < 0)t->n_offolders = 0;
				BUFFER_SIZE -= SZ;
				t->FSize += SZ;
				//copy.prv = t;
				t->Folders[t->n_offolders] = &copy;
				t->n_offolders++;
				t->exist[copy.FName] = 1;
			}
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Remove Folder or Remove Data
		else if (s == "RMDIR"){
			string name;
			cin >> name;
			int tem = -1;
			for (int i = 0; i < t->n_offolders; i++){
				if (t->Folders[i] != NULL)
				if (t->Folders[i]->FName == name){
					tem = i;
					break;
				}
			}
			if (tem == -1)cout << "This folder not found.\n";
			else {
				t->exist[t->Folders[tem]->FName] = 0;
				BUFFER_SIZE += t->Folders[tem]->FSize;
				t->Folders[tem] = NULL;
				delete t->Folders[tem];
			}
		}
		else if (s == "RMDATA"){
			string name;
			cin >> name;
			int tem = -1;
			for (int i = 0; i < t->n_ofdata; i++){
				if (t->data[i] != NULL)
				if (t->data[i]->name == name){
					tem = i;
					break;
				}
			}
			if (tem == -1)cout << "No Data by this name.\n";
			else {
				BUFFER_SIZE += t->data[tem]->Size;
				t->exist_data[t->data[tem]->name] = 0;
				t->data[tem] = NULL;
				delete t->data[tem];
			}
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Data Read and Write
		else if (s == "D_READ"){
			string path;
			getline(cin, path);
			int id = -1;
			char *pathfile = new char[(int(path.size()))];
			strcpy(pathfile, path.c_str());
			for (int i = 0; i < path.size(); i++){
				//pathfile[i] = path[i];
				if (path[i] == '\\'){
					id = i;
				}
			}
			if (id == -1){
				cout << "Error path.\n";
				continue;
			}
			string Name = path.substr(id + 1, path.size() - id);////ال ادي عبارة عن أخر اندكس للدبل سلاش قبل الاسم بتاع الملف ومنه بجيب اسم الملف
			char *name = new char[int(Name.size())];
			//
			strcpy(name, Name.c_str());
			if (t->exist_data[name]){
				cout << "Exist this data in the current folder.\n";
				continue;
			}
			if (t->n_ofdata < 0){
				t->n_ofdata = 0;
			}
			read(name, pathfile);
			if (done_read){
				t->n_ofdata++;
				t->exist_data[name] = 1;
				done_read = false;
			}
			else {

				done_read = false;
				continue;
			}
		}
		else if (s == "D_WRITE"){
			string path, name;
			cin >> name >> path;
			if (t->exist_data[name] == 0){
				cout << "No Data by this name.\n";
				continue;
			}
			int tem = -1;
			for (int i = 0; i < t->n_ofdata; i++){
				if (t->data[i] != NULL)
				if (t->data[i]->name == name){
					tem = i;
					break;
				}
			}
			path += "\\" + name;
			char *P = new char[int(path.size())];
			strcpy(P, path.c_str());
			t->data[tem];
			write(t->data[tem]->data, t->data[tem]->Size, P);
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////From Ram To Disk and From Disk Choose Folder To Ram
		else if (s == "R2DISK"){
			string a;
			getline(cin, a);
			char *P = new char[int(a.size())];
			strcpy(P, a.c_str());
			copy2disk(P, &disk);
		}
		else if (s == "D2RAM"){
			string a;
			getline(cin, a);
			load_disk(a, &disk);
			if (BUFFER_SIZE < 0){
				cout << "Failed,No empty space\n";
				disk = d;
				BUFFER_SIZE = lastB;
			}
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Rename Data
		else if (s == "D_RENAME"){
			string a, b;
			getline(cin, a);
			getline(cin, b);
			int tem = -1;
			for (int i = 0; i < t->n_ofdata; i++){
				if (t->data[i] != NULL)
				if (t->data[i]->name == a){
					t->data[i]->name = b;
					tem = i;
					break;
				}
			}
			cout << t->data[tem]->name << endl;
			if (tem == -1)cout << "No Data by this name.\n";
		}
		else if (s == "F_RENAME"){
			string a, b;
			getline(cin, a);
			getline(cin, b);
			int tem = -1;
			for (int i = 0; i < t->n_offolders; i++){
				if (t->Folders[i] != NULL)
				if (t->Folders[i]->FName == a){
					t->Folders[i]->FName = b;
					tem = i;
					break;
				}
			}
			if (tem == -1)cout << "This folder not found.\n";
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////Exit
		else if (s == "EXIT")break;
		else{
			cout << "Syntax Erorr.\n";
		}
		lastB = BUFFER_SIZE;
		d = disk;
	}

	return 0;
}
