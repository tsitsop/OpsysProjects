/*
  Project 2
  Zachary Maurer, George Tsitsopoulos, Jose Guvenilir
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "project2.h"

void noncontiguousManagement(std::vector<Process> processes) {
	int t = 0; // simulation timer
	Process freeMem = processes[processes.size()-1];

	std::cout << "time " << t << "ms: Simulator started (Non-contiguous)\n" << std::flush;

	while (1) {
		// if process remaining time = 0, remove it from memory
		for (unsigned int i = 0; i < processes.size()-1; i++) {
			// look at the remaining time of each process event
			for (unsigned int j = 0; j < processes[i].events.size(); j++) {
				// if a process has completed
				if (processes[i].events[j].remainingTime == 0) {
					// go through each section of the process and remove it, replacing it with freeMem
					std::vector<Section>::iterator itr;
					for (itr = processes[i].sections.begin(); itr != processes[i].sections.end(); itr++) {
						// create free memory section
						Section newSection;
						newSection.startLoc = itr->startLoc;
						newSection.length = itr->length;
						freeMem.sections.push_back(newSection);
						freeMem.size += newSection.length;

						std::sort(freeMem.sections.begin(), freeMem.sections.end(), sortFreeMemory);

						itr = processes[i].sections.erase(itr);
						itr--;
					}

					// after adding the free memory, clean it up by combining adjacent sections of free memory
					bool cleaned = false;
					for (itr = freeMem.sections.begin(); itr != freeMem.sections.end(); itr++) {
						Section newSection;
						// if this section ends where the next begins, the combined section starts where this
						//  section starts
						if (itr->startLoc + itr->length == next(itr)->startLoc) {
							newSection.startLoc = itr->startLoc;
							newSection.length = itr->length;
							cleaned = true;
						}

						// as long as the current section ends where the following one begins, combine their 
						//  lengths into the new section, delete the current one, and move onto the next
						while (itr->startLoc + itr->length == next(itr)->startLoc) {
							newSection.length += + next(itr)->length;						
							itr = freeMem.sections.erase(itr);
							if (itr == freeMem.sections.end()) {
								break;
							}
						}

						// at this point, the next section of free memory is not adjacent to the current section
						//  so replace the current section with the value of the new, combined section
						if (cleaned) {
							*itr = newSection;
							cleaned = false;
						}
					}

					// signal that the event is complete
					processes[i].events[j].remainingTime = -1;
					processes[i].events[j].active = false;

					std::cout << "time " << t << "ms: Process " << processes[i].id << " removed:\n" << std::flush;
					processes[processes.size()-1] = freeMem;
					printMem(processes);
				}
			}
		}

		// if process arrives, add it to memory
		for (unsigned int i = 0; i < processes.size()-1; i++) {
			// look at each arrival time of the process
			for (unsigned int j = 0; j < processes[i].events.size(); j++) {
				// if a process arrives at this time
				if (processes[i].events[j].arrivalTime == t) {
					std::cout << "time " << t << "ms: Process " << processes[i].id 
					<< " arrived (requires " << processes[i].size << " frames)\n" << std::flush;

					// if there is enough space to add it, add it
					if (processes[i].size <= freeMem.size) {
						// length of the process that is being added
						int len = 0;
						std::vector<Section>::iterator itr;

						//loop through the freeMem sections to figure out where
						// to add what piece of the process
						for (itr = freeMem.sections.begin(); itr != freeMem.sections.end(); itr++) {
							Section newSection;
							newSection.startLoc = itr->startLoc;
							// if process too big to fully fit in this block, fill the block with the process,
							//  eliminate freeMem from variable
							if (processes[i].size > itr->length) {

								newSection.length = itr->length;
								newSection.startLoc = itr->startLoc;
								itr = freeMem.sections.erase(itr);
								itr--;
							} else {
								// process size - what we've already added
								newSection.length = processes[i].size - len;
								newSection.startLoc = itr->startLoc;
								itr->startLoc += newSection.length;
								itr->length -= newSection.length;
								// if this fills the free space, we need to remove the free space
								if (itr->length == 0) {
									itr = freeMem.sections.erase(itr);
									itr--;
								}
							}

							len += newSection.length;
							processes[i].sections.push_back(newSection);
							freeMem.size -= len;

							// if the entire process has been stored, we are done
							if (len == processes[i].size) {
								std::cout << "time " << t << "ms: Placed process " << processes[i].id 
								<< ":\n" << std::flush;

								processes[i].events[j].active = true;
								processes[processes.size()-1] = freeMem;
								printMem(processes);


								break;
							}
						}
					} else {
						std::cout << "time " << t << "ms: Cannot place process " << processes[i].id << " -- skipped!\n" << std::flush;
					}
				}
			}
		}

		// take care of all timing
		for (unsigned int i = 0; i < processes.size()-1; i++) {
			for (unsigned int j = 0; j < processes[i].events.size(); j++) {
				// if a process event has started but hasn't completed
				if (processes[i].events[j].remainingTime > 0 
					&& processes[i].events[j].remainingTime <= processes[i].events[j].runTime 
					&& processes[i].events[j].active) {
					
					processes[i].events[j].remainingTime--;
				}
			}
		}

		// end condition
		bool exit = true;
		for (unsigned int i = 0; i < processes.size()-1; i++) {
			for (unsigned int j = 0; j < processes[i].events.size(); j++) {
				// if there are more processes coming or there are active processes we aren't done
				if (t < processes[i].events[j].arrivalTime || processes[i].events[j].active) {
					exit = false;
					break;
				}
			}
			if (exit == false) {
				break;
			}
		}

		if (exit == true) {
			break;
		}

		t++;
	}

	std::cout << "time " << t << "ms: Simulator ended (Non-contiguous)\n" << std::flush; 

}

// processes leave memory before others arrive
// other ties ordered using pid values

/*notes
    - use processes.size()-1 because the last process is freeMem
    - going to change freeMem's size every time event is added or removed.
*/
