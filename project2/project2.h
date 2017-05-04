/*
  Project 2
  Zachary Maurer, George Tsitsopoulos, Jose Guvenilir
*/

typedef struct
{
  int arrivalTime;
  int runTime;
  int remainingTime;
  bool active;
} Event ;


typedef struct
{
  int startLoc;
  int length;
} Section ;

typedef struct
{
  char id;
  int size;
  std::vector<Event> events;
  std::vector<Section> sections;
} Process ;


// functions
void noncontiguousManagement(std::vector<Process>);
void firstFit(std::vector<Process>);

void worstFit(std::vector<Process>);

void bestFit(std::vector<Process>);



// sort the free memory sections to be in order
bool sortFreeMemory(const Section, const Section);
bool sortProcessArrival(const Process, const Process);

//sort Processes in order
bool sortMemId(const Process p1, const Process p2);

// print memory
void printMem(std::vector<Process>);

// IMPLEMENTED FROM cppreference.com
/*template<class ForwardIt>
ForwardIt next(ForwardIt it,
               typename std::iterator_traits<ForwardIt>::difference_type n = 1)
{
    std::advance(it, n);
    return it;
}*/
