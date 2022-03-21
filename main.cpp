#include<iostream>
#include"skip_list.hpp"
#include "stress_test.hpp"
using namespace std;
int main() {
	SkipList<int, int> sl(8);
	//���Բ���
	sl.insert_element(10, 10);
	sl.insert_element(1, 30);
	sl.insert_element(2, 499);
	sl.insert_element(123, 99);
	sl.insert_element(-10, 33);
	//���Դ�ӡ
	sl.display_list();
	//����д�ļ�
	sl.dump_file();
	//����ɾ��
	sl.delete_element(2);
	sl.delete_element(10);
	sl.display_list();
	//���Լ����ļ�
	sl.load_file();
	sl.display_list();
	//���Բ���
	cout << "�Ƿ��ҵ�10   ��" << (bool)sl.search_element(10) << endl;
	
	StressTest t(2, 1000000, 18);
	t.start();
}