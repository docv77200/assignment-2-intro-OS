
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the struct GLOBALLY 
struct movie {
    char *title;
    int year;
    char *languages[5];
    int num_languages;
    double rating;
    struct movie *next;
};

// Function to parse languages from "[English;French]" format
void parseLanguages(char *langString, struct movie *newMovie) {
    // Remove the opening bracket '['
    langString++;  // Move pointer forward by 1 to skip '['
    
    // Find and remove the closing bracket ']'
    char *closeBracket = strchr(langString, ']');
    if (closeBracket != NULL) {
        *closeBracket = '\0';  // Replace ']' with null terminator
    }
    
    //  tokenize by semicolon
    char *saveptr2;
    char *lang = strtok_r(langString, ";", &saveptr2);
    int langCount = 0;
    
    while (lang != NULL && langCount < 5) {
        // Allocate memory and copy the language
        newMovie->languages[langCount] = calloc(strlen(lang) + 1, sizeof(char));
        strcpy(newMovie->languages[langCount], lang);
        langCount++;
        
        lang = strtok_r(NULL, ";", &saveptr2);
    }
    
    newMovie->num_languages = langCount;
}

// Function to create a new movie from a line of CSV
struct movie* createMovie(char *currLine) {
    struct movie *newMovie = malloc(sizeof(struct movie));
    
    char *saveptr;
    
    // Get title
    char *token = strtok_r(currLine, ",", &saveptr);
    newMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(newMovie->title, token);
    
    // Get year
    token = strtok_r(NULL, ",", &saveptr);
    newMovie->year = atoi(token);
    
    // Get languages
    token = strtok_r(NULL, ",", &saveptr);
    parseLanguages(token, newMovie);
    
    // Get rating
    token = strtok_r(NULL, ",", &saveptr);
    newMovie->rating = atof(token);
    
    newMovie->next = NULL;
    
    return newMovie;
}

// Function to process the CSV file and build linked list
struct movie* processFile(char *filePath) {
    FILE *movieFile = fopen(filePath, "r");
    
    if (movieFile == NULL) {
        printf("Error opening file\n");
        return NULL;
    }
    
    char *currLine = NULL;
    size_t len = 0;
    int movieCount = 0;
    
    struct movie *head = NULL;
    struct movie *tail = NULL;
    
    // Read first line (header) and skip it
    getline(&currLine, &len, movieFile);
    
    // Read the rest of the file line by line
    while (getline(&currLine, &len, movieFile) != -1) {
        // Remove newline character at end
        currLine[strcspn(currLine, "\n")] = '\0';
        
        // Create movie struct from this line
        struct movie *newMovie = createMovie(currLine);
        
        // Add to linked list
        if (head == NULL) {
            // First movie
            head = newMovie;
            tail = newMovie;
        } else {
            // Add to end of list
            tail->next = newMovie;
            tail = newMovie;
        }
        
        movieCount++;
    }
    
    free(currLine);
    fclose(movieFile);
    
    printf("Processed file %s and parsed data for %d movies\n", filePath, movieCount);
    
    return head;
}

// Show movies released in a specific year
void showMoviesByYear(struct movie *head) {
    int year;
    printf("Enter the year for which you want to see movies: ");
    scanf("%d", &year);
    
    int found = 0;
    struct movie *current = head;
    
    while (current != NULL) {
        if (current->year == year) {
            printf("%s\n", current->title);
            found = 1;
        }
        current = current->next;
    }
    
    if (found == 0) {
        printf("No data about movies released in the year %d\n", year);
    }
}

// Show highest rated movie for each year
void showHighestRatedByYear(struct movie *head) {
    struct movie *current = head;
    
    // We'll check each movie and see if it's the highest rated for its year
    while (current != NULL) {
        int year = current->year;
        double highestRating = current->rating;
        char *bestTitle = current->title;
        
        // Check all other movies to find the highest rated for this year
        struct movie *checker = head;
        while (checker != NULL) {
            if (checker->year == year && checker->rating > highestRating) {
                highestRating = checker->rating;
                bestTitle = checker->title;
            }
            checker = checker->next;
        }
        
        // Check if we already printed this year
    
        struct movie *alreadyPrinted = head;
        int shouldPrint = 1;
        while (alreadyPrinted != current) {
            if (alreadyPrinted->year == year) {
                shouldPrint = 0;
                break;
            }
            alreadyPrinted = alreadyPrinted->next;
        }
        
        if (shouldPrint) {
            printf("%d %.1f %s\n", year, highestRating, bestTitle);
        }
        
        current = current->next;
    }
}

// Show movies by language
void showMoviesByLanguage(struct movie *head) {
    char language[21];  // Max 20 chars + null terminator
    printf("Enter the language for which you want to see movies: ");
    scanf("%s", language);
    
    int found = 0;
    struct movie *current = head;
    
    while (current != NULL) {
        // Check each language in this movie
        for (int i = 0; i < current->num_languages; i++) {
            if (strcmp(current->languages[i], language) == 0) {
                printf("%d %s\n", current->year, current->title);
                found = 1;
                break;  // Found it, don't check other languages for this movie
            }
        }
        current = current->next;
    }
    
    if (found == 0) {
        printf("No data about movies released in %s\n", language);
    }
}

// Display the menu
void displayMenu() {
    printf("\n1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of release of all movies in a specific language\n");
    printf("4. Exit from the program\n");
    printf("\nEnter a choice from 1 to 4: ");
}

// Main menu loop
void menuLoop(struct movie *head) {
    int choice;
    
    while (1) {  // Infinite loop until user chooses to exit
        displayMenu();
        scanf("%d", &choice);
        
        if (choice == 1) {
            showMoviesByYear(head);
        }
        else if (choice == 2) {
            showHighestRatedByYear(head);
        }
        else if (choice == 3) {
            showMoviesByLanguage(head);
        }
        else if (choice == 4) {
            break;  // Exit the loop
        }
        else {
            printf("You entered an incorrect choice. Try again.\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies_sample_1.csv\n");
        return 1;
    }
    
    // Process the file and get the head of the linked list
    struct movie *movieList = processFile(argv[1]);
    
    // Start the interactive menu
    menuLoop(movieList);
    
    return 0;
}