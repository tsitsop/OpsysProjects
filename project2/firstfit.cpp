#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <set>

#include "project2.h"


void firstFit(std::vector<Process> processes)
{

  int t=0;
  int nextPlaced = 0;
  //int defragTime = -1;
  int totalDefragTime = 0;
  int frames;
  std::set<char> moved;


  Process freeMem = processes[processes.size()-1];

  std::cout<<"time "<<t+totalDefragTime<<"ms: Simulator started (Contiguous -- Next-Fit)\n"<<std::flush;

  while(1)
  {
    //remove done processes

    for(unsigned int i=0;i<processes.size()-1;i++)
    {
      for(unsigned int j=0;j<processes[i].events.size();j++)
      {
        if (processes[i].events[j].remainingTime==0)
        {
          std::vector<Section>::iterator itr;
          for(itr = processes[i].sections.begin();itr!=processes[i].sections.end();itr++)
          {
            Section newSection;
            newSection.startLoc = itr->startLoc;
						newSection.length = itr->length;
						freeMem.sections.push_back(newSection);
						freeMem.size += newSection.length;

						std::sort(freeMem.sections.begin(), freeMem.sections.end(), sortFreeMemory);

						itr = processes[i].sections.erase(itr);
						itr--;
          }
          // combining adjacent sections of free memory
					/*bool cleaned = false;
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
        }*/
          // signal that the event is complete
					processes[i].events[j].remainingTime = -1;
					processes[i].events[j].active = false;

					std::cout << "time " << t+totalDefragTime << "ms: Process " << processes[i].id << " removed:\n" << std::flush;
					processes[processes.size()-1] = freeMem;
					printMem(processes);

        }
      }
    }

  // if process arrives, add it to memory
		for (unsigned int i = 0; i < processes.size()-1; i++)
    {
			// look at each arrival time of the process
			for (unsigned int j = 0; j < processes[i].events.size(); j++) {
				// if a process arrives at this time
				if (processes[i].events[j].arrivalTime == t) {
          // std::cout << "first: " << nextPlaced << std::endl;
					std::cout << "time " << t+totalDefragTime << "ms: Process " << processes[i].id  << " arrived (requires " << processes[i].size << " frames)\n" << std::flush;

					// if there is enough space to add it, add it
          bool space = false;
          if (processes[i].size>freeMem.size)
          {
            std::cout << "time " <<t+totalDefragTime << "ms: Cannot place process " << processes[i].id << " -- skipped!\n" << std::flush;
            continue;
          }
          unsigned int k;
          int loc; // the index of the free memory section that we will be adding this to

          //check to see if we have exact match for next loc
          for (k = 0;k<freeMem.sections.size();k++)
          {
            // std::cout << "free mem start " << freeMem.sections[k].startLoc << std::endl;
            if (processes[i].size<=freeMem.sections[k].length && freeMem.sections[k].startLoc==nextPlaced) // if we should put the process here
            {
              space = true;
               loc = k;
              //std::cout << "loc = " << loc << std::endl;
              break;
            }
          }

          //see if we can add process later than optimal
          if(!space)
          {
            for (k = 0;k<freeMem.sections.size();k++)
            {
              if (processes[i].size<=freeMem.sections[k].length && freeMem.sections[k].startLoc>nextPlaced) //if there is enough space and this free memory section is past the next placed
              {
                space = true;
                loc = k;
                break;
              }
            }
          }

          // see if we can add the process before than optimal
          if(!space)
          {
            for (k = 0;k<freeMem.sections.size();k++)
            {
              if (processes[i].size<=freeMem.sections[k].length)
              {
                space = true;
                loc = k;
                break;
              }
            }
          }

          if (!space)
          {
            //Check if combined free memory will help
            std::vector<Section>::iterator itr;
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

           for (k = 0;k<freeMem.sections.size();k++)
           {
             if (processes[i].size<=freeMem.sections[k].length)
             {
               space = true;
               loc = k;
               break;
             }
           }
          }

          if (!space){
            //Defrag
            //  Find first free memory
            // move all frames after it backwards;
            // clear multiple free spaces;
            //  Find a section that fits it
            frames = 0;
            moved.clear();
            std::cout<<"time "<<t+totalDefragTime<<"ms: Cannot place process "<<processes[i].id<<" -- starting defragmentation\n"<<std::flush;
            int nextSpot = 0;
            std::vector<Process>::iterator pIt;
            for(unsigned int l=0;l<freeMem.sections.size();l++)
            {
              pIt = processes.begin();
              for (;pIt!=processes.end()-1;pIt++)
              {
                for(unsigned int m=0;m<pIt->sections.size();m++)
                {
                  if (pIt->sections[m].startLoc>freeMem.sections[l].startLoc)
                  {

                    pIt->sections[m].startLoc-=freeMem.sections[l].length;
                    frames+=pIt->sections[m].length;
                    moved.insert(pIt->id);
                    nextSpot = (pIt->sections[m].startLoc)+(pIt->size);

                  }
                }
              }
            }
            freeMem.sections.clear();

            Section freeSec;
            freeSec.length = freeMem.size;
            freeSec.startLoc = nextSpot;

            nextPlaced = nextSpot;

            freeMem.sections.push_back(freeSec);

            processes[processes.size()-1].sections.clear();
            processes[processes.size()-1].sections.push_back(freeSec);

            totalDefragTime+= frames;


            //
              std::cout<<"time "<<t+totalDefragTime<<"ms: Defragmentation complete (moved "<<frames<<" frames: ";
              std::set<char>::iterator it = moved.begin();
              std::cout<<*it;
              it++;
              for(;it!=moved.end();it++)
              {
                std::cout<<", "<<*it;
              }
              std::cout<<")\n"<<std::flush;
              //
              printMem(processes);
              //std::cout<<processes[i].size<<freeMem.sections[0].length<<"\n"<<std::flush;
              if (processes[i].size<=freeMem.sections[0].length)
              {
                space = true;
                loc = 0;
              }
          }
					if (space)
          {

						  //place in free memory
							Section newSection;
              newSection.startLoc = freeMem.sections[loc].startLoc;
							newSection.length = processes[i].size;

              nextPlaced = newSection.startLoc+newSection.length;

              freeMem.sections[loc].startLoc +=newSection.length;
              freeMem.sections[loc].length -=newSection.length;
							if (freeMem.sections[loc].length == 0) {
								freeMem.sections.erase(freeMem.sections.begin()+loc);
							}
							processes[i].sections.push_back(newSection);
							freeMem.size -= newSection.length;

							std::cout << "time " << t+totalDefragTime << "ms: Placed process " << processes[i].id<< ":\n" << std::flush;
              // std::cout << nextPlaced << std::endl;
							processes[i].events[j].active = true;
							processes[processes.size()-1] = freeMem;


							printMem(processes);
					  }
					 }
				 }
			}
      //std::sort(processes.begin(), processes.end(), sortMemId);
      //std::cout<"U";
    //

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
    //

  }
  std::cout << "time " << t+totalDefragTime << "ms: Simulator ended (Contiguous -- Next-Fit)\n" << std::flush;
}
