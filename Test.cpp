#include"Simulator.h"

int main() {

	cout << "Dobrodosli u simulator mreze gradskog prevoza. Molimo Vas, odaberite opciju:" << endl;
	cout << "1. ucitavanje podataka o mrezi gradskog prevoza" << endl << "0. kraj rada" << endl;
	int cnt, end;
	int test = 1;
	cin >> test;
	if (test == 1) {
		cout << "Molimo Vas, unesite putanju do fajla sa stajalistima:" << endl;
		string filepath_stations = "stajalista.txt";
		//cin >> filepath_stations;
		cout << "Molimo Vas, unesite putanju do fajla sa linijama:" << endl;
		string filepath_lines = "linije.txt";
		//cin >> filepath_lines;

		Simulator* simulator = new Simulator();
		simulator->readStations(filepath_stations);
		simulator->readBusLines(filepath_lines);
		cout << "Uspesno ucitano." << endl;
		int option = 1, strategy;
		while (option) {
			cout << "\nMolimo Vas, odaberite opciju:" << endl;
			cout << "1. Prikaz informacija o stajalistu" << endl;
			cout << "2. Prikaz osnovnih informacija o liniji gradskog prevoza" << endl;
			cout << "3. Prikaz statistickih informacija o liniji gradskog prevoza" << endl;
			cout << "4. Pronalazenje putanje izmedju 2 stajalista" << endl;
			cout << "0. Kraj rada" << endl;

			cin >> option;
			
			string station_code, line_name, starting_station_code, ending_station_code, current_time;
			
			//vector<Station*> stations = simulator->getAllStations();
			//simulator->simulate("578", "3112", current_time, strategy);
			/*
			//finds optimal path for all pairs of stations		
			vector<Station*>::iterator st_iter1 = stations.begin();
			while (st_iter1 != stations.end()) {
				vector<Station*>::iterator st_iter2 = stations.begin();
				while (st_iter2 != stations.end()) {
					starting_station_code = (*st_iter1)->getStationCode();
					ending_station_code = (*st_iter2)->getStationCode();
					if (starting_station_code != ending_station_code) {
						cout << (*st_iter1)->getStationName() << "->" << (*st_iter2)->getStationCode() << endl;
						simulator->simulate(starting_station_code, ending_station_code, current_time, strategy);
					}
					st_iter2++;
				}
				st_iter1++;
			}
			break;
			*/

			switch (option) {
			case 1:
				cout << "Molimo Vas, unesite sifre stajalista cije informacije zelite da vidite: " << endl;
				cnt = 1, end = 1;
				while (true) {
					cout << cnt++ << ". stajaliste: ";
					cin >> station_code;
					simulator->writeStationInfo(station_code);
					simulator->writeConnections();
					cout << "Unesite 1 za nastavak, 0 za kraj rada: ";
					cin >> end; 
					cout << '\n';
					if (end != 1) break;
				}
				break;
			case 2:
				cout << "Molimo Vas, unesite oznaku linije cije informacije zelite da vidite" << endl;
				cin >> line_name;
				while (true) {
					simulator->writeBusLineInfo(line_name);
					cout << "Molimo Vas, unesite oznaku nove linije cije informacije zelite da vidite ili 0 za povratak u meni:" << endl << '\t';
					cin >> line_name;
					if (line_name == "0") break;
				}
				break;
			case 3:
				cout << "Molimo Vas, unesite oznaku linije cije statisticke informacije zelite da vidite, 0 za povratak u meni:" << endl << '\t';
				cin >> line_name;
				while (true) {
					if (line_name == "0") break;
					simulator->writeBusLineStats(line_name);
					cout << "Molimo Vas, unesite oznaku linije cije statististicke informacije zelite da vidite, 0 za povratak u meni:" << endl << '\t';
					cin >> line_name;
				}
				break;
			case 4:
				cout << "\nMolimo Vas, unesite trenutno vreme" << endl;
				cin >> current_time;
				while (true) {
					cout << "\nMolimo Vas, unesite sifre polaznog i krajnjeg stajalista" << endl;
					cin >> starting_station_code;
					cin >> ending_station_code;

					cout << "\nMolimo Vas, odaberite opciju za nalazenje putanje:" << endl;
					cout << "1. Bilo koja putanja izmedju zadatih stajalista" << endl;
					cout << "2. Putanja koja ima najmanje stajalista izmedju zadatih stajalista" << endl;
					cout << "3. Putanja koja ima najmanje presedanja izmedju zadatih stajalista" << endl;
					cin >> strategy;

					simulator->simulate(starting_station_code, ending_station_code, current_time, strategy);
					cout << "Uspesno nadjena putanja!\n" << endl;

					cout << "Molimo Vas unesite 1 za novo generisanje putanje ili 0 za povratak u meni:" << endl;
					cin >> starting_station_code;
					if (starting_station_code == "0") break;
				}
				break;
			case 0:
				break;
			}
		}
	}
	return 0;
}