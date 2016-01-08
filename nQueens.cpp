#include <iostream>
#include <random> 
#include <ctime>
#include <vector>
#include <cstdlib>
#include <chrono>

#define BOARD_SIZE 17
#define NUM_OF_TESTS 200

void printBoard( int (&Board)[BOARD_SIZE] )
{
    for ( int i = BOARD_SIZE - 1; i >= 0; i-- )
    {
        // Correct printing
        for ( int j = 0; j < BOARD_SIZE; j++ )
        {
            if ( Board[j] == i )
            {
                std::cout << "1 ";
            }
            else
            {
                std::cout << "0 ";  
            }
            if ( j == BOARD_SIZE - 1 )
            {
                std::cout << "\n";
            }
        }
    }
    std::cout << std::endl;

    std::cout << "Just in case: ";
    for ( int i = 0; i < BOARD_SIZE; i++ )
    {
        if ( i < BOARD_SIZE - 1 )
            std::cout << Board[i] << ", ";
        else
            std::cout << Board[i] << "\n\n";
    }
}

/*
   Returns the number of Collisions on the N-Queen Board
*/
int getCollisions( int (&Board)[BOARD_SIZE] )
{
    int Collisions = 0;
    int Offset;
    for ( int i = 0; i < BOARD_SIZE; i++ )
    {
        for ( int j = i + 1; j < BOARD_SIZE; j++ )
        {
            // Check for Queens in same row
            if ( Board[i] == Board[j] )
                ++Collisions; 

            // Check for Queens in diagonal
            Offset = j - i;
            if ( (Board[i] == Board[j] + Offset) || ( Board[i] == Board[j] - Offset ) )
                ++Collisions;
        }
    }
    return Collisions;
}

struct verticalMove
{
    int Row;
    int Col;
    int Collisions;
};

/* 
   Performs the steepest ascent Hill Climbin Algorithm 
   and returns whether or not it succeeds.
*/
bool steepestHillClimbing( int (&Board)[BOARD_SIZE], int &SearchCost )
{
    int Iterations = 0;
    srand( time(NULL) );

    while ( true ) 
    {
        // Check board for collisions
        int Collisions = getCollisions( Board );
        //std::cout << "Collisions: " << Collisions << std::endl;

        // List of Collisions for every child of Board
        int ChildCollision[BOARD_SIZE * BOARD_SIZE];
        // Copy of Board to calculate Collisions in List above
        int Child[BOARD_SIZE];
        for ( int i = 0; i < BOARD_SIZE; i++ )
        {
            Child[i] = Board[i];
        }

        // Generate all possible new moves, Collisions per move
        for ( int i = 0; i < BOARD_SIZE; i++ )
        {
            for ( int j = 0; j < BOARD_SIZE; j++ )
            {
                if ( Board[i] != j ) // Look only at new moves
                {
                    Child[i] = j; // Make a move 
                    // Get, save Collisions after move
                    ChildCollision[i * BOARD_SIZE + j] = getCollisions( Child );
                    Child[i] = Board[i]; // Reset Child to same state as board
                }
                else
                {
                    ChildCollision[i * BOARD_SIZE + j] = 100000; // Just in case
                }
            }
        }

        // Find the moves with the fewest collisions (highest-valued)
        int LeastCollisions = Collisions;
        for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
        {
            if ( ChildCollision[i] < LeastCollisions )
            {
                LeastCollisions = ChildCollision[i];
            }
        }

        if ( LeastCollisions >= Collisions )
        {
            // Current state
            return false;
        }

        // Store the Children with the best moves
        std::vector<verticalMove> BestChildren;
        verticalMove Temp;
        for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
        {
            if ( ChildCollision[i] == LeastCollisions ) {
                // Create move data
                Temp.Row = i / BOARD_SIZE;
                Temp.Col = i % BOARD_SIZE;
                Temp.Collisions = ChildCollision[i];

                // Store move data
                BestChildren.push_back( Temp );
            }
        }

        // Choose one of the best children
        if ( !(BestChildren.empty()) )
        {
            // Choose a random best child
            int RandInt= rand() % BestChildren.size();
            Temp = BestChildren[RandInt];

            // Apply Child (Temp) move to Board
            Board[Temp.Row] = Temp.Col;

            if ( !(Temp.Collisions) ) // No Collisions
            {
                //printBoard( Board );
                return true;
            }
        }
        ++Iterations;
    }
    SearchCost = Iterations;
    return false;
}

bool minConflicts( int (&Board)[BOARD_SIZE], int &SearchCost )
{

    int ParentCollisions = getCollisions( Board );
    int Child[BOARD_SIZE];
    int Iterations = 0;
    srand( time(NULL) );

    // Repeat until No Collisions are left
    while ( ParentCollisions && Iterations < 1000 ) { // TODO: Count iter for cutoff 
        for ( int i = 0; i < BOARD_SIZE; i++ )
        {
            Child[i] = Board[i];
        }

        // Choose a Random queen/column
        int RandQueen = rand() % BOARD_SIZE;

        // Move it from the bottom to the top of the column
        // Record the Number of collisions for each placement of the one queen
        int ChildCollision[BOARD_SIZE];
        int LeastCollisions = ParentCollisions;
        for ( int i = 0; i < BOARD_SIZE; i++ )
        {
            // Skip iterations where i points to the queen
            if ( i != Child[RandQueen] )
            {
                Child[RandQueen] = i;
                ChildCollision[i] = getCollisions( Child );
            }
            else
            {
                ChildCollision[i] = ParentCollisions;
            }

            // Find the move with the lowest number of collisions
            if ( ChildCollision[i] < LeastCollisions )
            {
                LeastCollisions = ChildCollision[i];
            }
        }

        // Store the best moves for random tie breaker
        verticalMove Move;
        std::vector<verticalMove> BestMoves;
        for ( int i = 0; i < BOARD_SIZE; i++ )
        {
            if ( ChildCollision[i] == LeastCollisions )
            {
                Move.Row = i;
                Move.Col = RandQueen;
                Move.Collisions = ChildCollision[i];
                BestMoves.push_back( Move );
            }
        }

        // Choose the move with the least number of collisions
        int RandMove = rand() % BestMoves.size();
        Move = BestMoves[RandMove];

        // Apply the move with the least number of collisions
        Board[RandQueen] = Move.Row;
        ParentCollisions = Move.Collisions;

        ++Iterations;
    }
    SearchCost = Iterations;
    return true;
    // No Collisions, so we can return TRUE/SUCCESS
}

int main() 
{
    using namespace std::chrono;

    int Board1[BOARD_SIZE];
    int Board2[BOARD_SIZE];
    bool Success = 0;
    float SuccessCount = 0;
    int SearchCost = 0;
    int HillSearchCost = 0;
    int MCSearchCost = 0;
    srand( time(NULL) );

    high_resolution_clock::time_point Tinit;
    high_resolution_clock::time_point Tfinal;
    duration<double> HillClimbDuration;
    duration<double> MinConflictDuration;
    double HillClimbTime = 0;
    double MinConflictTime = 0;

    for ( int n = 0; n < NUM_OF_TESTS; n++ )
    {
        // Randomly generate board
        for ( int i = 0; i < BOARD_SIZE; i++ )
        {
            Board1[i] = rand() % BOARD_SIZE; // put Queen in random row in each col
            Board2[i] = rand() % BOARD_SIZE; 
        }
        
        Tinit = high_resolution_clock::now(); 
        Success = steepestHillClimbing( Board1, SearchCost );
        Tfinal = high_resolution_clock::now(); 
        HillSearchCost += SearchCost;
        HillClimbDuration = duration_cast<duration<double>>( Tfinal - Tinit );
        if ( Success )
        {
            SuccessCount += 1;
        }
        HillClimbTime += HillClimbDuration.count();

        Tinit = high_resolution_clock::now(); 
        minConflicts( Board2, SearchCost );
        Tfinal = high_resolution_clock::now(); 
        MCSearchCost += SearchCost;
        MinConflictDuration = duration_cast<duration<double>>( Tfinal - Tinit );
        MinConflictTime += MinConflictDuration.count();
    }

    std::cout << "Steepest Hill Climbing Algorithm:\n\t";
    std::cout << (SuccessCount / NUM_OF_TESTS * 100) << "% Success rate.\n";
    std::cout << "\tSearch Costs: " << HillSearchCost / NUM_OF_TESTS << " moves.\n";
    std::cout << "\tTotal Time:   " << HillClimbTime << " seconds.\n";
    std::cout << "\tAverage Time: " << HillClimbTime/NUM_OF_TESTS << " seconds.\n\n";

    std::cout << "Min Conflicts Algorithm:\n";
    std::cout << "\tSearch Costs: " << MCSearchCost / NUM_OF_TESTS << " moves.\n";
    std::cout << "\tTotal Time:   " << MinConflictTime << " seconds.\n";
    std::cout << "\tAverage Time: " << MinConflictTime / NUM_OF_TESTS << " seconds." << std::endl;

    return 0;
}
