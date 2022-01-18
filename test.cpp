#include "quadtree/Node.h"
#include "quadtree/Object.h"
#include "quadtree/TopkQuery.h"
#include <stdint.h>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
using namespace std;

vector<Object> load_data(string filename){
	vector<Object> data;
	string myText;
	ifstream MyReadFile("../Data/data/" + filename + ".txt");
	float x, y;
	float min_x = 999999, min_y = 999999, max_x = -999999, max_y = -999999;
	unordered_map<uint64_t, float> weightTerms;

	// Use a while loop together with the getline() function to read the file line by line
	while (getline(MyReadFile, myText)) {
		unordered_map<uint64_t, float> ().swap(weightTerms);
		stringstream check(myText);
		string text;
		getline(check, text, ',');
		x = stof(text);

		getline(check, text, ',');
		y = stof(text);

		while(getline(check, text, ','))
		{
			stringstream ss(text);
			string terms;
			getline(ss, terms, ' ');
			uint64_t key = stoi(terms);

			getline(ss, terms, ' ');
			float weight = stof(terms);
			weightTerms[key] = weight;
		}
		min_x = min(x, min_x);
		min_y = min(y, min_y);
		max_x = max(x, max_x);
		max_y = max(y, max_y);

		data.emplace_back(Object(x, y, weightTerms));
	}
	// Close the file
	MyReadFile.close();
	data.emplace_back(Object(min_x, min_y));
	data.emplace_back(Object(max_x, max_y));

	return data;
}

vector<TopkQuery> load_queries(string filename){
    vector<TopkQuery> queries;
    string myText;
    ifstream MyReadFile("../Data/query/" + filename + ".txt");
    float x0, y0, x1, y1;
    vector<uint64_t> k;
    // Use a while loop together with the getline() function to read the file line by line
    while (getline (MyReadFile, myText)) {
		vector<uint64_t> ().swap(k);
		// Output the text from the file
		stringstream check1(myText);
		string text;
		getline(check1, text, ',');
		x0 = stof(text);

		getline(check1, text, ',');
		y0 = stof(text);

		getline(check1, text, ',');
		stringstream keys(text);
		uint64_t key;
		while (keys >> key) {
			k.emplace_back(key);
		}
		queries.emplace_back(TopkQuery(x0, y0, k));
    }
    // Close the file
    MyReadFile.close();
    return queries;
}

int main(int argc, char *argv[])
{
	// argv[1] - data filename argv[2] - query filename
	// data format: x,y,term1 term1_weight, term2 term2_weight,...
	// query format: x,y,key1 key2 key3...
	
	string dataFile = argv[1];
	vector<Object> data = load_data(dataFile);
	Object max_space = data.back();
	data.pop_back();
	Object min_space = data.back();
	data.pop_back();

	cout << data.size() << endl;

	// for(int i = 0; i < data.size(); i++)
	// {
	// 	Object* o = &data.at(i);
	// 	cout << o->x << "," << o->y << ";";
	// 	for(auto &v : o->weightedKey)
	// 	{
	// 		cout << v.first << "," << v.second << " ";
	// 	}
	// 	cout << endl;
	// }

  	Node quadtree(min_space.x, min_space.y, max_space.x, max_space.y, 0);

  	auto build_start = chrono::high_resolution_clock::now();
	for(int n = 0; n < data.size(); n++)
	{
		Object *o = &data.at(n);
		quadtree.insert(o);
	}
  	auto build_end = chrono::high_resolution_clock::now();
	auto build_cost = chrono::duration_cast<chrono::seconds> (build_end - build_start).count();

	// quadtree.printInfo();

	cout << "Build Time: " << build_cost << "s" << endl;

	string queryFile = argv[2];
	vector<TopkQuery> queries = load_queries(queryFile);
	cout << queries.size() << endl;
	// vector<TopkQuery> testingQueries;
	// testingQueries.assign(queries.begin() + queries.size() / 2, queries.end());

	int k = 1;
	float cost = 0;
	for(int i = 0; i < 1; i++)
	{
		auto query_start = chrono::high_resolution_clock::now();
		for(TopkQuery &q : queries)
		{
			vector<Object*> res = quadtree.topkQuery(k, q);
			for(int i = 0; i < res.size(); i++)
			{
				Object* o = res.at(i);
				cout << o->x << "," << o->y << " ";
			}
			cout << endl;
		}
		auto query_end = chrono::high_resolution_clock::now();
		auto query_cost = chrono::duration_cast<chrono::microseconds> (query_end - query_start).count();

		cost += query_cost / queries.size();
  	}

  	cout << "Query Time: " << cost / 100 << "us" << endl;

  	return 0;
}
