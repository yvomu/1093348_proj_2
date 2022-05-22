#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <string>
using namespace std;
string stateType[4] = { "SN","WN","WT","ST" };
int registers[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


struct Instruct {
	string opperator;
	vector<int> reg;
	string branch;
	int immediate;
};
struct Label {
	int dis;
	string name;
};
vector<Instruct> inst;
vector<Label> label;
vector<string> strinst;
class Predictor {
public:
	Predictor() {
		bc = whichBC();
	}

	void renewHistory(bool act) {
		history[2] = history[1];
		history[1] = history[0];
		if (act == 1)
			history[0] = 1;
		else
			history[0] = 0;

	}

	int whichBC() {
		return history[2] * 4 + history[1] * 2 + history[0] * 1;
	}

	void renewBC(bool act) {
		if ((act == 1 && (state[bc]==0 || state[bc] == 1)) || (act == 0 && (state[bc] == 2 || state[bc] == 3))) {
			
			if (state[bc] == 0 || state[bc] == 1)
				state[bc]++;
			else if (state[bc] == 2 || state[bc] == 3)
				state[bc]--;
		}
		else {
			if (state[bc] == 1)
				state[bc]--;
			else if (state[bc] == 2)
				state[bc]++;
		}
	}
	void getHistory() {
		cout << history[2] << history[1] << history[0];
	}

	void getBC() {
		for (int i = 0; i < state.size(); i++)
			cout <<"  " << stateType[state[i]];
	}

	void getPred() {
		cout << stateType[state[bc]][1];
	}

	void dopred(bool act) {
		bc = whichBC();
		guess = state[bc];
		if ((act == 1 && (state[bc] == 0 || state[bc] == 1)) || (act == 0 && (state[bc] == 2 || state[bc] == 3))) 
			missprediction++;
	}

	void getmisspred() {
		cout << missprediction << endl;
	}
private:
	vector<int>  state = { 0,0,0,0,0,0,0,0 };
	vector<int> history = { 0,0,0 };
	int missprediction = 0, guess,bc;
	
};

void read() {
	ifstream in("test.txt");
	if (!in.is_open()) {
		cout << "can't open the file.";
		exit(0);
	}
	string str;
	
	
	while (getline(in,str)) {
		
		Instruct tempinst;
		if (str.back() == ':')
		{
			Label templabel;
			templabel.dis = inst.size();
			str.pop_back();
			templabel.name = str;
			label.push_back(templabel);
		}
		else {
			strinst.push_back(str);
			
			int tr = -1,i = 0;
			string tstr;
			while (i < str.size()) {
				while ( i < str.size() )
				{
					
					tstr.push_back(str[i]);
					i++;
					if (str[i] == ' ' || str[i] == ',')
						break;
					
						
				}
				if (tr == -1) {
					tempinst.opperator = tstr;
					tr++;
				}
				else {
					if (tstr[0] == 'R') {
						int reg = 0, r = 1;
						for (int j = tstr.size() - 1; j > 0; j--) {
							reg += (tstr[j] - '0') * r;
							r *= 10;
						}
						tempinst.reg.push_back(reg);
						tr++;
					}
					else if ((tstr[0] >= '0' && tstr[0] <= '9') || tstr[0] == '-') {
						int imm = 0,r = 1;
						if (tstr[0] == '-') {
							
							for (int j = tstr.size()-1; j > 0; j--) {
								imm += (tstr[j] - '0') * r;
								r *= 10;
							}
							tempinst.immediate = 0 - imm;
						}
						else
						{
							for (int j = tstr.size() - 1; j >= 0; j--) {
								imm += (tstr[j] - '0') * r;
								r *= 10;
							}
							tempinst.immediate = imm;
						}
						
						
					}
					else {
						tempinst.branch = tstr;
					}
				}
				tstr.clear();
				i++;
			}
			inst.push_back(tempinst);
		}
		
		
	}

}

int main() {
	
	read();
	int entries,now = 0,next;
	bool act;
	cout << "Please input entry(entry>0):\n";
	cin >> entries;
	//cout << " history              state               pred act missprediction\n";
	vector<Predictor> predictors(entries);
	while (now < inst.size()) {
		cout << "====================================================================\n";
		cout << "entries: " << now % entries<< "			" << strinst[now] << endl;
		
		
		if (inst[now].opperator == "add") {
			act = 0;
			registers[(inst[now].reg[0])] = registers[(inst[now].reg[1])] + registers[(inst[now].reg[2])];
			next = now+1;
		}
		else if (inst[now].opperator == "addi") {
			act = 0;
			registers[(inst[now].reg[0])] = registers[(inst[now].reg[1]) ] + inst[now].immediate;
			next = now + 1;
		}
		else if (inst[now].opperator == "beq") {
			if (registers[(inst[now].reg[0])] == registers[(inst[now].reg[1]) ])
			{
				act = 1;
				for (int i = 0; i < label.size(); i++) {
					if (inst[now].branch == label[i].name) {
						next = label[i].dis;
						break;
					}
						
				}
			}
			else {
				act = 0;
				next = now + 1;
			}
			
			
			
		}
		
		predictors[now % entries].dopred(act);

		cout << "prediction:";
		predictors[now % entries].getPred();
		cout << endl;
		if (act == 1)
			cout << "actural: T\n";
		else
			cout << "actural: N\n";
		cout << "missprediction: ";
		predictors[now % entries].getmisspred();
		cout << endl;
		for (int i = 0; i < predictors.size(); i++) {
			cout << i << ".  ";
			predictors[i].getHistory();
			predictors[i].getBC();
			cout << endl;
		}
		
		
		
		cout << endl;
		predictors[now % entries].renewBC(act);
		predictors[now % entries].renewHistory(act);
		now = next;
	}
	
	return 0;
}
