#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <pthread.h>

#define MAX_ARRAY_LIMIT 1048576
#define MAX_PROCESS_LIMIT 1024
#define DEFAULT_PROCESS_COUNT 8

/**/
#define DEBUG 1
/**/

int PROCESS_COUNT = DEFAULT_PROCESS_COUNT;
unsigned char CHRONO_TRIGGER[MAX_PROCESS_LIMIT];
char TODO_LIST[MAX_ARRAY_LIMIT][1024];
int LINE_COUNT;
int EXTRA;
int QUOTA;

void * workerProcess(void * argument) {
	long process_id = (long)argument;
	int start_index = process_id * QUOTA;
	int end_index = start_index + QUOTA;
	int filename_length;
	char command_string[6144];
	
	if(EXTRA) {
		if(process_id < EXTRA) {
			start_index += process_id;
			end_index += process_id + 1;
		}
		else {
			start_index += EXTRA;
			end_index += EXTRA;
		}
	}
	
	#ifdef DEBUG
		printf("Worker %ld: [%d - %d]\n", process_id, start_index, (end_index-1));
	#endif
	
	for(int i = start_index; i < end_index; i++) {
		#if (DEBUG > 1)
			printf("Worker %ld will do TODO_LIST[%d]: %s\n", process_id, i, TODO_LIST[i]);
		#endif
		
		strcpy(command_string, TODO_LIST[i]);
		
		#ifdef DEBUG
			printf("%s\n\n", command_string);
		#endif
		
		system(command_string);
	}
	
	#ifdef DEBUG
		printf("Worker %ld has finished and is joining with master.\n", process_id);
	#endif
	
	CHRONO_TRIGGER[process_id] = 1;
	
	return NULL;
}

int main(int argc, char** argv) {
	int worker_count;
	char todo_filename[2048];
	
	#ifdef DEBUG
		printf("%d Arguments received:\n", argc);
		for(int i = 0; i < argc; i++) {
			printf("%s\n", argv[i]);
		}
		printf("\n");
	#endif
	
	if(argc < 2) {
		printf("Require todo file path and optional process count (defaults to %d processes)\n", DEFAULT_PROCESS_COUNT);
		exit(EXIT_FAILURE);
	}
	else if(argc > 2) {
		PROCESS_COUNT = atoi(argv[2]);
	}
	
	worker_count = PROCESS_COUNT - 1;
	
	strcpy(todo_filename, argv[1]);
	
	FILE * todo_file = fopen(todo_filename, "r");
	
	if(todo_file == NULL) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	else {
		int i = 0;
		LINE_COUNT = 0;
		char read_char = fgetc(todo_file);
		
		while(read_char != EOF) {
			TODO_LIST[LINE_COUNT][i] = read_char;
			i++;
			
			if(read_char == '\n') {
				TODO_LIST[LINE_COUNT][i-1] = '\0';
				i = 0;
				
				#ifdef DEBUG
					printf("TODO_LIST[%d]=%s\n", LINE_COUNT, TODO_LIST[LINE_COUNT]);
				#endif
				
				LINE_COUNT++;
			}
			
			read_char = fgetc(todo_file);
		}
		
		fclose(todo_file);
	}
	
	if(worker_count > LINE_COUNT) {
		worker_count = LINE_COUNT;
	}
	
	EXTRA = LINE_COUNT % worker_count;
	QUOTA = LINE_COUNT / worker_count;
	
	#ifdef DEBUG
		printf("LINE_COUNT=%d\nWORKER_COUNT=%d\n", LINE_COUNT, worker_count);
		printf("EXTRA=%d\nQUOTA=%d\n", EXTRA, QUOTA);
	#endif
	
	int result_code;
	pthread_t process[worker_count];
	
	/* create processes */
	for(long i = 0; i < worker_count; i++) {
		CHRONO_TRIGGER[i] = 0;
		result_code = pthread_create(&process[i], NULL, workerProcess, (void *)i);
		assert(!result_code);
	}
	
	/* wait for processes to finish */
	unsigned char workers_finished = 0;
	while(!workers_finished) {
		workers_finished = 2;
		
		for(int i = 0; i < worker_count; i++) {
			if(CHRONO_TRIGGER[i] == 1) {
				result_code = pthread_join(process[i], NULL);
				assert(!result_code);
				CHRONO_TRIGGER[i] = 2;
			}
			
			workers_finished &= CHRONO_TRIGGER[i];
		}
	}
	
	printf("%d worker process(es) completed transforming CSVs\n", worker_count);
	
	exit(EXIT_SUCCESS);
}
