//--------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//--------------------------------------------------------------
//
#define MAX_BOATS 120
#define MAX_STRING 127

//--------------------------------------------------------------
typedef char String[MAX_STRING];

typedef enum {
	slip,
	land,
	trailor,
	storage,
	no_place
} StorageType;

typedef struct {
	int slip_number;
} Slip;

typedef struct {
	char bay_letter;
} Land;

typedef struct {
	String trailor_tag;
} Trailor;

typedef struct {
	int storage_number;
} Storage;

typedef union {
	Slip slip;
	Land land;
	Trailor trailor;
	Storage storage;
} Information;

typedef struct {
	String name;
	int length;
	StorageType place;
	Information info;
	double owed;
} Boat;

typedef enum {
	I,
	A,
	R,
	P,
	M,
	X,
	error
} Command;

//--------------------------------------------------------------

// A function for handling string input
void input_string(String string) {

	// Get string and replace newline character with termination character
	fgets(string, MAX_STRING, stdin);
	string[strcspn(string, "\n")] = '\0';
}

//--------------------------------------------------------------

// A function for transferring from string to storage datatype
StorageType string_to_storage(String storage_string) {

	if (!strcasecmp(storage_string,"slip"))
		return slip;
	if (!strcasecmp(storage_string,"land"))
		return land;
	if (!strcasecmp(storage_string,"trailor"))
		return trailor;
	if (!strcasecmp(storage_string,"storage"))
		return storage;
	return no_place;
}

//--------------------------------------------------------------

// A function for tranferring from storage datatype to string
char* storage_to_string(StorageType type) {

	switch (type) {
		case slip:
			return "slip";
		case land:
			return "land";
		case trailor:
			return "trailor";
		case storage:
			return "storage";
		case no_place:
			return "no_place";
		default:
			exit(EXIT_FAILURE);
			break;
	}
}

//--------------------------------------------------------------

// A function for tranferring from string to command datatype
Command string_to_command(String command_string) {

	if (!strcasecmp(command_string,"I"))
                return I;
        if (!strcasecmp(command_string,"A"))
                return A;
        if (!strcasecmp(command_string,"R"))
                return R;
        if (!strcasecmp(command_string,"P"))
                return P;
	if (!strcasecmp(command_string,"M"))
		return M;
	if (!strcasecmp(command_string,"X"))
		return X;
        return error;
}

//--------------------------------------------------------------

// A function for comparing boat names for sorting
int boat_comparator(const void *a, const void *b) {

	// Get the names of the two boat pointers and compare them
	Boat* boat_a = *(Boat**) a;
	Boat* boat_b = *(Boat**) b;

	return strcasecmp(boat_a->name,boat_b->name);
}

//--------------------------------------------------------------

// A function for showing the current boat inventory
void inventory(Boat** inventory,int num_boats) {

        String information;

	// For each boat, transform the unique information of its storage type into a string
        for (int i = 0; i < num_boats; i++) {

                switch (inventory[i]->place) {

                        case slip:
                                sprintf(information, "# %d", inventory[i]->info.slip.slip_number);
                                break;
                        case land:
                                sprintf(information, "%c", inventory[i]->info.land.bay_letter);
                                break;
                        case trailor:
                                strcpy(information, inventory[i]->info.trailor.trailor_tag);
                                break;
                        case storage:
                                sprintf(information, "# %d", inventory[i]->info.storage.storage_number);
                                break;
                        default:
                                information[0] = '\0';
                                break;
                }

		// Output boat information
                printf("%-20s\t%d'\t%8s %6s\t Owes $%7.2f\n",
                                inventory[i]->name,
                                inventory[i]->length,
                                storage_to_string(inventory[i]->place),
                                information,
                                inventory[i]->owed);
        }
}

//--------------------------------------------------------------

// A function for adding a new boat to inventory
void add_boat(Boat*** boats,int* num_boats,int* boat_spots,char* line) {

	char* token;
	String name;
	int length;
	StorageType place;

	//Allocate space for the boat pointers

        // If too many boats, flag error
        if (*num_boats > MAX_BOATS) {
        	printf("Too many boats\n");
                exit(EXIT_FAILURE);

	}

        // If space runs out, allocate more memory
        else if (*num_boats == *boat_spots) {
        	*boat_spots *= 2;

               	// Cannot allocate more than MAX_BOATS
                if (*boat_spots > MAX_BOATS) {
                	*boat_spots = MAX_BOATS;
                }

                // Allocate new space if possible
                *boats = (Boat**) realloc(*boats, *boat_spots * sizeof(Boat*));
                if (*boats == NULL) {
                	printf("Reallocation failed\n");
                	exit(EXIT_FAILURE);
                }
	}

	// Allocate memory for the boat
	(*boats)[*num_boats] = (Boat*) malloc(sizeof(Boat));
	if ((*boats)[*num_boats] == NULL) {
		printf("Boat memory allocation failled\n");
		exit(EXIT_FAILURE);
	}

	// Read name
	token = strtok(line, ",");
	strcpy((*boats)[*num_boats]->name,token);

	// If name already exists, flag user
	for (int i = 0; i < *num_boats; i++) {
		if (!strcasecmp(token,(*boats)[i]->name)) {
			printf("Boat already in system, please remove boat data first before changing it\n");
			free((*boats)[*num_boats]);
			return;
		}
	}

	// Read length
	token = strtok(NULL, ",");
	(*boats)[*num_boats]->length = atoi(token);

	// If length is not valid, flag user
	if (atoi(token) < 1 || atoi(token) > 100) {
		printf("Length not suitable, please input a length within 1 and 100");
		free((*boats)[*num_boats]);
		return;
	}

	// Read Storage location
	token = strtok(NULL, ",");
	(*boats)[*num_boats]->place = string_to_storage(token);

	// Read specific storage information
	token = strtok(NULL, ",");
	switch ((*boats)[*num_boats]->place) {
		case slip:
			(*boats)[*num_boats]->info.slip.slip_number = atoi(token);

			// If slip number is not valid, flag user
			if (atoi(token) < 1 || atoi(token) > 85) {
				printf("Slip number not suitable, please input a slip number between 1 and 85 inclusive\n");
				free((*boats)[*num_boats]);
				return;
			}
			break;

		case land:
			(*boats)[*num_boats]->info.land.bay_letter = token[0];

			// If bay letter is not valid, flag user
			if (strlen(token) != 1 || !isalnum(token[0]) || !isupper(token[0])) {
				printf("Bay letter not suitable, please input one capital letter from A-Z\n");
				free((*boats)[*num_boats]);
				return;
			}
			break;

		case trailor:
			strcpy((*boats)[*num_boats]->info.trailor.trailor_tag,token);
			break;
		case storage:
			(*boats)[*num_boats]->info.storage.storage_number = atoi(token);
			
			// If storage space number is not valid, flag user
			if (atoi(token) < 1 || atoi(token) > 50) {
                                printf("Storage space number not suitable, please input a storage space number between 1 and 50 inclusive\n");
				free((*boats)[*num_boats]);
                                return;
                        }
			break;

		default:
			printf("Unknown storage location\n");
			free((*boats)[*num_boats]);
			return;
	}

	// Read amount owed
	token = strtok(NULL, ",");
	(*boats)[*num_boats]->owed = atof(token);

	// Increment number of boats
	*num_boats += 1;

	// Sort boats by name
        qsort(*boats,*num_boats,sizeof(Boat*),boat_comparator);
}

//--------------------------------------------------------------

// A function to remove boats from the inventory
void remove_boat(Boat** boats,int* num_boats,char* name) {

	for (int i = 0; i < *num_boats; i++) {

		// If the boat is in inventory
		if (!strcasecmp(boats[i]->name,name)) {

			// Free the boat's information
			free(boats[i]);

			// Have the last boat take its place and decrement num_boats
			boats[i] = boats[*num_boats-1];
			*num_boats -= 1;
			
			// Sort remaining boats
			qsort(boats,*num_boats,sizeof(Boat*),boat_comparator);
			return;
		}
	}

	printf("No boat with that name\n");

}

//--------------------------------------------------------------

// A function to handle making payments
void payment(Boat** boats,int num_boats,char* name) {

	char payment_input[MAX_STRING];
	double payment;

	for (int i = 0; i < num_boats; i++) {

		// If the boat is in inventory
                if (!strcasecmp(boats[i]->name,name)) {

			// Get the desired payment
			printf("%-56s : ","Please enter the amount to be paid");
			input_string(payment_input);
			payment = atof(payment_input);

			// Check if its a valid payment and proceed accordingly
                        if (payment > boats[i]->owed) 
				printf("That is more than the amount owed, $%.2lf",boats[i]->owed);
			else
				boats[i]->owed -= payment;
                        return;
                }
        }

	printf("No boat with that name\n");

}

//--------------------------------------------------------------

// A function to update payments for the next month
void month_forward(Boat** boats,int num_boats) {

	for (int i = 0; i < num_boats; i++) {

		// For each boat, update the amount owed depending on its length and storage location
		switch (boats[i]->place) {
			case slip:
				boats[i]->owed += 12.50 * boats[i]->length;
				break;
			case land:
				boats[i]->owed += 14.00 * boats[i]->length;
				break;
			case trailor:
				boats[i]->owed += 25.00 * boats[i]->length;
				break;
			case storage:
				boats[i]->owed += 11.20 * boats[i]->length;
				break;
			default:
				printf("Improper storage location\n");
		}
	}
}

//--------------------------------------------------------------

// A function to control the input of commands
void command_terminal(Boat** boats,int* num_boats,int* boat_spots) {

	String input;
	Command command;
	char data[MAX_STRING + 30];

	// Command input loop
	while (1) {

		// Query and input command
		printf("\n%55s : ","(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it");
		input_string(input);
		command = string_to_command(input);

		// Proceed based on specific command
		switch (command) {
			case I:
				inventory(boats,*num_boats);
				break;
                	case A:
				printf("%-56s : ","Please enter the boat data in CSV format");
				input_string(data);
				add_boat(&boats,num_boats,boat_spots,data);
				break;
                	case R:
				printf("%-56s : ","Please enter the boat name");
				input_string(data);
				remove_boat(boats,num_boats,data);
				break;
                	case P:
				printf("%-56s : ","Please enter the boat name");
				input_string(data);
				payment(boats,*num_boats,data);
				break;
			case M:
				month_forward(boats,*num_boats);
				break;
			case X:
				return;
                	case error:
                        	printf("Invalid option %s\n",input);
				break;
                	default:
                        	exit(EXIT_FAILURE);
		}
	}
}

//--------------------------------------------------------------

// A function to initialize the array of boats based on the input file
void initialize(Boat*** boats,int* num_boats,int* boat_spots,char* file_name) {

	FILE *file;
	char line[200];
	
	// Allocate space for boat pointer array
	*boats = (Boat**) malloc(*boat_spots * sizeof(Boat*));
	if (*boats == NULL) {
		printf("Initial memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	
	// Read boat file
	if ((file = fopen(file_name, "r")) != NULL) {
		
		// Read each boat
		while (fgets(line, 200, file) != NULL) {

			add_boat(boats,num_boats,boat_spots,line);
		}

		// Close file
        	fclose(file);

	}

	// If nothing in file, the array with start empty
}

//--------------------------------------------------------------

void save(Boat** boats,int num_boats,char* output_name) {

	FILE *file;
	String information;

	// Open / Create file
	file = fopen(output_name,"w+");

	// Each row of the file stores the information of a boat specific to its storage location
	for (int i = 0; i < num_boats; i++) {
		
		switch (boats[i]->place) {
			case slip:
				fprintf(file,"%s,%d,%s,%d,%.2lf\n",
					boats[i]->name,
					boats[i]->length,
					storage_to_string(boats[i]->place),
					boats[i]->info.slip.slip_number,
					boats[i]->owed);
				break;
			case land:
                                fprintf(file,"%s,%d,%s,%c,%.2lf\n",
                                        boats[i]->name,
                                        boats[i]->length,
                                        storage_to_string(boats[i]->place),
                                        boats[i]->info.land.bay_letter,
                                        boats[i]->owed);
				break;
			case trailor:
                                fprintf(file,"%s,%d,%s,%s,%.2lf\n",
                                        boats[i]->name,
                                        boats[i]->length,
                                        storage_to_string(boats[i]->place),
                                        boats[i]->info.trailor.trailor_tag,
                                        boats[i]->owed);
				break;
			case storage:
                                fprintf(file,"%s,%d,%s,%d,%.2lf\n",
                                        boats[i]->name,
                                        boats[i]->length,
                                        storage_to_string(boats[i]->place),
                                        boats[i]->info.storage.storage_number,
                                        boats[i]->owed);
				break;
		}
	}

	// Close file
	fclose(file);

}

//--------------------------------------------------------------

int main(int argc, char *argv[]) {

	Boat** boats;
	int num_boats = 0;
	int boat_spots = 1;

	// No file for input/output provided
        if (argc == 1) {
		printf("Please input a file to read/store to\n");
		return EXIT_FAILURE;
	}

	// Initialize array
	initialize(&boats,&num_boats,&boat_spots,argv[1]);

	// Begin command cycle
	printf("\nWelcome to the Boat Management System\n");
	printf("--------------------------------------\n");
	command_terminal(boats,&num_boats,&boat_spots);
	printf("\nExiting the Boat Management System\n");

	// Save current boat array
	save(boats, num_boats, argv[1]);

	// Free the memory of each boat in the array
	for (int i = 0; i < num_boats; i++)
		free(boats[i]);

	// Free the memory of the array
	free(boats);

	return EXIT_SUCCESS;
}

//--------------------------------------------------------------
