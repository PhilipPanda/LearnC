/*
 * Binary Data Serialization
 * 
 * Writing and reading structured data to binary files.
 * Shows proper serialization with headers and validation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// File format header
typedef struct {
    char magic[4];       // "GAME"
    uint16_t version;    // Format version
    uint32_t player_count;
} FileHeader;

// Player data
typedef struct {
    char name[32];
    int level;
    int score;
    float health;
    int inventory_count;
    int* inventory;  // Dynamic array
} Player;

// Save players to binary file
int save_players(const char* filename, Player* players, int count) {
    printf("Saving %d players to %s...\n", count, filename);
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }
    
    // Write header
    FileHeader header = {
        .magic = {'G', 'A', 'M', 'E'},
        .version = 1,
        .player_count = count
    };
    
    if (fwrite(&header, sizeof(FileHeader), 1, file) != 1) {
        fprintf(stderr, "Failed to write header\n");
        fclose(file);
        return -1;
    }
    
    // Write each player
    for (int i = 0; i < count; i++) {
        Player* p = &players[i];
        
        // Write fixed-size fields
        if (fwrite(p->name, sizeof(p->name), 1, file) != 1 ||
            fwrite(&p->level, sizeof(int), 1, file) != 1 ||
            fwrite(&p->score, sizeof(int), 1, file) != 1 ||
            fwrite(&p->health, sizeof(float), 1, file) != 1 ||
            fwrite(&p->inventory_count, sizeof(int), 1, file) != 1) {
            fprintf(stderr, "Failed to write player data\n");
            fclose(file);
            return -1;
        }
        
        // Write dynamic inventory array
        if (p->inventory_count > 0) {
            if (fwrite(p->inventory, sizeof(int), p->inventory_count, file) != 
                (size_t)p->inventory_count) {
                fprintf(stderr, "Failed to write inventory\n");
                fclose(file);
                return -1;
            }
        }
    }
    
    if (fclose(file) != 0) {
        perror("fclose");
        return -1;
    }
    
    printf("Successfully saved!\n\n");
    return 0;
}

// Load players from binary file
Player* load_players(const char* filename, int* count) {
    printf("Loading players from %s...\n", filename);
    
    *count = 0;
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }
    
    // Read and validate header
    FileHeader header;
    if (fread(&header, sizeof(FileHeader), 1, file) != 1) {
        fprintf(stderr, "Failed to read header\n");
        fclose(file);
        return NULL;
    }
    
    // Check magic number
    if (memcmp(header.magic, "GAME", 4) != 0) {
        fprintf(stderr, "Invalid file format (bad magic number)\n");
        fclose(file);
        return NULL;
    }
    
    // Check version
    if (header.version > 1) {
        fprintf(stderr, "Unsupported version: %d\n", header.version);
        fclose(file);
        return NULL;
    }
    
    printf("File version: %d\n", header.version);
    printf("Player count: %d\n", header.player_count);
    
    // Allocate player array
    Player* players = malloc(header.player_count * sizeof(Player));
    if (players == NULL) {
        fprintf(stderr, "Out of memory\n");
        fclose(file);
        return NULL;
    }
    
    // Read each player
    for (uint32_t i = 0; i < header.player_count; i++) {
        Player* p = &players[i];
        
        // Read fixed-size fields
        if (fread(p->name, sizeof(p->name), 1, file) != 1 ||
            fread(&p->level, sizeof(int), 1, file) != 1 ||
            fread(&p->score, sizeof(int), 1, file) != 1 ||
            fread(&p->health, sizeof(float), 1, file) != 1 ||
            fread(&p->inventory_count, sizeof(int), 1, file) != 1) {
            fprintf(stderr, "Failed to read player data\n");
            free(players);
            fclose(file);
            return NULL;
        }
        
        // Read dynamic inventory array
        if (p->inventory_count > 0) {
            p->inventory = malloc(p->inventory_count * sizeof(int));
            if (p->inventory == NULL) {
                fprintf(stderr, "Out of memory\n");
                free(players);
                fclose(file);
                return NULL;
            }
            
            if (fread(p->inventory, sizeof(int), p->inventory_count, file) != 
                (size_t)p->inventory_count) {
                fprintf(stderr, "Failed to read inventory\n");
                free(p->inventory);
                free(players);
                fclose(file);
                return NULL;
            }
        } else {
            p->inventory = NULL;
        }
    }
    
    fclose(file);
    
    *count = header.player_count;
    printf("Successfully loaded!\n\n");
    return players;
}

// Print player info
void print_player(Player* p) {
    printf("Player: %s\n", p->name);
    printf("  Level:  %d\n", p->level);
    printf("  Score:  %d\n", p->score);
    printf("  Health: %.1f%%\n", p->health);
    printf("  Inventory (%d items): [", p->inventory_count);
    for (int i = 0; i < p->inventory_count; i++) {
        printf("%d", p->inventory[i]);
        if (i < p->inventory_count - 1) printf(", ");
    }
    printf("]\n");
}

// Free player data
void free_players(Player* players, int count) {
    for (int i = 0; i < count; i++) {
        if (players[i].inventory != NULL) {
            free(players[i].inventory);
        }
    }
    free(players);
}

// Create sample data
Player* create_sample_players(int* count) {
    *count = 3;
    Player* players = malloc(*count * sizeof(Player));
    
    // Player 1
    strcpy(players[0].name, "Alice");
    players[0].level = 10;
    players[0].score = 1500;
    players[0].health = 85.5f;
    players[0].inventory_count = 5;
    players[0].inventory = malloc(5 * sizeof(int));
    players[0].inventory[0] = 101;  // Sword
    players[0].inventory[1] = 102;  // Shield
    players[0].inventory[2] = 201;  // Potion
    players[0].inventory[3] = 201;  // Potion
    players[0].inventory[4] = 301;  // Key
    
    // Player 2
    strcpy(players[1].name, "Bob");
    players[1].level = 15;
    players[1].score = 2300;
    players[1].health = 100.0f;
    players[1].inventory_count = 3;
    players[1].inventory = malloc(3 * sizeof(int));
    players[1].inventory[0] = 103;  // Axe
    players[1].inventory[1] = 104;  // Bow
    players[1].inventory[2] = 202;  // Elixir
    
    // Player 3
    strcpy(players[2].name, "Charlie");
    players[2].level = 7;
    players[2].score = 950;
    players[2].health = 62.0f;
    players[2].inventory_count = 2;
    players[2].inventory = malloc(2 * sizeof(int));
    players[2].inventory[0] = 201;  // Potion
    players[2].inventory[1] = 205;  // Scroll
    
    return players;
}

int main(void) {
    printf("=== Binary Data Serialization Example ===\n\n");
    
    const char* filename = "savegame.dat";
    
    // Create sample data
    printf("Creating sample player data...\n\n");
    int count;
    Player* original_players = create_sample_players(&count);
    
    printf("Original players:\n");
    printf("=================\n");
    for (int i = 0; i < count; i++) {
        print_player(&original_players[i]);
        printf("\n");
    }
    
    // Save to file
    if (save_players(filename, original_players, count) != 0) {
        free_players(original_players, count);
        return 1;
    }
    
    // Get file size
    FILE* f = fopen(filename, "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fclose(f);
        printf("File size: %ld bytes\n\n", size);
    }
    
    // Load from file
    int loaded_count;
    Player* loaded_players = load_players(filename, &loaded_count);
    
    if (loaded_players == NULL) {
        free_players(original_players, count);
        return 1;
    }
    
    printf("Loaded players:\n");
    printf("===============\n");
    for (int i = 0; i < loaded_count; i++) {
        print_player(&loaded_players[i]);
        printf("\n");
    }
    
    // Verify data matches
    printf("=== Verification ===\n");
    if (count == loaded_count) {
        printf("✓ Player count matches\n");
        
        int match = 1;
        for (int i = 0; i < count; i++) {
            if (strcmp(original_players[i].name, loaded_players[i].name) != 0 ||
                original_players[i].level != loaded_players[i].level ||
                original_players[i].score != loaded_players[i].score) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            printf("✓ All player data matches\n");
        } else {
            printf("✗ Player data mismatch\n");
        }
    } else {
        printf("✗ Player count mismatch\n");
    }
    
    printf("\n=== Key Concepts ===\n");
    printf("- File header with magic number and version\n");
    printf("- Writing/reading fixed-size structs\n");
    printf("- Handling dynamic arrays (pointers)\n");
    printf("- Validating file format on load\n");
    printf("- Proper error handling and cleanup\n");
    
    // Cleanup
    free_players(original_players, count);
    free_players(loaded_players, loaded_count);
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
