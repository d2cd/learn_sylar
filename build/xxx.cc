#include<iostream>
#include<set>

int main(){

	std::set<int> ss;
	ss.insert(1);
	ss.insert(2);
	for(auto& i : ss){
		std::cout << i << std::endl;
	}
	return 0;
}
