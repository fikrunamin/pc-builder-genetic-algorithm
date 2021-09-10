#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <time.h>
#include <numeric>
#include <algorithm>

using namespace std;

struct Chromosome {
	vector <int> gene;
	double fitness, performance;

	bool operator() (Chromosome i, Chromosome j) {
		return (i.fitness < j.fitness);
	}
} chromosome;

struct Hardware{
	string ctg, brand, desc;
	int id;
	double price, performance;
};

vector<vector<Hardware>> category(7);

void prepare_file() {
	ofstream myFile;
	myFile.open("result.csv", ios::trunc);
	myFile << "best fitness,average fitness, best performance\n";
	myFile.close();
}

vector<Hardware> read_csv(vector<Hardware> data) {
	ifstream myFile("data_pc.csv");
	if (!myFile.is_open()) cout << "ERROR: File Open." << endl;

	int n = 0;
	string str;
	getline(myFile, str);

	while (n < data.size())
	{
		getline(myFile, str, ','); // category
		data.at(n).ctg.insert(0, str);
		
		getline(myFile, str, ','); // id
		stringstream id(str);
		data.at(n).id = 0;
		id >> data.at(n).id;

		getline(myFile, str, ','); // brand
		data.at(n).brand.insert(0, str);

		getline(myFile, str, ','); // price
		data.at(n).price = stod(str);

		getline(myFile, str, ','); // performance
		data.at(n).performance = stod(str);

		getline(myFile, str, '\n'); // description
		data.at(n).desc = str;
		n++;
	}
	myFile.close();

	return data;
}

void save_data(vector<Hardware> data) {
	for (int i = 0; i < data.size(); i++)
	{
		if (data.at(i).ctg == "monitor")
		{
			category.at(0).push_back(data.at(i));
			continue;
		}
		else if (data.at(i).ctg == "cpu")
		{
			category.at(1).push_back(data.at(i));
			continue;
		}
		else if (data.at(i).ctg == "mainboard")
		{
			category.at(2).push_back(data.at(i));
			continue;
		}
		else if (data.at(i).ctg == "graphic card")
		{
			category.at(3).push_back(data.at(i));
			continue;
		}
		else if (data.at(i).ctg == "power supply")
		{
			category.at(4).push_back(data.at(i));
			continue;
		}
		else if (data.at(i).ctg == "ram")
		{
			category.at(5).push_back(data.at(i));
			continue;
		}
		else if (data.at(i).ctg == "storage")
		{
			category.at(6).push_back(data.at(i));
			continue;
		}
	}
	/*cout << "Category\t|\tPrice\t|\tPerformance" << endl;
	cout << "============================" << endl;
	for (int i = 0; i < category.size(); i++)
	{
		for (int j = 0; j < 10; j++)
		{
			cout << category.at(i).at(j).ctg << "\t|\t" << category.at(i).at(j).price << "\t|\t" << category.at(i).at(j).performance << endl;
		}
	}*/
}

void input_data(vector<Hardware> data, double& target, int& pop_size, double& crossover_probability, double& mutation_probability) {

	save_data(data);

	target = 3500;
	pop_size = 30;
	crossover_probability = 0.9;
	mutation_probability = 0.1;

	/*input_target:
	cout << "budget (RM1879 - RM14135): ";
	cin >> target;
	if (target < 1879 | target > 14135) {
		system("cls");
		goto input_target;
	}

	input_pop_size:
	cout << "population size (5-10): ";
	cin >> pop_size;
	if (pop_size < 5 | pop_size > 10) {
		system("cls");
		goto input_pop_size;
	}

	input_crossover_prob:
	cout << "crossover probability (0 - 1): ";
	cin >> crossover_probability;
	if (crossover_probability < 0 | crossover_probability > 1) {
		system("cls");
		goto input_crossover_prob;
	}

	input_mutation_prob:
	cout << "mutation_probability (0 - 1): ";
	cin >> mutation_probability;
	if (mutation_probability < 0 | mutation_probability > 1) {
		system("cls");
		goto input_mutation_prob;
	}*/
}

double calculate_fitness(vector <int> gene, double target, int target_length) {
	double best_fitness, sum = 0;

	for (int i = 0; i < target_length; i++)
	{
		 sum += category.at(i).at(gene.at(i)).price;
	}
	best_fitness = sum < target ? (sum / target) * 100 : (target / sum) * 100;
	return best_fitness;
}

double calculate_performance(vector <int> gene, double target, int target_length) {
	double sum = 0;

	for (int i = 0; i < target_length; i++)
	{
		sum += category.at(i).at(gene.at(i)).performance;
	}
	return (sum/70) * 100;
}

vector<Chromosome> create_population(vector <Chromosome> population, double target, int target_length) {
	int pop_size = population.size();
	srand(time(0));

	for (int i = 0; i < pop_size; i++)
	{
		// create gene
		for (int j = 0; j < target_length; j++)
		{
			int random_number = rand() % 10;
			population.at(i).gene.push_back(random_number);
		}

		// calculate fitness
		population.at(i).fitness = calculate_fitness(population.at(i).gene, target, target_length);
		population.at(i).performance = calculate_performance(population.at(i).gene, target, target_length);
	}
	return population;
}

void print_chromosome(vector <Chromosome> chromosome, int target_length, bool show_fitness = true, bool show_performance = true) {
	for (int i = 0; i < chromosome.size(); i++)
	{
		cout << "chromosome " << i + 1 << "\t: ";

		for (int j = 0; j < target_length; j++)
		{
			cout << chromosome.at(i).gene.at(j);
		}
		
		if (show_fitness) cout << "\t| fitness : " << chromosome.at(i).fitness;
		if (show_performance) cout << "\t| performance : " << chromosome.at(i).performance;
		cout << endl;
	}
	cout << "\n";
}

double calculate_avg_fitness(vector <Chromosome> population) {
	double sum = 0;

	for (int i = 0; i < population.size(); i++)
	{
		sum += population.at(i).fitness;
	}
	return sum / population.size();
}

vector<Chromosome> parent_selection(vector <Chromosome> &chromosome) {
	int parent1, parent2, winner[2];
	vector <Chromosome> parent(2);

	// tournament selection
	do
	{
		for (int i = 0; i < 2; i++) { 
			parent1 = rand() % chromosome.size();
			do {
				parent2 = rand() % chromosome.size();
			} while (parent1 == parent2);

			// cout << parent1 << " vs " << parent2 << endl;
			// cout << "fitness parent 1 : " << chromosome.at(parent1).fitness << " | " << "fitness parent 2 : " << chromosome.at(parent2).fitness << endl;

			if (chromosome.at(parent1).fitness <= chromosome.at(parent2).fitness) {
				parent.at(i).gene = chromosome.at(parent1).gene;
				parent.at(i).fitness = chromosome.at(parent1).fitness;
				winner[i] = parent1;
			}
			else {
				parent.at(i).gene = chromosome.at(parent2).gene;
				parent.at(i).fitness = chromosome.at(parent2).fitness;
				winner[i] = parent2;
			}

			// chromosome.erase(chromosome.begin() + winner[i]);
			// cout << "\n\t Players: " << parent1 << " vs " << parent2;
			// cout << "\n\t Fitness: " << chromosome.at(parent1).fitness << " vs " << chromosome.at(parent2).fitness;
			// cout << "\n\t Winner: " << winner[i] << endl;
		}
	} while (parent.at(0).gene == parent.at(1).gene);

	return parent;
}

vector<Chromosome> crossover(vector <Chromosome> children, double crossover_probability, int target_length) {
	double prob;
	int co_point;

	vector<Chromosome> parent(2);
	parent = children;

	prob = ((rand() % 10) + 1) / 10.0;
	co_point = rand() % target_length;

	if (prob < crossover_probability) {
		// cout << "Crossover point : " << co_point << endl;

		//crossover starts
		for (int i = co_point; i < target_length; i++) {

			children.at(0).gene.at(i) = parent.at(1).gene.at(i);
			children.at(1).gene.at(i) = parent.at(0).gene.at(i);
		}
	}

	return children;
}

vector<Chromosome> mutation(vector <Chromosome> children, double target, double mutation_probability, int target_length) {
	double prob;
	int mut_point;

	vector<Chromosome> mutated_children(2);
	mutated_children = children;

	for (int i = 0; i < 2; i++) {
		prob = (rand() % 11) / 10.0;
		mut_point = rand() % target_length;

		if (prob < mutation_probability) {
			//cout << "Mutation Point : " << mut_point << endl;
			
			mutated_children.at(i).gene.at(mut_point) = rand() % 10;
		}
		mutated_children.at(i).fitness = calculate_fitness(mutated_children.at(i).gene, target, target_length);
		mutated_children.at(i).performance = calculate_performance(mutated_children.at(i).gene, target, target_length);
	}

	return mutated_children;
}

vector<Chromosome> regenerate_population(vector<Chromosome> population, vector<Chromosome> children) {

	sort(population.begin(), population.end(), chromosome);

	population.erase(population.begin());
	population.erase(population.begin());

	for (int i = 0; i < children.size(); i++)
	{
		population.push_back(children.at(i));
	}
		
	return population;
}

vector<Chromosome> solution_checking(vector<Chromosome> population, bool& isLooping) {
	vector<Chromosome> best_solution;
	int pop_size = population.size();

	sort(population.begin(), population.end(), chromosome);

	best_solution.push_back(population.at(pop_size - 1));

	isLooping = best_solution.at(0).fitness >= 100 ? false : true;

	return best_solution;
}

void write_to_csv(double best_fitness, double average_fitness, double best_performance) {
	ofstream myFile;
	myFile.open("result.csv", ios::app);
	myFile << best_fitness << "," << average_fitness << "," << best_performance << "\n";
	myFile.close();
}

double total_price(vector<Chromosome> solution, int target_length) {
	double total_price = 0;

	for (int i = 0; i < target_length; i++)
	{
		total_price += category.at(i).at(solution.at(0).gene.at(i)).price;
	}

	return total_price;
}

void logging(vector<Chromosome> population, int target, vector<Chromosome> solution, int generation, int target_length, double average_fitness) {
	double best_fitness = solution.at(0).fitness;
	double performance = solution.at(0).performance;

	system("cls");
	print_chromosome(population, target_length);

	cout << "generation : " << generation << endl;

	cout << "best solution : ";

	for (int i : solution.at(0).gene)
		cout << i;

	write_to_csv(best_fitness, average_fitness, performance);

	cout << "\nbest fitness : " << best_fitness;
	cout << "\naverage fitness : " << average_fitness;
	cout << "\ntotal performance : " << performance;

	cout << "\n\nBudget : RM" << target;

	cout << "\n\nResult : \n\n";
	cout << "Best monitor\t\t: " << category.at(0).at(solution.at(0).gene.at(0)).desc << endl;
	cout << "Best cpu\t\t: " << category.at(1).at(solution.at(0).gene.at(1)).desc << endl;
	cout << "Best mainboard\t\t: " << category.at(2).at(solution.at(0).gene.at(2)).desc << endl;
	cout << "Best graphic card\t: " << category.at(3).at(solution.at(0).gene.at(3)).desc << endl;
	cout << "Best power supply\t: " << category.at(4).at(solution.at(0).gene.at(4)).desc << endl;
	cout << "Best ram\t\t: " << category.at(5).at(solution.at(0).gene.at(5)).desc << endl;
	cout << "Best storage\t\t: " << category.at(6).at(solution.at(0).gene.at(6)).desc << endl;
	cout << "Total price\t\t: RM" << total_price(solution, target_length) << endl;
}

void main() {
	prepare_file();

	vector<Hardware> data(70);
	data = read_csv(data);

	double target;
	int pop_size, generation = 0;
	double average_fitness, crossover_probability, mutation_probability;
	bool isLooping = true;

	int target_length = 7;
	
	input_data(data, target, pop_size, crossover_probability, mutation_probability);
	
	vector<Chromosome> population(pop_size);
	vector<Chromosome> parent(2);
	vector<Chromosome> children(2);
	vector<Chromosome> mutated_children(2);
	vector<Chromosome> solution;

	// GA starts here :

	population = create_population(population, target, target_length);

	average_fitness = calculate_avg_fitness(population);

	while (isLooping & generation != 200)
	{
		parent = parent_selection(population);

		children = crossover(parent, crossover_probability, target_length);

		mutated_children = mutation(children, target, mutation_probability, target_length);

		population = regenerate_population(population, mutated_children);

		average_fitness = calculate_avg_fitness(population);

		generation++;

		solution = solution_checking(population, isLooping);

		logging(population, target, solution, generation, target_length, average_fitness); 
	}

	cout << "\nGA Done!" << endl;
	system("pause");
}