#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
bool checks_cycle(int winner, int loser);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // check if the name is in the list of candidates
    // update ranks where each voter has their own ranks array
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    // else
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // Update the preferences[i][j] array with the right numbers
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = (i + 1); j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // after the election is done check the preferences array for preference pairs and add them
    // remember to skip ties
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = (i + 1); j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[i][j] == preferences[j][i])
            {
                continue;
            }
            else if (preferences[j][i] > preferences[i][j])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // take the pairs array of structs and sort them
    for (int i = 0; i < pair_count - 1; i++)
    {
        for (int j = (i + 1); j < pair_count; j++)
        {
            // create variables to store the preferences of each candidate over the other
            int strength_i = preferences[pairs[i].winner][pairs[i].loser];
            int strength_j = preferences[pairs[j].winner][pairs[j].loser];

            // compare and bubble sort the bigger ones
            if (strength_j > strength_i)
            {
                pair temp = pairs[i];
                pairs[i] = pairs[j];
                pairs[j] = temp;
            }
        }
    }
}

// Recursive function to check if there are any locked pairs from the winner to the other candidates
bool checks_cycle(int winner, int loser)
{
    // If it ever come back to the original winner, we formed a cycle
    if (loser == winner)
    {
        return true;
    }

    // Follow all edges out of current loser to see if we can reach the winner
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[loser][i])
        {
            if (checks_cycle(winner, i))
            {
                return true;
            }
        }
    }
    return false; // if after all these checks, no cycle is found
}

void lock_pairs(void)
{
    // Pairs are already sorted strongest to weakest
    for (int i = 0; i < pair_count; i++)
    {
        int w = pairs[i].winner;
        int l = pairs[i].loser;

        // Only lock if it doesn’t create a cycle
        if (!checks_cycle(w, l))
        {
            locked[w][l] = true;
        }
    }
}

// Print the winner of the election
void print_winner(void)
{

    for (int i = 0; i < candidate_count; i++)
    {
        // create a base case boolean
        bool arrows = false;

        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                arrows = true;
                break;
            }
        }

        if (!arrows)
        {
            printf("%s. \n", candidates[i]);
        }
    }
}
