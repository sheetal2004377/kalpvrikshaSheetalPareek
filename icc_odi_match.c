#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define MAX_NAME_LENGTH 50
#define MAX_PLAYERS_PER_TEAM 50
#define MAX_PLAYER_ID 1000
#define NUM_TEAMS 10
#define NUM_ROLES 3

#define BATSMAN 0
#define BOWLER 1
#define ALL_ROUNDER 2

const char *roleNames[NUM_ROLES] = {"Batsman", "Bowler", "All-rounder"};
const char *roleGroupNames[NUM_ROLES] = {"Batsmen", "Bowlers", "All-rounders"};

typedef struct PlayerInfo
{
    int playerId;
    char name[MAX_NAME_LENGTH + 1];
    char teamName[MAX_NAME_LENGTH + 1];
    int role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
} PlayerInfo;

typedef struct PlayerNode
{
    PlayerInfo data;
    struct PlayerNode *next;
} PlayerNode;

typedef struct RoleNode
{
    PlayerInfo *player;
    struct RoleNode *next;
} RoleNode;

typedef struct Team
{
    int teamId;
    char name[MAX_NAME_LENGTH + 1];
    int totalPlayers;
    float totalBattingStrikeRate;
    int battingPlayers;
    float averageBattingStrikeRate;
    PlayerNode *head;
    PlayerNode *tail;
    RoleNode *roleHead[NUM_ROLES];
} Team;

typedef struct Heap
{
    RoleNode *items[NUM_TEAMS];
    int size;
} Heap;

Team teamList[NUM_TEAMS];
int totalTeams = 0;

void clearBuffer()
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

int readNumber(char message[])
{
    int number;

    printf("%s", message);

    if (scanf("%d", &number) != 1)
    {
        printf("Invalid input.\n");
        clearBuffer();
        return -1;
    }

    clearBuffer();
    return number;
}

float readDecimal(char message[])
{
    float number;

    printf("%s", message);

    if (scanf("%f", &number) != 1)
    {
        printf("Invalid input.\n");
        clearBuffer();
        return -1;
    }

    clearBuffer();
    return number;
}

int readName(char name[])
{
    int c;

    printf("Name: ");

    if (scanf(" %50[^\n]", name) != 1)
    {
        printf("Invalid input.\n");
        clearBuffer();
        return 0;
    }

    c = getchar();

    if (c != '\n' && c != EOF)
    {
        printf("Name must be between 1 and %d characters.\n", MAX_NAME_LENGTH);
        clearBuffer();
        return 0;
    }

    return 1;
}

int getRoleFromText(const char *text)
{
    if (strcmp(text, "Batsman") == 0)
    {
        return BATSMAN;
    }

    if (strcmp(text, "Bowler") == 0)
    {
        return BOWLER;
    }

    if (strcmp(text, "All-rounder") == 0)
    {
        return ALL_ROUNDER;
    }

    return -1;
}

int readRole()
{
    int choice;

    choice = readNumber("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");

    if (choice < 1 || choice > 3)
    {
        printf("Invalid role. Please enter 1, 2 or 3.\n");
        return -1;
    }

    return choice - 1;
}

float calculatePerformanceIndex(int role, float average, float strikeRate, int wickets, float economyRate)
{
    if (role == BATSMAN)
    {
        return (average * strikeRate) / 100;
    }

    if (role == BOWLER)
    {
        return (wickets * 2) + (100 - economyRate);
    }

    return ((average * strikeRate) / 100) + (wickets * 2);
}

int isBefore(PlayerInfo *first, PlayerInfo *second)
{
    if (first->performanceIndex != second->performanceIndex)
    {
        return first->performanceIndex > second->performanceIndex;
    }

    return first->playerId < second->playerId;
}

Team *findTeam(int teamId)
{
    int low = 0;
    int high = totalTeams - 1;
    int mid;

    while (low <= high)
    {
        mid = low + (high - low) / 2;

        if (teamList[mid].teamId == teamId)
        {
            return &teamList[mid];
        }

        if (teamList[mid].teamId < teamId)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return NULL;
}

Team *findTeamByName(const char *name)
{
    int i;

    for (i = 0; i < totalTeams; i++)
    {
        if (strcmp(teamList[i].name, name) == 0)
        {
            return &teamList[i];
        }
    }

    return NULL;
}

int playerExists(int playerId)
{
    int i;
    PlayerNode *current;

    for (i = 0; i < totalTeams; i++)
    {
        current = teamList[i].head;

        while (current != NULL)
        {
            if (current->data.playerId == playerId)
            {
                return 1;
            }

            current = current->next;
        }
    }

    return 0;
}

int insertInRoleList(Team *team, PlayerInfo *player)
{
    RoleNode *newNode;
    RoleNode **link;

    newNode = (RoleNode *)malloc(sizeof(RoleNode));

    if (newNode == NULL)
    {
        return 0;
    }

    newNode->player = player;

    link = &team->roleHead[player->role];

    while (*link != NULL && isBefore((*link)->player, player))
    {
        link = &(*link)->next;
    }

    newNode->next = *link;
    *link = newNode;

    return 1;
}

int attachPlayer(Team *team, PlayerInfo player)
{
    PlayerNode *newNode;

    newNode = (PlayerNode *)malloc(sizeof(PlayerNode));

    if (newNode == NULL)
    {
        return 0;
    }

    newNode->data = player;
    newNode->next = NULL;

    if (team->tail == NULL)
    {
        team->head = newNode;
    }
    else
    {
        team->tail->next = newNode;
    }

    team->tail = newNode;

    if (!insertInRoleList(team, &newNode->data))
    {
        return 0;
    }

    team->totalPlayers++;

    if (player.role == BATSMAN || player.role == ALL_ROUNDER)
    {
        team->totalBattingStrikeRate += player.strikeRate;
        team->battingPlayers++;
        team->averageBattingStrikeRate = team->totalBattingStrikeRate / team->battingPlayers;
    }

    return 1;
}

void loadData()
{
    int i;
    PlayerInfo player;
    Team *team;
    int role;

    for (i = 0; i < teamCount && i < NUM_TEAMS; i++)
    {
        memset(&teamList[i], 0, sizeof(Team));
        teamList[i].teamId = i + 1;
        strncpy(teamList[i].name, teams[i], MAX_NAME_LENGTH);
        teamList[i].name[MAX_NAME_LENGTH] = '\0';
        totalTeams++;
    }

    for (i = 0; i < playerCount; i++)
    {
        role = getRoleFromText(players[i].role);
        team = findTeamByName(players[i].team);

        if (role == -1 || team == NULL)
        {
            continue;
        }

        memset(&player, 0, sizeof(PlayerInfo));
        player.playerId = players[i].id;
        strncpy(player.name, players[i].name, MAX_NAME_LENGTH);
        player.name[MAX_NAME_LENGTH] = '\0';
        strncpy(player.teamName, players[i].team, MAX_NAME_LENGTH);
        player.teamName[MAX_NAME_LENGTH] = '\0';
        player.role = role;
        player.totalRuns = players[i].totalRuns;
        player.battingAverage = players[i].battingAverage;
        player.strikeRate = players[i].strikeRate;
        player.wickets = players[i].wickets;
        player.economyRate = players[i].economyRate;
        player.performanceIndex = calculatePerformanceIndex(role, player.battingAverage, player.strikeRate, player.wickets, player.economyRate);

        if (!attachPlayer(team, player))
        {
            printf("Memory allocation failed.\n");
            exit(1);
        }
    }
}

void printLine(int length)
{
    int i;

    for (i = 0; i < length; i++)
    {
        printf("=");
    }

    printf("\n");
}

void printTeamHeader()
{
    printLine(92);
    printf("%-5s %-24s %-12s %8s %6s %7s %5s %5s %10s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printLine(92);
}

void printPlayerRow(PlayerInfo *player)
{
    printf("%-5d %-24s %-12s %8d %6.1f %7.1f %5d %5.1f %10.2f\n", player->playerId, player->name, roleNames[player->role], player->totalRuns, player->battingAverage, player->strikeRate, player->wickets, player->economyRate, player->performanceIndex);
}

void printAcrossHeader()
{
    printLine(112);
    printf("%-5s %-24s %-14s %-12s %8s %6s %7s %5s %5s %10s\n", "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printLine(112);
}

void printAcrossRow(PlayerInfo *player)
{
    printf("%-5d %-24s %-14s %-12s %8d %6.1f %7.1f %5d %5.1f %10.2f\n", player->playerId, player->name, player->teamName, roleNames[player->role], player->totalRuns, player->battingAverage, player->strikeRate, player->wickets, player->economyRate, player->performanceIndex);
}

void printTeamIds()
{
    int i;

    printf("Teams: ");

    for (i = 0; i < totalTeams; i++)
    {
        printf("%d-%s", teamList[i].teamId, teamList[i].name);

        if (i < totalTeams - 1)
        {
            printf(", ");
        }
    }

    printf("\n");
}

void addPlayer()
{
    int teamId;
    int roleChoice;
    char name[MAX_NAME_LENGTH + 1];
    Team *team;
    PlayerInfo player;

    printTeamIds();
    teamId = readNumber("Enter Team ID to add player: ");
    team = findTeam(teamId);

    if (team == NULL)
    {
        printf("Team ID not found.\n");
        return;
    }

    if (team->totalPlayers >= MAX_PLAYERS_PER_TEAM)
    {
        printf("Team %s already has %d players.\n", team->name, MAX_PLAYERS_PER_TEAM);
        return;
    }

    memset(&player, 0, sizeof(PlayerInfo));

    printf("Enter Player Details:\n");

    player.playerId = readNumber("Player ID: ");

    if (player.playerId < 1 || player.playerId > MAX_PLAYER_ID)
    {
        printf("Player ID must be between 1 and %d.\n", MAX_PLAYER_ID);
        return;
    }

    if (playerExists(player.playerId))
    {
        printf("Player ID already exists.\n");
        return;
    }

    if (!readName(name))
    {
        return;
    }

    roleChoice = readNumber("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");

    if (roleChoice < 1 || roleChoice > 3)
    {
        printf("Invalid role.\n");
        return;
    }

    player.totalRuns = readNumber("Total Runs: ");

    if (player.totalRuns < 0)
    {
        printf("Total runs must be a non-negative number.\n");
        return;
    }

    player.battingAverage = readDecimal("Batting Average: ");

    if (player.battingAverage < 0)
    {
        printf("Batting average must be a non-negative number.\n");
        return;
    }

    player.strikeRate = readDecimal("Strike Rate: ");

    if (player.strikeRate < 0)
    {
        printf("Strike rate must be a non-negative number.\n");
        return;
    }

    player.wickets = readNumber("Wickets: ");

    if (player.wickets < 0)
    {
        printf("Wickets must be a non-negative number.\n");
        return;
    }

    player.economyRate = readDecimal("Economy Rate: ");

    if (player.economyRate < 0)
    {
        printf("Economy rate must be a non-negative number.\n");
        return;
    }

    strcpy(player.name, name);
    strcpy(player.teamName, team->name);
    player.role = roleChoice - 1;
    player.performanceIndex = calculatePerformanceIndex(player.role, player.battingAverage, player.strikeRate, player.wickets, player.economyRate);

    if (!attachPlayer(team, player))
    {
        printf("Memory allocation failed. Player not added.\n");
        return;
    }

    printf("Player added successfully to Team %s!\n", team->name);
}

void displayTeamPlayers()
{
    int teamId;
    Team *team;
    PlayerNode *current;

    printTeamIds();
    teamId = readNumber("Enter Team ID: ");
    team = findTeam(teamId);

    if (team == NULL)
    {
        printf("Team ID not found.\n");
        return;
    }

    printf("Players of Team %s:\n", team->name);
    printTeamHeader();

    current = team->head;

    while (current != NULL)
    {
        printPlayerRow(&current->data);
        current = current->next;
    }

    printLine(92);
    printf("Total Players: %d\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", team->averageBattingStrikeRate);
}

void mergeTeams(Team *list[], Team *temp[], int low, int mid, int high)
{
    int i = low;
    int j = mid + 1;
    int k = low;

    while (i <= mid && j <= high)
    {
        if (list[i]->averageBattingStrikeRate >= list[j]->averageBattingStrikeRate)
        {
            temp[k++] = list[i++];
        }
        else
        {
            temp[k++] = list[j++];
        }
    }

    while (i <= mid)
    {
        temp[k++] = list[i++];
    }

    while (j <= high)
    {
        temp[k++] = list[j++];
    }

    for (k = low; k <= high; k++)
    {
        list[k] = temp[k];
    }
}

void mergeSortTeams(Team *list[], Team *temp[], int low, int high)
{
    int mid;

    if (low >= high)
    {
        return;
    }

    mid = low + (high - low) / 2;

    mergeSortTeams(list, temp, low, mid);
    mergeSortTeams(list, temp, mid + 1, high);
    mergeTeams(list, temp, low, mid, high);
}

void displayTeamsByStrikeRate()
{
    Team *sortedTeams[NUM_TEAMS];
    Team *temp[NUM_TEAMS];
    int i;

    for (i = 0; i < totalTeams; i++)
    {
        sortedTeams[i] = &teamList[i];
    }

    mergeSortTeams(sortedTeams, temp, 0, totalTeams - 1);

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printLine(57);
    printf("%-4s %-16s %-14s %-14s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printLine(57);

    for (i = 0; i < totalTeams; i++)
    {
        printf("%-4d %-16s %-14.2f %-14d\n", sortedTeams[i]->teamId, sortedTeams[i]->name, sortedTeams[i]->averageBattingStrikeRate, sortedTeams[i]->totalPlayers);
    }

    printLine(57);
}

void displayTopPlayers()
{
    int teamId;
    int role;
    int k;
    int shown = 0;
    Team *team;
    RoleNode *current;

    printTeamIds();
    teamId = readNumber("Enter Team ID: ");
    team = findTeam(teamId);

    if (team == NULL)
    {
        printf("Team ID not found.\n");
        return;
    }

    role = readRole();

    if (role == -1)
    {
        return;
    }

    k = readNumber("Enter number of players: ");

    if (k <= 0)
    {
        printf("Number of players must be greater than 0.\n");
        return;
    }

    printf("Top %d %s of Team %s:\n", k, roleGroupNames[role], team->name);
    printTeamHeader();

    current = team->roleHead[role];

    while (current != NULL && shown < k)
    {
        printPlayerRow(current->player);
        shown++;
        current = current->next;
    }

    printLine(92);

    if (shown < k)
    {
        printf("Only %d player(s) of this role are available in the team.\n", shown);
    }
}

void swapNodes(RoleNode **first, RoleNode **second)
{
    RoleNode *temp = *first;
    *first = *second;
    *second = temp;
}

void heapPush(Heap *heap, RoleNode *node)
{
    int index = heap->size;
    int parent;

    heap->items[index] = node;
    heap->size++;

    while (index > 0)
    {
        parent = (index - 1) / 2;

        if (isBefore(heap->items[index]->player, heap->items[parent]->player))
        {
            swapNodes(&heap->items[index], &heap->items[parent]);
            index = parent;
        }
        else
        {
            break;
        }
    }
}

RoleNode *heapPop(Heap *heap)
{
    RoleNode *top = heap->items[0];
    int index = 0;
    int left;
    int right;
    int best;

    heap->size--;

    if (heap->size > 0)
    {
        heap->items[0] = heap->items[heap->size];

        while (1)
        {
            left = 2 * index + 1;
            right = 2 * index + 2;
            best = index;

            if (left < heap->size && isBefore(heap->items[left]->player, heap->items[best]->player))
            {
                best = left;
            }

            if (right < heap->size && isBefore(heap->items[right]->player, heap->items[best]->player))
            {
                best = right;
            }

            if (best == index)
            {
                break;
            }

            swapNodes(&heap->items[index], &heap->items[best]);
            index = best;
        }
    }

    return top;
}

void displayRoleAcrossTeams()
{
    int role;
    int count = 0;
    int i;
    Heap heap;
    RoleNode *top;

    role = readRole();

    if (role == -1)
    {
        return;
    }

    heap.size = 0;

    for (i = 0; i < totalTeams; i++)
    {
        if (teamList[i].roleHead[role] != NULL)
        {
            heapPush(&heap, teamList[i].roleHead[role]);
        }
    }

    printf("%s of all teams:\n", roleGroupNames[role]);
    printAcrossHeader();

    while (heap.size > 0)
    {
        top = heapPop(&heap);
        printAcrossRow(top->player);
        count++;

        if (top->next != NULL)
        {
            heapPush(&heap, top->next);
        }
    }

    printLine(112);
    printf("Total %s: %d\n", roleGroupNames[role], count);
}

void printMenu()
{
    printLine(78);
    printf("ICC ODI Player Performance Analyzer\n");
    printLine(78);
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printLine(78);
}

void freeMemory()
{
    int i;
    int j;
    PlayerNode *playerNode;
    PlayerNode *nextPlayer;
    RoleNode *roleNode;
    RoleNode *nextRole;

    for (i = 0; i < totalTeams; i++)
    {
        playerNode = teamList[i].head;

        while (playerNode != NULL)
        {
            nextPlayer = playerNode->next;
            free(playerNode);
            playerNode = nextPlayer;
        }

        for (j = 0; j < NUM_ROLES; j++)
        {
            roleNode = teamList[i].roleHead[j];

            while (roleNode != NULL)
            {
                nextRole = roleNode->next;
                free(roleNode);
                roleNode = nextRole;
            }
        }
    }
}

int main()
{
    int choice;

    loadData();

    while (1)
    {
        printMenu();
        choice = readNumber("Enter your choice: ");

        if (choice == -1 && feof(stdin))
        {
            break;
        }

        switch (choice)
        {
        case 1:
            addPlayer();
            break;

        case 2:
            displayTeamPlayers();
            break;

        case 3:
            displayTeamsByStrikeRate();
            break;

        case 4:
            displayTopPlayers();
            break;

        case 5:
            displayRoleAcrossTeams();
            break;

        case 6:
            printf("Exiting ICC ODI Player Performance Analyzer.\n");
            freeMemory();
            return 0;

        default:
            printf("Invalid choice. Please enter a number between 1 and 6.\n");
        }

        printf("\n");
    }

    freeMemory();
    return 0;
}