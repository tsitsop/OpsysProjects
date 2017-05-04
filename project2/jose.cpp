/*
  Project 2
  Zachary Maurer, George Tsitsopoulos, Jose Guvenilir
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <set>

#include "project2.h"

void bestFit(std::vector<Process> processes) {
  int t = 0; // simulation timer
  int totalDefragTime = 0;
  Process freeMem = processes[processes.size()-1];

  std::cout << "time " << t + totalDefragTime << "ms: Simulator started (Contiguous -- Best-Fit)\n" << std::flush;

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

          std::cout << "time " << t + totalDefragTime << "ms: Process " << processes[i].id << " removed:\n" << std::flush;
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
          std::cout << "time " << t + totalDefragTime << "ms: Process " << processes[i].id 
          << " arrived (requires " << processes[i].size << " frames)\n" << std::flush;

          // if there is enough space to add it, add it
          if (processes[i].size <= freeMem.size) {
            // length of the process that is being added
            //int len = 0;
            Section curBest; // current best section
            curBest.length = -1;
            curBest.startLoc = -1;
            std::vector<Section>::iterator itr;
            std::vector<Section>::iterator CB;

            // loop through the freeMem sections to find the
            // smallest freeMem section that is big enough for
            // the current process
            for (itr = freeMem.sections.begin(); itr != freeMem.sections.end(); itr++) {
              // if process too big to fully fit in this block,
              // proceed to next section
              // std::cout << processes[i].size << std::endl;
              //   std::cout << itr->length << std::endl;
                
              if (processes[i].size > itr->length) {

                continue;
              } else {
                // if current section is first large enough section
                if (curBest.length == -1) {
                  curBest.length = itr->length;
                  curBest.startLoc = itr->startLoc;
                  CB = itr;
                } else if (curBest.length > itr->length) {
                  curBest.length = itr->length;
                  curBest.startLoc = itr->startLoc;
                  CB = itr;
                }
              }
            }
            // if no suitable section is found
            if(curBest.length == -1) {
              // need to defragment
              std::cout << "time " << t +totalDefragTime << "ms: Cannot place process " << processes[i].id 
              << " -- starting defragmentation\n" << std::flush;
              //int totalDefragTime = 0;

                      //Defrag
                      //  Find first free memory
                      // move all frames after it backwards;
                      // clear multiple free spaces;
                      //  Find a section that fits it
                      //int loc;
                      int frames = 0;
                      //int nextPlaced = 0;
                      std::set<char> moved;
                      moved.clear();
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

                      //nextPlaced = nextSpot;

                      freeMem.sections.push_back(freeSec);

                      processes[processes.size()-1].sections.clear();
                      processes[processes.size()-1].sections.push_back(freeSec);

                      totalDefragTime+= frames;

                      //t += totalDefragTime;
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
                          //space = true;
                          //loc = 0;
                        }





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





          
                        Section newSection;
                        newSection.length = processes[i].size;
                        newSection.startLoc = freeMem.sections[0].startLoc;

                        freeMem.sections[0].startLoc += newSection.length;
                        freeMem.sections[0].length -= newSection.length;

                        if (freeMem.sections[0].length == 0) {
                          freeMem.sections.erase(freeMem.sections.begin());
                        }

                        processes[i].sections.push_back(newSection);
                        freeMem.size -= newSection.length;

                        std::cout << "time " << t + totalDefragTime << "ms: Placed process " << processes[i].id 
                          << ":\n" << std::flush;

                          processes[i].events[j].active = true;
                          processes[processes.size()-1] = freeMem;
                          printMem(processes);




              //std::cout << "time " << t << "ms: Defragmentation complete (moved " << " frames: ";
              //loop through processes currently in memory

            } else {
              // create the new section for the process
              Section newSection;
              newSection.length = processes[i].size;
              newSection.startLoc = CB->startLoc;
              //shrink the free space
              CB->startLoc += newSection.length;
              CB->length -= newSection.length;
              // if this fills the free space, we need to remove the free space
              if (CB->length == 0) {
                CB = freeMem.sections.erase(CB);
              }

              processes[i].sections.push_back(newSection);
              freeMem.size -= newSection.length;

              std::cout << "time " << t + totalDefragTime << "ms: Placed process " << processes[i].id 
                << ":\n" << std::flush;

                processes[i].events[j].active = true;
                processes[processes.size()-1] = freeMem;
                printMem(processes);



            }
          } else {
            std::cout << "time " << t + totalDefragTime << "ms: Cannot place process " << processes[i].id 
            << " -- skipped!\n" << std::flush;
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

  std::cout << "time " << t + totalDefragTime << "ms: Simulator ended (Contiguous -- Best-Fit)\n\n" << std::flush; 
}

void worstFit(std::vector<Process> processes) {
  int t = 0; // simulation timer
  int totalDefragTime = 0;
  Process freeMem = processes[processes.size()-1];

  std::cout << "time " << t + totalDefragTime << "ms: Simulator started (Contiguous -- Worst-Fit)\n" << std::flush;

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

          std::cout << "time " << t + totalDefragTime << "ms: Process " << processes[i].id << " removed:\n" << std::flush;
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
          std::cout << "time " << t + totalDefragTime << "ms: Process " << processes[i].id 
          << " arrived (requires " << processes[i].size << " frames)\n" << std::flush;

          // if there is enough space to add it, add it
          if (processes[i].size <= freeMem.size) {
            // length of the process that is being added
            //int len = 0;
            Section curBest; // current best section
            curBest.length = -1;
            curBest.startLoc = -1;
            std::vector<Section>::iterator itr;
            std::vector<Section>::iterator CB;

            // loop through the freeMem sections to find the
            // biggest freeMem section that is big enough for
            // the current process
            for (itr = freeMem.sections.begin(); itr != freeMem.sections.end(); itr++) {
              // if process too big to fully fit in this block,
              // proceed to next section
              // std::cout << processes[i].size << std::endl;
              //   std::cout << itr->length << std::endl;
                
              if (processes[i].size > itr->length) {

                continue;
              } else {
                // if current section is first large enough section
                if (curBest.length == -1) {
                  curBest.length = itr->length;
                  curBest.startLoc = itr->startLoc;
                  CB = itr;
                } else if (curBest.length < itr->length) {
                  curBest.length = itr->length;
                  curBest.startLoc = itr->startLoc;
                  CB = itr;
                }
              }
            }
            // if no suitable section is found
            if(curBest.length == -1) {
              // need to defragment
              std::cout << "time " << t +totalDefragTime << "ms: Cannot place process " << processes[i].id 
              << " -- starting defragmentation\n" << std::flush;
              //int totalDefragTime = 0;

                      //Defrag
                      //  Find first free memory
                      // move all frames after it backwards;
                      // clear multiple free spaces;
                      //  Find a section that fits it
                      //int loc;
                      int frames = 0;
                      //int nextPlaced = 0;
                      std::set<char> moved;
                      moved.clear();
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

                      //nextPlaced = nextSpot;

                      freeMem.sections.push_back(freeSec);

                      processes[processes.size()-1].sections.clear();
                      processes[processes.size()-1].sections.push_back(freeSec);

                      totalDefragTime+= frames;

                      //t += totalDefragTime;
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
                          //space = true;
                          //loc = 0;
                        }





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





          
                        Section newSection;
                        newSection.length = processes[i].size;
                        newSection.startLoc = freeMem.sections[0].startLoc;

                        freeMem.sections[0].startLoc += newSection.length;
                        freeMem.sections[0].length -= newSection.length;

                        if (freeMem.sections[0].length == 0) {
                          freeMem.sections.erase(freeMem.sections.begin());
                        }

                        processes[i].sections.push_back(newSection);
                        freeMem.size -= newSection.length;

                        std::cout << "time " << t + totalDefragTime << "ms: Placed process " << processes[i].id 
                          << ":\n" << std::flush;

                          processes[i].events[j].active = true;
                          processes[processes.size()-1] = freeMem;
                          printMem(processes);




              //std::cout << "time " << t << "ms: Defragmentation complete (moved " << " frames: ";
              //loop through processes currently in memory

            } else {
              // create the new section for the process
              Section newSection;
              newSection.length = processes[i].size;
              newSection.startLoc = CB->startLoc;
              //shrink the free space
              CB->startLoc += newSection.length;
              CB->length -= newSection.length;
              // if this fills the free space, we need to remove the free space
              if (CB->length == 0) {
                CB = freeMem.sections.erase(CB);
              }

              processes[i].sections.push_back(newSection);
              freeMem.size -= newSection.length;

              std::cout << "time " << t + totalDefragTime << "ms: Placed process " << processes[i].id 
                << ":\n" << std::flush;

                processes[i].events[j].active = true;
                processes[processes.size()-1] = freeMem;
                printMem(processes);



            }
          } else {
            std::cout << "time " << t + totalDefragTime << "ms: Cannot place process " << processes[i].id 
            << " -- skipped!\n" << std::flush;
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

  std::cout << "time " << t + totalDefragTime << "ms: Simulator ended (Contiguous -- Worst-Fit)\n\n" << std::flush; 
}
