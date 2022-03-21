#include <iostream>
#include <chrono>
#include <cstdlib>
#include <future>
#include <time.h>
#include "skip_list.hpp"
using namespace std;

//#define NUM_THREADS 6
//#define TEST_COUNT 100000


class StressTest {
private:
	SkipList<int, std::string> skipList;
	int NUM_THREADS;
	int TEST_COUNT;
	int insertElement() {
		//cout << this_thread::get_id() << endl;
		long long tmp = TEST_COUNT / NUM_THREADS;
		for (int count = 0; count < tmp; ++count) {
			skipList.insert_element(rand() % TEST_COUNT, "a");
		}
		return 1;
	}

	int getElement() {
		//cout << this_thread::get_id() << endl;
		int tmp = TEST_COUNT / NUM_THREADS;
		for (int count = 0; count < tmp; ++count) {
			skipList.search_element(rand() % TEST_COUNT);
		}
		return 1;
	}

public:
	StressTest(int NUM_THREADS, int TEST_COUNT, int maxLevel):skipList(maxLevel), NUM_THREADS(NUM_THREADS), TEST_COUNT(TEST_COUNT) {}
	~StressTest() = default;

	void start() {
		srand(time(nullptr));
		chrono::microseconds insert_cost_u, search_cost_u;
		chrono::seconds insert_cost_s, search_cost_s;
		{
			
			vector<future<int>> futs;
			futs.resize(NUM_THREADS);
			int i;

			auto start = std::chrono::high_resolution_clock::now();

			for (i = 0; i < NUM_THREADS; ++i) {
				//cout << "main() : creating thread, " << i << endl;
				futs[i] = async(launch::async, &StressTest::insertElement, this);
			}
			for (i = 0; i < NUM_THREADS; ++i) {
				if (futs[i].get() != 1) {
					cout << "bug!!" << endl;
					exit(1);
				}
			}
			auto end = chrono::high_resolution_clock::now();
			//chrono::duration<double> cost = end - start;
			insert_cost_u = chrono::duration_cast<chrono::microseconds>(end - start);
			insert_cost_s = chrono::duration_cast<chrono::seconds>(end - start);
			// cout << "inserted  " << TEST_COUNT << " cost " << chrono::duration_cast<chrono::microseconds>(end - start).count() << "us" << endl;
			// cout << "inserted  " << TEST_COUNT << " cost " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s" << endl;
		}
		{
			
			vector<future<int>> futs;
			futs.resize(NUM_THREADS);
			int i;

			auto start = std::chrono::high_resolution_clock::now();

			for (i = 0; i < NUM_THREADS; ++i) {
				//cout << "main() : creating thread, " << i << endl;
				futs[i] = async(launch::async, &StressTest::getElement, this);
			}
			for (i = 0; i < NUM_THREADS; ++i) {
				if (futs[i].get() != 1) {
					cout << "bug!!" << endl;
					exit(1);
				}
			}
			auto end = chrono::high_resolution_clock::now();
			//chrono::duration<double> cost = end - start;
			search_cost_u = chrono::duration_cast<chrono::microseconds>(end - start);
			search_cost_s = chrono::duration_cast<chrono::seconds>(end - start);
			// cout << "fetch  " << TEST_COUNT << " cost " << chrono::duration_cast<chrono::microseconds>(end - start).count() << "us" << endl;
			// cout << "fetch  " << TEST_COUNT << " cost " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s" << endl;
		}
		cout << "insert  " << TEST_COUNT << " cost " << insert_cost_u.count() << "us" << endl;
		cout << "insert " << TEST_COUNT << " cost " << insert_cost_s.count() << "s" << endl;
		cout<<"============================================"<<endl;
		cout << "search  " << TEST_COUNT << " cost " << search_cost_u.count() << "us" << endl;
		cout << "search " << TEST_COUNT << " cost " << search_cost_s.count() << "s" << endl;
	}
};

