#include <omp.h>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <mpi.h>

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

vector<vector<Struct>> ReadStuff(string file);
vector<string> ReadLines(string file);

int main(int argc, char *argv[])
{
	auto input = ReadStuff("LapunasD.txt");
	MPI_Init(&argc, &argv);
	int nr;
	MPI_Comm_rank(MPI_COMM_WORLD, &nr);
	
	asyncOut(nr, input[nr]);

	system("pause");
	MPI_Finalize();
	return 0;
}

vector<vector<Struct>> ReadStuff(string file)
{
	auto lines = ReadLines(file);
	vector<vector<Struct>> ret;
	vector<Struct> tmp;
	for(int i = 0; i < lines.size(); i++)
	{
		if(lines[i] == "")
		{
			ret.push_back(move(tmp));
		}
		else
		{
			tmp.emplace_back(lines[i]);
		}
	}
	return ret;
}

vector<string> ReadLines(string file)
{
	vector<string> ret;
	ifstream duom(file);
	while(!duom.eof())
	{
		string line;
		getline(duom, line);
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