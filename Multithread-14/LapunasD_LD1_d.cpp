#include <omp.h>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <mpi.h>

/*
	1. Procesas padaro atsitiktini skaiciu iteraciju
	2. Atsitiktine

	3. openMP
	4. 2.0 - 2.8 GHz, 4 fiziniai, 8 loginiai branduoliai, 8GB RAM, GT 525m
*/

using namespace std;

struct Struct
{
	string pav;
	int kiekis;
	double kaina;
	Struct(string input);
	string Print();
};

Struct::Struct(string input)
{
	int start, end;
	start = 0;
	end = input.find(' ');
	pav = input.substr(0, end);
	start = end + 1;
	end = input.find(' ', start);
	kiekis = stoi(input.substr(start, end - start));
	start = end + 1;
	kaina = stod(input.substr(start));
}

string Struct::Print()
{
	stringstream ss;
	ss << setw(15) << pav << setw(7) << kiekis << setw(20) << kaina;
	return ss.str();
}

string Titles();
string Print(int nr, Struct &s);
void syncOut(vector<vector<Struct>>&);
void asyncOut(int proc, vector<Struct>&);

//perskaito proceso duomenis pagal nr
vector<Struct> ReadStuff(string file, int nr);
//perskaito proceso duomenu eilutes pagal nr
vector<string> ReadLines(string file, int nr);

int main(int argc, char *argv[])
{
	//MPI paleidimas
	MPI_Init(&argc, &argv);
	int nr;
	//proceso nr
	MPI_Comm_rank(MPI_COMM_WORLD, &nr);

	//pirmas procesas atlieka sinchronini visu duomenu isvedima ir ispausdina antrastes
	if(nr == 0)
	{
		int size;
		//procesu kiekis
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		vector<vector<Struct>> all;
		//perskaitomi visu procesu duomenys
		for(int i = 0; i < size; i++)
			all.push_back(move(ReadStuff("LapunasD.txt", i)));
		cout << "\nsinchroninis isvedimas\n\n";
		syncOut(all);
		cout << "\nasinchroninis isvedimas\n\n";
		cout << setw(12) << "Procesas" << setw(3) << "Nr" << Titles() << "\n\n";
	}

	//blokuoja kol visi procesai pasiekia barjera
	//naudojamas, kad procesai lauktu kol pirmas atlieka spausdinima
	MPI_Barrier(MPI_COMM_WORLD);
	auto input = ReadStuff("LapunasD.txt", nr);

	//lygiagretus spausdinimas
	asyncOut(nr, input);

	MPI_Finalize();
	return 0;
}

vector<Struct> ReadStuff(string file, int nr)
{
	auto lines = ReadLines(file, nr);
	vector<Struct> ret;
	for(int i = 0; i < lines.size(); i++)
	{
		ret.emplace_back(lines[i]);
	}
	return ret;
}

vector<string> ReadLines(string file, int nr)
{
	vector<string> ret;
	ifstream duom(file);
	int prc = 0;
	string line;
	while(!duom.eof() && prc != nr)
	{
		getline(duom, line);
		if(line == "")
			prc++;
	}
	while(!duom.eof())
	{
		getline(duom, line);
		if(line == "")
			break;
		ret.push_back(line);
	}
	return ret;
}

string Titles()
{
	stringstream ss;
	ss << setw(15) << "Pavadiniams" << setw(7) << "Kiekis" << setw(20) << "Kaina";
	return ss.str();
}

void syncOut(vector<vector<Struct>> &data)
{
	cout << setw(3) << "Nr" << Titles() << endl << endl;
	for(int i = 0; i < data.size(); i++)
	{
		auto vec = data[i];
		cout << "Procesas_" << i << endl;
		for(int j = 0; j < vec.size(); j++)
		{
			cout << Print(j, vec[j]) << endl;
		}
	}
}

void asyncOut(int proc, vector<Struct> &vec)
{
	for(int i = 0; i < vec.size(); i++)
	{
		cout << setw(11) << "Procesas_" << proc << setw(3) << i << vec[i].Print() << endl;
	}
}

string Print(int nr, Struct &s)
{
	stringstream ss;
	ss << setw(3) << nr << s.Print();
	return ss.str();
}