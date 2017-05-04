/*
  Project 2
  Zachary Maurer, George Tsitsopoulos, Jose Guvenilir
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "project2.h"

const int p_width = 32;
const int p_height = 8;

void printMem(std::vector<Process> p)
{
  std::vector<char> screen((p_height*p_width),'.');

  for (unsigned int i=0;i<p.size();i++)
  {
    for(unsigned int j=0;j<p[i].sections.size();j++)
    {
      int l= p[i].sections[j].startLoc;
      for(int k =0 ;k<p[i].sections[j].length;k++,l++)
      {
        screen[l] = p[i].id;
      }
    }
  }

  std::cout<<std::string(32, '=')<<"\n";
  for(unsigned int i=0;i<screen.size();i++)
  {
    std::cout<<screen[i];
    if (((i+1)%p_width)==0)
    {
      std::cout<<"\n";
    }
  }

  std::cout<<std::string(32, '=')<<"\n" << std::flush;

}



// sort the free memory sections to be in order
bool sortFreeMemory(const Section s1, const Section s2) {
	return s1.startLoc < s2.startLoc;
}

bool sortMemId(const Process p1, const Process p2)
{
  return p1.id<p2.id;
}

bool sortProcessArrival(const Process p1, const Process p2) {
	if (p1.events[0].arrivalTime == p2.events[0].arrivalTime) {
		return p1.id < p2.id;
	}

	return p1.events[0].arrivalTime < p2.events[0].arrivalTime;
}



int main(int argc,char* argv[])
{

  std::vector<Process> processes;
  int numP;
  std::ifstream f;
  std::string line;
  std::string eventString;
  std::string tmp;
  f.open(argv[1]);

  std::getline(f,line);
  std::istringstream n_stream(line);
  n_stream>>numP;

  while(std::getline(f,line))
  {
    Process p;
    std::istringstream iss(line);
    iss>>p.id;
    iss>>p.size;

    while (iss>>eventString)
    {
      Event e;
      std::istringstream e_stream(eventString);
      getline(e_stream,tmp,'/');
      std::istringstream a_stream(tmp);
      a_stream>>e.arrivalTime;
      getline(e_stream,tmp,'/');
      std::istringstream r_stream(tmp);
      r_stream>>e.runTime;
      e.remainingTime = e.runTime;
      e.active = false;

      p.events.push_back(e);
    }

    processes.push_back(p);

  }
  f.close();

  Process freeMem;
  Event e;
  Section s;

  std::sort(processes.begin(), processes.end(), sortProcessArrival);

  freeMem.id = '.';
  freeMem.size = 256;
  e.arrivalTime = 0;
  e.runTime = 0;
  freeMem.events.push_back(e);
  s.startLoc = 0;
  s.length = 256;
  freeMem.sections.push_back(s);
  processes.push_back(freeMem);



 //  for (int i = 0; i < processes.size(); i++) {
	// // look at the remaining time of each process event
	// std::cout << processes[i].id << std::endl;
	// std::cout << processes[i].size << std::endl;

	// std::cout << " events:\n";
	// for (int j = 0; j < processes[i].events.size(); j++) {
	// 	std::cout << "  " << processes[i].events[j].arrivalTime << std::endl;
	// 	std::cout << "  " << processes[i].events[j].runTime << std::endl;
	// 	std::cout << "  " << processes[i].events[j].remainingTime << std::endl;
	// }
	// std::cout << " sections:\n";

	// for (int j = 0; j < processes[i].sections.size(); j++) {
	// 	std::cout << "  " << processes[i].sections[j].startLoc << std::endl;
	// 	std::cout << "  " << processes[i].sections[j].length << std::endl;
	// }
 //  }



  firstFit(processes);
  bestFit(processes);
  worstFit(processes);
  noncontiguousManagement(processes);

  return 0;
}
